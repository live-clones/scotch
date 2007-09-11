/* Copyright 2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : dgraph_match_sync.c                     **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This file implements the matching       **/
/**                phase of the multi-level method.        **/
/**                The implementation uses an syncation    **/
/**                paradigme in order to limit amount of   **/
/**                communications needed.                  **/
/**                                                        **/
/**   DATES      : # Version 0.5  : from : 14 sep 2006     **/
/**                                 to   : 10 sep 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define DGRAPH_MATCH_C

/* #define SCOTCH_DEBUG_MATCHING */

#ifdef SCOTCH_DEBUG_MATCHING
# define SCOTCH_DETERMINIST
#endif /* SCOTCH_DEBUG_MATCHING */


/* #define SCOTCH_MATCH_STATS */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <mpi.h>

#include "module.h"
#include "common.h"
#include "graph.h"
#include "dgraph.h"
#include "dgraph_allreduce.h"
#include "dgraph_coarsen.h"
#include "dgraph_match.h"
#include "dgraph_match_tools.h"
#include "dgraph_match_sync.h"

#ifdef TIME_MEASURE
static double total_time=0.0L;
#endif /* TIME_MEASURE */

DGRAPHALLREDUCEMAXSUMOP (0,3)

/*
 * Here's the function calling the two threads
 *
 */

int dgraphMatchSync (
  const Dgraph * restrict const finegraph,        /* Graph to match                               */
  DgraphMatchParam   * restrict param,            /* Shared tables concerning the vertices states */
  int                           coarvertmax)      /* Maximum number of matching pairs             */
{
  int retval;
#ifdef TIME_MEASURE
  double               reftime;
#endif /* TIME_MEASURE */

#ifdef TIME_MEASURE
  reftime = MPI_Wtime();
#endif /* TIME_MEASURE */

  retval = dgraphMatchSyncDoMatching (finegraph, param, coarvertmax); /* Really do matching */

#ifdef SCOTCH_DEBUG_MATCHING
  retval |= (dgraphMatchCheck (finegraph, param));/* Verify matching result */
#endif /* SCOTCH_DEBUG_MATCHING */

#ifdef TIME_MEASURE
  total_time += (MPI_Wtime() - reftime);
  fprintf (stderr, "proclocnum : %d, temps : %g\n", finegraph->proclocnum,
           total_time);
#endif /* TIME_MEASURE */
#ifdef SCOTCH_DEBUG_MATCHING
  DEBUG_PRINT (3,"%d exiting from the matching routine\n", finegraph->proclocnum);
#endif /* SCOTCH_DEBUG_MATCHING */
  return (retval);
}


/*
 *Here's an implementation of the sending thread
 *
 */

int
dgraphMatchSyncDoMatching (
const Dgraph *  const             finegraph,      /* Graph to match                               */
DgraphMatchParam * restrict param,                /* Shared tables concerning the vertices states */
int                               coarvertmax)    /* Maximum number of matching pairs             */
{
  Gnum                 finevertfrstnum;           /* Shortcut to finegraph->procvrttab[proclocnum] */
  Gnum                 finevertlastnum;           /* Shortcut to finegraph->procvrttab[proclocnum + 1] */
  int                  proclocnum;                /* Shortcut to finegraph->proclocnum            */
  Gnum                 baseval;                   /* Shortcut to finegraph->basevalue             */
  DgraphMatchSyncQueue nonmatequeue;              /* Queue of non mated vertices */
  Gnum                 finevertnum;               /* Currently selected fine vertex               */
  Gnum                 finematenum;               /* Vertex to be matched with                    */
  Gnum                 locmatenum;                /* Vertex to be matched, local index            */
  int                  loopnum;                   /* Number of necessary attempt of matching      */
  int                  ownerproc;                 /* Owner of the mate vertex */
  int                  procngbnum;                /* Index of this owner vertex in procngbtab     */
  Gnum                 edgenum;                   /* Index on adjacent vertices                  */
  int                  finepertbas;               /* Index on finecoartax of the area's first index */
  int                  finepertnbr;               /* Perturbation area size                      */
  Gnum                 finevertlocnum;
  int                  cheklocval = 0;
  int                  chekglbval = 0;
  Gnum                 nonmatenbr;                /* number of non mated vertices                 */
  Gnum                 prvglnnonmate;             /* Previous global number of nonmate vertices   */
  Gnum                 reduloctab[3];  /* 0 : beginning color 1 : nonmatenbr, 2 : final coarmultsize */
  Gnum                 reduglbtab[3];  /* 0 : beginning color 1 : glbnonmatenbr, 2 : final glbcoarmultsize */
  DgraphMatchSyncComm  comm;                      /* Used to choose data exchange method */
#ifdef MATCH_ANTICIP
  int       * restrict probatax;
  int                  procsidnum;                /* Browse frontier vertices */
  int 	               probamatch = 1;
#endif /* MATCH_ANTICIP */
#ifdef SCOTCH_MATCH_STATS
  int                  displaystats = 1;
  Gnum                 glbdstmatchnbr;
#endif /* SCOTCH_MATCH_STATS */
  int                  progress = PROGRESS_VAL;   /* PROGRESS_VAL chances to do loop with no progress */

  proclocnum = finegraph->proclocnum;
  finevertfrstnum = finegraph->procvrttab[proclocnum];
  finevertlastnum = finegraph->procvrttab[proclocnum + 1];
  baseval = finegraph->baseval;
  param->coarmultnum = finegraph->baseval;

#ifndef SCOTCH_DETERMINIST
  for (procngbnum = 0 ; procngbnum < finegraph->proclocnum ; ++ procngbnum ) { /* Desynchronise random */
    loopnum = intRandVal (42);                    /* Affect to avoid warning */
  }
#endif /* SCOTCH_DETERMINIST */

  if ((memAllocGroup ((void **) (void *)
                      &nonmatequeue.qtab, (size_t) ((finegraph->vertlocnbr + 1) * sizeof (Gnum)), /* +1 to avoid tail=head when the list is full */
#ifdef MATCH_ANTICIP
                      &probatax,          (size_t) (finegraph->vertgstnbr * sizeof (int)),
#endif /* MATCH_ANTICIP */
                      NULL) == NULL)) {
    errorPrint ("dgraphMatchSyncDoMatching : out of memory (1)");
    cheklocval = 1;
  }

  if (cheklocval == 0) {
    comm.method = PREPAR_SYNC;
/*     comm.method = SIMPLE_SYNC; */
/*      comm.method = LUBY_SYNC; */
/*     comm.method = LUBY_SIMPLE_SYNC; */
    cheklocval = dgraphMatchSyncCommInit (finegraph, param, &comm);

#ifdef MATCH_ANTICIP
    if ((comm.method == LUBY_SIMPLE_SYNC) || (comm.method == LUBY_SYNC))
      probamatch = 0;                             /* Disallow probabilities when using Luby */
#endif /* MATCH_ANTICIP */
  }
#ifdef SCOTCH_DEBUG_DGRAPH1
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, finegraph->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphMatchSyncDoMatching : communication error (2)");
    return     (1);
  }
  if (chekglbval != 0)
    return (1);
#endif /* SCOTCH_DEBUG_DGRAPH1 */

#ifdef MATCH_ANTICIP
  memset (probatax, 0, finegraph->vertgstnbr * sizeof(int));
  probatax -= finegraph->baseval;
  for (procsidnum = 0, finevertlocnum = finegraph->baseval;
       procsidnum < finegraph->procsidnbr; procsidnum ++) {
    int                 procsidval;

    procsidval = finegraph->procsidtab[procsidnum];
    if (procsidval < 0)
      finevertlocnum -= (Gnum) procsidval;
    else {
      Gnum neighborload = 0;
      Gnum totalload = 0;

      probatax[finevertlocnum] = 0;
      if (finegraph->edloloctax != NULL)          /* Edges are weighted */
        for (edgenum =  finegraph->vertloctax[finevertlocnum] ;
             edgenum < finegraph->vendloctax[finevertlocnum] ; ++ edgenum) {
          totalload += finegraph->edloloctax[edgenum];
          if (!dgraphVertexLocal(finegraph, finegraph->edgeloctax[edgenum]))
            neighborload += finegraph->edloloctax[edgenum];
        }
      else {
        totalload = finegraph->vendloctax[finevertlocnum] - finegraph->vertloctax[finevertlocnum];
        for (edgenum =  finegraph->vertloctax[finevertlocnum] ;
             edgenum < finegraph->vendloctax[finevertlocnum] ; ++ edgenum)
          neighborload += (dgraphVertexLocal(finegraph, finegraph->edgeloctax[edgenum])&1); /* To avoid 1 test */
        neighborload = totalload - neighborload;
      }
      probatax[finevertlocnum] = (neighborload*100)/totalload;
    }
  }
#endif /* MATCH_ANTICIP */

  dgraphMatchSyncQueueInit(&nonmatequeue, finegraph->vertlocnbr + 1); /* Initialize Queue */

  for (finevertnum = finegraph->baseval ; finevertnum < finegraph->vertgstnnd ; ++ finevertnum) {
    param->finecoartax[finevertnum] = -1;        /* Initialize finecoartax to -1 */
  }

  /*
   * Compute the cache friendly perturbation on indices of all non mated vertices (perturb nonmatetab)
   */
  finepertnbr = 2 + intRandVal (COARPERTPRIME - 2); /* Compute perturbation area size (avoid DIV0 in random) */
  for (finepertbas = baseval; finepertbas < finegraph->vertlocnnd; /* Run cache-friendly perturbation */
       finepertbas += finepertnbr) {
    Gnum                finepertval;              /* Current index in perturbation area */
    if (finepertbas + finepertnbr > finegraph->vertlocnnd)
      finepertnbr = finegraph->vertlocnnd - finepertbas;

    finepertval = 0;                              /* Start from first perturbation vertex */
    do {                                          /* Loop on perturbation vertices        */
      finevertnum = finepertbas + finepertval;    /* Compute corresponding vertex number  */
      finepertval  = (finepertval + COARPERTPRIME) % finepertnbr; /* Compute next perturbation index */
      dgraphMatchSyncQueuePut(&nonmatequeue, finevertfrstnum + finepertbas + finepertval - baseval);
      /* Perturb by pointing last index on next   */
    } while (finepertval != 0);
  } /* End of cache friendly perturbation computing */


  nonmatenbr = finegraph->vertlocnbr;
  loopnum = LOOPNUMBER;

  reduglbtab[1] = finegraph->vertglbnbr + 1;         /* No vertex matched */
  reduglbtab[2] = finegraph->vertglbnbr;             /* No coarsening done yet */
  prvglnnonmate = finegraph->vertglbnbr + 1;         /* We are decreasing the number of non mate vertices ! */
  /**
   *  Main loop of matching thread
   ***/
  while ((loopnum-- != 0) &&                         /* We can do loop */
         (reduglbtab[1] > 0) && (progress >= 0)) {   /* glbnonmatenbr > 0 && glbnonmatenbr is decreasing */
/*          (reduglbtab[2] > coarvertmax)) {            /\* We can contract the graph *\/ */
    /* && the max number of loops has not been reached */

    dgraphMatchSyncQueueSetReady (&nonmatequeue);
    comm.coarmultgstnum = 2;
      if ((loopnum > 0) || (progress < PROGRESS_VAL)) {
/* 	  || (reduglbtab[1] < (Gnum) (0.7 * finegraph->vertglbnbr))){ /\* Disallow probabilistics *\/ */
        if (comm.method == PREPAR_SYNC) {
          comm.method = LUBY_SYNC;
/* 	  progress = PROGRESS_VAL; */
#ifdef MATCH_ANTICIP
	  probamatch = 0;
#endif /* MATCH_ANTICIP */
	}
	if (comm.method == LUBY_SIMPLE_SYNC) {
          comm.method = SIMPLE_SYNC;
/* 	  progress = PROGRESS_VAL; */
#ifdef MATCH_ANTICIP
	  probamatch = 1;
#endif /* MATCH_ANTICIP */
/* 	  probamatch = 0; */
	}
      }

    while (dgraphMatchSyncQueueReady(&nonmatequeue)) {
      Gnum maxload = 0;
      int selfmatched = 1;

      finevertnum = dgraphMatchSyncQueueGet(&nonmatequeue);
      finevertlocnum = finevertnum - finevertfrstnum + baseval;
#ifdef SCOTCH_DEBUG_DGRAPH2
      if (!dgraphVertexLocal (finegraph, finevertnum)) {
        errorPrint ("dgraphMatchSyncThreadMatching : internal error (2)");
        return (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      if (param->finecoartax[finevertlocnum] > -1) {/* If vertex been picked already  */
          nonmatenbr --;                          /* distant (already received answer) or local matching */
          continue;
      }

      finematenum = finevertnum;                /* No matching vertex found yet */

#ifdef MATCH_ANTICIP
      if ((probamatch == 1) && (intRandVal (100) < probatax[finevertlocnum])) { /* Think this vertex can be remotely matched */
        dgraphMatchSyncQueuePut(&nonmatequeue, finevertnum);
        continue;
      }
#endif /* MATCH_ANTICIP */
      finematenum = finevertnum;
      if (finegraph->edloloctax != NULL) {      /* Heavy Edge matching */
        for (edgenum = finegraph->vertloctax[finevertlocnum]; /* Compute index for vertex   */
             edgenum < finegraph->vendloctax[finevertlocnum] ;
             edgenum ++) {
          Gnum vertgstend;

          vertgstend = finegraph->edgegsttax[edgenum];
          if ((maxload <= finegraph->edloloctax[edgenum])
              && (param->finecoartax[vertgstend] == -1)) {  /* This vertex could be a best choice */
            if ((maxload < finegraph->edloloctax[edgenum]) ||
                ((maxload == finegraph->edloloctax[edgenum]) && (intRandVal(2) == 0))) {
              finematenum = finegraph->edgeloctax[edgenum]; /* Choose this neighboor */
              maxload = finegraph->edloloctax[edgenum];
              locmatenum = vertgstend;
            }
          }
          if (param->finecoartax[vertgstend] < -1)
            selfmatched = 0;
        }
      }
      else {                                     /* Probabilistic matching */
        int degree;  /* TODO : verify that a pre-computed permutation is more expensive */
        int try;

        degree = finegraph->vendloctax[finevertlocnum] - finegraph->vertloctax[finevertlocnum];
        for (try = degree ; try > 0 ; --try) {
          Gnum vertgstend;
          int  offset;

          offset = intRandVal(degree);
          vertgstend = finegraph->edgegsttax[finegraph->vertloctax[finevertlocnum] + offset];
          if (param->finecoartax[vertgstend] == -1) { /* Choose this neighboor */
            finematenum = finegraph->edgeloctax [finegraph->vertloctax[finevertlocnum] + offset];
            locmatenum = vertgstend;
          }
        }
      }

      /* TODO : take into account the fact that gst vertices could be available */
      if (finematenum == finevertnum) { /* No mate vertex found */
        if (selfmatched == 1) {                 /* All extermities are already matched */
          param->finefoldtax[finevertlocnum] = proclocnum;
          /* Create a new multinode */
          param->coarmulttax[param->coarmultnum].vertnum[0] = finevertnum;
          param->coarmulttax[param->coarmultnum].vertnum[1] = finevertnum;/* Build new multinode */
          param->finecoartax[finevertlocnum] = param->coarmultnum; /* Point to coarse vertex */
          param->coarmultnum ++;
          nonmatenbr --;
        }
        else { /* We must add this vertex in queue to retest vertices */
          dgraphMatchSyncQueuePut(&nonmatequeue, finevertnum);
        }
        continue;
      }

      if ((finematenum < finevertlastnum) && (finematenum >= finevertfrstnum)) { /* If mate vertex is local */
        param->finefoldtax[finevertlocnum] = proclocnum;
        param->finefoldtax[finematenum - finevertfrstnum + baseval] = proclocnum;

          /* Create a new multinode */
        param->coarmulttax[param->coarmultnum].vertnum[0] = finevertnum;
        param->coarmulttax[param->coarmultnum].vertnum[1] = finematenum;/* Build new multinode */
        param->finecoartax[finevertlocnum] = param->coarmultnum;/* Point to coarse vertex */
        param->finecoartax[finematenum - finevertfrstnum + baseval] = param->coarmultnum;
        param->coarmultnum ++;
        nonmatenbr --;
      }
      else { /* Want to match with a non local vertex */
        /* It's a harder situation !! */
        ownerproc = dgraphVertexProc (finegraph, finematenum);

        /* Lock the local vertex waiting for an answer */
        param->finecoartax[finevertlocnum]  = -comm.coarmultgstnum;
        /* Lock the non local vertex waiting for an answer */
        param->finecoartax[locmatenum] = -comm.coarmultgstnum;
        comm.coarmultgsttab[comm.coarmultgstnum].vertnum[0] = finevertnum;
        comm.coarmultgsttab[comm.coarmultgstnum].vertnum[1] = finematenum;/* Build new multinode */
        ++ comm.coarmultgstnum;
        nonmatenbr --;
      }                                         /* End want to match with a non local vertex*/
    }                                             /* End of the loop on non mated vertices */

    if (finegraph->procngbnbr > 0) {              /* Exchange datas with neighbor processes */
      if (dgraphMatchSyncCommDo (finegraph, &comm, param, &nonmatenbr, &nonmatequeue) != 0) {
        dgraphMatchSyncCommExit(finegraph, &comm);
        memFree (nonmatequeue.qtab);                    /* Free memory group leader */
        errorPrint ("dgraphMatchSyncDoMatching : internal error (1)");
      }
    }
#ifdef SCOTCH_DETERMINIST
    reduloctab[0] = 0;
#else /* SCOTCH_DETERMINIST */
    reduloctab[0] = (Gnum)intRandVal (MAX(127, finegraph->procglbnbr));
#endif /* SCOTCH_DETERMINIST */
    reduloctab[1] = nonmatenbr;
    reduloctab[2] = param->coarmultnum - 1 + nonmatenbr;
    if (dgraphAllreduceMaxSum (reduloctab, reduglbtab, 0, 3, finegraph->proccomm) != 0) { /* Also used for synchronisation */
      errorPrint ("dgraphMatchSyncDoMatching : cannot compute reductions");
      return     (1);
    }
#ifdef SCOTCH_DETERMINIST
    comm.begcolor = 0;
#endif /* SCOTCH_DETERMINIST */

/*     if (finegraph->proclocnum == 0) { */
/*       fprintf (stderr, "V=%.2f\t%d\n", (float)(prvglnnonmate - reduglbtab[1])/(prvglnnonmate), -loopnum); */
/*     } */

/*     if (reduglbtab[1] >= prvglnnonmate) { */
    if ((float)(prvglnnonmate - reduglbtab[1])/(prvglnnonmate) < MINSPEED) {
      progress --;
#ifdef MATCH_ANTICIP
      probamatch = 1;
#endif /* MATCH_ANTICIP */
    }
    prvglnnonmate = reduglbtab[1];

#ifdef SCOTCH_MATCH_STATS
    if(displaystats) {
      Gnum locdstmatchnbr;

      locdstmatchnbr = finegraph->vertlocnbr - (nonmatenbr + param->coarmultnum - finegraph->baseval);
      MPI_Reduce (&locdstmatchnbr, &glbdstmatchnbr, 1, GNUM_MPI, MPI_SUM, 0, finegraph->proccomm);
      displaystats = 0;
    }
#endif /* SCOTCH_MATCH_STATS */
  }                                               /* End of main loop */

  dgraphMatchSyncQueueSetReady (&nonmatequeue);   /* Update nonmatenbr */
  while (dgraphMatchSyncQueueReady(&nonmatequeue)) {
    finevertlocnum = dgraphMatchSyncQueueGet(&nonmatequeue) - finevertfrstnum + baseval;
    if (param->finecoartax[finevertlocnum] > -1) {/* If vertex been picked already  */
      nonmatenbr --;                          /* distant (already received answer) or local matching */
      continue;
    }
  }
  if (dgraphMatchSyncBalance (finegraph, &comm, param, nonmatenbr) != 0) {
    return (1);
  }

  dgraphMatchSyncCommExit(finegraph, &comm);

#ifdef SCOTCH_MATCH_STATS
  {
    Gnum size;
    MPI_Reduce (&param->coarmultnum, &size, 1, GNUM_MPI, MPI_SUM, 0, finegraph->proccomm);
    if (finegraph->proclocnum == 0) {
      fprintf (stdout, "deg=%.2f\t%.2f\t%.2f\t%d\t%d\n",
	       (float)finegraph->edgeglbnbr/(float)finegraph->vertglbnbr,
	       (float)(glbdstmatchnbr)/(float)(finegraph->vertglbnbr),
	       (float)(size)/(float)(finegraph->vertglbnbr),
	       -loopnum, finegraph->procglbnbr);
    }
  }
#endif /* SCOTCH_MATCH_STATS */

#ifdef SCOTCH_DEBUG_DGRAPH2
  MPI_Barrier (finegraph->proccomm);
#endif /* SCOTCH_DEBUG_DGRAPH2 */


  memFree (nonmatequeue.qtab);                    /* Free memory group leader */

  return (0);
}

/* This routine computes a luby coloration of the processors of
** the distributed graph in order to have independant sets for
** terminal communications
** It returns :
** - 0 : on success
** -!0 : on error
*/

int
dgraphMatchLubyColor (
const Dgraph * restrict const dgrafptr,
int *                         mycolor,
int * restrict                colors,
int *                         nbrcolor)
{
  Dgraph dgrafproc;
  Graph  grafproc;
  Gnum   vertloctax[2];
  Gnum * vendloctax;
  Gnum             veloloctax[1];
  Gnum           * edgeloctax;
  Gnum             procnum;
  DgraphMatchSyncQueue queue;

  dgraphInit (&dgrafproc, dgrafptr->proccomm);

  vendloctax    = vertloctax + 1;                 /* Use compact representation */
  vertloctax[0] = 0;
  vendloctax[0] = dgrafptr->procngbnbr;
#ifdef SCOTCH_DETERMINIST
  veloloctax[0] = dgrafptr->proclocnum + 1;
#else /* SCOTCH_DETERMINIST */
  veloloctax[0] = intRandVal (dgrafptr->procglbnbr * 2) + 1;
#endif /* SCOTCH_DETERMINIST */

  edgeloctax = (Gnum *) memAlloc (vendloctax[0] * sizeof (Gnum));
  for (procnum = 0 ; procnum < vendloctax[0] ; procnum ++)
    edgeloctax[procnum] = dgrafptr->procngbtab[procnum];

  if (dgraphBuild2 (&dgrafproc, 0, 1, 1, vertloctax, vendloctax, veloloctax, veloloctax[0],
                    NULL, NULL, vendloctax[0], vendloctax[0], edgeloctax, NULL, NULL,
                    vendloctax[0]) != 0) {
    errorPrint ("dgraphMatchLubyColor: cannot build distributed graph");
    return     (1);
  }

  if (dgraphGatherAll (&dgrafproc, &grafproc) != 0) {
    errorPrint ("dgraphMatchLubyColor: cannot build centralized graph");
    return     (1);
  }
  dgraphExit (&dgrafproc);
  memFree    (edgeloctax);

  queue.qtab = (Gnum *) memAlloc ((grafproc.vertnnd + 1) * sizeof (Gnum)); /* Allocate space for queue */
  dgraphMatchSyncQueueInit (&queue, grafproc.vertnnd + 1);
  for (procnum = 0; procnum < grafproc.vertnnd; procnum ++)
    dgraphMatchSyncQueuePut (&queue, procnum);    /* Enqueue vertex in queue */


  *nbrcolor = -1;
  while (! dgraphMatchSyncQueueEmpty (&queue)) {  /* Loop on color number */
    dgraphMatchSyncQueueSetReady (&queue);
    (*nbrcolor) ++;
    while (dgraphMatchSyncQueueReady (&queue)) {  /* Loop on vertices */
      Gnum * edgeptr;
      Gnum * edgennd;
      Gnum mynum;
      int flag;

      procnum = dgraphMatchSyncQueueGet (&queue);
      mynum   = grafproc.velotax[procnum];
      flag = 1;
      for (edgeptr = grafproc.edgetax + grafproc.verttax[procnum], edgennd = grafproc.edgetax + grafproc.vendtax[procnum];
           edgeptr < edgennd ; ++ edgeptr) {
        if ((grafproc.velotax[*edgeptr] > mynum) ||              /* Not best num */
            ((grafproc.velotax[*edgeptr] == mynum) && (*edgeptr < procnum )) /* Not best num also */
            || (grafproc.velotax[*edgeptr] == -(*nbrcolor))) {    /* A neighboor has been already chosen */
          flag = 0;                                          /* Not allowed to color this vertex */
          dgraphMatchSyncQueuePut (&queue, procnum);
          break;
        }
      }
      if (flag == 1)
        grafproc.velotax[procnum] = - (*nbrcolor);             /* This is the good color for this vertex */
    }
  }

  for (procnum = 0 ; procnum < dgrafptr->procngbnbr ; ++ procnum) /* Fill the color tab with the luby colors computed */
    colors [procnum] = -grafproc.velotax[dgrafptr->procngbtab[procnum]];

#ifdef SCOTCH_DEBUG_DGRAPH2
  for (procnum = 0 ; procnum < dgrafptr->procngbnbr ; ++ procnum) /* Fill the color tab with the luby colors computed */
    if (colors[procnum] < 0) {
      errorPrint ("dgraphMatchLubyColor : internal error (1)");
      return (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  *mycolor = -grafproc.velotax[dgrafptr->proclocnum];

  memFree (queue.qtab);
  graphExit (&grafproc);

  return (0);
}

int
dgraphMatchSyncCommInit (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchParam    * restrict shared,
DgraphMatchSyncComm * restrict commptr)
{
  DgraphMatchSyncMessage * message;
  MPI_Aint                 disp[4];  /* 3 elements in structure + end mark */
  MPI_Datatype             type[4] = {GNUM_MPI, GNUM_MPI, GNUM_MPI, MPI_UB};
  int                      blocklen[4] = {1, 1, 1, 1};
  int                      i;
  int                      procrcvnbr;
  int                      procngbnum;
  int                      procnum;
  DgraphMatchSyncMessage * messageforother;

  procrcvnbr = finegraph->vertgstnbr - finegraph->vertlocnbr;
  if ((memAllocGroup ((void **) (void *)
                      &commptr->comsndtab,       (size_t) (finegraph->procngbnbr * sizeof (DgraphMatchSyncCommProc)),
                      &commptr->comrcvtab,       (size_t) (finegraph->procngbnbr * sizeof (DgraphMatchSyncCommProc)),
                      &commptr->requesttab,      (size_t) (2 * finegraph->procngbnbr * sizeof (MPI_Request)),
                      &message,                  (size_t) ((procrcvnbr + finegraph->procsndnbr) * sizeof (DgraphMatchSyncMessage)),
                      &commptr->coarmultgsttab,  (size_t) (procrcvnbr * sizeof (DgraphCoarsenMulti)),
                      &commptr->verttosndtab,    (size_t) (finegraph->procglbnbr * sizeof (Gnum*)),
                      &messageforother,          (size_t) ((procrcvnbr) * sizeof (DgraphMatchSyncMessage)),
                      &commptr->proccoarmulttab, (size_t) (finegraph->procngbnbr * sizeof (DgraphMatchSyncMessage*)), NULL) == NULL)) {
    errorPrint ("dgraphMatchSyncCommInit: out of memory (1)");
    return     (1);
  }
  commptr->coarmultgsttab -= 2;
  commptr->coarmultgstnum = 2;

#ifdef SCOTCH_DEBUG_DGRAPH2
  memset (message, ~0, (procrcvnbr + finegraph->procsndnbr) * sizeof (DgraphMatchSyncMessage));
  memset (messageforother, ~0, (procrcvnbr) * sizeof (DgraphMatchSyncMessage));
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  if (commptr->method != SIMPLE_SYNC){
    if ((commptr->colortab = memAlloc (finegraph->procngbnbr * sizeof(Gnum))) == NULL) {
      memFree (commptr->comsndtab);               /* Free memory group leader */
      errorPrint ("dgraphMatchSyncCommInit: out of memory (2)");
      return (1);
    }
    commptr->begcolor = 0;

    if (dgraphMatchLubyColor (finegraph, &commptr->mycolor, /* Make Luby coloration of processors */
                              commptr->colortab, &commptr->nbrcolor) != 0) {
      memFree (commptr->colortab);
      memFree (commptr->comsndtab);               /* Free memory group leader */
      errorPrint ("dgraphMatchSyncCommInit: internal error (2)");
      return (1);
    }
  }

  if (finegraph->procngbnbr == 0)
    return (0);

  commptr->proccoarmulttab[0] = messageforother;
  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr - 1 ; /* Fill matched indirection structure */
       ++ procngbnum)
    commptr->proccoarmulttab[procngbnum + 1] = commptr->proccoarmulttab[procngbnum]
      + MIN(finegraph->procrcvtab[finegraph->procngbtab[procngbnum]],
	    finegraph->procsndtab[finegraph->procngbtab[procngbnum]]);

#ifdef SCOTCH_DEBUG_DGRAPH2
  if ((finegraph->procngbnbr > 0) &&
      (commptr->proccoarmulttab[finegraph->procngbnbr - 1] - commptr->proccoarmulttab[0] > procrcvnbr)) {
    *(int*)NULL= 0;
    errorPrint ("dgraphMatchSyncCommInit: internal error (1)");
    return (1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ; /* Compute size */
       ++ procngbnum) {
    procnum = finegraph->procngbtab[procngbnum];
    commptr->comsndtab[procngbnum].maxsize = finegraph->procsndtab[procnum];
    commptr->comrcvtab[procngbnum].maxsize = finegraph->procrcvtab[procnum];
    commptr->requesttab[procngbnum] = MPI_REQUEST_NULL; /* Send */
    commptr->requesttab[finegraph->procngbnbr + procngbnum] = MPI_REQUEST_NULL; /* Recv */
  }

  commptr->comsndtab[0].begin =
  commptr->comsndtab[0].end = message;
  commptr->comrcvtab[0].begin =
  commptr->comrcvtab[0].end = message + finegraph->procsndnbr;
  for (procngbnum = 1 ; procngbnum < finegraph->procngbnbr ; /* Fill in indirections */
       ++ procngbnum) {
    procnum = finegraph->procngbtab[procngbnum - 1];
    commptr->comsndtab[procngbnum].begin =
    commptr->comsndtab[procngbnum].end =
      commptr->comsndtab[procngbnum - 1].begin + commptr->comsndtab[procngbnum - 1].maxsize;
    commptr->comrcvtab[procngbnum].begin =
    commptr->comrcvtab[procngbnum].end =
      commptr->comrcvtab[procngbnum - 1].begin + commptr->comrcvtab[procngbnum - 1].maxsize;
  }

  /* Create Datatype for communications */
  MPI_Address(&message->finevertdstnum, disp);
  MPI_Address(&message->finevertsrcnum, disp+1);
  MPI_Address(&message->coarvertnum, disp+2);
  MPI_Address(message + 1, disp+3);
  for (i = 3 ; i >= 0 ; --i ) disp[i] -= disp[0]; /* Compute relative addresses */
  MPI_Type_struct (4, blocklen, disp, type, &commptr->datatype);
  MPI_Type_commit (&commptr->datatype);           /* Validate structure */

    /* Initialize ghost to global tab */
  dgraphMatchConvertInit (finegraph, &commptr->convertstruct);

  commptr->verttosndtab[0] = shared->verttosndtab;          /* Structure to stock moving vertices */
  for (procnum = 1; procnum < finegraph->procglbnbr; procnum ++)
    commptr->verttosndtab[procnum] = commptr->verttosndtab[procnum - 1] + finegraph->procrcvtab[procnum - 1];

  return (0);
}

void
dgraphMatchSyncCommExit (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr) {
  dgraphMatchConvertExit (finegraph, &commptr->convertstruct); /* Free memory used to convert gst and glb numbers */
  if ((commptr->method != SIMPLE_SYNC))
    memFree (commptr->colortab);
  memFree (commptr->comsndtab);               /* Free memory group leader */
}

int
dgraphMatchSyncCommDo (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
Gnum                         * nonmatenbr,
DgraphMatchSyncQueue * restrict nonmatequeue)
{
  int procngbnum;

  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ; ++procngbnum) {
    commptr->comsndtab[procngbnum].end = commptr->comsndtab[procngbnum].begin;
    commptr->comrcvtab[procngbnum].end = commptr->comrcvtab[procngbnum].begin;
#ifdef SCOTCH_DEBUG_DGRAPH2
    commptr->requesttab[procngbnum] = MPI_REQUEST_NULL;
    commptr->requesttab[procngbnum + finegraph->procngbnbr] = MPI_REQUEST_NULL;
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  }

  switch (commptr->method) {
  case LUBY_SYNC:
  case LUBY_SIMPLE_SYNC:
    return (dgraphMatchSyncLubyExchange (finegraph, commptr, shared, nonmatenbr, nonmatequeue));
  case SIMPLE_SYNC:
  case PREPAR_SYNC:
    return (dgraphMatchSyncSimpleExchange (finegraph, commptr, shared, nonmatenbr, nonmatequeue));
#ifdef SCOTCH_DEBUG_DGRAPH2
  default:
    errorPrint ("dgraphMatchSyncCommDo: param error");
    return (1);
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  }

  return (0);
}

int
dgraphMatchSyncLubyExchange (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
Gnum                         *  nonmatenbr,
DgraphMatchSyncQueue * restrict nonmatequeue)
{
  int currentcolor;
  int colornum;
  DgraphCoarsenMulti * coarmultptr;
  int flag;
  MPI_Status status;
  int nbrrequests = 0;
  int index;
  int procngbnum;
  Gnum finevertlocnum;
  Gnum                  finevertfrstnum;          /* Shortcut to finegraph->procvrttab[proclocnum] */
  Gnum                  finevertlastnum;          /* Shortcut to finegraph->procvrttab[proclocnum + 1] */
  Gnum                  baseval;
  DgraphMatchSyncMessage * message;

  finevertfrstnum = finegraph->procvrttab[finegraph->proclocnum];
  finevertlastnum = finegraph->procvrttab[finegraph->proclocnum + 1];
  baseval = finegraph->baseval;

  for (colornum = 0 ; colornum <= commptr->nbrcolor ; ++ colornum) {
    MPI_Barrier (finegraph->proccomm);
    currentcolor = (colornum + commptr->begcolor)%(commptr->nbrcolor+1);

    if (currentcolor == commptr->mycolor) {            /* We are the sender */
      for (finevertlocnum = finegraph->vertlocnnd, procngbnum = 0 ; finevertlocnum < finegraph->vertgstnnd ;
           ++ finevertlocnum) {
        Gnum finevertglbnum;

        if (shared->finecoartax[finevertlocnum] >= -1) /* Already matched */
          continue;

        coarmultptr = commptr->coarmultgsttab - shared->finecoartax[finevertlocnum]; /* finecoartax is negative ! */
        finevertglbnum = dgraphMatchVertGst2Glb(&commptr->convertstruct, finevertlocnum);
#ifdef SCOTCH_DEBUG_DGRAPH2
        if (dgraphVertexLocal(finegraph, finevertglbnum)) {
          errorPrint ("dgraphMatchSyncLubyExchange: internal error (1)");
          return (1);
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
        while ((finegraph->procvrttab[finegraph->procngbtab[procngbnum]] > finevertglbnum) ||
               (finegraph->procvrttab[finegraph->procngbtab[procngbnum] + 1] <= finevertglbnum)) {
#ifdef SCOTCH_DEBUG_DGRAPH2
          if (procngbnum == finegraph->procngbnbr) {
            errorPrint ("dgraphMatchSyncLubyExchange: internal error (2)");
            return (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
          ++ procngbnum;
        }
#ifdef SCOTCH_DEBUG_DGRAPH2
        if ((commptr->comsndtab[procngbnum].end - commptr->comsndtab[procngbnum].begin >= commptr->comsndtab[procngbnum].maxsize) ||
            (dgraphVertexProc (finegraph, finevertglbnum) != finegraph->procngbtab[procngbnum])) {
          errorPrint ("dgraphMatchSyncLubyExchange: internal error (3)");
          return (1);
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

        message = commptr->comsndtab[procngbnum].end ++;
        message->finevertdstnum = finevertglbnum;
        message->finevertsrcnum =
          (coarmultptr->vertnum[0] != message->finevertdstnum)?(coarmultptr->vertnum[0]):(coarmultptr->vertnum[1]);
        message->degree = finegraph->vendloctax[message->finevertsrcnum - finevertfrstnum + baseval]
          - finegraph->vertloctax[message->finevertsrcnum - finevertfrstnum + baseval];
#ifdef  SCOTCH_DEBUG_DGRAPH2
        if (! dgraphVertexLocal (finegraph, message->finevertsrcnum)) {
          errorPrint ("dgraphMatchSyncLubyExchange: internal error (4)");
          return (1);
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      }
      for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ;  ++ procngbnum) {
        int messagesize;

        messagesize = commptr->comsndtab[procngbnum].end - commptr->comsndtab[procngbnum].begin;
        MPI_Isend(commptr->comsndtab[procngbnum].begin, messagesize,
                  commptr->datatype, finegraph->procngbtab[procngbnum],
                  TAGCOARINFO, finegraph->proccomm, &commptr->requesttab[procngbnum]);
        MPI_Test (&commptr->requesttab[procngbnum], &flag, MPI_STATUS_IGNORE);  /* Validate Send */
        MPI_Irecv(commptr->comrcvtab[procngbnum].begin, messagesize,       /* Post recv for answers */
                  commptr->datatype, finegraph->procngbtab[procngbnum],
                  TAGCOARANSWER, finegraph->proccomm, &commptr->requesttab[procngbnum+finegraph->procngbnbr]);
        nbrrequests += 2;
      }
    }
    else {                                    /* We are waiting info from the good colored procs */
      for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ;  ++ procngbnum) {
        if (commptr->colortab[procngbnum] != currentcolor) {/* We wait only for processors which can send us datas */
	  commptr->requesttab[procngbnum] = MPI_REQUEST_NULL;
	  commptr->requesttab[procngbnum + finegraph->procngbnbr] = MPI_REQUEST_NULL;
          continue;
	}
        MPI_Irecv(commptr->comrcvtab[procngbnum].begin, commptr->comrcvtab[procngbnum].maxsize, /* Post recv for info */
                  commptr->datatype, finegraph->procngbtab[procngbnum],
                  TAGCOARINFO, finegraph->proccomm, &commptr->requesttab[procngbnum + finegraph->procngbnbr]);
        commptr->requesttab[procngbnum] = MPI_REQUEST_NULL;
        nbrrequests ++;
      }
    }
    while (nbrrequests > 0) {
      int count;
      int procnum;

#ifdef SCOTCH_DETERMINIST
      MPI_Status * statuses;

      if ((statuses = (MPI_Status*)memAlloc(2 * finegraph->procngbnbr * sizeof(MPI_Status)))
	  == NULL) {
	errorPrint ("dgraphMatchSyncLubyExchange : allocation error (1)");
	return     (1);
      }
      MPI_Waitall (2 * finegraph->procngbnbr, commptr->requesttab, statuses);
      for (index = 0 ; index < finegraph->procngbnbr ; ++index) {
	commptr->requesttab[index] = MPI_REQUEST_NULL;
	if ((currentcolor != commptr->mycolor)
	    && (commptr->colortab[index] != currentcolor)) {
	  continue;
	}
	if (MPI_Get_count(&statuses[index + finegraph->procngbnbr], commptr->datatype, &count) != MPI_SUCCESS) {
	  errorPrint ("dgraphMatchSyncLubyExchange: internal error (5 b)");
	  return     (1);
	}

#else /* SCOTCH_DETERMINIST */
      MPI_Waitany (2 * finegraph->procngbnbr, commptr->requesttab, &index, &status);
      nbrrequests --;

      if (index < finegraph->procngbnbr) {        /* A send is completed */
        continue;
      }

      if (MPI_Get_count(&status, commptr->datatype, &count) != MPI_SUCCESS) {
        errorPrint ("dgraphMatchSyncLubyExchange: internal error (5)");
        return     (1);
      }
      index -= finegraph->procngbnbr;
#endif /* SCOTCH_DETERMINIST */
      if (commptr->mycolor == currentcolor) {          /* The communication is an answer */
        commptr->comrcvtab[index].end = commptr->comrcvtab[index].begin + count;
        for (message=commptr->comrcvtab[index].begin ;
             message < commptr->comrcvtab[index].end ; ++ message) {
          Gnum finevertlocnum;
          Gnum finevertgstnum;

          finevertlocnum = message->finevertsrcnum - finevertfrstnum + baseval;
          finevertgstnum = dgraphMatchVertGlb2Gst(finegraph, message->finevertdstnum,
                                                  finevertlocnum, &commptr->convertstruct);
          shared->finecoartax[finevertgstnum] = message->coarvertnum;
#ifdef SCOTCH_DEBUG_DGRAPH2
          if (shared->finecoartax[finevertlocnum] >= 0) {
            errorPrint ("dgraphMatchSyncLubyExchange: internal error (6)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
          shared->finecoartax[finevertlocnum] = message->coarvertnum;

#ifdef SCOTCH_DEBUG_DGRAPH2
          if (message->coarvertnum < -1) {
            errorPrint ("dgraphMatchSyncLubyExchange: internal error (7)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
          if (message->coarvertnum >= 0) {    /* Matching is done */
            procnum = finegraph->procngbtab[index];
            shared->finefoldtax[finevertgstnum] = procnum;
            shared->finefoldtax[finevertlocnum] = procnum;
            commptr->verttosndtab[procnum][shared->vertsndgsttab[procnum]] = finevertlocnum; /* Put vertex in the outgoing tab ! */
            memCpy (commptr->proccoarmulttab[index] + shared->vertsndgsttab[procnum], /* Keep Data for next communications */
                    message, sizeof (DgraphMatchSyncMessage));
            shared->vertsndgsttab[procnum] ++;
/*             shared->edgesndgsttab[procnum] +=  /\* Recomputed to avoid increasing communication size *\/ */
/*               (finegraph->vendloctax[finevertlocnum] - finegraph->vertloctax[finevertlocnum]); */

          }
          else {                              /* Matching is not done */
            shared->finefoldtax[finevertgstnum] = -1;
            shared->finefoldtax[finevertlocnum] = -1;
            dgraphMatchSyncQueuePut(nonmatequeue, message->finevertsrcnum);
            (*nonmatenbr) ++;
          }
        }
      }
      else {                                  /* Communication is a request */
        commptr->comrcvtab[index].end = commptr->comrcvtab[index].begin + count;
        for (message=commptr->comrcvtab[index].begin;
             message < commptr->comrcvtab[index].end ; ++ message) {
          Gnum finevertlocnum;
          Gnum coarmultnum;
          Gnum finevertgstnum;
          Gnum finevertnum;

#ifdef SCOTCH_DEBUG_DGRAPH2
          if ((!dgraphVertexLocal (finegraph, message->finevertdstnum))
              || (dgraphVertexProc (finegraph, message->finevertsrcnum) != finegraph->procngbtab[index])) {
            errorPrint ("dgraphMatchSyncLubyExchange: internal error (8)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
          finevertlocnum = message->finevertdstnum - finevertfrstnum + baseval;
          finevertgstnum = dgraphMatchVertGlb2Gst(finegraph,message->finevertsrcnum,
                                                  finevertlocnum,
                                                  &commptr->convertstruct);
          coarmultnum = shared->finecoartax[finevertlocnum];
          if (coarmultnum >= 0) {             /* Already Matched */
            message->coarvertnum = -1;        /* Abort this matching */
            continue;
          }
          if (coarmultnum < -1) {             /* Vertex is locked */
            finevertnum = commptr->coarmultgsttab[-coarmultnum].vertnum[0];
            if (finevertnum == message->finevertdstnum)
              finevertnum = commptr->coarmultgsttab[-coarmultnum].vertnum[1];
            finevertnum = dgraphMatchVertGlb2Gst(finegraph, finevertnum,
                                                 finevertlocnum,
                                                 &commptr->convertstruct);
            shared->finefoldtax[finevertnum] = -1; /* Mark other vertex as available */
            shared->finecoartax[finevertnum] = -1;
          }

          coarmultnum = shared->finecoartax[finevertgstnum];
#ifdef SCOTCH_DEBUG_DGRAPH2
          if (coarmultnum >= 0) {             /* Already Matched */
            errorPrint ("dgraphMatchSyncLubyExchange: internal error (9)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
          if (coarmultnum < -1) {             /* Vertex is locked */
            finevertnum = commptr->coarmultgsttab[-coarmultnum].vertnum[0];
            if (finevertnum == message->finevertsrcnum)
              finevertnum = commptr->coarmultgsttab[-coarmultnum].vertnum[1];
            shared->finefoldtax[finevertnum - finevertfrstnum + baseval] = -1; /* Mark other vertex as available */
            shared->finecoartax[finevertnum - finevertfrstnum + baseval] = -1;
            dgraphMatchSyncQueuePut(nonmatequeue, finevertnum);
            (*nonmatenbr) ++;
          }

          shared->finefoldtax[finevertlocnum] = finegraph->proclocnum;
          shared->finefoldtax[finevertgstnum] = finegraph->proclocnum;
          procnum = finegraph->procngbtab[index];

              /* Create a new multinode */
/*           shared->coarmulttax[shared->coarmultnum].vertnum[0] = message->finevertsrcnum; */
/*           shared->coarmulttax[shared->coarmultnum].vertnum[1] = message->finevertdstnum; /\* Build new multinode *\/ */
          shared->finecoartax[finevertlocnum] = shared->coarmultnum;/* Point to coarse vertex */
          shared->finecoartax[finevertgstnum] = shared->coarmultnum;
          memCpy (commptr->proccoarmulttab[index] + shared->vertsndgsttab[procnum], /* Keep Data for next communications */
                  message, sizeof (DgraphMatchSyncMessage));
          commptr->verttosndtab[procnum][shared->vertsndgsttab[procnum]] = finevertlocnum; /* Put vertex in the outgoing tab ! */
          shared->vertsndgsttab[procnum] ++;
          message->degree = (finegraph->vendloctax[finevertlocnum] - finegraph->vertloctax[finevertlocnum]);
/*           shared->vertrcvgsttab[finegraph->procngbtab[index]] ++;  /\* Precompute amount of data exchanged *\/ */
/*           shared->edgercvgsttab[finegraph->procngbtab[index]] += message->degree; */
        }
        MPI_Isend(commptr->comrcvtab[index].begin, count, /* comrcvtab since we do inplace answer */
                  commptr->datatype, finegraph->procngbtab[index],
                  TAGCOARANSWER, finegraph->proccomm, &commptr->requesttab[index]);
        nbrrequests ++;
      }
#ifdef SCOTCH_DETERMINIST
      }
      nbrrequests = 0;
      memFree (statuses);
      MPI_Waitall (finegraph->procngbnbr, commptr->requesttab, MPI_STATUSES_IGNORE);
#endif /* SCOTCH_DETERMINIST */
    }
  }                                           /* End of color loop */
  return (0);
}

/* A ParMetis-like exchange function */

int
dgraphMatchSyncSimpleExchange (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
Gnum                         *  nonmatenbr,
DgraphMatchSyncQueue * restrict nonmatequeue)
{
  DgraphCoarsenMulti * coarmultptr;
  int flag;
  int procngbnum;
  Gnum finevertlocnum;
  Gnum                      finevertfrstnum;       /* Shortcut to finegraph->procvrttab[proclocnum] */
  Gnum                      finevertlastnum;       /* Shortcut to finegraph->procvrttab[proclocnum + 1] */
  Gnum                      baseval;
  DgraphMatchSyncMessage  * message;
  int            * restrict ordertab;              /* In which order our neighbors must be treated */
  MPI_Status     * restrict statustab;
  DgraphMatchSyncCommProc * comsndtab;            /* Shortcut to commptr->com*tab to do in place answer */
  DgraphMatchSyncCommProc * comrcvtab;
  int                       i;

  finevertfrstnum = finegraph->procvrttab[finegraph->proclocnum];
  finevertlastnum = finegraph->procvrttab[finegraph->proclocnum + 1];
  baseval = finegraph->baseval;

  if (memAllocGroup ((void **) (void *)
                     &ordertab,  (size_t) (finegraph->procngbnbr     * sizeof (int)),
                     &statustab, (size_t) (finegraph->procngbnbr * 2 * sizeof (MPI_Status)), NULL) == NULL) {
    errorPrint ("dgraphMatchSyncSimpleExchange: out of memory (1)");
    return     (1);
  }

  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ; ++ procngbnum)
    ordertab[procngbnum] = procngbnum;

  intPerm (ordertab, finegraph->procngbnbr);

  for (finevertlocnum = finegraph->vertlocnnd, procngbnum = 0;
       finevertlocnum < finegraph->vertgstnnd; finevertlocnum ++) {
    Gnum                  finevertglbnum;

    if (shared->finecoartax[finevertlocnum] >= -1) /* Already matched */
      continue;

    coarmultptr = commptr->coarmultgsttab - shared->finecoartax[finevertlocnum]; /* finecoartax is negative ! */
    finevertglbnum = dgraphMatchVertGst2Glb (&commptr->convertstruct, finevertlocnum);
#ifdef SCOTCH_DEBUG_DGRAPH2
    if (dgraphVertexLocal (finegraph, finevertglbnum)) {
      errorPrint ("dgraphMatchSyncSimpleExchange: internal error (1)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
    while ((finegraph->procvrttab[finegraph->procngbtab[procngbnum]]     >  finevertglbnum) ||
           (finegraph->procvrttab[finegraph->procngbtab[procngbnum] + 1] <= finevertglbnum)) {
#ifdef SCOTCH_DEBUG_DGRAPH2
      if (procngbnum == finegraph->procngbnbr) {
        errorPrint ("dgraphMatchSyncSimpleExchange: internal error (2)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      procngbnum ++;
    }
#ifdef SCOTCH_DEBUG_DGRAPH2
    if ((commptr->comsndtab[procngbnum].end - commptr->comsndtab[procngbnum].begin
         >= commptr->comsndtab[procngbnum].maxsize ) ||
        (dgraphVertexProc(finegraph, finevertglbnum) != finegraph->procngbtab[procngbnum])) {
      errorPrint ("dgraphMatchSyncSimpleExchange: internal error (3)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

    message = commptr->comsndtab[procngbnum].end ++;
    message->finevertdstnum = finevertglbnum;
    message->finevertsrcnum =
      (coarmultptr->vertnum[0] != message->finevertdstnum)?(coarmultptr->vertnum[0]):(coarmultptr->vertnum[1]);
    message->degree = finegraph->vendloctax[message->finevertsrcnum - finevertfrstnum + baseval]
      - finegraph->vertloctax[message->finevertsrcnum - finevertfrstnum + baseval];
#ifdef  SCOTCH_DEBUG_DGRAPH2
    if (! dgraphVertexLocal (finegraph, message->finevertsrcnum)) {
      errorPrint ("dgraphMatchSyncSimpleExchange: internal error (4)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  }

  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ;  ++ procngbnum) {
    MPI_Isend(commptr->comsndtab[procngbnum].begin, commptr->comsndtab[procngbnum].end - commptr->comsndtab[procngbnum].begin,
	      commptr->datatype, finegraph->procngbtab[procngbnum],
	      TAGCOARINFO, finegraph->proccomm, &commptr->requesttab[procngbnum]);
    MPI_Test (&commptr->requesttab[procngbnum], &flag, MPI_STATUS_IGNORE);  /* Validate Send */
    MPI_Irecv(commptr->comrcvtab[procngbnum].begin, commptr->comrcvtab[procngbnum].maxsize,/* Post recv for answers */
	      commptr->datatype, finegraph->procngbtab[procngbnum],
	      TAGCOARINFO, finegraph->proccomm, &commptr->requesttab[procngbnum+finegraph->procngbnbr]);
  }

  MPI_Waitall (2 * finegraph->procngbnbr, commptr->requesttab, statustab); /* Wait for all communications */

  comsndtab = commptr->comrcvtab;                 /* Do in place answers */
  comrcvtab = commptr->comsndtab;

  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ;  ++ procngbnum) { /* Post Receives */
    MPI_Irecv(comrcvtab[procngbnum].begin, comrcvtab[procngbnum].end - comrcvtab[procngbnum].begin,
	      commptr->datatype, finegraph->procngbtab[procngbnum],
	      TAGCOARANSWER, finegraph->proccomm, &commptr->requesttab[procngbnum + finegraph->procngbnbr]);
  }

  for (i = 0 ; i < finegraph->procngbnbr ; ++ i) {/* Examine datas and do matching */
    int count;
    int procnum;

    procngbnum = ordertab[i];
    if (MPI_Get_count(&statustab[procngbnum + finegraph->procngbnbr], commptr->datatype, &count) !=  MPI_SUCCESS) {
      errorPrint ("dgraphMatchSyncSimpleExchange: internal error (5)");
      return     (1);
    }

    commptr->comrcvtab[procngbnum].end = commptr->comrcvtab[procngbnum].begin + count;
    procnum = finegraph->procngbtab[procngbnum];
    for (message=commptr->comrcvtab[procngbnum].begin;
         message < commptr->comrcvtab[procngbnum].end ; ++ message) {
      Gnum finevertlocnum;
      Gnum coarmultnum;
      Gnum finevertgstnum;

#ifdef SCOTCH_DEBUG_DGRAPH2
      if ((!dgraphVertexLocal (finegraph, message->finevertdstnum))
          || (message->finevertsrcnum < finegraph->procvrttab[finegraph->procngbtab[procngbnum]])
	  || (message->finevertsrcnum >= finegraph->procvrttab[finegraph->procngbtab[procngbnum] + 1])) {
        errorPrint ("dgraphMatchSyncSimpleExchange : internal error (6)");
        return (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      finevertlocnum = message->finevertdstnum - finevertfrstnum + baseval;
      coarmultnum = shared->finecoartax[finevertlocnum];
      if (coarmultnum > -1) {
        message->coarvertnum = -1;        /* Abort this matching */
        continue;
      }

      if ((coarmultnum < -1)                      /* If we want the same matching */
          && ((commptr->coarmultgsttab[-coarmultnum].vertnum[1] != message->finevertsrcnum)
              || (procnum < finegraph->proclocnum))) { /* and we are not prioritary */
        message->coarvertnum = -1;        /* Abort this matching */
        continue;
      }
      finevertgstnum = dgraphMatchVertGlb2Gst(finegraph,message->finevertsrcnum,
                                              finevertlocnum,
                                              &commptr->convertstruct);

      shared->finefoldtax[finevertlocnum] = finegraph->proclocnum;
      shared->finefoldtax[finevertgstnum] = finegraph->proclocnum;

      shared->finecoartax[finevertlocnum] = message->degree;  /* To keep information */
      shared->finecoartax[finevertgstnum] = message->degree;
      if (procnum < finegraph->proclocnum) { /* To keep pre-matched vertices in the same order than other proc */
        memCpy (commptr->proccoarmulttab[procngbnum] + shared->vertsndgsttab[procnum], /* Keep Data for next communications */
                message, sizeof (DgraphMatchSyncMessage));
        commptr->verttosndtab[procnum][shared->vertsndgsttab[procnum]] = finevertlocnum; /* Put vertex in the outgoing tab ! */
        shared->vertsndgsttab[procnum] ++;
      }
      message->degree = (finegraph->vendloctax[finevertlocnum] - finegraph->vertloctax[finevertlocnum]);
    }
    MPI_Isend(comsndtab[procngbnum].begin, count, /* comrcvtab since we do inplace answer */
	      commptr->datatype, finegraph->procngbtab[procngbnum],
	      TAGCOARANSWER, finegraph->proccomm, &commptr->requesttab[procngbnum]);
    MPI_Test (&commptr->requesttab[procngbnum], &flag, MPI_STATUS_IGNORE);  /* Validate Send */
    comsndtab[procngbnum].end = comsndtab[procngbnum].begin + count;
  }

  memFree (ordertab); /* Free memory group leader */

  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ; ++ procngbnum) {
    int index;
    int count;
    MPI_Status status;
    int procnum;

    MPI_Waitany (finegraph->procngbnbr, commptr->requesttab + finegraph->procngbnbr, &index, &status); /* Wait for an answer */

    if (MPI_Get_count(&status, commptr->datatype, &count) !=  MPI_SUCCESS) {
      errorPrint ("dgraphMatchSyncSimpleExchange : internal error (7)");
      return (1);
    }

    comrcvtab[index].end = comrcvtab[index].begin + count;
    procnum = finegraph->procngbtab[index];
    for (message=comrcvtab[index].begin ;
         message < comrcvtab[index].end ; ++ message) {
      Gnum finevertlocnum;
      Gnum finevertgstnum;

      finevertlocnum = message->finevertsrcnum - finevertfrstnum + baseval;
      if (shared->finecoartax[finevertlocnum] > -1) /* Reciproque matching */
        continue;

      finevertgstnum = dgraphMatchVertGlb2Gst(finegraph, message->finevertdstnum,
                                              finevertlocnum, &commptr->convertstruct);

      shared->finecoartax[finevertgstnum] = message->coarvertnum;
      shared->finecoartax[finevertlocnum] = message->coarvertnum;

#ifdef SCOTCH_DEBUG_DGRAPH2
      if (message->coarvertnum < -1) {
        errorPrint ("dgraphMatchSyncSimpleExchange : internal error (9)");
        return (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      if (message->coarvertnum >= 0) {            /* Matching is done */
        shared->finefoldtax[finevertgstnum] = procnum;
        shared->finefoldtax[finevertlocnum] = procnum;
        memCpy (commptr->proccoarmulttab[index] + shared->vertsndgsttab[procnum], /* Keep Data for next communications */
                message, sizeof (DgraphMatchSyncMessage));
        commptr->verttosndtab[procnum][shared->vertsndgsttab[procnum]] = finevertlocnum; /* Put vertex in the outgoing tab ! */
        shared->vertsndgsttab[procnum] ++;
      }
      else {                                      /* Matching is not done */
        shared->finefoldtax[finevertgstnum] = -1;
        shared->finefoldtax[finevertlocnum] = -1;
        dgraphMatchSyncQueuePut(nonmatequeue, message->finevertsrcnum);
        (*nonmatenbr) ++;
      }
    }

    /* To keep same order of pre-matched tables on the 2 sides */
    if (procnum > finegraph->proclocnum) {
      for (message=comsndtab[index].begin ;
           message < comsndtab[index].end ; ++ message) {
        Gnum finevertlocnum;
        if (message->coarvertnum == -1)           /* We have refused matching */
          continue;
        finevertlocnum = message->finevertdstnum - finevertfrstnum + baseval;
#ifdef SCOTCH_DEBUG_DGRAPH2
	if (shared->finecoartax[finevertlocnum] <= -1) {
	  errorPrint ("dgraphMatchSyncSimpleExchange : internal error (10)");
	  return (1);
	}
#endif /* SCOTCH_DEBUG_DGRAPH2 */
        memCpy (commptr->proccoarmulttab[index] + shared->vertsndgsttab[procnum], /* Keep Data for next communications */
                message, sizeof (DgraphMatchSyncMessage));
	commptr->proccoarmulttab[index][shared->vertsndgsttab[procnum]].degree =  shared->finecoartax[finevertlocnum] ;  /* To keep information */
        commptr->verttosndtab[procnum][shared->vertsndgsttab[procnum]] = finevertlocnum; /* Put vertex in the outgoing tab ! */
        shared->vertsndgsttab[procnum] ++;
      }
    }
  }

  MPI_Waitall (finegraph->procngbnbr, commptr->requesttab, MPI_STATUSES_IGNORE); /* Wait for send communications */

  return (0);
}

int
dgraphMatchSyncBalance (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
int                            nonmatenbr)
{
  int procngbnum;
  int proclocnum;
  DgraphMatchSyncMessage * message;
  int offset;
  Gnum newcoarmultnum;
  Gnum finevertfrstnum;

  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ;  ++ procngbnum) {
    MPI_Irecv(commptr->comrcvtab[procngbnum].begin, shared->vertsndgsttab[finegraph->procngbtab[procngbnum]],/* Post recv for answers */
              GNUM_MPI, finegraph->procngbtab[procngbnum], /* GNUM_MPI to save communication volume */
              TAGCOARINFO, finegraph->proccomm, &commptr->requesttab[procngbnum+finegraph->procngbnbr]);
  }

  proclocnum = finegraph->proclocnum;
  finevertfrstnum = finegraph->procvrttab[proclocnum];
  offset = shared->coarmultnum + nonmatenbr;      /* Keep other vertices and hold them before exchanged */
  newcoarmultnum = offset;
  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ;  ++ procngbnum) {
    int procnum;
    int coarvertnnd;
    int coarvertbgn;
    int coarvertnum;
    int flag;
    Gnum *sndbuff;

    procnum = finegraph->procngbtab[procngbnum];
    coarvertnnd = shared->vertsndgsttab[procnum]/2;
    if (proclocnum < procnum)                     /* Have the first part */
      coarvertbgn = 0;
    else {                                        /* Have the second part */
      coarvertbgn = coarvertnnd;
      coarvertnnd = shared->vertsndgsttab[procnum];
    }

    shared->vertrcvgsttab[procnum] = 0;
    shared->edgercvgsttab[procnum] = 0;           /* Reinit values */
    for (coarvertnum = coarvertbgn, message = commptr->proccoarmulttab[procngbnum] + coarvertbgn,
           sndbuff = (Gnum*)(commptr->proccoarmulttab[procngbnum] + coarvertbgn);
         coarvertnum < coarvertnnd ; ++ coarvertnum, ++ message, ++sndbuff) { /* Loop of vertices to send */
      Gnum finevertlocnum;

      if (dgraphVertexLocal(finegraph, message->finevertdstnum)) { /* Ensure the order is good */
        Gnum swap;                                                 /* src = us */
        swap = message->finevertdstnum;                            /* dst = target */
        message->finevertdstnum = message->finevertsrcnum;
        message->finevertsrcnum = swap;
      }

#ifdef SCOTCH_DEBUG_DGRAPH2
      if ((message->finevertdstnum < finegraph->procvrttab[procnum])
	  || (message->finevertdstnum >= finegraph->procvrttab[procnum+1])
	  || (message->degree > finegraph->degrglbmax)
	  || (message->degree < 0)) {
	errorPrint ("dgraphMatchSyncBalance : internal error (1)");
	return (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

      finevertlocnum = message->finevertsrcnum - finevertfrstnum + finegraph->baseval;
      shared->finefoldtax[finevertlocnum] = proclocnum;
      shared->finecoartax[finevertlocnum] = newcoarmultnum;
      shared->vertrcvgsttab[procnum] ++;
      shared->edgercvgsttab[procnum] +=  message->degree;
      shared->coarmulttax[newcoarmultnum].vertnum[0] = message->finevertsrcnum;
      shared->coarmulttax[newcoarmultnum].vertnum[1] = message->finevertdstnum; /* Build new multinode */
      *sndbuff = newcoarmultnum;
      newcoarmultnum ++;
    }

    MPI_Isend(commptr->proccoarmulttab[procngbnum] + coarvertbgn, coarvertnnd - coarvertbgn,
	      GNUM_MPI, finegraph->procngbtab[procngbnum],
	      TAGCOARINFO, finegraph->proccomm, &commptr->requesttab[procngbnum]);
    MPI_Test (&commptr->requesttab[procngbnum], &flag, MPI_STATUS_IGNORE);  /* Validate Send */
  }

  /* Now take care of incoming vertices */
  for (procngbnum = 0 ; procngbnum < finegraph->procngbnbr ;  ++ procngbnum) {
    int coarvertnnd;
    int coarvertbgn;
    int procnum;
    int index;
    MPI_Status status;
    int count;
    Gnum * rcvbuf;
    Gnum * rcvnnd;

    MPI_Waitany (finegraph->procngbnbr, commptr->requesttab + finegraph->procngbnbr, &index, &status); /* Wait for an answer */

    if (MPI_Get_count(&status, GNUM_MPI, &count) !=  MPI_SUCCESS) {
      errorPrint ("dgraphMatchSyncBalance : internal error (2)");
      return (1);
    }

    procnum = finegraph->procngbtab[index];
    coarvertnnd = shared->vertsndgsttab[procnum]/2;
    if (proclocnum > procnum)                     /* Have the first part */
      coarvertbgn = 0;
    else {                                        /* Have the second part */
      coarvertbgn = coarvertnnd;
      coarvertnnd = shared->vertsndgsttab[procnum];
    }
    shared->vertsndgsttab[procnum] = 0;
    shared->edgesndgsttab[procnum] = 0;           /* Reinit values */

    rcvbuf = (Gnum *) commptr->comrcvtab[index].begin;
    rcvnnd = rcvbuf + count;
    for (message = commptr->proccoarmulttab[index] + coarvertbgn ;
         rcvbuf < rcvnnd ; ++ rcvbuf, ++message) {
      Gnum finevertlocnum;

      if (dgraphVertexLocal(finegraph, message->finevertsrcnum)) { /* Ensure the order is good */
        Gnum swap;                                                 /* dst = us */
        swap = message->finevertdstnum;                            /* src = target */
        message->finevertdstnum = message->finevertsrcnum;
        message->finevertsrcnum = swap;
      }

      finevertlocnum = message->finevertdstnum - finevertfrstnum + finegraph->baseval;
#ifdef SCOTCH_DEBUG_DGRAPH2
      if ((message->finevertdstnum < finevertfrstnum) ||
          (message->finevertdstnum >= finegraph->procvrttab[finegraph->proclocnum + 1]) ||
          (finegraph->vendloctax[finevertlocnum] - finegraph->vertloctax[finevertlocnum] > finegraph->degrglbmax)) {
        errorPrint ("dgraphMatchSyncBalance: internal error (3)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

      shared->finecoartax[finevertlocnum] = *rcvbuf;

      shared->finefoldtax[finevertlocnum] = procnum;
      commptr->verttosndtab[procnum][shared->vertsndgsttab[procnum]] = finevertlocnum; /* Put vertex in the outgoing tab ! */
      shared->vertsndgsttab[procnum] ++;
      shared->edgesndgsttab[procnum] +=           /* Recomputed to avoid increasing communication size */
        (finegraph->vendloctax[finevertlocnum] - finegraph->vertloctax[finevertlocnum]);
    }
  }

  /* Now take care about non-matched vertices */
  if (nonmatenbr != 0) {                          /* There are still non mated vertices */
    Gnum finevertlastnum;
    Gnum finevertnum;
    Gnum baseval;

    finevertlastnum = finegraph->procvrttab[proclocnum + 1];
    baseval = finegraph->baseval;
    for (finevertnum = finevertfrstnum; finevertnum < finevertlastnum ; finevertnum ++) {
      /* match each remaining non mated vertex to itself*/
      if (shared->finecoartax[finevertnum - finevertfrstnum + baseval] <= -1) {
        shared->finefoldtax[finevertnum - finevertfrstnum + baseval] = proclocnum;
        shared->finecoartax[finevertnum - finevertfrstnum + baseval] = shared->coarmultnum; /* Point to coarse vertex                        */
        shared->coarmulttax[shared->coarmultnum].vertnum[0] = finevertnum;
        shared->coarmulttax[shared->coarmultnum].vertnum[1] = finevertnum;  /* Build new multinode matching the vertex to itself           */
        shared->coarmultnum ++;
        nonmatenbr --;
      }
    }
  }

#ifdef SCOTCH_DEBUG_DGRAPH2
  if (offset != shared->coarmultnum) {
    errorPrint ("dgraphMatchSyncBalance : internal error (3)");
    return (1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  shared->coarmultnum = newcoarmultnum;
  MPI_Waitall (finegraph->procngbnbr, commptr->requesttab, MPI_STATUSES_IGNORE);
  return (0);
}
