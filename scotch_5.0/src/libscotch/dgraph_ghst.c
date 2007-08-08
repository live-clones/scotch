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
/**********************************************************/
/*                                                        */
/*   NAME       : dgraph_ghst.c                           */
/*                                                        */
/*   AUTHOR     : Francois PELLEGRINI                     */
/*                Francois CHATENET (P0.0)                */
/*                Sebastien FOUCAULT (P0.0)               */
/*                Nicolas GICQUEL (P0.1)                  */
/*                Jerome LACOSTE (P0.1)                   */
/*                                                        */
/*   FUNCTION   : Part of a parallel static mapper.       */
/*                This module contains the halo building  */
/*                routine.                                */
/*                                                        */
/*                # Version P0.0 : from : 01 apr 1997     */
/*                                 to     20 jun 1997     */
/*                # Version P0.1 : from : 14 apr 1998     */
/*                                 to     20 jun 1998     */
/*                # Version 5.0  : from : 28 feb 2006     */
/*                                 to     03 aug 2007     */
/*                                                        */
/**********************************************************/

/*
** The defines and includes.
*/

#define DGRAPH_GHST

#include "module.h"
#include "common.h"
#include "dgraph.h"
#include "dgraph_ghst.h"

/* This routine builds the ghost structures
** required by the halo routines.
*/

int
dgraphGhst (
Dgraph * restrict const     grafptr)              /* Graph structure */
{
  int                       procngbnbr;           /* Number of neighboring processes               */
  int                       procngbnum;           /* Number of current neighboring process         */
  int                       procgstmax;           /* Maximum number of ghost vertices per neighbor */
  int                       proclocnum;
  Gnum                      procsndnbr;
  int * restrict            procsidtab;           /* Send index array                              */
  int                       procsidnbr;           /* Number of entries in send index array         */
  Gnum                      vertsidnum;           /* Last vertex index in send index array         */
  Gnum                      vertlocmin;           /* Smallest index of local vertices              */
  Gnum                      vertlocmax;           /* Largest index of local vertices, + 1          */
  Gnum                      vertlocbas;           /* Base index for ghost edge array               */
  Gnum                      vertlocnum;           /* Current vertex number (based)                 */
  Gnum * restrict           vertsidtab;           /* Flag array for building procs(i|n)dtab        */
  Gnum                      vertgstnum;           /* Number of current ghost vertex                */
  DgraphGhstSort * restrict sortloctab;           /* Array for sorting ghost vertices              */
  DgraphGhstSort * restrict sortlocptr;           /* Array for sorting ghost vertices              */
  Gnum                      sortlocnbr;           /* Number of ghost edges in sort array           */
  Gnum                      sortlocnum;
  int                       reduloctab[3];
  int                       reduglbtab[3];
  int                       cheklocval;

  if ((grafptr->flagval & DGRAPHHASEDGEGST) != 0) /* If ghost edge array already computed, do nothing */
    return (0);

  cheklocval = 0;
  if (grafptr->edgegsttax == NULL) {              /* If ghost edge array not allocated yet */
    if ((grafptr->edgegsttax = (Gnum *) memAlloc (grafptr->edgelocsiz * sizeof (Gnum))) == NULL) {
      errorPrint ("dgraphGhst: out of memory (1)");
      cheklocval = 1;
    }
    else {
      grafptr->edgegsttax -= grafptr->baseval;
      grafptr->flagval |= DGRAPHFREEEDGEGST;      /* Free array on exit */
    }
  }
  if ((cheklocval == 0) &&
      (memAllocGroup ((void **)
                      &procsidtab, (size_t) ((grafptr->edgelocnbr + grafptr->vertlocnbr) * sizeof (int)),
                      &vertsidtab, (size_t) (grafptr->procglbnbr                         * sizeof (Gnum)),
                      &sortloctab, (size_t) ((grafptr->edgelocnbr + 1)                   * sizeof (DgraphGhstSort)), NULL) == NULL)) {
    errorPrint ("dgraphGhst: out of memory (2)");
    cheklocval = 1;
  }

  reduloctab[0] = 1;                              /* Assume memory error and prepare data for aborting */
  reduloctab[1] =
  reduloctab[2] = 0;
  if (cheklocval != 0) {
    if (MPI_Allreduce (reduloctab, reduglbtab, 3, MPI_INT, MPI_MAX, grafptr->proccomm) != MPI_SUCCESS)
      errorPrint ("dgraphGhst: communication error (1)");
    return (1);
  }

  vertlocmin = grafptr->procvrttab[grafptr->proclocnum];
  vertlocmax = grafptr->procvrttab[grafptr->proclocnum + 1];
  vertlocbas = vertlocmin - grafptr->baseval;
  memSet (grafptr->procrcvtab, 0, grafptr->procglbnbr * sizeof (int));
  memSet (grafptr->procsndtab, 0, grafptr->procglbnbr * sizeof (int));
  memSet (vertsidtab,         ~0, grafptr->procglbnbr * sizeof (Gnum));

  for (vertlocnum = vertsidnum = grafptr->baseval, sortlocptr = sortloctab, procsidnbr = 0;
       vertlocnum < grafptr->vertlocnnd; vertlocnum ++) {
    Gnum   edgelocnum;

    for (edgelocnum = grafptr->vertloctax[vertlocnum];
         edgelocnum < grafptr->vendloctax[vertlocnum]; edgelocnum ++) {
      Gnum   vertlocend;

      vertlocend = grafptr->edgeloctax[edgelocnum];
#ifdef SCOTCH_DEBUG_HDGRAPH2
      if ((vertlocend < grafptr->baseval) || (vertlocend >= (grafptr->procvrttab[grafptr->procglbnbr]))) {
        errorPrint  ("dgraphGhst: invalid edge array");
        if (MPI_Allreduce (reduloctab, reduglbtab, 3, MPI_INT, MPI_MAX, grafptr->proccomm) != MPI_SUCCESS)
          errorPrint ("dgraphGhst: communication error (2)");
        memFree (procsidtab);                     /* Free group leader */
        return  (1);
      }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

      if ((vertlocend >= vertlocmin) && (vertlocend < vertlocmax)) /* If edge is local */
        grafptr->edgegsttax[edgelocnum] = vertlocend - vertlocbas; /* Adjust its index */
      else {                                      /* End vertex is not local           */
        int               procngbmin;
        int               procngbmax;

        sortlocptr->vertglbnum = vertlocend;      /* Add it to sort array */
        sortlocptr->edgegstnum = edgelocnum;
        sortlocptr ++;

        for (procngbmin = 0, procngbmax = grafptr->procglbnbr;
             procngbmax - procngbmin > 1; ) {
          procngbnum = (procngbmax + procngbmin) / 2;
          if (grafptr->procvrttab[procngbnum] <= vertlocend)
            procngbmin = procngbnum;
          else
            procngbmax = procngbnum;
        }

        if (vertsidtab[procngbmin] != vertlocnum) { /* If vertex not already sent to process  */
          vertsidtab[procngbmin] = vertlocnum;    /* Neighbor process will receive vertex     */
          grafptr->procsndtab[procngbmin] ++;     /* One more vertex to send to this neighbor */

          while ((vertlocnum - vertsidnum) >= DGRAPHGHSTSIDMAX) { /* If Gnum range too long for int */
            procsidtab[procsidnbr ++] = -DGRAPHGHSTSIDMAX; /* Decrease by maximum int distance      */
            vertsidnum               += DGRAPHGHSTSIDMAX;
          }
          if (vertsidnum != vertlocnum) {         /* If communication concerns new local vertex   */
            procsidtab[procsidnbr ++] = - (vertlocnum - vertsidnum); /* Encode jump in procsidtab */
            vertsidnum = vertlocnum;              /* Current local vertex is last send vertex     */
          }
          procsidtab[procsidnbr ++] = procngbmin; /* Send this vertex data to this processor */
        }
      }
    }
  }

  sortlocnbr = sortlocptr - sortloctab;
  vertgstnum = grafptr->vertlocnnd;               /* No ghost vertices yet     */
  procngbnbr = 0;                                 /* No neighbor processes yet */
  procsndnbr = 0;                                 /* No vertex to send yet     */

  if (sortlocnbr > 0) {                           /* If there are ghost vertices    */
    Gnum                vertgstbas;               /* Number of current ghost vertex */

    intSort2asc1 (sortloctab, sortlocnbr);        /* Sort them by ascending end vertex */

    sortlocnum = 0;                               /* Start adjacency search from beginning                        */
    procngbnum = -1;                              /* Start neighbor search from begnning                          */
    do {                                          /* For each distinct neighbor process                           */
      vertgstbas = vertgstnum;                    /* Record first ghost number used for it                        */
      grafptr->edgegsttax[sortloctab[sortlocnum].edgegstnum] = vertgstnum; /* First ghost is always allocated     */
      while (grafptr->procvrttab[++ procngbnum + 1] <= sortloctab[sortlocnum].vertglbnum) { /* Find owner process */
#ifdef SCOTCH_DEBUG_DGRAPH2
        if ((procngbnum > grafptr->procglbnbr) || /* If we have skipped a neighbor to which we have to send something */
            (grafptr->procsndtab[procngbnum] != 0)) {
          errorPrint ("dgraphGhst: internal error (1)");
          if (MPI_Allreduce (reduloctab, reduglbtab, 3, MPI_INT, MPI_MAX, grafptr->proccomm) != MPI_SUCCESS)
            errorPrint ("dgraphGhst: communication error (3)");
          memFree (procsidtab);                   /* Free group leader */
          return (1);
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      }
#ifdef SCOTCH_DEBUG_DGRAPH2
      if (grafptr->procsndtab[procngbnum] == 0) { /* If we had in fact no edges to send to this neighbor */
        errorPrint ("dgraphGhst: internal error (2)");
        if (MPI_Allreduce (reduloctab, reduglbtab, 3, MPI_INT, MPI_MAX, grafptr->proccomm) != MPI_SUCCESS)
          errorPrint ("dgraphGhst: communication error (4)");
        return (1);
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      procsndnbr += grafptr->procsndtab[procngbnum]; /* Sum-up vertices to send            */
      grafptr->procngbtab[procngbnbr ++] = procngbnum; /* Add it to neighbor process array */
      while (++ sortlocnum < sortlocnbr) {        /* For all following ghost edges         */
        if (sortloctab[sortlocnum].vertglbnum !=  /* If new ghost vertex                   */
            sortloctab[sortlocnum - 1].vertglbnum) {
          vertgstnum ++;                          /* Allocate new ghost vertex number */
          if (grafptr->procvrttab[procngbnum + 1] <= sortloctab[sortlocnum].vertglbnum) { /* If new neighbor */
            grafptr->procrcvtab[procngbnum] = vertgstnum - vertgstbas; /* Sum-up data for old neighbor       */
            break;                                /* Process new neighbor                                    */
          }
        }
        grafptr->edgegsttax[sortloctab[sortlocnum].edgegstnum] = vertgstnum; /* Allocate ghost */
      }
    } while (sortlocnum < sortlocnbr);
    vertgstnum ++;                                /* Size is one above last number              */
    grafptr->procrcvtab[procngbnum] = vertgstnum - vertgstbas; /* Sum-up data for last neighbor */
  }

  grafptr->vertgstnbr = vertgstnum - grafptr->baseval;
  grafptr->vertgstnnd = grafptr->vertgstnbr + grafptr->baseval;
  grafptr->procngbnbr = procngbnbr;
  grafptr->procsndnbr = procsndnbr;

  grafptr->procsidtab = memRealloc (procsidtab, procsidnbr * sizeof (int)); /* Reallocate send index array */
  grafptr->procsidnbr = procsidnbr;

  for (proclocnum = 0, procgstmax = 1; proclocnum < grafptr->procglbnbr; proclocnum ++) {
    if (grafptr->proccnttab[proclocnum] > procgstmax)
      procgstmax = grafptr->proccnttab[proclocnum];
  }

  reduloctab[0] = 0;                              /* No memory error                            */
  reduloctab[1] = grafptr->procngbnbr;            /* Set maximum number of neighbors            */
  reduloctab[2] = procgstmax;                     /* Set maximum number of ghosts per neighbors */
  if (MPI_Allreduce (reduloctab, reduglbtab, 3, MPI_INT, MPI_MAX, grafptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphGhst: communication error (5)");
    return     (1);
  }
  if (reduglbtab[0] != 0)                         /* If error, propagated by some previous reduction operator */
    return (1);

  grafptr->flagval   |= DGRAPHHASEDGEGST;         /* Graph now has a valid ghost edge array */
  grafptr->procngbmax = reduglbtab[1];
  grafptr->procgstmax = reduglbtab[2];

  return (0);
}
