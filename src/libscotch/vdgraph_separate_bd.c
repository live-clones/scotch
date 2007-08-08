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
/**   NAME       : vdgraph_separate_bd.c                   **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This module computes a separator of the **/
/**                given distributed separator graph by    **/
/**                creating a band graph of given witdh    **/
/**                around the current separator, computing **/
/**                an improved separator of the band       **/
/**                graph, and projecting back the obtained **/
/**                separator in the original graph.        **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 04 mar 2006     **/
/**                                 to   : 04 sep 2006     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VDGRAPH_SEPARATE_BD

#include "module.h"
#include "common.h"
#include "parser.h"
#include "dgraph.h"
#include "vdgraph.h"
#include "vdgraph_separate_bd.h"
#include "vdgraph_separate_st.h"

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine computes a distributed band graph
** of given width around the current separator and
** applies distributed separation routines to it.
** The distributed graph is not guaranteed to be
** balanced at at all.
** It returns:
** - 0   : if the bipartitioning could be computed.
** - !0  : on error.
*/

int
vdgraphSeparateBd (
Vdgraph * const                       grafptr,    /*+ Distributed graph +*/
const VdgraphSeparateBdParam * const  paraptr)    /*+ Method parameters +*/
{
  Vdgraph                 bandgrafdat;            /* Vertex separator band graph structure                     */
  Gnum                    bandvertlocnbr;         /* Number of local band vertices (including anchor vertices) */
  Gnum                    bandvertlocnnd;         /* End of local band vertex array, (without anchor vertices) */
  Gnum                    bandvertlvlnum;         /* Index of first band vertex belonging to last level        */
  Gnum                    bandvertlocnum;
  Gnum                    bandvelolocsum;
  Gnum                    bandvelolocsum1;
  Gnum                    bandedgelocnbr;         /* Number of local edges in band graph                       */
  Gnum                    bandedgelocnum;
  Gnum                    bandedgeloctmp;
  Gnum * restrict         bandvnumgsttax;         /* Indices of selected band vertices in band graph           */
  Gnum                    bandvertlocnbr1;        /* Number of band graph vertices in part 1 except anchor 1   */
  Gnum                    bandvertlocadj;         /* Ajust value for local-to-global band vertex indices       */
  Gnum                    bandvertlocancadj;      /* Flag set when anchor(s) represent unexistent vertices     */
  Gnum                    bandvertglbancadj;      /* Global adjustment of anchor vertices                      */
  Gnum                    banddegrlocmax;
  Gnum                    complocsizeadj0;
  Gnum                    complocsizeadj1;
  Gnum * restrict         mesgbuftab;             /* Global array holding send and receive message buffers     */
  Gnum ** restrict        mesgptrtab;             /* Array of pointers to first free zone in send arrays       */
  int                     mesgrcvnum;             /* Counter of received messages                              */
  MPI_Status * restrict   statsndtab;             /* Status array for send operations                          */
  MPI_Request * restrict  requbuftab;             /* Global array holding receive and send request sub-arrays  */
#define mesgrcvtab(i)               (mesgbuftab + (grafptr->s.procgstmax * (i)))
#define mesgsndtab(i)               (mesgbuftab + (grafptr->s.procgstmax * (grafptr->s.procngbnbr + (i))))
#define requrcvtab                  requbuftab
#define requsndtab                  (requbuftab + grafptr->s.procngbnbr)
  Gnum * restrict         queuloctab;             /* Queue array holding all of the selected local vertices    */
  Gnum                    queuheadidx;            /* Index of head of queue                                    */
  Gnum                    queutailidx;            /* Index of tail of queue                                    */
  Gnum                    distval;
  Gnum                    reduloctab[3];
  Gnum                    reduglbtab[3];
  Gnum                    fronlocnum;
  int                     cheklocval;
  int                     procngbidx;
  int                     procngbnum;

  if (grafptr->fronglbnbr == 0)                   /* If no frontier to base on  */
    return (0);                                   /* Then do nothing            */
  if (paraptr->distmax < 1)                       /* If distance is 0 (or less) */
    return (0);                                   /* Then do nothing            */

  if (dgraphGhst (&grafptr->s) != 0) {            /* Compute ghost edge array if not already present */
    errorPrint ("vdgraphSeparateBd: cannot compute ghost edge array");
    return     (1);
  }

  cheklocval = 0;                                 /* Assume everything is all right */
  if (memAllocGroup ((void **)
                     &bandvnumgsttax, (size_t) (grafptr->s.vertgstnbr       * sizeof (Gnum)),
                     &mesgptrtab,     (size_t) (grafptr->s.procngbnbr       * sizeof (Gnum *)),
                     &statsndtab,     (size_t) (grafptr->s.procngbnbr       * sizeof (MPI_Status)),
                     &requbuftab,     (size_t) ((grafptr->s.procngbnbr * 2) * sizeof (MPI_Request)),
                     &mesgbuftab,     (size_t) ((grafptr->s.procngbnbr * grafptr->s.procgstmax * 2) * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("vdgraphSeparateBd: out of memory (1)");
    cheklocval = 1;
  }
#ifdef SCOTCH_DEBUG_VDGRAPH1                      /* Communication not needed and not absorbable by the algorithm */
  reduloctab[0] =   paraptr->distmax;
  reduloctab[1] = - paraptr->distmax;
  reduloctab[2] =   cheklocval;
  if (MPI_Allreduce (reduloctab, reduglbtab, 3, GNUM_MPI, MPI_MAX, grafptr->s.proccomm) != MPI_SUCCESS) {
    errorPrint ("vdgraphSeparateBd: communication error (1)");
    return     (1);
  }
  if (reduglbtab[1] != - reduglbtab[0]) {
    errorPrint ("vdgraphSeparateBd: invalid parameters");
    reduglbtab[2] = 1;
  }
  if (reduglbtab[2] != 0) {
    if (bandvnumgsttax != NULL)
      memFree (bandvnumgsttax);                   /* Free group leader */
    return (1);
  }
#else /* SCOTCH_DEBUG_VDGRAPH1 */
  if (cheklocval == 1)                            /* Dirty return because cannot absorb communication cost */
    return (1);
#endif /* SCOTCH_DEBUG_VDGRAPH1 */

  for (procngbidx = 0; procngbidx < grafptr->s.procngbnbr; procngbidx ++) { /* Create receive requests */
    if (MPI_Recv_init (mesgrcvtab (procngbidx), grafptr->s.procgstmax * sizeof (Gnum), GNUM_MPI,
                       grafptr->s.procngbtab[procngbidx], MPI_ANY_TAG,
                       grafptr->s.proccomm, requrcvtab + procngbidx) != MPI_SUCCESS) {
      errorPrint ("vdgraphSeparateBd: communication error (2)");
      return     (1);
    }
  }

  bandvertlvlnum =                                /* Start index of last level is start index */
  bandvertlocnnd = grafptr->s.baseval;            /* Reset number of band vertices, plus base */
  bandedgelocnbr = 0;
  memSet (bandvnumgsttax, ~0, grafptr->s.vertgstnbr * sizeof (Gnum)); /* Reset part array */
  bandvnumgsttax -= grafptr->s.baseval;

  for (fronlocnum = 0; fronlocnum < grafptr->fronlocnbr; fronlocnum ++) { /* All separator vertices will be first vertices of band graph */
    Gnum              vertlocnum;

    vertlocnum = grafptr->fronloctab[fronlocnum];
    bandvnumgsttax[vertlocnum] = bandvertlocnnd ++; /* Keep separator vertex in band */
    bandedgelocnbr += grafptr->s.vendloctax[vertlocnum] - grafptr->s.vertloctax[vertlocnum]; /* Account for its edges */
  }
  queuloctab  = grafptr->fronloctab;              /* Use frontier array as queue array          */
  queuheadidx = 0;                                /* No queued vertex read yet                  */
  queutailidx = grafptr->fronlocnbr;              /* All frontier vertices are already in queue */

  for (distval = 0; ++ distval <= paraptr->distmax; ) {
    Gnum              queunextidx;                /* Tail index for enqueuing vertices of next band */

    if (MPI_Startall (grafptr->s.procngbnbr, requrcvtab) != MPI_SUCCESS) { /* Start all receive operations from neighbors */
      errorPrint ("vdgraphSeparateBd: communication error (3)");
      return     (1);
    }

    bandvertlvlnum = bandvertlocnnd;              /* Save start index of current level, based             */
    for (procngbidx = 0; procngbidx < grafptr->s.procngbnbr; procngbidx ++)  /* Reset send buffer indices */
      mesgptrtab[procngbidx] = mesgsndtab (procngbidx);

    for (queunextidx = queutailidx; queuheadidx < queutailidx; ) { /* For all vertices in queue */
      Gnum              vertlocnum;
      Gnum              edgelocnum;

      vertlocnum = queuloctab[queuheadidx ++];    /* Dequeue vertex */
      for (edgelocnum = grafptr->s.vertloctax[vertlocnum]; edgelocnum < grafptr->s.vendloctax[vertlocnum]; edgelocnum ++) {
        Gnum              vertlocend;

        vertlocend = grafptr->s.edgegsttax[edgelocnum];
        if (bandvnumgsttax[vertlocend] != ~0)     /* If end vertex has already been processed */
          continue;                               /* Skip to next vertex                      */

        if (vertlocend < grafptr->s.vertlocnnd) { /* If end vertex is local           */
          bandvnumgsttax[vertlocend] = bandvertlocnnd ++; /* Label vertex as enqueued */
          queuloctab[queunextidx ++] = vertlocend; /* Enqueue vertex for next pass    */
          bandedgelocnbr += grafptr->s.vendloctax[vertlocend] - grafptr->s.vertloctax[vertlocend]; /* Account for its edges */
        }
        else {                                    /* End vertex is a ghost */
          Gnum              vertglbend;
          int               procngbidx;
          int               procngbnum;

          bandvnumgsttax[vertlocend]  = 0;        /* Label ghost vertex as enqueued          */
          vertglbend = grafptr->s.edgeloctax[edgelocnum]; /* Get global number of end vertex */
          for (procngbidx = 0; procngbidx < grafptr->s.procngbnbr; procngbidx ++) {
            procngbnum = grafptr->s.procngbtab[procngbidx]; /* Search for neighbor hosting this vertex */
            if ((vertglbend >= grafptr->s.procdsptab[procngbnum]) && (vertglbend < grafptr->s.procdsptab[procngbnum + 1]))
              break;
          }
#ifdef SCOTCH_DEBUG_VDGRAPH2
          if (procngbidx >= grafptr->s.procngbnbr) {
            errorPrint ("vdgraphSeparateBd: internal error (1)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
          *(mesgptrtab[procngbidx] ++) = vertglbend - grafptr->s.procdsptab[procngbnum] + grafptr->s.baseval; /* Buffer local value on neighbor processor */
        }
      }
    }

    for (procngbidx = 0; procngbidx < grafptr->s.procngbnbr; procngbidx ++) { /* Send all buffers to neighbors */
      if (MPI_Isend (mesgsndtab (procngbidx), mesgptrtab[procngbidx] - mesgsndtab (procngbidx),
                     GNUM_MPI, grafptr->s.procngbtab[procngbidx], 0, grafptr->s.proccomm,
                     &requsndtab[procngbidx]) != MPI_SUCCESS) {
        errorPrint ("vdgraphSeparateBd: communication error (4)");
        return     (1);
      }
    }

    for (mesgrcvnum = 0; mesgrcvnum < grafptr->s.procngbnbr; mesgrcvnum ++) { /* Receive messages one by one */
      int               procngbidx;
      int               vertmsgnbr;
      int               vertmsgnum;
      MPI_Status        statdat;

      if (MPI_Waitany (grafptr->s.procngbnbr, requrcvtab, &procngbidx, &statdat) != MPI_SUCCESS) {
        errorPrint ("vdgraphSeparateBd: communication error (5)");
        return     (1);
      }
      MPI_Get_count (&statdat, GNUM_MPI, &vertmsgnbr); /* Get number of vertices in message */

      for (vertmsgnum = 0; vertmsgnum < vertmsgnbr; vertmsgnum ++) {
        Gnum              vertlocend;

        vertlocend = (mesgrcvtab (procngbidx))[vertmsgnum]; /* Get local vertex from message */
#ifdef SCOTCH_DEBUG_VDGRAPH2
        if ((vertlocend < 0) || (vertlocend >= grafptr->s.vertlocnnd)) {
          errorPrint ("vdgraphSeparateBd: internal error (2)");
          return     (1);
        }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
        if (bandvnumgsttax[vertlocend] != ~0)     /* If end vertex has already been processed */
          continue;                               /* Skip to next vertex                      */

        bandvnumgsttax[vertlocend] = bandvertlocnnd ++; /* Label vertex as enqueued */
        queuloctab[queunextidx ++] = vertlocend;  /* Enqueue vertex for next pass   */
        bandedgelocnbr += grafptr->s.vendloctax[vertlocend] - grafptr->s.vertloctax[vertlocend]; /* Account for its edges */
      }
    }

    queutailidx = queunextidx;                    /* Prepare queue for next sweep */

    if (MPI_Waitall (grafptr->s.procngbnbr, requsndtab, statsndtab) != MPI_SUCCESS) { /* Wait until all send operations completed */
      errorPrint ("vdgraphSeparateBd: communication error (6)");
      return     (1);
    }
  }
  for (procngbidx = 0; procngbidx < grafptr->s.procngbnbr; procngbidx ++) { /* Free persistent receive requests */
    if (MPI_Request_free (requrcvtab + procngbidx) != MPI_SUCCESS) {
      errorPrint ("vdgraphSeparateBd: communication error (7)");
      return     (1);
    }
  }
  bandvnumgsttax  = memRealloc (bandvnumgsttax + grafptr->s.baseval, MAX (grafptr->s.vertgstnbr, grafptr->s.procglbnbr) * sizeof (Gnum)); /* TRICK: re-use array for further error collective communications */
  bandvnumgsttax -= grafptr->s.baseval;
  bandvertlocnbr  = bandvertlocnnd - grafptr->s.baseval + 2; /* Un-base number of vertices and add anchor vertices          */
  bandedgelocnbr += 2 * ((bandvertlocnnd - bandvertlvlnum) + (grafptr->s.procglbnbr - 1)); /* Add edges to and from anchors */

  vdgraphInit (&bandgrafdat, grafptr->s.proccomm);
  bandgrafdat.s.flagval = DGRAPHFREETABS | DGRAPHVERTGROUP | DGRAPHEDGEGROUP; /* Arrays created by the routine itself */
  bandgrafdat.s.baseval = grafptr->s.baseval;

  cheklocval = 0;
  if (memAllocGroup ((void **)                    /* Allocate distributed graph private data */
                     &bandgrafdat.s.procdsptab, (size_t) ((grafptr->s.procglbnbr + 1) * sizeof (int)),
                     &bandgrafdat.s.proccnttab, (size_t) (grafptr->s.procglbnbr       * sizeof (int)),
                     &bandgrafdat.s.procngbtab, (size_t) (grafptr->s.procglbnbr       * sizeof (int)),
                     &bandgrafdat.s.procrcvtab, (size_t) (grafptr->s.procglbnbr       * sizeof (int)),
                     &bandgrafdat.s.procsndtab, (size_t) (grafptr->s.procglbnbr       * sizeof (int)), NULL) == NULL) {
    errorPrint ("vdgraphSeparateBd: out of memory (2)");
    cheklocval = 1;
  }
  else if (bandgrafdat.s.procvrttab = bandgrafdat.s.procdsptab, /* Graph does not have holes */
           memAllocGroup ((void **)               /* Allocate distributed graph public data  */
                          &bandgrafdat.s.vertloctax, (size_t) ((bandvertlocnbr + 1) * sizeof (Gnum)), /* Compact vertex array */
                          &bandgrafdat.s.vnumloctax, (size_t) (bandvertlocnbr       * sizeof (Gnum)),
                          &bandgrafdat.s.veloloctax, (size_t) (bandvertlocnbr       * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("vdgraphSeparateBd: out of memory (3)");
    cheklocval = 1;
  }
  else if (bandgrafdat.s.vertloctax -= bandgrafdat.s.baseval,
           bandgrafdat.s.veloloctax -= bandgrafdat.s.baseval,
           bandgrafdat.s.vnumloctax -= bandgrafdat.s.baseval,
           ((bandgrafdat.s.edgeloctax = memAlloc (bandedgelocnbr * sizeof (Gnum))) == NULL)) { /* No edge loads needed */
    errorPrint ("vdgraphSeparateBd: out of memory (4)");
    cheklocval = 1;
  }
  else if (bandgrafdat.s.edgeloctax -= bandgrafdat.s.baseval,
           ((bandgrafdat.fronloctab = memAlloc (bandvertlocnbr * sizeof (Gnum))) == NULL)) {
    errorPrint ("vdgraphSeparateBd: out of memory (5)");
    cheklocval = 1;
  }
  else if ((bandgrafdat.partgsttax = memAlloc (bandedgelocnbr * sizeof (GraphPart))) == NULL) { /* Upper bound on number of ghost vertices */
    errorPrint ("vdgraphSeparateBd: out of memory (6)");
    cheklocval = 1;
  }
  else
    bandgrafdat.partgsttax -= bandgrafdat.s.baseval;

  if (cheklocval != 0) {                          /* In case of memory error */
    bandgrafdat.s.procdsptab[0] = -1;
    if (MPI_Allgather (&bandgrafdat.s.procdsptab[0], 1, MPI_INT, /* Send received data to dummy array */
                       bandvnumgsttax + bandgrafdat.s.baseval, 1, MPI_INT, grafptr->s.proccomm) != MPI_SUCCESS) {
      errorPrint ("vdgraphSeparateBd: communication error (8)");
      return     (1);
    }
    vdgraphExit (&bandgrafdat);
    memFree     (bandvnumgsttax + bandgrafdat.s.baseval);
    return      (1);
  }
  else {
    bandgrafdat.s.procdsptab[0] = (int) bandvertlocnbr;
    if (MPI_Allgather (&bandgrafdat.s.procdsptab[0], 1, MPI_INT,
                       &bandgrafdat.s.procdsptab[1], 1, MPI_INT, grafptr->s.proccomm) != MPI_SUCCESS) {
      errorPrint ("vdgraphSeparateBd: communication error (9)");
      return     (1);
    }
  }
  bandgrafdat.s.procdsptab[0] = bandgrafdat.s.baseval; /* Build vertex-to-process array */
#ifdef SCOTCH_DEBUG_VDGRAPH2
  memSet (bandgrafdat.s.vnumloctax + bandgrafdat.s.baseval, ~0, (bandvertlocnbr * sizeof (Gnum)));
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

  for (procngbnum = 1; procngbnum <= grafptr->s.procglbnbr; procngbnum ++) { /* Process potential error flags from other processes */
    if (bandgrafdat.s.procdsptab[procngbnum] < 0) { /* If error notified by another process                                        */
      vdgraphExit (&bandgrafdat);
      memFree     (bandvnumgsttax + bandgrafdat.s.baseval);
      return      (1);
    }
    bandgrafdat.s.procdsptab[procngbnum] += bandgrafdat.s.procdsptab[procngbnum - 1];
    bandgrafdat.s.proccnttab[procngbnum - 1] = bandgrafdat.s.procdsptab[procngbnum] - bandgrafdat.s.procdsptab[procngbnum - 1];
  }

  for (fronlocnum = 0, bandvertlocnum = bandgrafdat.s.baseval, bandvertlocadj = bandgrafdat.s.procdsptab[grafptr->s.proclocnum] - bandgrafdat.s.baseval;
       fronlocnum < grafptr->fronlocnbr; fronlocnum ++, bandvertlocnum ++) { /* Turn all graph separator vertices into band separator vertices */
    Gnum              vertlocnum;

    bandgrafdat.fronloctab[fronlocnum] = bandvertlocnum; /* All frontier vertices are first vertices of band graph */
    vertlocnum = grafptr->fronloctab[fronlocnum];
    bandgrafdat.s.vnumloctax[bandvertlocnum] = vertlocnum;
    bandvnumgsttax[vertlocnum] += bandvertlocadj; /* Turn local indices in band graph into global indices */
  }
  for ( ; bandvertlocnum < bandvertlocnnd; fronlocnum ++, bandvertlocnum ++) { /* Pick selected band vertices from rest of frontier array without anchors */
    Gnum              vertlocnum;

    vertlocnum = grafptr->fronloctab[fronlocnum];
    bandgrafdat.s.vnumloctax[bandvertlocnum] = vertlocnum;
    bandvnumgsttax[vertlocnum] += bandvertlocadj; /* Turn local indices in band graph into global indices */
  }

  if (dgraphHaloSync (&grafptr->s, (byte *) (bandvnumgsttax + bandgrafdat.s.baseval), GNUM_MPI) != 0) { /* Share global indexing of halo vertices */
    errorPrint ("vdgraphSeparateBd: cannot perform halo exchange");
    return     (1);
  }

  bandvertlocnbr1 = 0;
  bandvelolocsum  = 0;
  bandvelolocsum1 = 0;
  banddegrlocmax  = 0;
  for (bandvertlocnum = bandedgelocnum = bandgrafdat.s.baseval; /* Build global vertex array of band graph            */
       bandvertlocnum < bandvertlvlnum; bandvertlocnum ++) { /* For all vertices that do not belong to the last level */
    Gnum              vertlocnum;
    Gnum              edgelocnum;
    GraphPart         partval;
    int               partval1;

    vertlocnum = bandgrafdat.s.vnumloctax[bandvertlocnum];
    partval    = grafptr->partgsttax[vertlocnum];
#ifdef SCOTCH_DEBUG_VDGRAPH2
    if (partval > 2) {
        errorPrint ("vdgraphSeparateBd: internal error (3)");
        return     (1);
    }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
    partval1   = partval & 1;

    bandvertlocnbr1 += (partval & 1);             /* Count vertices in part 1 */
    bandgrafdat.partgsttax[bandvertlocnum] = partval;
    bandgrafdat.s.vertloctax[bandvertlocnum] = bandedgelocnum;
    if (grafptr->s.veloloctax != NULL) {
      Gnum              veloval;

      veloval  = grafptr->s.veloloctax[vertlocnum];
      bandvelolocsum  += veloval;                 /* Sum vertex loads                   */
      bandvelolocsum1 += veloval & (0 - partval1); /* Sum vertex load if (partval == 1) */
      bandgrafdat.s.veloloctax[bandvertlocnum] = veloval;
    }
    else
      bandgrafdat.s.veloloctax[bandvertlocnum] = 1; /* Load of vertices is set to 1 */

    if (banddegrlocmax < (grafptr->s.vendloctax[vertlocnum] - grafptr->s.vertloctax[vertlocnum]))
      banddegrlocmax = (grafptr->s.vendloctax[vertlocnum] - grafptr->s.vertloctax[vertlocnum]);

    for (edgelocnum = grafptr->s.vertloctax[vertlocnum]; /* For all original edges */
         edgelocnum < grafptr->s.vendloctax[vertlocnum]; edgelocnum ++) {
#ifdef SCOTCH_DEBUG_VDGRAPH2
      if (bandvnumgsttax[grafptr->s.edgegsttax[edgelocnum]] == ~0) { /* All ends should belong to the band graph too */
        errorPrint ("vdgraphSeparateBd: internal error (4)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
      bandgrafdat.s.edgeloctax[bandedgelocnum ++] = bandvnumgsttax[grafptr->s.edgegsttax[edgelocnum]];
    }
  }
  for ( ; bandvertlocnum < bandvertlocnnd; bandvertlocnum ++) { /* For all vertices that belong to the last level except anchors */
    Gnum              vertlocnum;
    Gnum              edgelocnum;
    GraphPart         partval;
    int               partval1;

    vertlocnum = bandgrafdat.s.vnumloctax[bandvertlocnum];
    partval    = grafptr->partgsttax[vertlocnum];
    partval1   = partval & 1;

    bandvertlocnbr1 += (partval & 1);                   /* Count vertices in part 1 */
    bandgrafdat.partgsttax[bandvertlocnum] = partval;
    bandgrafdat.s.vertloctax[bandvertlocnum] = bandedgelocnum;
    if (grafptr->s.veloloctax != NULL) {
      Gnum              veloval;

      veloval  = grafptr->s.veloloctax[vertlocnum];
      bandvelolocsum  += veloval;                 /* Sum vertex loads                   */
      bandvelolocsum1 += veloval & (0 - partval1); /* Sum vertex load if (partval == 1) */
      bandgrafdat.s.veloloctax[bandvertlocnum] = veloval;
    }
    else
      bandgrafdat.s.veloloctax[bandvertlocnum] = 1; /* Load of vertices is set to 1 */

    for (edgelocnum = grafptr->s.vertloctax[vertlocnum]; /* For all original edges */
         edgelocnum < grafptr->s.vendloctax[vertlocnum]; edgelocnum ++) {
      Gnum              bandvertlocend;

      bandvertlocend = bandvnumgsttax[grafptr->s.edgegsttax[edgelocnum]];
      if (bandvertlocend != ~0)                   /* If end vertex belongs to band graph */
        bandgrafdat.s.edgeloctax[bandedgelocnum ++] = bandvertlocend;
    }
    bandgrafdat.s.edgeloctax[bandedgelocnum ++] = bandvertlocnnd + bandvertlocadj + partval1; /* Add edge to anchor of proper part */
    if (banddegrlocmax < (bandedgelocnum - bandgrafdat.s.vertloctax[bandvertlocnum]))
      banddegrlocmax = (bandedgelocnum - bandgrafdat.s.vertloctax[bandvertlocnum]);
  }

  bandgrafdat.partgsttax[bandvertlocnnd]     = 0; /* Set parts of anchor vertices */
  bandgrafdat.partgsttax[bandvertlocnnd + 1] = 1;
  bandgrafdat.s.vertloctax[bandvertlocnum] = bandedgelocnum; /* Process anchor vertex in part 0 */
  for (procngbnum = 0; procngbnum < grafptr->s.proclocnum; procngbnum ++)
    bandgrafdat.s.edgeloctax[bandedgelocnum ++] = bandgrafdat.s.procdsptab[procngbnum + 1] - 2; /* Build clique with anchors of part 0 */
  for (procngbnum ++; procngbnum < grafptr->s.procglbnbr; procngbnum ++)
    bandgrafdat.s.edgeloctax[bandedgelocnum ++] = bandgrafdat.s.procdsptab[procngbnum + 1] - 2; /* Build clique with anchors of part 0 */
  bandedgeloctmp = bandedgelocnum + (bandvertlocnnd - bandvertlvlnum);
  for (procngbnum = 0; procngbnum < grafptr->s.proclocnum; procngbnum ++)
    bandgrafdat.s.edgeloctax[bandedgeloctmp ++] = bandgrafdat.s.procdsptab[procngbnum + 1] - 1; /* Build clique with anchors of part 1 */
  for (procngbnum ++; procngbnum < grafptr->s.procglbnbr; procngbnum ++)
    bandgrafdat.s.edgeloctax[bandedgeloctmp ++] = bandgrafdat.s.procdsptab[procngbnum + 1] - 1; /* Build clique with anchors of part 1 */
  bandgrafdat.s.vertloctax[bandvertlocnnd + 2] = bandedgeloctmp;
  bandedgelocnbr = bandedgeloctmp - bandgrafdat.s.baseval; /* Set real number of edges */
  for (bandvertlocnum = bandvertlvlnum, bandedgeloctmp = bandedgelocnum + (bandvertlocnnd - bandvertlvlnum); /* Link vertices of last level to anchors */
       bandvertlocnum < bandvertlocnnd; bandvertlocnum ++) {
    if (bandgrafdat.partgsttax[bandvertlocnum] == 0)
      bandgrafdat.s.edgeloctax[bandedgelocnum ++] = bandvertlocnum + bandvertlocadj;
    else
      bandgrafdat.s.edgeloctax[-- bandedgeloctmp] = bandvertlocnum + bandvertlocadj;
  }
  bandgrafdat.s.vertloctax[bandvertlocnnd + 1] = bandedgeloctmp;
  if (banddegrlocmax < (bandgrafdat.s.vertloctax[bandvertlocnnd + 1] - bandgrafdat.s.vertloctax[bandvertlocnnd]))
    banddegrlocmax = (bandgrafdat.s.vertloctax[bandvertlocnnd + 1] - bandgrafdat.s.vertloctax[bandvertlocnnd]);
  if (banddegrlocmax < (bandgrafdat.s.vertloctax[bandvertlocnnd + 2] - bandgrafdat.s.vertloctax[bandvertlocnnd + 1]))
    banddegrlocmax = (bandgrafdat.s.vertloctax[bandvertlocnnd + 2] - bandgrafdat.s.vertloctax[bandvertlocnnd + 1]);

  if (grafptr->s.veloloctax == NULL) {            /* If original graph is not weighted */
    bandgrafdat.s.veloloctax[bandvertlocnnd]     = grafptr->complocsize[0] - (bandvertlocnbr - 2 - bandvertlocnbr1 - grafptr->fronlocnbr); /* Minus 2 for anchors */
    bandgrafdat.s.veloloctax[bandvertlocnnd + 1] = grafptr->complocsize[1] - bandvertlocnbr1;
  }
  else {
    bandgrafdat.s.veloloctax[bandvertlocnnd]     = grafptr->complocload[0] - (bandvelolocsum - bandvelolocsum1 - grafptr->complocload[2]);
    bandgrafdat.s.veloloctax[bandvertlocnnd + 1] = grafptr->complocload[1] - bandvelolocsum1;
  }
  bandvertlocancadj = 0;
  if ((bandgrafdat.s.veloloctax[bandvertlocnnd] == 0) || /* If at least one anchor is empty */
      (bandgrafdat.s.veloloctax[bandvertlocnnd + 1] == 0)) {
    bandvertlocancadj = 1;
    bandgrafdat.s.veloloctax[bandvertlocnnd] ++;  /* Increase weight of both anchors to keep balance */
    bandgrafdat.s.veloloctax[bandvertlocnnd + 1] ++;
  }

  bandgrafdat.s.vertlocnbr = bandvertlocnbr;
  bandgrafdat.s.vertlocnnd = bandvertlocnbr + bandgrafdat.s.baseval;
  bandgrafdat.s.vendloctax = bandgrafdat.s.vertloctax + 1; /* Band graph is compact */
  bandgrafdat.s.velolocsum = grafptr->s.velolocsum + 2 * bandvertlocancadj;
  bandgrafdat.s.edgelocnbr = bandedgelocnbr;
  bandgrafdat.s.edgelocsiz = bandedgelocnbr;
  bandgrafdat.s.degrglbmax = banddegrlocmax;      /* Local maximum degree will be turned into global maximum degree */
  if (dgraphBuild4 (&bandgrafdat.s) != 0) {
    errorPrint ("vdgraphSeparateBd: cannot build band graph");
    return     (1);
  }
#ifdef SCOTCH_DEBUG_VDGRAPH2
  if (dgraphCheck (&bandgrafdat.s) != 0) {
    errorPrint ("vdgraphSeparateBd: internal error (5)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

  bandgrafdat.complocsize[0] = bandvertlocnbr - (bandvertlocnbr1 + 1) - grafptr->fronlocnbr; /* Add 1 for anchor vertex 1 */
  bandgrafdat.complocsize[1] = bandvertlocnbr1 + 1; /* Add 1 for anchor vertex 1 */
  complocsizeadj0 = grafptr->complocsize[0] - bandgrafdat.complocsize[0];
  complocsizeadj1 = grafptr->complocsize[1] - bandgrafdat.complocsize[1];
  reduloctab[0] = bandgrafdat.complocsize[0];
  reduloctab[1] = bandgrafdat.complocsize[1];
  reduloctab[2] = bandvertlocancadj;                 /* Sum increases in size and load */
  if (MPI_Allreduce (&reduloctab[0], &reduglbtab[0], 3, GNUM_MPI, MPI_SUM, grafptr->s.proccomm) != MPI_SUCCESS) {
    errorPrint ("vdgraphSeparateBd: communication error (10)");
    return     (1);
  }
  bandvertglbancadj = reduglbtab[2];
  bandgrafdat.compglbload[0] = grafptr->compglbload[0] + bandvertglbancadj; /* All loads are kept in band graph */
  bandgrafdat.compglbload[1] = grafptr->compglbload[1] + bandvertglbancadj;
  bandgrafdat.compglbload[2] = grafptr->compglbload[2];
  bandgrafdat.compglbloaddlt = grafptr->compglbloaddlt; /* Balance is not changed by anchor vertices */
  bandgrafdat.fronglbnbr     = grafptr->fronglbnbr; /* All separator vertices are kept in band graph */
  bandgrafdat.complocload[0] = grafptr->complocload[0] + bandvertlocancadj;
  bandgrafdat.complocload[1] = grafptr->complocload[1] + bandvertlocancadj;
  bandgrafdat.complocload[2] = grafptr->complocload[2];
  bandgrafdat.compglbsize[0] = reduglbtab[0];
  bandgrafdat.compglbsize[1] = reduglbtab[1];
  bandgrafdat.fronlocnbr     = grafptr->fronlocnbr;
  bandgrafdat.levlnum        = grafptr->levlnum;

  memFree (bandvnumgsttax + bandgrafdat.s.baseval);  /* Free useless space */

#ifdef SCOTCH_DEBUG_VDGRAPH2
  if (vdgraphCheck (&bandgrafdat) != 0) {
    errorPrint ("vdgraphSeparateBd: internal error (6)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

  if (vdgraphSeparateSt (&bandgrafdat, paraptr->strat) != 0) { /* Separate distributed band graph */
    errorPrint  ("vdgraphSeparateBd: cannot separate band graph");
    vdgraphExit (&bandgrafdat);
    return      (1);
  }

  reduloctab[0] = ((bandgrafdat.partgsttax[bandvertlocnnd] == 0) && /* Check if anchor vertices remain in their parts */
                   (bandgrafdat.partgsttax[bandvertlocnnd + 1] == 1)) ? 0 : 1;
  reduloctab[1] = bandgrafdat.complocsize[0] + complocsizeadj0;
  reduloctab[2] = bandgrafdat.complocsize[1] + complocsizeadj1;
  if (MPI_Allreduce (&reduloctab[0], &reduglbtab[0], 3, GNUM_MPI, MPI_SUM, grafptr->s.proccomm) != MPI_SUCCESS) {
    errorPrint ("vdgraphSeparateBd: communication error (11)");
    return     (1);
  }
  if (reduglbtab[0] != 0) {                       /* If at least one anchor changed of part */
    vdgraphExit (&bandgrafdat);                   /* Then keep original partition           */
    return      (0);
  }

  grafptr->compglbload[0] = bandgrafdat.compglbload[0] - bandvertglbancadj;
  grafptr->compglbload[1] = bandgrafdat.compglbload[1] - bandvertglbancadj;
  grafptr->compglbload[2] = bandgrafdat.compglbload[2];
  grafptr->compglbloaddlt = bandgrafdat.compglbloaddlt;
  grafptr->compglbsize[0] = reduglbtab[1];
  grafptr->compglbsize[1] = reduglbtab[2];
  grafptr->fronglbnbr     = bandgrafdat.fronglbnbr;
  grafptr->complocload[0] = bandgrafdat.complocload[0] - bandvertlocancadj;
  grafptr->complocload[1] = bandgrafdat.complocload[1] - bandvertlocancadj;
  grafptr->complocload[2] = bandgrafdat.complocload[2];
  grafptr->complocsize[0] = reduloctab[1];
  grafptr->complocsize[1] = reduloctab[2];
  grafptr->fronlocnbr     = bandgrafdat.fronlocnbr;
  for (fronlocnum = 0; fronlocnum < bandgrafdat.fronlocnbr; fronlocnum ++) /* Project back separator */
    grafptr->fronloctab[fronlocnum] = bandgrafdat.s.vnumloctax[bandgrafdat.fronloctab[fronlocnum]];
  for (bandvertlocnum = bandgrafdat.s.baseval; bandvertlocnum < bandvertlocnnd; bandvertlocnum ++) /* For all vertices except anchors */
    grafptr->partgsttax[bandgrafdat.s.vnumloctax[bandvertlocnum]] = bandgrafdat.partgsttax[bandvertlocnum];

#ifdef SCOTCH_DEBUG_VDGRAPH2
  if (vdgraphCheck (grafptr) != 0) {
    errorPrint ("vdgraphSeparateBd: internal error (7)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

  vdgraphExit (&bandgrafdat);

  return (0);
}
