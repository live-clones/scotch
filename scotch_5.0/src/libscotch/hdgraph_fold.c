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
/**   NAME       : hdgraph_fold.c                          **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the halo distribu-  **/
/**                ted graph folding function.             **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 23 apr 2006     **/
/**                                 to   : 06 sep 2006     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HDGRAPH

#include "module.h"
#include "common.h"
#include "dgraph.h"
#include "dgraph_fold_comm.h"
#include "hdgraph.h"
#include "hdgraph_fold.h"

/******************************/
/*                            */
/* These routines handle halo */
/* distributed source graphs. */
/*                            */
/******************************/

/* This routine builds a folded graph by
** merging graph data to the processes of
** the first half or to the second half
** of the communicator.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
hdgraphFold (
const Hdgraph * restrict const  orggrafptr,
const int                       partval,          /* 0 for first half, 1 for second half */
Hdgraph * restrict const        fldgrafptr)
{
  int               fldprocglbnbr;
  int               fldproclocnum;                /* Index of local process in folded communicator   */
  int               fldproccol;                   /* Color of receiver or not wanted in communicator */
  MPI_Comm          fldproccomm;                  /* Communicator of folded part                     */

  fldprocglbnbr = (orggrafptr->s.procglbnbr + 1) / 2;
  if (partval == 1) {
    fldproclocnum = orggrafptr->s.proclocnum - fldprocglbnbr;
    fldprocglbnbr = orggrafptr->s.procglbnbr - fldprocglbnbr; 
  }
  else
    fldproclocnum = orggrafptr->s.proclocnum;

  fldproccol = ((fldproclocnum >= 0) && (fldproclocnum < fldprocglbnbr)) ? 0 : MPI_UNDEFINED;

  if (MPI_Comm_split (orggrafptr->s.proccomm, fldproccol, fldproclocnum, &fldproccomm) != MPI_SUCCESS) {
    errorPrint ("hdgraphFold: communication error");
    return     (1);
  }

  return (hdgraphFold2 (orggrafptr, partval, fldgrafptr, fldproccomm));
}

int
hdgraphFold2 (
const Hdgraph * restrict const  orggrafptr,
const int                       partval,          /* 0 for first half, 1 for second half */
Hdgraph * restrict const        fldgrafptr,
MPI_Comm                        fldproccomm)      /* Pre-computed communicator */
{
  int                   fldcommtypval;            /* Type of communication for this process                 */
  DgraphFoldCommData    fldcommdattab[DGRAPHFOLDCOMMNBR]; /* Array of two communication data                */
  Gnum                  fldcommvrttab[DGRAPHFOLDCOMMNBR]; /* Starting global send indices of communications */
  Gnum                  fldvertidxtab[DGRAPHFOLDCOMMNBR]; /* Start indices of remote vertex arrays          */
  Gnum                  fldvertlocnbr;            /* Number of vertices in local folded part                */
  Gnum                  fldedgelocsiz;            /* (Upper bound of) number of edges in folded graph       */
  int                   fldprocglbnbr;
  int                   fldproclocnum;            /* Index of local process in folded communicator          */
  int                   fldvertadjnbr;
  Gnum * restrict       fldvertadjtab;            /* Array of index adjustments for original vertices       */
  Gnum * restrict       fldvertdlttab;            /* Array of index adjustments for original vertices       */
  Gnum * restrict       fldvhalloctax;            /* Index array for remote halo vertex renumbering         */
  int                   cheklocval;
  int                   chekglbval;
  int                   requnbr;
  MPI_Request           requtab[HDGRAPHFOLDTAGNBR * DGRAPHFOLDCOMMNBR];
  MPI_Status            stattab[HDGRAPHFOLDTAGNBR * DGRAPHFOLDCOMMNBR];

#ifdef SCOTCH_DEBUG_HDGRAPH1
  if (orggrafptr->vhndloctax != (orggrafptr->s.vertloctax + 1)) {
    errorPrint ("hdgraphFold2: halo graph must be compact");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_HDGRAPH1 */

  fldprocglbnbr = (orggrafptr->s.procglbnbr + 1) / 2;
  if (partval == 1) {
    fldproclocnum = orggrafptr->s.proclocnum - fldprocglbnbr;
    fldprocglbnbr = orggrafptr->s.procglbnbr - fldprocglbnbr; 
  }
  else
    fldproclocnum = orggrafptr->s.proclocnum;

  fldcommtypval = ((fldproclocnum >= 0) && (fldproclocnum < fldprocglbnbr)) ? DGRAPHFOLDCOMMRECV : DGRAPHFOLDCOMMSEND;

  cheklocval = 0;
  if (fldcommtypval == DGRAPHFOLDCOMMRECV) {      /* If we are going to receive */
#ifdef SCOTCH_DEBUG_HDGRAPH2
    if (fldgrafptr == NULL) {
      errorPrint ("hdgraphFold2: invalid parameters (1)");
      return     (1);
    }
    if (fldproccomm == MPI_COMM_NULL) {
      errorPrint ("hdgraphFold2: invalid parameters (2)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

    memSet (fldgrafptr, 0, sizeof (Hdgraph));     /* Pre-initialize graph fields */

    fldgrafptr->s.proccomm   = fldproccomm;
    fldgrafptr->s.procglbnbr = fldprocglbnbr;
    fldgrafptr->s.proclocnum = fldproclocnum;
    fldgrafptr->s.flagval    = DGRAPHFREETABS | DGRAPHVERTGROUP | DGRAPHEDGEGROUP | DGRAPHFREECOMM; /* For premature freeing on error; do not free vhndloctab as it is grouped with vertloctab */

    if (memAllocGroup ((void **)                  /* Allocate distributed graph private data */
                       &fldvertadjtab, (size_t) (orggrafptr->s.procglbnbr * DGRAPHFOLDCOMMNBR * sizeof (Gnum)),
                       &fldvertdlttab, (size_t) (orggrafptr->s.procglbnbr * DGRAPHFOLDCOMMNBR * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("hdgraphFold2: out of memory (1)");
      cheklocval = 1;
    }
    if (memAllocGroup ((void **)                  /* Allocate distributed graph private data */
                       &fldgrafptr->s.procdsptab, (size_t) ((fldprocglbnbr + 1) * sizeof (int)),
                       &fldgrafptr->s.proccnttab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->s.procngbtab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->s.procrcvtab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->s.procsndtab, (size_t) (fldprocglbnbr       * sizeof (int)), NULL) == NULL) {
      errorPrint ("hdgraphFold2: out of memory (2)");
      cheklocval = 1;
    }
    else if (dgraphFoldComm (&orggrafptr->s, partval, &fldcommtypval, fldcommdattab, fldcommvrttab, /* Process can become a sender receiver */
                             fldgrafptr->s.proccnttab, &fldvertadjnbr, fldvertadjtab, fldvertdlttab) != 0) {
      errorPrint ("hdgraphFold2: cannot compute folding communications (1)");
      cheklocval = 1;
    }
    else {
      Gnum              fldvelolocnbr;

      if ((fldcommtypval & DGRAPHFOLDCOMMSEND) == 0) { /* If process is a normal receiver */
        int               i;

        for (i = 0, fldvertlocnbr = orggrafptr->s.proccnttab[orggrafptr->s.proclocnum];
             (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++)
          fldvertlocnbr += fldcommdattab[i].vertnbr;
        fldedgelocsiz = orggrafptr->s.edgelocsiz + orggrafptr->s.edgeglbsmx * i; /* Upper bound */
      }
      else {                                      /* Process is a sender receiver */
        fldvertlocnbr = fldcommvrttab[0] - orggrafptr->s.procvrttab[orggrafptr->s.proclocnum]; /* Communications will remove vertices */
        fldedgelocsiz = orggrafptr->s.vertloctax[fldvertlocnbr + orggrafptr->s.baseval] - orggrafptr->s.baseval;
      }
      fldvelolocnbr = (orggrafptr->s.veloloctax != NULL) ? fldvertlocnbr : 0;

      if (memAllocGroup ((void **)                /* Allocate distributed graph public data */
                         &fldgrafptr->s.vertloctax, (size_t) ((fldvertlocnbr + DGRAPHFOLDCOMMNBR + 1) * sizeof (Gnum)), /* More slots for received vertex arrays  */
                         &fldgrafptr->s.vendloctax, (size_t) (fldvertlocnbr                           * sizeof (Gnum)), /* Vertex end array for non-halo vertices */
                         &fldgrafptr->s.vnumloctax, (size_t) (fldvertlocnbr                           * sizeof (Gnum)),
                         &fldgrafptr->s.veloloctax, (size_t) (fldvelolocnbr                           * sizeof (Gnum)), NULL) == NULL) {
        errorPrint ("hdgraphFold2: out of memory (3)");
        cheklocval = 1;
      }
      else if (fldgrafptr->s.vertloctax -= orggrafptr->s.baseval,
               fldgrafptr->s.vendloctax -= orggrafptr->s.baseval,
               fldgrafptr->s.vnumloctax -= orggrafptr->s.baseval,
               fldgrafptr->s.veloloctax = ((fldvelolocnbr != 0) ? fldgrafptr->s.veloloctax - orggrafptr->s.baseval : NULL),
               memAllocGroup ((void **)
                              &fldgrafptr->s.edgeloctax, (size_t) (fldedgelocsiz            * sizeof (Gnum)),
                              &fldvhalloctax,            (size_t) (orggrafptr->s.edgeglbsmx * sizeof (Gnum)), NULL) == NULL) {
        errorPrint ("hdgraphFold2: out of memory (4)");
        cheklocval = 1;
      }
      else {
        fldgrafptr->s.edgeloctax -= orggrafptr->s.baseval;
        fldvhalloctax            -= orggrafptr->s.baseval;
      }
    }
  }
  else {                                          /* Process is a sender */
#ifdef SCOTCH_DEBUG_HDGRAPH2
    if (fldproccomm != MPI_COMM_NULL) {
      errorPrint ("hdgraphFold2: invalid parameters (3)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

    if (dgraphFoldComm (&orggrafptr->s, partval, &fldcommtypval, fldcommdattab, fldcommvrttab, NULL, NULL, NULL, NULL) != 0) {
      errorPrint ("hdgraphFold2: cannot compute folding communications (2)");
      cheklocval = 1;
    }
  }

#ifdef SCOTCH_DEBUG_HDGRAPH1                      /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, orggrafptr->s.proccomm) != MPI_SUCCESS) {
    errorPrint ("hdgraphFold2: communication error (3)");
    chekglbval = 1;
  }
#else /* SCOTCH_DEBUG_HDGRAPH1 */
  chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_HDGRAPH1 */
  if (chekglbval != 0) {
    if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) {
      hdgraphExit (fldgrafptr);
      memFree     (fldvertadjtab);                /* Free group leader */
    }
    return (1);
  }

  if ((fldcommtypval & DGRAPHFOLDCOMMSEND) != 0) { /* If process is (also) a sender */
    Gnum              vertsndbas;
    Gnum              vertsndnbr;
    int               i;

    vertsndnbr = ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) ? (fldcommvrttab[0] - orggrafptr->s.procvrttab[orggrafptr->s.proclocnum]) : 0; /* If process is also a receiver, start sending after kept vertices */

    for (i = 0, requnbr = 0, vertsndbas = orggrafptr->s.baseval; /* For all send communications to perform */
         (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
      Gnum              edgelocbas;
      Gnum              edgelocsiz;
      int               procsndnum;               /* Rank of process to send to */

      vertsndbas += vertsndnbr;
      vertsndnbr  = fldcommdattab[i].vertnbr;
      edgelocbas  = orggrafptr->s.vertloctax[vertsndbas];
      edgelocsiz  = orggrafptr->s.vertloctax[vertsndbas + vertsndnbr] - edgelocbas;
      procsndnum  = fldcommdattab[i].procnum;

      if (MPI_Isend (orggrafptr->s.vertloctax + vertsndbas, vertsndnbr + 1, GNUM_MPI, /* Send one more vertex to have end value */
                     procsndnum, TAGVERTLOCTAB * orggrafptr->s.procglbnbr + orggrafptr->s.proclocnum,
                     orggrafptr->s.proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("hdgraphFold2: communication error (4,%d)", i);
        cheklocval = 1;
      }
      else if (MPI_Isend (orggrafptr->s.vendloctax + vertsndbas, vertsndnbr, GNUM_MPI,
                          procsndnum, TAGVENDLOCTAB * orggrafptr->s.procglbnbr + orggrafptr->s.proclocnum,
                          orggrafptr->s.proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("hdgraphFold2: communication error (5,%d)", i);
        cheklocval = 1;
      }
      else if (MPI_Isend (orggrafptr->s.edgeloctax + edgelocbas, edgelocsiz, GNUM_MPI,
                          procsndnum, TAGEDGELOCTAB * orggrafptr->s.procglbnbr + orggrafptr->s.proclocnum,
                          orggrafptr->s.proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("hdgraphFold2: communication error (6,%d)", i);
        cheklocval = 1;
      }
      else if ((orggrafptr->s.veloloctax != NULL) &&
               (MPI_Isend (orggrafptr->s.veloloctax + vertsndbas, vertsndnbr, GNUM_MPI,
                           procsndnum, TAGVELOLOCTAB * orggrafptr->s.procglbnbr + orggrafptr->s.proclocnum,
                           orggrafptr->s.proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("hdgraphFold2: communication error (7,%d)", i);
        cheklocval = 1;
      }
      else if ((orggrafptr->s.vnumloctax != NULL) &&
               (MPI_Isend (orggrafptr->s.vnumloctax + vertsndbas, vertsndnbr, GNUM_MPI,
                           procsndnum, TAGVNUMLOCTAB * orggrafptr->s.procglbnbr + orggrafptr->s.proclocnum,
                           orggrafptr->s.proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("hdgraphFold2: communication error (8,%d)", i);
        cheklocval = 1;
      }
    }

    if ((fldcommtypval & DGRAPHFOLDCOMMRECV) == 0) { /* If process is a normal sender   */
      if (MPI_Waitall (requnbr, requtab, stattab) != MPI_SUCCESS) { /* Wait for all now */
        errorPrint  ("hdgraphFold2: communication error (9");
        cheklocval = 1;
      }
    }                                             /* Communications of sender-receivers will be completed in the receiving phase */
  }

  if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) { /* If process is (also) a receiver */
    Gnum                orgvertlocnbr;
    Gnum                orgvertlocnnd;
    Gnum                fldvertlocnum;
    Gnum                fldedgelocnum;
    Gnum                fldvelolocsum;
    Gnum                fldvhallocnum;
    Gnum                fldehallocnbr;
    Gnum                fldvertrcvbas;
    Gnum                fldvertrcvnnd;
    int                 fldprocnum;
    int                 i;

    fldgrafptr->s.procvrttab = fldgrafptr->s.procdsptab; /* Graph does not have holes                           */
    fldgrafptr->s.procdsptab[0] = orggrafptr->s.baseval; /* Build private data of folded graph and array        */
    for (fldprocnum = 0; fldprocnum < fldprocglbnbr; fldprocnum ++) /* New subdomain indices start from baseval */
      fldgrafptr->s.procdsptab[fldprocnum + 1] = fldgrafptr->s.procdsptab[fldprocnum] + fldgrafptr->s.proccnttab[fldprocnum];

    requnbr = 0;                                  /* Optional communications without further processing at beginning of array */

    if ((fldcommtypval & DGRAPHFOLDCOMMSEND) == 0) { /* If process is a full receiver */
      Gnum                vertrcvbas;
      int                 vertrcvnbr;
      Gnum                orgvertlocmin;
      Gnum                orgvertlocmax;
      Gnum                fldvertlocadj;
      int                 procngbmin;
      int                 procngbmax;

      for (i = 0, vertrcvbas = orggrafptr->s.vertlocnnd, vertrcvnbr = 0; /* For all receive communications to perform */
           (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
        Gnum                procrcvnum;

        vertrcvbas += (Gnum) vertrcvnbr;
        vertrcvnbr  = (int) fldcommdattab[i].vertnbr;
        procrcvnum  = fldcommdattab[i].procnum;

        if (MPI_Irecv (fldgrafptr->s.vertloctax + vertrcvbas + 1 + i, vertrcvnbr + 1, GNUM_MPI, /* Receive one more, after the end of previous array */
                       procrcvnum, TAGVERTLOCTAB * orggrafptr->s.procglbnbr + procrcvnum,
                       orggrafptr->s.proccomm, &requtab[HDGRAPHFOLDTAGVERTLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("hdgraphFold2: communication error (10,%d)", i);
          cheklocval = 1;
        }
        else if (MPI_Irecv (fldgrafptr->s.vendloctax + vertrcvbas, vertrcvnbr, GNUM_MPI,
                            procrcvnum, TAGVENDLOCTAB * orggrafptr->s.procglbnbr + procrcvnum,
                            orggrafptr->s.proccomm, &requtab[HDGRAPHFOLDTAGVENDLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("hdgraphFold2: communication error (11,%d)", i);
          cheklocval = 1;
        }
        else if (MPI_Irecv (fldgrafptr->s.edgeloctax + orggrafptr->s.edgelocsiz + orggrafptr->s.baseval + orggrafptr->s.edgeglbsmx * i,
                            orggrafptr->s.edgeglbsmx, GNUM_MPI,
                            procrcvnum, TAGEDGELOCTAB * orggrafptr->s.procglbnbr + procrcvnum,
                            orggrafptr->s.proccomm, &requtab[HDGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("hdgraphFold2: communication error (12,%d)", i);
          cheklocval = 1;
        }
        else if ((orggrafptr->s.veloloctax != NULL) &&
                 (MPI_Irecv (fldgrafptr->s.veloloctax + vertrcvbas, vertrcvnbr, GNUM_MPI,
                             procrcvnum, TAGVELOLOCTAB * orggrafptr->s.procglbnbr + procrcvnum,
                             orggrafptr->s.proccomm, &requtab[HDGRAPHFOLDTAGVELOLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS)) {
          errorPrint ("hdgraphFold2: communication error (13,%d)", i);
          cheklocval = 1;
        }
        else if ((orggrafptr->s.vnumloctax != NULL) &&
                 (MPI_Irecv (fldgrafptr->s.vnumloctax + vertrcvbas, vertrcvnbr, GNUM_MPI,
                             procrcvnum, TAGVNUMLOCTAB * orggrafptr->s.procglbnbr + procrcvnum,
                             orggrafptr->s.proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
          errorPrint ("hdgraphFold2: communication error (14,%d)", i);
          cheklocval = 1;
        }
      }

      orgvertlocnbr = orggrafptr->s.vertlocnbr;   /* Process all local vertices */
      orgvertlocnnd = orggrafptr->s.vertlocnnd;

      if (orggrafptr->s.vnumloctax == NULL) {     /* If original graph noes not have vertex numbers, create remote parts of vertex number array */
        Gnum              fldvertlocnum;
        Gnum              fldvertlocadj;
        int               i;

        fldvertlocnum = orgvertlocnnd;
        for (i = 0; (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
          Gnum              fldverttmpnnd;

          for (fldverttmpnnd = fldvertlocnum + fldcommdattab[i].vertnbr, fldvertlocadj = fldcommvrttab[i];
               fldvertlocnum < fldverttmpnnd; fldvertlocnum ++)
            fldgrafptr->s.vnumloctax[fldvertlocnum] = fldvertlocadj ++;
        }
      }

      for (procngbmin = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
           procngbmax - procngbmin > 1; ) {
        int               procngbnum;

        procngbnum = (procngbmax + procngbmin) / 2;
        if (fldvertadjtab[procngbnum] <= orggrafptr->s.procvrttab[orggrafptr->s.proclocnum])
          procngbmin = procngbnum;
        else
          procngbmax = procngbnum;
      }
      orgvertlocmin = fldvertadjtab[procngbmin];
      orgvertlocmax = fldvertadjtab[procngbmax];
      fldvertlocadj = fldvertdlttab[procngbmin];
      for (fldvertlocnum = fldedgelocnum = orggrafptr->s.baseval; /* Adjust local part of edge array */
           fldvertlocnum < orggrafptr->s.vertlocnnd; ) {
        for ( ; fldedgelocnum < orggrafptr->s.vendloctax[fldvertlocnum]; fldedgelocnum ++) { /* Reorder end vertices */
          Gnum              orgvertlocend;

#ifdef SCOTCH_DEBUG_HDGRAPH2
          if (fldedgelocnum >= (fldedgelocsiz + orggrafptr->s.baseval)) {
            errorPrint  ("hdgraphFold2: internal error (1)");
            return      (1);
          }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

          orgvertlocend = orggrafptr->s.edgeloctax[fldedgelocnum];

          if ((orgvertlocend >= orgvertlocmin) && /* If end vertex is local */
              (orgvertlocend <  orgvertlocmax))
            fldgrafptr->s.edgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
          else {                                  /* End vertex is not local */
            int               procngbmin;
            int               procngbmax;

            for (procngbmin = 0, procngbmax = fldvertadjnbr;
                 procngbmax - procngbmin > 1; ) {
              int               procngbnum;

              procngbnum = (procngbmax + procngbmin) / 2;
              if (fldvertadjtab[procngbnum] <= orgvertlocend)
                procngbmin = procngbnum;
              else
                procngbmax = procngbnum;
            }
            fldgrafptr->s.edgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbmin];
          }
        }
        fldvertlocnum ++;
        for ( ; fldedgelocnum < orggrafptr->s.vertloctax[fldvertlocnum]; fldedgelocnum ++) { /* Copy halo part as is */
#ifdef SCOTCH_DEBUG_HDGRAPH2
          if ((orggrafptr->s.edgeloctax[fldedgelocnum] < orggrafptr->s.baseval) ||
              (orggrafptr->s.edgeloctax[fldedgelocnum] >= (orggrafptr->vhallocnbr + orggrafptr->s.baseval))) {
            errorPrint  ("hdgraphFold2: internal error (2)");
            return      (1);
          }
          if (fldedgelocnum >= (fldedgelocsiz + orggrafptr->s.baseval)) {
            errorPrint  ("hdgraphFold2: internal error (3)");
            return      (1);
          }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */
          fldgrafptr->s.edgeloctax[fldedgelocnum] = orggrafptr->s.edgeloctax[fldedgelocnum];
        }
      }

      fldehallocnbr = orggrafptr->ehallocnbr;     /* Normal receivers have at least all of their local halo vertices */
      fldvhallocnum = orggrafptr->vhallocnbr + orggrafptr->s.baseval; /* Index of next halo vertex number to assign  */
      fldvelolocsum = orggrafptr->s.velolocsum;   /* In case there are vertex loads: we keep at least existing load  */
    }
    else {                                        /* Receiver process is also a sender */
      Gnum              orgvertlocmin;
      Gnum              orgvertlocmax;
      Gnum              fldvertlocadj;
      Gnum              fldvhallocmax;            /* Maximum current size of halo vertex array */
      int               procngbmin;
      int               procngbmax;

      orgvertlocnbr = fldvertlocnbr;              /* Process only remaining local vertices */
      orgvertlocnnd = fldvertlocnbr + orggrafptr->s.baseval;

      for (procngbmin = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
           procngbmax - procngbmin > 1; ) {
        int               procngbnum;

        procngbnum = (procngbmax + procngbmin) / 2;
        if (fldvertadjtab[procngbnum] <= orggrafptr->s.procvrttab[orggrafptr->s.proclocnum])
          procngbmin = procngbnum;
        else
          procngbmax = procngbnum;
      }
      orgvertlocmin = fldvertadjtab[procngbmin];
      orgvertlocmax = fldvertadjtab[procngbmax];
      fldvertlocadj = fldvertdlttab[procngbmin];
      fldvhallocmax = orggrafptr->s.baseval - 1;  /* Reset halo vertex array for local part as halo vertices may have disappeared */
      fldehallocnbr = 0;                          /* Recount all remaining halo vertices and edges                                */
      fldvhallocnum = orggrafptr->s.baseval;
      for (fldvertlocnum = fldedgelocnum = orggrafptr->s.baseval; /* Copy remaining local part of edge array */
           fldvertlocnum < orgvertlocnnd; ) {
        for ( ; fldedgelocnum < orggrafptr->s.vendloctax[fldvertlocnum]; fldedgelocnum ++) { /* Reorder end vertices */
          Gnum              orgvertlocend;

#ifdef SCOTCH_DEBUG_HDGRAPH2
          if (fldedgelocnum >= (fldedgelocsiz + orggrafptr->s.baseval)) {
            errorPrint  ("hdgraphFold2: internal error (4)");
            return      (1);
          }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

          orgvertlocend = orggrafptr->s.edgeloctax[fldedgelocnum];

          if ((orgvertlocend >= orgvertlocmin) && /* If end vertex is local */
              (orgvertlocend <  orgvertlocmax))
            fldgrafptr->s.edgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
          else {
            int               procngbmin;
            int               procngbmax;

            for (procngbmin = 0, procngbmax = fldvertadjnbr;
                 procngbmax - procngbmin > 1; ) {
              int               procngbnum;

              procngbnum = (procngbmax + procngbmin) / 2;
              if (fldvertadjtab[procngbnum] <= orgvertlocend)
                procngbmin = procngbnum;
              else
                procngbmax = procngbnum;
            }
            fldgrafptr->s.edgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbmin];
          }
        }
        fldvertlocnum ++;
        fldehallocnbr += orggrafptr->s.vertloctax[fldvertlocnum] - fldedgelocnum;
        for ( ; fldedgelocnum < orggrafptr->s.vertloctax[fldvertlocnum]; fldedgelocnum ++) { /* Copy halo part as is */
          Gnum              orgverthalend;
          Gnum              fldvhallocend;

          orgverthalend = orggrafptr->s.edgeloctax[fldedgelocnum];
#ifdef SCOTCH_DEBUG_HDGRAPH2
          if ((orgverthalend < orggrafptr->s.baseval)                             ||
              (orgverthalend >= (orggrafptr->vhallocnbr + orggrafptr->s.baseval)) ||
              (fldedgelocnum >= (fldedgelocsiz + orggrafptr->s.baseval))) {
            errorPrint ("hdgraphFold2: internal error (5)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

          while (fldvhallocmax < orgverthalend)   /* Expand halo vertex index array whenever necessary */
            fldvhalloctax[++ fldvhallocmax] = ~0;
          fldvhallocend = fldvhalloctax[orgverthalend]; /* Get renumbered halo vertex */
          if (fldvhallocend < 0) {                /* If new halo vertex not yet given */
            fldvhallocend                =        /* Allocate it                      */
            fldvhalloctax[orgverthalend] = fldvhallocnum ++;
          }
          fldgrafptr->s.edgeloctax[fldedgelocnum] = fldvhallocend;
        }
      }

      if (orggrafptr->s.veloloctax != NULL)       /* If original graph has vertex loads                         */
        for (fldvertlocnum = orggrafptr->s.baseval, fldvelolocsum = 0; /* Accumulate load sum of remaining part */
             fldvertlocnum < orgvertlocnnd; fldvertlocnum ++)
          fldvelolocsum += orggrafptr->s.veloloctax[fldvertlocnum];

      fldcommdattab[0].procnum = -1;              /* Turn sender-receiver into normal receiver without any communications to perform */
    }

    if (orggrafptr->s.veloloctax != NULL)         /* If original graph has vertex loads                 */
      memCpy (fldgrafptr->s.veloloctax + orggrafptr->s.baseval, /* Copy local part of vertex load array */
              orggrafptr->s.veloloctax + orggrafptr->s.baseval, orgvertlocnbr * sizeof (Gnum));

    if (orggrafptr->s.vnumloctax != NULL)         /* If original graph has vertex numbers                 */
      memCpy (fldgrafptr->s.vnumloctax + orggrafptr->s.baseval, /* Copy local part of vertex number array */
              orggrafptr->s.vnumloctax + orggrafptr->s.baseval, orgvertlocnbr * sizeof (Gnum));
    else {                                        /* Build local part of vertex number array */
      Gnum              fldvertlocnum;
      Gnum              fldvertlocadj;

      for (fldvertlocnum = orggrafptr->s.baseval,
           fldvertlocadj = orggrafptr->s.procvrttab[orggrafptr->s.proclocnum];
           fldvertlocnum < orgvertlocnnd; fldvertlocnum ++)
        fldgrafptr->s.vnumloctax[fldvertlocnum] = fldvertlocadj ++;
    }

    memCpy (fldgrafptr->s.vertloctax + orggrafptr->s.baseval, /* Copy local part of vertex arrays */
            orggrafptr->s.vertloctax + orggrafptr->s.baseval, (orgvertlocnbr + 1) * sizeof (Gnum));
    memCpy (fldgrafptr->s.vendloctax + orggrafptr->s.baseval,
            orggrafptr->s.vendloctax + orggrafptr->s.baseval, orgvertlocnbr * sizeof (Gnum));

    for (i = 0, fldvertlocnum = orggrafptr->s.vertlocnnd + 1;
         (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
      if (MPI_Wait (&requtab[HDGRAPHFOLDTAGVERTLOCTAB * DGRAPHFOLDCOMMNBR + i],
                    &stattab[HDGRAPHFOLDTAGVERTLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
        errorPrint ("hdgraphFold2: communication error (15)");
        cheklocval = 1;
      }
      else {                                      /* Adjust first remote part of vertex array */
        Gnum              fldvertlocadj;
        Gnum              fldverttmpnnd;

        fldvertlocadj = fldgrafptr->s.vertloctax[fldvertlocnum - 1] - fldgrafptr->s.vertloctax[fldvertlocnum + i];
        fldvertidxtab[i] = fldgrafptr->s.vertloctax[fldvertlocnum + i];

        for (fldverttmpnnd = fldvertlocnum + fldcommdattab[i].vertnbr; fldvertlocnum < fldverttmpnnd; fldvertlocnum ++)
          fldgrafptr->s.vertloctax[fldvertlocnum] = fldgrafptr->s.vertloctax[fldvertlocnum + i + 1] + fldvertlocadj;
      }
    }

    for (i = 0, fldvertrcvbas = orggrafptr->s.vertlocnnd;
         (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++, fldvertrcvbas = fldvertrcvnnd) {
      fldvertrcvnnd = fldvertrcvbas + fldcommdattab[i].vertnbr;

      if (MPI_Wait (&requtab[HDGRAPHFOLDTAGVENDLOCTAB * DGRAPHFOLDCOMMNBR + i],
                    &stattab[HDGRAPHFOLDTAGVENDLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
        errorPrint ("hdgraphFold2: communication error (16,%d)", i);
        cheklocval = 1;
      }
      else {                                      /* Adjust remote part(s) of vertex end array */
        Gnum              fldvertlocadj;

        for (fldvertlocnum = fldvertrcvbas, fldvertlocadj = fldgrafptr->s.vertloctax[fldvertlocnum] - fldvertidxtab[i];
             fldvertlocnum < fldvertrcvnnd; fldvertlocnum ++)
          fldgrafptr->s.vendloctax[fldvertlocnum] += fldvertlocadj;
      }

      if (MPI_Wait (&requtab[HDGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i],
                    &stattab[HDGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
        errorPrint ("hdgraphFold2: communication error (17)");
        cheklocval = 1;
      }
      else if (cheklocval == 0) {                 /* Adjust remote part(s) of edge array */
        Gnum              orgvertlocmin;
        Gnum              orgvertlocmax;
        Gnum              fldvertlocadj;
        Gnum              fldvhallocmax;          /* Maximum current size of halo vertex array */
        Gnum              fldedgercvnum;
        int               procngbmin;
        int               procngbmax;
#ifdef SCOTCH_DEBUG_HDGRAPH2
        int               fldedgercvnbr;

        MPI_Get_count (&stattab[HDGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i], GNUM_MPI, &fldedgercvnbr); /* Can test this because halo graphs are compact */
        if (fldedgercvnbr != (fldgrafptr->s.vertloctax[fldvertrcvnnd] - fldgrafptr->s.vertloctax[fldvertrcvbas])) {
          errorPrint ("hdgraphFold2: internal error (6)");
          return     (1);
        }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

        for (procngbmin = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
             procngbmax - procngbmin > 1; ) {
          int               procngbnum;

          procngbnum = (procngbmax + procngbmin) / 2;
          if (fldvertadjtab[procngbnum] <= fldcommvrttab[i])
            procngbmin = procngbnum;
          else
            procngbmax = procngbnum;
        }
        orgvertlocmin = fldvertadjtab[procngbmin];
        orgvertlocmax = fldvertadjtab[procngbmax];
        fldvertlocadj = fldvertdlttab[procngbmin];
        fldvhallocmax = orggrafptr->s.baseval - 1; /* Reset halo vertex array for each remote part                                    */
        for (fldvertlocnum = fldvertrcvbas, fldedgelocnum = fldgrafptr->s.vertloctax[fldvertlocnum], /* Copy local part of edge array */
             fldedgercvnum = orggrafptr->s.edgelocsiz + orggrafptr->s.baseval + orggrafptr->s.edgeglbsmx * i;
             fldvertlocnum < fldvertrcvnnd; ) {
          for ( ; fldedgelocnum < fldgrafptr->s.vendloctax[fldvertlocnum]; fldedgelocnum ++) { /* Reorder end vertices */
            Gnum              orgvertlocend;

#ifdef SCOTCH_DEBUG_HDGRAPH2
            if (fldedgelocnum >= (fldedgelocsiz + orggrafptr->s.baseval)) {
              errorPrint  ("hdgraphFold2: internal error (7)");
              return      (1);
            }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

            orgvertlocend = fldgrafptr->s.edgeloctax[fldedgercvnum ++];

            if ((orgvertlocend >= orgvertlocmin) && /* If end vertex is local */
                (orgvertlocend <  orgvertlocmax))
              fldgrafptr->s.edgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
            else {
              int               procngbmin;
              int               procngbmax;

              for (procngbmin = 0, procngbmax = fldvertadjnbr;
                   procngbmax - procngbmin > 1; ) {
                int               procngbnum;

                procngbnum = (procngbmax + procngbmin) / 2;
                if (fldvertadjtab[procngbnum] <= orgvertlocend)
                  procngbmin = procngbnum;
                else
                  procngbmax = procngbnum;
              }
              fldgrafptr->s.edgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbmin];
            }
          }
          fldvertlocnum ++;
          fldehallocnbr += fldgrafptr->s.vertloctax[fldvertlocnum] - fldedgelocnum;
          for ( ; fldedgelocnum < fldgrafptr->s.vertloctax[fldvertlocnum]; fldedgelocnum ++) { /* Copy halo part as is */
            Gnum              orgverthalend;
            Gnum              fldvhallocend;

            orgverthalend = fldgrafptr->s.edgeloctax[fldedgercvnum ++];
#ifdef SCOTCH_DEBUG_HDGRAPH2
            if ((orgverthalend < orggrafptr->s.baseval) ||
                (orgverthalend >= (orggrafptr->s.edgeglbsmx + orggrafptr->s.baseval)) ||
                (fldedgelocnum >= (fldedgelocsiz + orggrafptr->s.baseval))) {
              errorPrint ("hdgraphFold2: internal error (8)");
              return     (1);
            }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

            while (fldvhallocmax < orgverthalend) /* Expand halo vertex index array whenever necessary */
              fldvhalloctax[++ fldvhallocmax] = ~0;
            fldvhallocend = fldvhalloctax[orgverthalend]; /* Get renumbered halo vertex */
            if (fldvhallocend < 0) {              /* If new halo vertex not yet given   */
              fldvhallocend                =      /* Allocate it                        */
              fldvhalloctax[orgverthalend] = fldvhallocnum ++;
            }
            fldgrafptr->s.edgeloctax[fldedgelocnum] = fldvhallocend;
          }
        }
      }
    }
#ifdef SCOTCH_DEBUG_HDGRAPH2
    if (fldedgelocnum != fldgrafptr->s.vertloctax[fldvertlocnbr + orggrafptr->s.baseval]) {
      errorPrint ("hdgraphFold2: internal error (9)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

    memFree (fldvertadjtab);                      /* Free group leader */

    fldedgelocsiz = fldedgelocnum - orggrafptr->s.baseval;
    fldgrafptr->s.edgeloctax  = memRealloc (fldgrafptr->s.edgeloctax + orggrafptr->s.baseval,  (fldedgelocsiz * sizeof (Gnum)));
    fldgrafptr->s.edgeloctax -= orggrafptr->s.baseval;

    fldgrafptr->vhallocnbr = fldvhallocnum - orggrafptr->s.baseval;
    fldgrafptr->vhndloctax = fldgrafptr->s.vertloctax + 1; /* Compact edge array with halo vertices */
    fldgrafptr->ehallocnbr = fldehallocnbr;
    fldgrafptr->levlnum    = orggrafptr->levlnum; /* Folded graph is of same level */

    if (orggrafptr->s.veloloctax == NULL)         /* If no vertex loads, reset graph vertex load to number of vertices */
      fldvelolocsum = fldvertlocnbr;
    else {                                        /* Graph has vertex loads and load of local part has already been computed */
      Gnum                vertrcvbas;
      Gnum                vertrcvnnd;

      for (i = 0, vertrcvnnd = orggrafptr->s.vertlocnnd; /* For all receive communications to perform */
           (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
        vertrcvbas  = vertrcvnnd;
        vertrcvnnd += fldcommdattab[i].vertnbr;

        if (MPI_Wait (&requtab[HDGRAPHFOLDTAGVELOLOCTAB * DGRAPHFOLDCOMMNBR + i],
                      &stattab[HDGRAPHFOLDTAGVELOLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold: communication error (18)");
          cheklocval = 1;
        }
        else {                                    /* Accumulate vertex loads for received vertex load array */
          for (fldvertlocnum = vertrcvbas; fldvertlocnum < vertrcvnnd; fldvertlocnum ++)
            fldvelolocsum += fldgrafptr->s.veloloctax[fldvertlocnum];
        }
      }
    }

    if (MPI_Waitall (requnbr, requtab, stattab) != MPI_SUCCESS) { /* Make sure graph data is safe because graph could be freed after routine completes */
      errorPrint  ("hdgraphFold2: communication error (19)");
      cheklocval = 1;
    }

    fldgrafptr->s.baseval    = orggrafptr->s.baseval;
    fldgrafptr->s.vertlocnbr = fldvertlocnbr;
    fldgrafptr->s.vertlocnnd = fldvertlocnbr + orggrafptr->s.baseval;
    fldgrafptr->s.velolocsum = fldvelolocsum;
    fldgrafptr->s.edgelocnbr = fldedgelocsiz - fldehallocnbr;
    fldgrafptr->s.edgelocsiz = fldedgelocsiz;
    fldgrafptr->s.degrglbmax = orggrafptr->s.degrglbmax;
    if (dgraphBuild4 (&fldgrafptr->s) != 0) {
      errorPrint  ("hdgraphFold2: cannot build folded graph");
      hdgraphExit (fldgrafptr);
      return      (1);
    }
#ifdef SCOTCH_DEBUG_HDGRAPH2
    if (hdgraphCheck (fldgrafptr) != 0) {         /* Check graph consistency */
      errorPrint  ("hdgraphFold2: internal error (10)");
      hdgraphExit (fldgrafptr);
      return      (1);
    }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */
  }

#ifdef SCOTCH_DEBUG_HDGRAPH1                      /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, orggrafptr->s.proccomm) != MPI_SUCCESS) {
    errorPrint ("hdgraphFold2: communication error (20)");
    chekglbval = 1;
  }
#else /* SCOTCH_DEBUG_HDGRAPH1 */
  chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_HDGRAPH1 */

  return (chekglbval);
}
