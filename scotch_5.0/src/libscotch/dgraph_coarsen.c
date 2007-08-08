/* Copyright 2007 INRIA
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
*/
/************************************************************/
/**                                                        **/
/**   NAME       : dgraph_coarsen.c                        **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This file implements the coarsening     **/
/**                phase of the multi-level method.        **/
/**                The implementation uses several         **/
/**                processes, which could have several     **/
/**                threads each (3 at this time).          **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 27 jul 2005     **/
/**                                 to   : 25 jul 2007     **/
/**                                                        **/
/************************************************************/

#define DGRAPH_COARSEN_C

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "module.h"
#include "common.h"
#include "dgraph.h"
#include "dgraph_allreduce.h"

/* #define SCOTCH_ASYNC */

#include "dgraph_coarsen.h"
#include "dgraph_match.h"

#define DGRAPHCOARSENVERTSND(procnum, procglbnbr) (0*(procglbnbr) + (procnum))
#define DGRAPHCOARSENVERTRCV(procnum, procglbnbr) (1*(procglbnbr) + (procnum))
#define DGRAPHCOARSENEDGESND(procnum, procglbnbr) (2*(procglbnbr) + (procnum))
#define DGRAPHCOARSENEDGERCV(procnum, procglbnbr) (3*(procglbnbr) + (procnum))
#define DGRAPHCOARSENEDLOSND(procnum, procglbnbr) (4*(procglbnbr) + (procnum))
#define DGRAPHCOARSENEDLORCV(procnum, procglbnbr) (5*(procglbnbr) + (procnum))

#ifdef TIME_MEASURE
static double total_time_coarsen=0.0L;
#endif /* TIME_MEASURE */

/***************************/
/*                         */
/* The coarsening routine. */
/*                         */
/***************************/

/**
   @brief
   This routine coarsens the given "finegraph" into
   "coargraph", as long as the coarsening ratio remains
   below some threshold value and the coarsened graph
   is not too small.

   @param finegrafptr pointer to finer graph
   @param coargrafptr pointer to coarser graph
   @param coarmultptr pointer to multinode table
   @param coarnbr     minimum number of coarse vertices
   @param coarrat     maximum contraction ratio

   @return
   - 0  : if the graph has been coarsened.
   - 1  : if the graph could not be coarsened.
   - 2  : on error.
*/

DGRAPHALLREDUCEMAXSUMOP (4,2)

int
dgraphCoarsen (
Dgraph * restrict const               finegrafptr,/*+ Graph to coarsen                     +*/
Dgraph * restrict const               coargrafptr,/*+ Coarse graph to build                +*/
DgraphCoarsenMulti * restrict * const coarmultptr,/*+ Pointer to multinode table to build  +*/
const Gnum                            coarnbr,    /*+ Minimum number of coarse vertices    +*/
const int                             dofolddup,  /*+ Allow fold/dup or fold or nofold     +*/
const Gnum                            dupmax,     /*+ Minimum number of vertices to do dup +*/
const double                          coarrat,    /*+ Maximum contraction ratio            +*/
const unsigned int                    reqsize)    /*+ Sizeof MPI buffer in matching        +*/
{
  Gnum                           coarvertnbr;     /* Number of coarse vertices              */
  Gnum                           coarvertmax;     /* Maximum number of multinode vertices   */
  Gnum                           sndvertnbr;
  Gnum                           sndedgenbr;
  Gnum                           rcvvertnbr;
  Gnum                           rcvedgenbr;
#ifdef PTSCOTCH_FOLD_DUP
  Dgraph                         mycoargraf;
#endif /* PTSCOTCH_FOLD_DUP */
  Dgraph *                       mycoargrafptr;
  DgraphCoarsenShared * restrict sharedcoar;
  DgraphMatchParam * restrict   sharedmatch;
  Gnum                           vertlocnbr;
  Gnum                           edgelocnbr;
  int                            procnum;
  int                            cheklocval = 0;
  int                            chekglbval = 0;

#ifdef SCOTCH_DEBUG_DGRAPH2
  Gnum                           finevertnum;
  Gnum                           coarmultnum;
  Gnum                           finevertfrstnum;
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  Gnum                           reduloctab[6];
  Gnum                           reduglbtab[6];
#ifdef TIME_MEASURE
  double                         currtime;
#endif /* TIME_MEASURE */

#ifdef TIME_MEASURE
  currtime = MPI_Wtime();
#endif /* TIME_MEASURE */


#ifdef SCOTCH_DEBUG_DGRAPH1
  if (coarrat < 0.5L)                             /* If impossible coarsening ratio wanted */
    return (1);                                   /* We will never succeed                 */
#endif /* SCOTCH_DEBUG_DGRAPH1 */

  if (finegrafptr->vertglbnbr < coarnbr)          /* If there are too few vertices in graph */
    return (1);                                   /* It is useless to go any further        */
  coarvertmax = (Gnum) ((double) finegrafptr->vertglbnbr * 0.5); /* Maximum number of coarse vertices */

  /** Step 1
   ** Compute matching pairs with dgraphMatch
   **/

  if (dgraphGhst (finegrafptr) != 0) {            /* Compute ghost edge array if not already present */
    errorPrint ("dgraphCoarsen: cannot compute ghost edge array");
    return     (1);
  }

  if (memAllocGroup ((void **)
                     &sharedmatch, (size_t) sizeof (DgraphMatchParam),
                     &sharedcoar,  (size_t) sizeof (DgraphCoarsenShared), NULL) == NULL) {
    errorPrint ("dgraphCoarsen: shared out of memory (1)");
    cheklocval = 1;
  }

#ifdef PTSCOTCH_FOLD_DUP
  mycoargrafptr = &mycoargraf;
#else /* PTSCOTCH_FOLD_DUP */
  mycoargrafptr = coargrafptr;
#endif /* PTSCOTCH_FOLD_DUP */

  /* sharedcoar->proccomm = finegrafptr->proccomm; */
  if (memAllocGroup ((void **)
                     &sharedmatch->coarmulttax, (size_t) (finegrafptr->vertgstnbr * sizeof (DgraphCoarsenMulti)),
                     &sharedmatch->finecoartax, (size_t) (finegrafptr->vertgstnbr * sizeof (Gnum)),
                     &sharedmatch->finefoldtax, (size_t) (finegrafptr->vertgstnbr * sizeof (int)),
                     &sharedcoar->vertrcv,      (size_t) (finegrafptr->procglbnbr * sizeof (int)),
                     &sharedcoar->vertsnd,      (size_t) (finegrafptr->procglbnbr * sizeof (int)),
                     &sharedcoar->edgercv,      (size_t) (finegrafptr->procglbnbr * sizeof (int)),
                     &sharedcoar->edgesnd,      (size_t) (finegrafptr->procglbnbr * sizeof (int)),
                     &sharedcoar->verttosndtab, (size_t) ((finegrafptr->vertgstnbr - finegrafptr->vertlocnbr) * sizeof (Gnum)),
                     NULL) == NULL) {
    errorPrint ("dgraphCoarsen: out of memory (2)");
    memFree    (sharedmatch);
    cheklocval = 2;
  }

  sharedmatch->reqsize = reqsize;
  sharedmatch->vertsndgsttab = sharedcoar->vertsnd;
  sharedmatch->vertrcvgsttab = sharedcoar->vertrcv;
  sharedmatch->edgesndgsttab = sharedcoar->edgesnd;
  sharedmatch->edgercvgsttab = sharedcoar->edgercv;
  sharedmatch->verttosndtab  = sharedcoar->verttosndtab;
  sharedcoar->finecoartax = (sharedmatch->finecoartax -= finegrafptr->baseval);
  sharedcoar->finefoldtax = (sharedmatch->finefoldtax -= finegrafptr->baseval);
  sharedcoar->coarmulttax = (sharedmatch->coarmulttax -= finegrafptr->baseval);

#ifdef SCOTCH_DEBUG_DGRAPH2
  memSet (sharedcoar->finecoartax + finegrafptr->baseval, ~0, (finegrafptr->vertgstnbr) * sizeof (Gnum));
  memSet (sharedcoar->finefoldtax + finegrafptr->baseval, ~0, (finegrafptr->vertgstnbr) * sizeof (int));
  memSet (sharedcoar->coarmulttax + finegrafptr->baseval, ~0, (finegrafptr->vertgstnbr) * sizeof (DgraphCoarsenMulti));
  memSet (sharedcoar->verttosndtab, ~0, (finegrafptr->vertgstnbr - finegrafptr->vertlocnbr) * sizeof (Gnum));
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  memSet (sharedcoar->vertrcv, 0, finegrafptr->procglbnbr * sizeof (int));
  memSet (sharedcoar->vertsnd, 0, finegrafptr->procglbnbr * sizeof (int));
  memSet (sharedcoar->edgercv, 0, finegrafptr->procglbnbr * sizeof (int));
  memSet (sharedcoar->edgesnd, 0, finegrafptr->procglbnbr * sizeof (int));

  if (dgraphCoarsenSharedInit (sharedcoar, finegrafptr, mycoargrafptr) != 0) {
    errorPrint ("dgraphCoarsen: out of memory (2)");
    memFree    (sharedmatch->coarmulttax + finegrafptr->baseval);
    memFree    (sharedmatch);
    cheklocval = 3;
  }

#ifdef SCOTCH_DEBUG_DGRAPH1                       /* This communication cannot be covered by a useful one */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, finegrafptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphCoarsen: communication error (1)");
    return     (1);
  }
#else /* SCOTCH_DEBUG_DGRAPH1 */
  chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_DGRAPH1 */
  if (chekglbval != 0)
    return (1);

#ifdef TIME_MEASURE
  total_time_coarsen += (MPI_Wtime () - currtime);
#endif /* TIME_MEASURE */

#ifdef SCOTCH_ASYNC
  if (dgraphMatchAsync (finegrafptr, sharedmatch, coarvertmax) != 0) {
#else /* SCOTCH_ASYNC */
  if (dgraphMatchSync (finegrafptr, sharedmatch, coarvertmax) != 0) {
#endif /*SCOTCH_ASYNC */
    errorPrint ("dgraphCoarsen: matching error");
    memFree    (sharedmatch->coarmulttax + finegrafptr->baseval);
    memFree    (sharedmatch);
    return     (1);
  }

#ifdef TIME_MEASURE
  currtime = MPI_Wtime ();
#endif /* TIME_MEASURE */

  MPI_Allreduce (&sharedmatch->coarmultnum, &coarvertnbr, 1, GNUM_MPI, MPI_SUM, finegrafptr->proccomm);

  coarvertnbr -= finegrafptr->baseval * finegrafptr->procglbnbr;
  coarvertmax = (Gnum) ((double) finegrafptr->vertglbnbr * coarrat); /* Maximum number of coarse vertices */
  if (coarvertnbr >= coarvertmax) {               /* If coarsened graph too large */
    memFree (sharedmatch->coarmulttax + finegrafptr->baseval); /* Do not proceed any further   */
    memFree (sharedmatch);
    return  (1);
  }

  /* Compute vertex and edge number */
  sndvertnbr = sndedgenbr =
  rcvvertnbr = rcvedgenbr = 0;
  vertlocnbr = sharedmatch->coarmultnum - finegrafptr->baseval ;
  edgelocnbr = finegrafptr->edgelocnbr; /* To have an overestimation of edgelocnbr */
  for (procnum = 0; procnum < finegrafptr->procglbnbr; ++ procnum) {
    rcvvertnbr += sharedcoar->vertrcv[procnum];
    rcvedgenbr += sharedcoar->edgercv[procnum];
    sndvertnbr -= sharedcoar->vertsnd[procnum];
    sndedgenbr -= sharedcoar->edgesnd[procnum];
  }
  edgelocnbr += rcvedgenbr - sndedgenbr;

#ifdef SCOTCH_DEBUG_DGRAPH2
  for (finevertnum = finegrafptr->baseval, finevertfrstnum = finegrafptr->procdsptab[finegrafptr->proclocnum];
       finevertnum < finegrafptr->vertlocnnd; finevertnum ++) {
    if ((sharedmatch->finecoartax[finevertnum] < 0)
        || (sharedmatch->finefoldtax[finevertnum] < 0)) {         /* If coarsening not aborted, this should not happen */
      errorPrint ("dgraphCoarsen: internal error (1)");
      cheklocval = 1;
      break;
    }
    if (sharedmatch->finefoldtax[finevertnum] == finegrafptr->proclocnum) /* Verify data coherence */
      if ((sharedmatch->coarmulttax[sharedmatch->finecoartax[finevertnum]].vertnum[0]
           != finevertfrstnum + finevertnum - finegrafptr->baseval)
          && (sharedmatch->coarmulttax[sharedmatch->finecoartax[finevertnum]].vertnum[1]
              != finevertfrstnum + finevertnum - finegrafptr->baseval)) {
        errorPrint ("dgraphCoarsen: internal error (1bis)");
        fprintf (stderr, "finvert : %ld (%ld/%ld) (%ld et %ld) (%ld)\n",
                 (long) finevertnum + finevertfrstnum, (long) sharedmatch->finecoartax[finevertnum], (long) sharedmatch->coarmultnum,
                 (long) sharedmatch->coarmulttax[sharedmatch->finecoartax[finevertnum]].vertnum[0],
                 (long) sharedmatch->coarmulttax[sharedmatch->finecoartax[finevertnum]].vertnum[1], (long) finevertfrstnum);
        cheklocval = 2;
        break;
      }
  }

  if (cheklocval == 0) {
    for (coarmultnum = finegrafptr->baseval ;
         coarmultnum < sharedmatch->coarmultnum ; coarmultnum ++) {
      if ((sharedmatch->coarmulttax[coarmultnum].vertnum[0] < 0) ||
          (sharedmatch->coarmulttax[coarmultnum].vertnum[1] < 0)){
        /* If coarsening not aborted, this should not happen */
        errorPrint ("dgraphCoarsen: internal error (1ter)");
        fprintf (stderr, "pour %ld/%ld (%ld %ld)", (long) coarmultnum, (long) sharedmatch->coarmultnum,
                 (long) sharedmatch->coarmulttax[coarmultnum].vertnum[0], (long) sharedmatch->coarmulttax[coarmultnum].vertnum[1]);
        cheklocval = 3;
      }
    }
  }

  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, finegrafptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphCoarsen: communication error (2)");
    return     (1);
  }
  if (chekglbval != 0) {
    memFree (sharedmatch->coarmulttax + finegrafptr->baseval);
    memFree (sharedmatch);
    return  (2);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  mycoargrafptr->vertlocnbr = vertlocnbr;
  mycoargrafptr->edgelocnbr = edgelocnbr;         /* Set temporary number of edges */

  dgraphCoarsenThreadConstruct (sharedcoar);

  dgraphCoarsenSharedExit (sharedcoar);

  if (memReallocGroup ((void *) (sharedmatch->coarmulttax + mycoargrafptr->baseval),
                       (void **) coarmultptr, (size_t) (mycoargrafptr->vertlocnbr * sizeof (DgraphCoarsenMulti)), NULL) == NULL) {
    errorPrint("dgraphCoarsen: out of memory (4)");
    memFree (sharedmatch);
    return (2);
  }
  *coarmultptr -= finegrafptr->baseval;

  memFree (sharedmatch);

  reduloctab[0] = mycoargrafptr->vertlocnbr;      /* Get maximum over all processes */
  reduloctab[1] = -mycoargrafptr->vertlocnbr;     /* Get minimum over all processes */
  reduloctab[2] = mycoargrafptr->edgelocnbr;
  reduloctab[3] = mycoargrafptr->degrglbmax;
  reduloctab[4] = mycoargrafptr->vertlocnbr;      /* Sum local sizes */
  reduloctab[5] = mycoargrafptr->edgelocnbr;

  if (dgraphAllreduceMaxSum (reduloctab, reduglbtab, 4, 2, mycoargrafptr->proccomm) != 0) {
    errorPrint ("dgraphCoarsen: communication error (X)");
    return     (1);
  }

  mycoargrafptr->vertglbmax = reduglbtab[0];
  mycoargrafptr->edgeglbmax = reduglbtab[2];
  mycoargrafptr->degrglbmax = reduglbtab[3];
  mycoargrafptr->vertglbnbr = reduglbtab[4];
  mycoargrafptr->edgeglbnbr = reduglbtab[5];
  mycoargrafptr->edgelocsiz = mycoargrafptr->edgelocnbr;
  mycoargrafptr->edgeglbsmx = mycoargrafptr->edgeglbmax;

#ifdef SCOTCH_DEBUG_DGRAPH2
  if (dgraphCheck (mycoargrafptr) != 0) {            /* Check graph consistency */
    errorPrint ("dgraphCoarsen: inconsistent graph data");
    dgraphFree (mycoargrafptr);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

#ifdef PTSCOTCH_FOLD_DUP
  if ((mycoargrafptr->procglbnbr >= 2) &&
      ((dofolddup != 0) || (dupmax > mycoargrafptr->vertglbnbr) ||
       (reduglbtab[1] == 0))) {                   /* No vertex on at least one processor */
    MPI_Datatype coarmultype;
    DgraphCoarsenMulti * restrict coarmultsav;    /* To unalloc old coarmulttax */

    MPI_Type_contiguous (2, GNUM_MPI, &coarmultype); /* Define type for MPI transfer */
    MPI_Type_commit (&coarmultype);               /* Commit new type                 */

    coarmultsav = *coarmultptr;
    if ((dofolddup < 0) || 
	((reduglbtab[1] == 0) && (dupmax < mycoargrafptr->vertglbnbr))) { /* Do a simple folding */
      memSet (coargrafptr, 0, sizeof(Dgraph));    /* To allow a good freeing :)          */
      coargrafptr->procglbnbr = 0;                /* Mark processor that does not handle the graph */
      coargrafptr->baseval = mycoargrafptr->baseval; /* Keep baseval for all processors */
      dgraphFold (mycoargrafptr, 0, coargrafptr, (void *) coarmultsav, (void **) coarmultptr, coarmultype);
    }
    else {                         /* Do a duplicant-folding */
      int loop;
      int tmp;

      dgraphFoldDup (mycoargrafptr, coargrafptr, (void *) coarmultsav, (void**)coarmultptr, coarmultype);
      loop = intRandVal (finegrafptr->proclocnum + intRandVal (finegrafptr->proclocnum * 2 + 1) + 1);
      while (loop --) {                           /* Desynchronise pseudo-randomizer between process */
        tmp = intRandVal (100);
      }
    }
    dgraphExit    (mycoargrafptr);
    MPI_Type_free (&coarmultype);
    memFree (coarmultsav + coargrafptr->baseval);
  }
  else
    *coargrafptr = *mycoargrafptr;
#endif /* PTSCOTCH_FOLD_DUP */

#ifdef TIME_MEASURE
  total_time_coarsen += (MPI_Wtime () - currtime);
  fprintf (stderr, "pour proclocnum %d, coar time : %g\n",
           finegrafptr->proclocnum, total_time_coarsen);
#endif /* TIME_MEASURE */

  return (0);
}

/* This routine performs the coarsening of edges
** with respect to the coarmulttax array computed
** by dgraphMatch.
*/

static void
dgraphCoarsenEdge (
  const DgraphCoarsenShared * restrict const shared,      /*+ Graph to coarsen      +*/
  const DgraphCoarsenMulti * restrict const  coarmulttax, /*+ Multinode array       +*/
  DgraphCoarsenHash * restrict const         coarhashtab, /*+ End vertex hash table +*/
  const Gnum                                 coarhashmsk) /*+ Hash table mask       +*/
{
  const Dgraph *      finegrafptr;      /* Shortcut to fine graph    */
  Dgraph *            coargrafptr;      /* Shortcut to coarsen graph */
  Gnum *              finecoartax;
  Gnum                coarvertnum;
  Gnum                coaredgenum;
  Gnum                coardegrmax;
  Gnum                coarvertfrstnum;
  Gnum                finevertfrstnum;
  Gnum                baseval;

  finegrafptr = shared->finegrafptr;
  coargrafptr = shared->coargrafptr;
  finecoartax = shared->finecoartax;
  baseval     = coargrafptr->baseval;

  finevertfrstnum = finegrafptr->procdsptab[finegrafptr->proclocnum];
  coarvertfrstnum = coargrafptr->procdsptab[coargrafptr->proclocnum];

  /* For all local vertices on coargraph */
  for (coarvertnum = coarvertfrstnum, coaredgenum = coargrafptr->baseval, coardegrmax = 0;
       coarvertnum < coargrafptr->procdsptab [coargrafptr->proclocnum + 1]; coarvertnum ++) {
    Gnum                finevertnum;
    int                 i;
    int                 procnum;

    coargrafptr->vertloctax [coarvertnum - coarvertfrstnum + baseval] = coaredgenum ; /* Set vertex edge index */
    i = 0;
    do {                                          /* For all fine edges of multinode vertices */
      Gnum                fineedgenum;

      finevertnum = coarmulttax[coarvertnum - coarvertfrstnum + baseval].vertnum[i];
      if (!dgraphVertexLocal (finegrafptr, finevertnum)) {
        /* Vertex have been received and extremity of edges are already converted */
        procnum = dgraphVertexProc (finegrafptr, finevertnum);
#ifdef SCOTCH_DEBUG_DGRAPH2
        if ((procnum < 0) || (finevertnum < 0)) {
          fprintf (stderr, "procnum %d finevertnum %ld coarvertnum %ld/%ld i %d\n",
                   procnum, (long)finevertnum, (long)coarvertnum, (long)coargrafptr->procdsptab[coargrafptr->proclocnum +1], i);
          errorPrint ("dgraphCoarsenEdge: proc not found (1)");
          return ;
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
        if (shared->requesttab[DGRAPHCOARSENVERTRCV(procnum, finegrafptr->procglbnbr)] != MPI_REQUEST_NULL) {
          /* We have to wait vertex data from procnum */
          MPI_Wait (&shared->requesttab[DGRAPHCOARSENVERTRCV(procnum, finegrafptr->procglbnbr)],
                    MPI_STATUS_IGNORE);
          shared->requesttab[DGRAPHCOARSENVERTRCV(procnum, finegrafptr->procglbnbr)] = MPI_REQUEST_NULL; /* Ensure no call to MPI_Wait next time */
        }
        finevertnum = dgraphCoarsenSearchVert (shared, procnum, finevertnum);
#ifdef SCOTCH_DEBUG_DGRAPH2
        if (finevertnum < 0) {
          fprintf (stderr, "procnum %d finevertnum %ld coarvertnum %ld/%ld i %d\n",
                   procnum, (long)coarmulttax[coarvertnum - coarvertfrstnum + baseval].vertnum[i], (long)coarvertnum, (long)coargrafptr->procdsptab[coargrafptr->proclocnum +1], i);
          errorPrint ("dgraphCoarsenEdge: vertex not found (1)");
          return ;
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

        if (shared->requesttab[DGRAPHCOARSENEDGERCV(procnum, finegrafptr->procglbnbr)] != MPI_REQUEST_NULL) {
          /* We have to wait edge data from procnum */
          MPI_Wait (&shared->requesttab[DGRAPHCOARSENEDGERCV(procnum, finegrafptr->procglbnbr)],
                    MPI_STATUS_IGNORE);
          shared->requesttab[DGRAPHCOARSENEDGERCV(procnum, finegrafptr->procglbnbr)] = MPI_REQUEST_NULL; /* Ensure no call to MPI_Wait next time */
          if (finegrafptr->edloloctax != NULL) /* Wait also weights of edge */ {
            MPI_Wait (&shared->requesttab[DGRAPHCOARSENEDLORCV(procnum, finegrafptr->procglbnbr)],
                      MPI_STATUS_IGNORE);
            shared->requesttab[DGRAPHCOARSENEDLORCV(procnum, finegrafptr->procglbnbr)] = MPI_REQUEST_NULL; /* Ensure no call to MPI_Wait next time */          }
        }

        for (fineedgenum = 0; fineedgenum < shared->vertproctab[procnum][finevertnum].edgenbr ;
             fineedgenum ++) {
          Gnum                coarvertend;        /* Number of coarse vertex which is end of fine edge */
          Gnum                h;

          coarvertend = shared->edgeproctab[procnum][shared->vertproctab[procnum][finevertnum].edgebegin + fineedgenum] ;

          if (coarvertend != coarvertnum) {       /* If not end of collapsed edge */
            for (h = (coarvertend * COARHASHPRIME) & coarhashmsk; ; h = (h + 1) & coarhashmsk) {
              if (coarhashtab[h].vertorgnum != coarvertnum) { /* If old slot           */
                coarhashtab[h].vertorgnum = coarvertnum; /* Mark it in reference array */
                coarhashtab[h].vertendnum = coarvertend;
                coarhashtab[h].edgenum    = coaredgenum;

                coargrafptr->edgeloctax [coaredgenum] = coarvertend; /* One more edge created      */
                if (finegrafptr->edloloctax != NULL)  /* Initialize edge load entry */
                  coargrafptr->edloloctax[coaredgenum] =
                    shared->edloproctab[procnum][shared->vertproctab[procnum][finevertnum].edgebegin + fineedgenum];
                else /* no weight on edges */
                  coargrafptr->edloloctax[coaredgenum] = 1;
                coaredgenum ++;
                break;                            /* Give up hashing */
              }
              if (coarhashtab[h].vertendnum == coarvertend) { /* If coarse edge already exists */
                if (finegrafptr->edloloctax != NULL) /* Accumulate edge load                    */
                  coargrafptr->edloloctax[coarhashtab[h].edgenum] +=
                    shared->edloproctab[procnum][shared->vertproctab[procnum][finevertnum].edgebegin + fineedgenum];
                else                              /* no weight in finer graph */
                  coargrafptr->edloloctax[coarhashtab[h].edgenum] += 1;
                break;                            /* Give up hashing                         */
              }
            }
          }
        }
      }
      else { /* Vertex is a local one */
        finevertnum = coarmulttax[coarvertnum - coarvertfrstnum + baseval].vertnum[i] - finevertfrstnum;
        for (fineedgenum = finegrafptr->vertloctax[finevertnum + baseval];
             fineedgenum < finegrafptr->vendloctax[finevertnum + baseval]; fineedgenum ++) {
          Gnum                coarvertend;        /* Number of coarse vertex which is end of fine edge */
          Gnum                h;

          coarvertend = finecoartax[finegrafptr->edgegsttax[fineedgenum]];

#ifdef SCOTCH_DEBUG_DGRAPH2
          if (coarvertend < 0) {
            errorPrint ("dgraphCoarsenEdge: external error (1)");
            return ;
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

          if (coarvertend != coarvertnum) {       /* If not end of collapsed edge */
#ifdef SCOTCH_DEBUG_DGRAPH2
            int count = 0;
#endif /* SCOTCH_DEBUG_DGRAPH2 */
            for (h = (coarvertend * COARHASHPRIME) & coarhashmsk; ; h = (h + 1) & coarhashmsk) {
#ifdef SCOTCH_DEBUG_DGRAPH2
              count ++;
              if (count == coarhashmsk) {
                errorPrint ("dgraphCoarsenEdge: internal error (1)");
                exit (1);
              }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
              if (coarhashtab[h].vertorgnum != coarvertnum) { /* If old slot           */
#ifdef SCOTCH_DEBUG_DGRAPH2
                if (coaredgenum >= coargrafptr->edgelocnbr + coargrafptr->baseval) {
                  errorPrint ("dgraphCoarsenEdge: internal error (2)");
                  return ;
                }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
                coarhashtab[h].vertorgnum = coarvertnum; /* Mark it in reference array */
                coarhashtab[h].vertendnum = coarvertend;
                coarhashtab[h].edgenum    = coaredgenum;
                coargrafptr->edgeloctax [coaredgenum] = coarvertend; /* One more edge created      */
                if (finegrafptr->edloloctax != NULL) /* Initialize edge load                    */
                  coargrafptr->edloloctax[coarhashtab[h].edgenum] =
                    finegrafptr->edloloctax [fineedgenum];
                else /* no weight in finer graph */
                  coargrafptr->edloloctax[coarhashtab[h].edgenum] = 1;

                coaredgenum ++;
                break;                            /* Give up hashing */
              }
              if (coarhashtab[h].vertendnum == coarvertend) { /* If coarse edge already exists */
                if (finegrafptr->edloloctax != NULL) /* Accumulate edge load                    */
                  coargrafptr->edloloctax[coarhashtab[h].edgenum] +=
                    finegrafptr->edloloctax [fineedgenum];
                else                              /* no weight in finer graph */
                  coargrafptr->edloloctax[coarhashtab[h].edgenum] += 1;
                break;                            /* Give up hashing                         */
              }
            }
          }
        }
      }
      finevertnum = coarmulttax[coarvertnum - coarvertfrstnum + baseval].vertnum[i];
    } while (i ++, finevertnum != coarmulttax[coarvertnum - coarvertfrstnum + baseval].vertnum[1]);
    if (coardegrmax < (coaredgenum - coargrafptr->vertloctax[coarvertnum - coarvertfrstnum + baseval]))
      coardegrmax = coaredgenum - coargrafptr->vertloctax[coarvertnum - coarvertfrstnum + baseval];
    coargrafptr->vertloctax [coarvertnum + 1 - coarvertfrstnum + baseval] = coaredgenum ;
  }
  coargrafptr->vendloctax [coargrafptr->vertlocnnd - 1] = coaredgenum ;
  coargrafptr->degrglbmax = coardegrmax;

}


Gnum
dgraphCoarsenSharedInit (DgraphCoarsenShared * restrict shared,
                         const Dgraph * const           finegrafptr,
                         Dgraph * const                 coargrafptr)
{
  Gnum procglbnbr;

  shared->finegrafptr = finegrafptr;
  shared->coargrafptr = coargrafptr;

  procglbnbr = finegrafptr->procglbnbr;
  if (memAllocGroup ((void **)
                     &shared->coarproctab,  (size_t) (procglbnbr * sizeof (DgraphCoarsenMulti*)),
                     &shared->vertproctab,  (size_t) (procglbnbr * sizeof (DgraphCoarsenVert*)),
                     NULL) == NULL) {
    errorPrint ("dgraphCoarsenSharedInit: out of memory (1)");
    return (1);
  }
  shared->vertproctab[0] = NULL;

  return (0);
}

void
dgraphCoarsenSharedExit (DgraphCoarsenShared * shared)
{
  memFree (shared->coarproctab);
}

Gnum
dgraphCoarsenThreadConstruct (DgraphCoarsenShared * restrict shared)
{
  const Dgraph *               finegrafptr;      /* Alias to shared->finegrafptr    */
  Dgraph *                     coargrafptr;      /* Shortcut to shared->coargrafptr */
  Gnum                         coarhashnbr;      /* Size of the hash table                   */
  Gnum                         coarhashmsk;      /* Mask for access to hash table            */
  DgraphCoarsenHash * restrict coarhashtab;      /* Table of edges to other multinodes       */
  int                          procnum;
  int                          procglbnbr;
  Gnum                         finevertfrstnum;
  int                          proclocnum;
  Gnum                         coarvertnum;
  Gnum                         finevertnum;
  Gnum                       * finecoartax;
  int                        * finefoldtax;
  DgraphCoarsenMulti *         coarmulttax;
  Gnum                         vertlocnbr;
  Gnum                         edgelocnbr;
  size_t                       coarvelooftval;
  Gnum *                       oldedloloctab;
  Gnum                         baseval;

#ifdef SCOTCH_DEBUG_DGRAPH2
  Gnum                         count;
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  finegrafptr = shared->finegrafptr;
  coargrafptr = shared->coargrafptr;
  procglbnbr  = finegrafptr->procglbnbr;
  proclocnum  = finegrafptr->proclocnum;
  coarmulttax = shared->coarmulttax;
  baseval     = finegrafptr->baseval;
  finecoartax = shared->finecoartax;
  finefoldtax = shared->finefoldtax;

  vertlocnbr = coargrafptr->vertlocnbr;           /* Save numbers of vertices and edges */
  edgelocnbr = coargrafptr->edgelocnbr;

  memSet (coargrafptr, 0, sizeof (Dgraph));       /* Initialize coarse graph */
  coargrafptr->baseval    = finegrafptr->baseval;
  coargrafptr->vertlocnbr = vertlocnbr;
  coargrafptr->edgelocnbr = edgelocnbr;
  coargrafptr->vertgstnbr = coargrafptr->vertlocnbr + (finegrafptr->vertgstnbr - finegrafptr->vertlocnbr); /* Upper bound on number of ghosts */
  coargrafptr->vertgstnnd = coargrafptr->vertgstnbr + coargrafptr->baseval;
  coargrafptr->vertlocnnd = coargrafptr->vertlocnbr + coargrafptr->baseval;
  coargrafptr->veloglbsum = finegrafptr->veloglbsum;
  coargrafptr->proccomm   = finegrafptr->proccomm;
  coargrafptr->procglbnbr = finegrafptr->procglbnbr;
  coargrafptr->proclocnum = finegrafptr->proclocnum;

  for (coarhashmsk = 31 ; coarhashmsk < finegrafptr->degrglbmax * 2 - 2 ; /* Maximum degree we can have in coargraph */
       coarhashmsk = coarhashmsk * 2 + 1) ;

  coarhashmsk = coarhashmsk * 4 + 3;
  coarhashnbr = coarhashmsk + 1;

  if (memAllocGroup ((void **)
                     &coargrafptr->vertloctax, (size_t) ((coargrafptr->vertlocnbr + 1) * sizeof (Gnum)), /* +1 for compact representation */
                     &coargrafptr->veloloctax, (size_t) (coargrafptr->vertlocnbr * sizeof (Gnum)),
                     &coarhashtab,             (size_t) (coarhashnbr * sizeof (DgraphCoarsenHash)), NULL) == NULL) {
    errorPrint ("dgraphCoarsenThreadInit: out of memory (1)");
    return (1);
  }

  if ((coargrafptr->edgeloctax = memAlloc (coargrafptr->edgelocnbr * sizeof (Gnum))) == NULL) {
    errorPrint ("dgraphCoarsenThreadInit: out of memory (2)");
    memFree (coargrafptr->vertloctax);
    return (1);
  }

  if ((coargrafptr->edloloctax = memAlloc (coargrafptr->edgelocnbr * sizeof (Gnum))) == NULL) {
    errorPrint ("dgraphCoarsenThreadInit: out of memory (3)");
    memFree (coargrafptr->edgeloctax);
    memFree (coargrafptr->vertloctax);
    return (1);
  }

  if (memAllocGroup ((void **)
                     &coargrafptr->procdsptab, (size_t) ((coargrafptr->procglbnbr + 1) * sizeof (int)),
                     &coargrafptr->proccnttab, (size_t) (coargrafptr->procglbnbr       * sizeof (int)),
                     &coargrafptr->procngbtab, (size_t) (coargrafptr->procglbnbr       * sizeof (int)),
                     &coargrafptr->procrcvtab, (size_t) (coargrafptr->procglbnbr       * sizeof (int)),
                     &coargrafptr->procsndtab, (size_t) (coargrafptr->procglbnbr       * sizeof (int)), NULL) == NULL) {
    errorPrint ("dgraphCoarsenThreadInit: out of memory (4)");
    memFree (coargrafptr->edloloctax);
    memFree (coargrafptr->edgeloctax);
    memFree (coargrafptr->vertloctax);
    return (1);
  }
  coargrafptr->procvrttab = coargrafptr->procdsptab; /* Graph does not have holes */

  coargrafptr->vertloctax -= coargrafptr->baseval;
  coargrafptr->veloloctax -= coargrafptr->baseval;
  coargrafptr->edgeloctax -= coargrafptr->baseval;
  coargrafptr->edloloctax -= coargrafptr->baseval;
  coargrafptr->vendloctax = coargrafptr->vertloctax + 1;

  coargrafptr->procdsptab[0] = (int)coargrafptr->vertlocnbr; /* Trick to allow the following communication with the good type */
  MPI_Allgather (&coargrafptr->procdsptab[0], 1, MPI_INT, coargrafptr->proccnttab, 1, MPI_INT, finegrafptr->proccomm);
  coargrafptr->procdsptab[0] = coargrafptr->baseval; /* Put the true value of procdsptab[0] */
  for (procnum = 0; procnum < procglbnbr; procnum ++)
    coargrafptr->procdsptab[procnum + 1] = coargrafptr->procdsptab[procnum] + coargrafptr->proccnttab[procnum];


  if (finegrafptr->procglbnbr > 1) { /* Launch the other functions in case of non multi threaded coarsening */

    for (finevertnum = finegrafptr->baseval ;       /* Update finecoartax with global numbers */
         finevertnum < finegrafptr->vertlocnnd ; ++ finevertnum) {
      finecoartax[finevertnum] += coargrafptr->procdsptab[finefoldtax[finevertnum]] - finegrafptr->baseval;
    }

    dgraphCoarsenThreadMigrate (shared);
  }

  /*
   * Construct coarsened graph
   */
  finevertfrstnum        = finegrafptr->procdsptab[proclocnum];


  memSet (coarhashtab, ~0, coarhashnbr * sizeof (DgraphCoarsenHash));

  dgraphCoarsenEdge (shared, coarmulttax, coarhashtab, coarhashmsk);
  coargrafptr->edgelocnbr = coargrafptr->vendloctax[coargrafptr->vertlocnnd-1]
    - coargrafptr->baseval; /* Set exact number of edges */

#ifdef SCOTCH_DEBUG_DGRAPH2
  count = 0;
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  /* Now update the load of vertices */
  if (finegrafptr->veloloctax != NULL) {             /* If fine graph is weighted */
    for (coarvertnum = coargrafptr->baseval; coarvertnum < coargrafptr->vertlocnnd; coarvertnum ++) {
      Gnum                coarveloval;
      Gnum                offset;
      int                 i;
      Gnum                vertother;

#ifdef SCOTCH_DEBUG_DGRAPH2
      count++;
#endif /* SCOTCH_DEBUG_DGRAPH2 */

      for (i = 0, vertother = -1, coarveloval = 0 ; i < 2 ; ++i) {
        if (coarmulttax[coarvertnum].vertnum[i] == vertother)
          continue;                       /* Not a coars vertex */
        if (i == 0)
          vertother = coarmulttax[coarvertnum].vertnum[0];
        else
          vertother = -1;

        if (dgraphVertexLocal (finegrafptr, coarmulttax[coarvertnum].vertnum[i]))
           /* We have the weight */
          coarveloval += finegrafptr->veloloctax[coarmulttax[coarvertnum].vertnum[i] - finevertfrstnum + baseval];
        else { /* Vertex was not on this proc, wait for reception */
          procnum = dgraphVertexProc (finegrafptr, coarmulttax[coarvertnum].vertnum[i]);
          offset = dgraphCoarsenSearchVert (shared, procnum, coarmulttax[coarvertnum].vertnum[i]);
          coarveloval += shared->vertproctab[procnum][offset].weight;
        }
      }
      coargrafptr->veloloctax[coarvertnum] = coarveloval;
      coargrafptr->velolocsum += coarveloval; /* Computes velolocsum */
    }
  }
  else {                                          /* Fine graph is not weighted */
    for (coarvertnum = coargrafptr->baseval; coarvertnum < coargrafptr->vertlocnnd; coarvertnum ++) {
#ifdef SCOTCH_DEBUG_DGRAPH2
      count++;
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      coargrafptr->veloloctax[coarvertnum] =
        (coarmulttax[coarvertnum].vertnum[0] != coarmulttax[coarvertnum].vertnum[1]) ? 2 : 1;
      coargrafptr->velolocsum += coargrafptr->veloloctax[coarvertnum]; /* Update velolocsum field */
    }
  }

#ifdef SCOTCH_DEBUG_DGRAPH2
  if (count != coargrafptr->vertlocnbr) {
    errorPrint("dgraphCoarsenThreadConstruct: internal error (1)");
    memFree (shared->finecoartax + finegrafptr->baseval);
    memFree (shared);
    return (2);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  /* Reallocate results */

  coarvelooftval = coargrafptr->veloloctax - coargrafptr->vertloctax;
  oldedloloctab = coargrafptr->edloloctax + coargrafptr->baseval;

  if (memReallocGroup ((void *) (coargrafptr->vertloctax + coargrafptr->baseval),
                       &coargrafptr->vertloctax, (size_t) ((coargrafptr->vertlocnbr + 1) * sizeof (Gnum)), /* +1 for compact representation */
                       &coargrafptr->veloloctax, (size_t) (coargrafptr->vertlocnbr       * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("dgraphCoarsenThreadInit: out of memory (4)");
    memFree    (coargrafptr->edloloctax + coargrafptr->baseval);
    return     (1);
  }
  if (memReallocGroup ((void *) (coargrafptr->edgeloctax + coargrafptr->baseval),
                       &coargrafptr->edgeloctax, (size_t) (coargrafptr->edgelocnbr * sizeof (Gnum)),
                       &coargrafptr->edloloctax, (size_t) (coargrafptr->edgelocnbr * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("dgraphCoarsenThreadInit: out of memory (5)");
    memFree (coargrafptr->edloloctax + coargrafptr->baseval);
    memFree (coargrafptr->vertloctax);
    return (1);
  }


  coargrafptr->vertloctax -= coargrafptr->baseval;
  coargrafptr->veloloctax = coargrafptr->vertloctax + coarvelooftval;
  coargrafptr->edgeloctax -= coargrafptr->baseval;
  coargrafptr->edloloctax -= coargrafptr->baseval;
  coargrafptr->edgegsttax = NULL;
  coargrafptr->vendloctax = coargrafptr->vertloctax + 1;

  memCpy (coargrafptr->edloloctax + coargrafptr->baseval, oldedloloctab, sizeof(Gnum) * coargrafptr->edgelocnbr);
  memFree (oldedloloctab);

  coargrafptr->flagval = DGRAPHEDGEGROUP | DGRAPHVERTGROUP | DGRAPHFREETABS;

  if ((finegrafptr->procglbnbr > 1) &&  (shared->requesttab = NULL)) {
    if (finegrafptr->edloloctax == NULL)            /* Make sure sends are done */
      MPI_Waitall (4 * finegrafptr->procglbnbr, shared->requesttab, MPI_STATUSES_IGNORE);
    else
      MPI_Waitall (6 * finegrafptr->procglbnbr, shared->requesttab, MPI_STATUSES_IGNORE);
  }

  if (shared->vertproctab[0] != NULL)
    memFree (shared->vertproctab[0]);

  return (0);
}


Gnum
dgraphCoarsenThreadMigrate (DgraphCoarsenShared * restrict shared)
{
  Dgraph * restrict              finegrafptr;
  Dgraph * restrict              coargrafptr;
  int                          * finefoldtax;
  Gnum                         * finecoartax;
  Gnum                           verttosndnbr;
  Gnum                           verttorcvnbr;
  Gnum                           edgetosndnbr;
  Gnum                           edgetorcvnbr;
  int                            procglbnbr;
  int                            procnum;
  DgraphCoarsenVert * restrict * verttab;
  Gnum * restrict                posedgetab;
  Gnum * restrict *              edgetab;
  Gnum * restrict *              edlotab;
  DgraphCoarsenVert *            vertsndtab;
  DgraphCoarsenVert *            vertrcvtab;
  Gnum *                         edgesndtab;
  Gnum *                         edgercvtab;
  Gnum *                         edlosndtab;
  Gnum *                         edlorcvtab;
  Gnum                           finevertnum;

  MPI_Aint                       disp[5];  /* 4 elements in structure + end mark */
  MPI_Datatype                   type[5] = {GNUM_MPI, GNUM_MPI, GNUM_MPI, GNUM_MPI, MPI_UB};
  int                            blocklen[5] = {1, 1, 1, 1, 1};
  MPI_Datatype                   dgraphCoarsenVertType;
  int                            i;
  int                            requestsnbr;
  Gnum                           finevertfrstnum;
  Gnum                           baseval;
  Gnum *                         vertptr;

  finegrafptr = shared->finegrafptr;
  coargrafptr = shared->coargrafptr;
  finefoldtax = shared->finefoldtax;
  finecoartax = shared->finecoartax;
  procglbnbr  = finegrafptr->procglbnbr;
  finevertfrstnum = finegrafptr->procdsptab[finegrafptr->proclocnum];
  baseval = finegrafptr->baseval;

  dgraphHaloSync (finegrafptr, (byte*) (finecoartax + finegrafptr->baseval), GNUM_MPI); /* Spread finecoartax with global vertex numbers */

  for (procnum = 0, verttosndnbr = verttorcvnbr = edgetosndnbr = edgetorcvnbr = 0 ;
       procnum < finegrafptr->procglbnbr ; ++ procnum) { /* Compute memory needed */
    verttosndnbr += shared->vertsnd[procnum];
    verttorcvnbr += shared->vertrcv[procnum];
    edgetosndnbr += shared->edgesnd[procnum];
    edgetorcvnbr += shared->edgercv[procnum];

#ifdef SCOTCH_DEBUG_DGRAPH2
    if ((shared->edgesnd[procnum] > (shared->vertsnd[procnum] * finegrafptr->degrglbmax))
        ||(shared->edgercv[procnum] > (shared->vertrcv[procnum] * finegrafptr->degrglbmax))) {
      errorPrint ("dgraphCoarsenThreadMigrate: external error (1)");
      return (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  }

#ifdef SCOTCH_DEBUG_DGRAPH2
  if ((edgetosndnbr > (verttosndnbr * finegrafptr->degrglbmax))
      ||(edgetorcvnbr > (verttorcvnbr * finegrafptr->degrglbmax))) {
    errorPrint ("dgraphCoarsenThreadMigrate: external error (2)");
    return (1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  if ((verttosndnbr == 0) && (verttorcvnbr == 0)) {
    shared->requesttab = NULL;                    /* Mark requesttab as non allocated */
    return (0);
  }

  if (finegrafptr->edloloctax != NULL) {
    requestsnbr = 6 * procglbnbr;
    memAllocGroup ((void*)
                   &vertrcvtab, (size_t) verttorcvnbr * sizeof (DgraphCoarsenVert),
                   &edgercvtab, (size_t) edgetorcvnbr * sizeof (Gnum),
                   &edlorcvtab, (size_t) edgetorcvnbr * sizeof (Gnum),
                   &shared->edgeproctab, (size_t) procglbnbr * sizeof (Gnum *),
                   &shared->edloproctab, (size_t) procglbnbr * sizeof (Gnum *),
                   &vertsndtab, (size_t) verttosndnbr * sizeof (DgraphCoarsenVert),
                   &edgesndtab, (size_t) edgetosndnbr * sizeof (Gnum),
                   &edlosndtab, (size_t) edgetosndnbr * sizeof (Gnum),
                   &verttab,    (size_t) procglbnbr * sizeof (DgraphCoarsenVert*),
                   &edgetab,    (size_t) procglbnbr * sizeof (Gnum*),
                   &posedgetab, (size_t) procglbnbr * sizeof (Gnum),
                   &edlotab,    (size_t) procglbnbr * sizeof (Gnum*),
                   &shared->requesttab,   (size_t) requestsnbr * sizeof (MPI_Request),
                   NULL);
  }
  else {                                          /* No edge loads */
    requestsnbr = 4 * procglbnbr;
    memAllocGroup ((void*)
                   &vertrcvtab, (size_t) verttorcvnbr * sizeof (DgraphCoarsenVert),
                   &edgercvtab, (size_t) edgetorcvnbr * sizeof (Gnum),
                   &shared->edgeproctab, (size_t) procglbnbr * sizeof (Gnum *),
                   &vertsndtab, (size_t) verttosndnbr * sizeof (DgraphCoarsenVert),
                   &edgesndtab, (size_t) edgetosndnbr * sizeof (Gnum),
                   &verttab,    (size_t) procglbnbr * sizeof (DgraphCoarsenVert *),
                   &edgetab,    (size_t) procglbnbr * sizeof (Gnum *),
                   &posedgetab, (size_t) procglbnbr * sizeof (Gnum),
                   &shared->requesttab,   (size_t) requestsnbr * sizeof (MPI_Request),
                   NULL);
#ifdef SCOTCH_DEBUG_DGRAPH2
    edlosndtab = NULL;
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  }
  if (vertrcvtab == NULL) {
    errorPrint ("dgraphCoarsenThreadMigrate: out of memory (1)");
    return     (1);
  }

  memSet (posedgetab, 0, procglbnbr * sizeof(Gnum));

#ifdef SCOTCH_DEBUG_DGRAPH2
  memSet (vertrcvtab, ~0, (size_t) verttorcvnbr * sizeof (DgraphCoarsenVert));
  memSet (edgercvtab, ~0, (size_t) edgetorcvnbr * sizeof (Gnum));
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  /* Create Datatype for communications */
  MPI_Address (&vertsndtab->vertnum,   disp);
  MPI_Address (&vertsndtab->edgenbr,   disp + 1);
  MPI_Address (&vertsndtab->edgebegin, disp + 2);
  MPI_Address (&vertsndtab->weight,    disp + 3);
  MPI_Address (vertsndtab + 1,         disp + 4);

  for (i = 4; i >= 0; i --)                       /* Compute relative addresses */
    disp[i] -= disp[0];

  MPI_Type_struct (5, blocklen, disp, type, &dgraphCoarsenVertType);
  MPI_Type_commit (&dgraphCoarsenVertType);

  shared->vertproctab[0] = vertrcvtab;
  shared->edgeproctab[0] = edgercvtab;
  if (finegrafptr->edloloctax != NULL) {
    shared->edloproctab[0] = edlorcvtab;
    edlotab[0] = edlosndtab;
  }
  else {
    shared->edloproctab = NULL;
    edlotab = NULL;
  }
  verttab[0] = vertsndtab;
  edgetab[0] = edgesndtab;

  for (procnum = 1; procnum < finegrafptr->procglbnbr; procnum ++) { /* Initialize table indices */
    shared->vertproctab[procnum] = shared->vertproctab[procnum - 1] + shared->vertrcv[procnum - 1];
    verttab[procnum] = verttab[procnum - 1] + shared->vertsnd[procnum - 1];
    shared->edgeproctab[procnum] = shared->edgeproctab[procnum - 1] + shared->edgercv[procnum - 1];
    edgetab[procnum] = edgetab[procnum - 1] + shared->edgesnd[procnum - 1];
    if (finegrafptr->edloloctax != NULL) {
      shared->edloproctab[procnum] = shared->edloproctab[procnum - 1] + shared->edgercv[procnum - 1];
      edlotab[procnum] = edlotab[procnum - 1] + shared->edgesnd[procnum - 1];
    }
  }

  for (procnum = 0; procnum < finegrafptr->procglbnbr; procnum ++) { /* Initialize receive requests */
    if (shared->vertrcv[procnum] == 0) {/* No vertices to receive */
      shared->requesttab[DGRAPHCOARSENVERTRCV(procnum, procglbnbr)] =
        shared->requesttab[DGRAPHCOARSENEDGERCV(procnum, procglbnbr)] = MPI_REQUEST_NULL;
      if (finegrafptr->edloloctax != NULL)
        shared->requesttab[DGRAPHCOARSENEDLORCV(procnum, procglbnbr)] = MPI_REQUEST_NULL;
      continue;  /* Go to next processor */
    }
    MPI_Irecv (shared->vertproctab[procnum], shared->vertrcv[procnum], /* Receive vertices */
               dgraphCoarsenVertType, procnum, TAGCOARVERT, finegrafptr->proccomm, &shared->requesttab[DGRAPHCOARSENVERTRCV(procnum, procglbnbr)]);
    MPI_Irecv (shared->edgeproctab[procnum], shared->edgercv[procnum], /* Receive edges */
               GNUM_MPI, procnum, TAGCOAREDGE,finegrafptr->proccomm, &shared->requesttab[DGRAPHCOARSENEDGERCV(procnum, procglbnbr)]);

    if (finegrafptr->edloloctax != NULL) {
      MPI_Irecv (shared->edloproctab[procnum], shared->edgercv[procnum], /* Receive edges load */
                 GNUM_MPI, procnum, TAGCOAREDLO,finegrafptr->proccomm, &shared->requesttab[DGRAPHCOARSENEDLORCV(procnum, procglbnbr)]);
    }
  }

  for (procnum = 0 ; procnum < finegrafptr->procglbnbr ; ++ procnum) { /* Initialize send shared->requesttab */
    /* No buffer already */
    shared->requesttab[DGRAPHCOARSENVERTSND(procnum, procglbnbr)] =
      shared->requesttab[DGRAPHCOARSENEDGESND(procnum, procglbnbr)] = MPI_REQUEST_NULL ;
    if (finegrafptr->edloloctax != NULL)
      shared->requesttab[DGRAPHCOARSENEDLOSND(procnum, procglbnbr)] = MPI_REQUEST_NULL ;
  }

  /*
   * Select vertices and edges to be sent
   */
  for (procnum = 0, vertptr = shared->verttosndtab; procnum < finegrafptr->procglbnbr; procnum ++) { /* Loop on neighbors */
    Gnum                vertpos;

    if (shared->vertsnd[procnum] > 0)
      intSort1asc1 (vertptr, shared->vertsnd[procnum]); /* To allow the search functions to work */
    for (vertpos = 0; vertpos < shared->vertsnd[procnum]; vertpos ++) {
      Gnum *              edgeptr;
      Gnum *              edgennd;
      Gnum *              comedgeptr;
      DgraphCoarsenVert * coarsenvert;

      finevertnum = vertptr[vertpos];

#ifdef SCOTCH_DEBUG_DGRAPH2
      if (finefoldtax[finevertnum] != procnum) {
        errorPrint ("dgraphCoarsenThreadMigrate: internal error (3b)");
        return (1);
      }
      if (vertpos >= shared->vertsnd[procnum]) {
        errorPrint ("dgraphCoarsenThreadMigrate: internal error (3)");
        return (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

      /* We must send this vertex */
      coarsenvert = verttab[procnum] + vertpos;
      coarsenvert->vertnum = finevertnum + finegrafptr->procdsptab[finegrafptr->proclocnum] - baseval;
      coarsenvert->edgenbr = finegrafptr->vendloctax[finevertnum] - finegrafptr->vertloctax [finevertnum];
      coarsenvert->weight = (finegrafptr->veloloctax != NULL)?finegrafptr->veloloctax[finevertnum]:1;
      coarsenvert->edgebegin = posedgetab[procnum];

      /* and its edges */
      for (edgeptr = finegrafptr->edgegsttax + finegrafptr->vertloctax[finevertnum],
             edgennd = finegrafptr->edgegsttax + finegrafptr->vendloctax[finevertnum],
             comedgeptr = edgetab[procnum] + coarsenvert->edgebegin ;
           edgeptr < edgennd ;
           ++ edgeptr, ++ comedgeptr) {

        *comedgeptr = finecoartax[*edgeptr];
#ifdef SCOTCH_DEBUG_DGRAPH2
        if (*comedgeptr < 0) {
          errorPrint ("dgraphCoarsenThreadMigrate: internal error (4)");
          return (1);
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      }

      if (finegrafptr->edloloctax != NULL) /* Copy edges' weight */
        memCpy (edlotab[procnum] + coarsenvert->edgebegin,
                finegrafptr->edloloctax + finegrafptr->vertloctax[finevertnum],
                coarsenvert->edgenbr * sizeof (Gnum));

#ifdef SCOTCH_DEBUG_DGRAPH2
      if (comedgeptr - (edgetab[procnum] + posedgetab[procnum] ) != coarsenvert->edgenbr) {
        errorPrint ("dgraphCoarsenThreadMigrate: internal error (5)");
        return (1);
      }
#endif
      posedgetab[procnum] += coarsenvert->edgenbr;

#ifdef SCOTCH_DEBUG_DGRAPH2
      if (posedgetab[procnum] > shared->edgesnd[procnum]) {
        errorPrint ("dgraphCoarsenThreadMigrate: internal error (6)");
        return (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
    }
    if (shared->vertsnd[procnum] > 0) {
      int flag;
      MPI_Isend(verttab[procnum], shared->vertsnd[procnum], dgraphCoarsenVertType, procnum, TAGCOARVERT,
               finegrafptr->proccomm, &shared->requesttab[DGRAPHCOARSENVERTSND(procnum, procglbnbr)]);        /* Send vertices */
      MPI_Test (&shared->requesttab[DGRAPHCOARSENVERTSND(procnum, procglbnbr)], &flag, MPI_STATUS_IGNORE);         /* Validate send */
      MPI_Isend(edgetab[procnum], posedgetab[procnum], GNUM_MPI, procnum, TAGCOAREDGE,
               finegrafptr->proccomm, &shared->requesttab[DGRAPHCOARSENEDGESND(procnum, procglbnbr)]);      /* Send edges */
      MPI_Test (&shared->requesttab[DGRAPHCOARSENEDGESND(procnum, procglbnbr)], &flag, MPI_STATUS_IGNORE);         /* Validate send */
      if (finegrafptr->edloloctax != NULL) {       /* Send edge loads if any */
        MPI_Isend(edlotab[procnum], posedgetab[procnum], GNUM_MPI, procnum, TAGCOAREDLO,
                 finegrafptr->proccomm, &shared->requesttab[DGRAPHCOARSENEDLOSND(procnum, procglbnbr)]);
        MPI_Test (&shared->requesttab[DGRAPHCOARSENEDLOSND(procnum, procglbnbr)], &flag, MPI_STATUS_IGNORE);         /* Validate send */
      }
    }
    vertptr += finegrafptr->procrcvtab[procnum];  /* Point on next interesting vertex */
  }

/*   MPI_Waitall (requestsnbr, shared->requesttab, MPI_STATUSES_IGNORE); */

  MPI_Type_free (&dgraphCoarsenVertType);

  return (0);
}
