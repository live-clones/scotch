/* Copyright 2004,2007 INRIA
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
/**   NAME       : vgraph_separate_bd.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This module construct a band graph      **/
/**                around the separator in order to        **/
/**                decrease problem size during            **/
/**                uncoarsening refinement.                **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 18 oct 2004     **/
/**                                 to   : 19 dec 2006     **/
/**                                                        **/
/************************************************************/


/*
**  The defines and includes.
*/

#define VGRAPH_SEPARATE_BD

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "vgraph.h"
#include "vgraph_separate_bd.h"
#include "vgraph_separate_st.h"

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

int
vgraphSeparateBd (
Vgraph * restrict const             orggrafptr,   /*+ Active graph      +*/
const VgraphSeparateBdParam * const paraptr)      /*+ Method parameters +*/
{
  VgraphSeparateBdQueue     queudat;              /* Neighbor queue                                               */
  Gnum * restrict           orgdisttax;           /* Based access to distance array for original graph            */
  Gnum                      orgdistmax;           /* Maximum distance allowed                                     */
#define orgindxtax          orgdisttax            /* Recycle distance array as number indexing array              */
  Vgraph                    bndgrafdat;           /* Band graph structure                                         */
  Gnum                      bndvertnbr;           /* Number of regular vertices in band graph (without anchors)   */
  Gnum                      bndvertnnd;
  Gnum * restrict           bndvnumtax;           /* Band vertex number array, recycling queudat.qtab             */
  Gnum                      bndcompsize1;         /* Number of regular vertices in part 1 of band graph           */
  Gnum                      bndcompload1;         /* Load of regular vertices in part 1                           */
  Gnum                      bndvlvlnum;           /* Index of first band graph vertex to belong to the last layer */
  Gnum                      bndvertnum;
  Gnum                      bndvelosum;           /* Load of regular vertices in band graph                       */
  Gnum                      bndedgenbr;           /* Upper bound on the number of edges, including anchor edges   */
  Gnum                      bndeancnbr;           /* Number of anchor edges                                       */
  Gnum                      bndedgenum;
  Gnum                      bndedgetmp;
  Gnum                      bnddegrmax;
  Gnum                      fronnum;

  if (orggrafptr->fronnbr == 0)                   /* If no separator vertices, apply strategy to full graph */
    return (vgraphSeparateSt (orggrafptr, paraptr->strat));

  orgdistmax = (Gnum) paraptr->distmax;
  if (orgdistmax < 1)                             /* To simplify algorithm, always at least one layer of vertices around separator */
    orgdistmax = 1;

  if (memAllocGroup ((void **)
                     &queudat.qtab, (size_t) (orggrafptr->s.vertnbr * sizeof (Gnum)), /* TRICK: no need of "+ 2" for anchor vertices (see below) */
                     &orgdisttax,   (size_t) (orggrafptr->s.vertnbr * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("vgraphSeparateBd: out of memory (1)");
    return     (1);
  }
  memSet (orgdisttax, ~0, orggrafptr->s.vertnbr * sizeof (Gnum)); /* Initialize distance array */
  orgdisttax -= orggrafptr->s.baseval;

  vgraphSeparateBdQueueFlush (&queudat);          /* Flush vertex queue                       */
  bndedgenbr = 0;                                 /* Guess upper bound on the number of edges */
  bndvelosum = 0;
  for (fronnum = 0; fronnum < orggrafptr->fronnbr; fronnum ++) { /* Enqueue separator vertices */
    Gnum                orgvertnum;

    orgvertnum = orggrafptr->frontab[fronnum];
#ifdef SCOTCH_DEBUG_VGRAPH2
    if ((orgvertnum < orggrafptr->s.baseval) || (orgvertnum >= orggrafptr->s.vertnnd)) {
      errorPrint ("vgraphSeparateBd: internal error (1)");
      memFree    (queudat.qtab);                  /* Free group leader */
      return     (1);
    }
    if (orgdisttax[orgvertnum] != ~0) {
      errorPrint ("vgraphSeparateBd: internal error (2)");
      memFree    (queudat.qtab);                  /* Free group leader */
      return     (1);
    }
    if (orggrafptr->parttax[orgvertnum] != 2) {
      errorPrint ("vgraphSeparateBd: internal error (3)");
      memFree    (queudat.qtab);                  /* Free group leader */
      return     (1);
    }
#endif /* SCOTCH_DEBUG_VGRAPH2 */
    orgdisttax[orgvertnum] = 0;
    vgraphSeparateBdQueuePut (&queudat, orgvertnum);
    if (orggrafptr->s.velotax != NULL)
      bndvelosum += orggrafptr->s.velotax[orgvertnum];
  }

  bndcompsize1 = 0;
  bndcompload1 = 0;
  do {                                            /* Loop on vertices in queue */
    Gnum                orgvertnum;
    Gnum                orgedgenum;
    Gnum                orgdistval;

    orgvertnum = vgraphSeparateBdQueueGet (&queudat);
#ifdef SCOTCH_DEBUG_VGRAPH2
    if ((orgvertnum < orggrafptr->s.baseval) || (orgvertnum >= orggrafptr->s.vertnnd)) {
      errorPrint ("vgraphSeparateBd: internal error (4)");
      memFree    (queudat.qtab);                  /* Free group leader */
      return     (1);
    }
#endif /* SCOTCH_DEBUG_VGRAPH2 */
    bndedgenbr += orggrafptr->s.vendtax[orgvertnum] - orggrafptr->s.verttax[orgvertnum]; /* Exact or upper bound on number of edges, including anchor edge(s) */

    orgdistval = orgdisttax[orgvertnum];          /* Get vertex distance                    */
    if (orgdistval >= orgdistmax) {               /* If we belong to the farthest layer     */
      bndedgenbr ++;                              /* One more anchor edge, for the opposite */
      continue;
    }

    orgdistval ++;                                /* Distance of neighbors */
    for (orgedgenum = orggrafptr->s.verttax[orgvertnum];
         orgedgenum < orggrafptr->s.vendtax[orgvertnum]; orgedgenum ++) {
      Gnum                orgvertend;
      Gnum                orgpartval1;

      orgvertend = orggrafptr->s.edgetax[orgedgenum];
      if (orgdisttax[orgvertend] == ~0) {         /* If vertex not visited yet */

#ifdef SCOTCH_DEBUG_VGRAPH2
        if (orggrafptr->parttax[orgvertend] > 1) {
          errorPrint ("vgraphSeparateBd: internal error (5)");
          memFree    (queudat.qtab);              /* Free group leader */
          return     (1);
        }
#endif /* SCOTCH_DEBUG_VGRAPH2 */
        orgpartval1 = orggrafptr->parttax[orgvertend] & 1;
        orgdisttax[orgvertend] = orgdistval;      /* Enqueue vertex */
        vgraphSeparateBdQueuePut (&queudat, orgvertend);
        bndcompsize1 += orgpartval1;              /* Count vertices in part 1 */
        if (orggrafptr->s.velotax != NULL) {
          bndvelosum   += orggrafptr->s.velotax[orgvertend];
          bndcompload1 += orggrafptr->s.velotax[orgvertend] * orgpartval1;
        }
      }
    }
  } while (! vgraphSeparateBdQueueEmpty (&queudat)); /* As long as queue is not empty */

  bndvertnbr = queudat.head - queudat.qtab;       /* Number of regular band graph vertices (withour anchors) is number of enqueued vertices */

  if (orggrafptr->s.velotax == NULL) {
    bndvelosum   = bndvertnbr;
    bndcompload1 = bndcompsize1;
  }

  if ((bndcompsize1 >= orggrafptr->compsize[1]) || /* If either part has all of its vertices in band, use plain graph instead */
      ((bndvertnbr - bndcompsize1 - orggrafptr->fronnbr) >= orggrafptr->compsize[0])) {
    memFree (queudat.qtab);                       /* Free group leader */
    return  (vgraphSeparateSt (orggrafptr, paraptr->strat));
  }                                               /* TRICK: since always at least one missing vertex per part, there is room for anchor vertices */

  bndvertnnd = bndvertnbr + orggrafptr->s.baseval;
  bndvnumtax = queudat.qtab - orggrafptr->s.baseval; /* TRICK: re-use queue array as vertex number array as vertices taken in queue order */
  for (bndvertnum = orggrafptr->s.baseval; bndvertnum < bndvertnnd; bndvertnum ++) { /* For vertices not belonging to last layer          */
    Gnum                orgvertnum;

    orgvertnum = bndvnumtax[bndvertnum];          /* Get distance index of vertex      */
    if (orgindxtax[orgvertnum] >= paraptr->distmax) /* If vertex belongs to last layer */
      break;
    orgindxtax[orgvertnum] = bndvertnum;
  }
  bndvlvlnum = bndvertnum;                        /* Get index of first vertex of last layer */
  for ( ; bndvertnum < bndvertnnd; bndvertnum ++) /* For vertices belonging to last layer    */
    orgindxtax[bndvnumtax[bndvertnum]] = bndvertnum;

  memSet (&bndgrafdat, 0, sizeof (Vgraph));
  bndgrafdat.s.flagval = GRAPHFREETABS | GRAPHVERTGROUP | GRAPHEDGEGROUP;
  bndgrafdat.s.baseval = orggrafptr->s.baseval;
  bndgrafdat.s.vertnbr = bndvertnbr + 2;          /* "+ 2" for anchor vertices */
  bndgrafdat.s.vertnnd = bndvertnnd + 2;

  if (memAllocGroup ((void **)                    /* Do not allocate vnumtab but keep queudat.qtab instead */
                     &bndgrafdat.s.verttax, (size_t) ((bndvertnbr + 3) * sizeof (Gnum)),
                     &bndgrafdat.s.velotax, (size_t) ((bndvertnbr + 2) * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("vgraphSeparateBd: out of memory (2)");
    memFree    (queudat.qtab);
    return     (1);
  }
  bndgrafdat.s.verttax -= orggrafptr->s.baseval;  /* Adjust base of arrays   */
  bndgrafdat.s.vendtax  = bndgrafdat.s.verttax + 1; /* Band graph is compact */
  bndgrafdat.s.velotax -= orggrafptr->s.baseval;
  bndgrafdat.s.vnumtax  = bndvnumtax;
  bndgrafdat.s.velosum  = orggrafptr->s.velosum;
  bndgrafdat.s.velotax[bndvertnnd]     = orggrafptr->compload[0] - (bndvelosum - orggrafptr->compload[2] - bndcompload1); /* Set loads of anchor vertices */
  bndgrafdat.s.velotax[bndvertnnd + 1] = orggrafptr->compload[1] - bndcompload1;

  if (((bndgrafdat.s.edgetax = (Gnum *) memAlloc (bndedgenbr * sizeof (Gnum))) == NULL) ||
      (bndgrafdat.s.edgetax -= orggrafptr->s.baseval,
       ((bndgrafdat.parttax = (GraphPart *) memAlloc ((bndvertnbr + 2) * sizeof (GraphPart))) == NULL))) {
    errorPrint ("vgraphSeparateBd: out of memory (3)");
    graphExit  (&bndgrafdat.s);
    memFree    (queudat.qtab);
    return     (1);
  }
  bndgrafdat.parttax -= orggrafptr->s.baseval;    /* From now on we should free a Vgraph and not a Graph */

  for (bndvertnum = bndedgenum = orggrafptr->s.baseval, bnddegrmax = 0; /* Fill index array for vertices not belonging to last level */
       bndvertnum < bndvlvlnum; bndvertnum ++) {
    Gnum                orgvertnum;
    Gnum                orgedgenum;

    orgvertnum = bndvnumtax[bndvertnum];
    bndgrafdat.s.verttax[bndvertnum] = bndedgenum;
    bndgrafdat.s.velotax[bndvertnum] = (orggrafptr->s.velotax != NULL) ? orggrafptr->s.velotax[orgvertnum] : 1;

    for (orgedgenum = orggrafptr->s.verttax[orgvertnum]; /* All edges of first levels are kept */
         orgedgenum < orggrafptr->s.vendtax[orgvertnum]; orgedgenum ++, bndedgenum ++) {
#ifdef SCOTCH_DEBUG_VGRAPH2
      if ((bndedgenum >= (bndedgenbr + orggrafptr->s.baseval)) ||
          (orgindxtax[orggrafptr->s.edgetax[orgedgenum]] < 0)) {
        errorPrint ("vgraphSeparateBd: internal error (6)");
        vgraphExit (&bndgrafdat);
        memFree    (queudat.qtab);
        return     (1);
      }
#endif /* SCOTCH_DEBUG_VGRAPH2 */
      bndgrafdat.s.edgetax[bndedgenum] = orgindxtax[orggrafptr->s.edgetax[orgedgenum]];
    }
    bndgrafdat.parttax[bndvertnum] = orggrafptr->parttax[orgvertnum]; /* Copy part array */

    if (bnddegrmax < (bndedgenum - bndgrafdat.s.verttax[bndvertnum]))
      bnddegrmax = (bndedgenum - bndgrafdat.s.verttax[bndvertnum]);
  }
  bndeancnbr = 0;
  for ( ; bndvertnum < bndvertnnd; bndvertnum ++) { /* Fill index array for vertices belonging to last level */
    Gnum                orgvertnum;
    Gnum                orgedgenum;
    GraphPart           orgpartval;
    Gnum                bnddegrval;

    orgvertnum = bndvnumtax[bndvertnum];
    bndgrafdat.s.verttax[bndvertnum] = bndedgenum;
    bndgrafdat.s.velotax[bndvertnum] = (orggrafptr->s.velotax != NULL) ? orggrafptr->s.velotax[orgvertnum] : 1;

    for (orgedgenum = orggrafptr->s.verttax[orgvertnum]; /* Keep only band edges */
         orgedgenum < orggrafptr->s.vendtax[orgvertnum]; orgedgenum ++) {
      Gnum                bndvertend;

#ifdef SCOTCH_DEBUG_VGRAPH2
      if (bndedgenum >= (bndedgenbr + orggrafptr->s.baseval)) {
        errorPrint ("vgraphSeparateBd: internal error (7)");
        vgraphExit (&bndgrafdat);
        memFree    (queudat.qtab);
        return     (1);
      }
#endif /* SCOTCH_DEBUG_VGRAPH2 */
      bndvertend = orgindxtax[orggrafptr->s.edgetax[orgedgenum]];
      if (bndvertend != ~0)
        bndgrafdat.s.edgetax[bndedgenum ++] = bndvertend;
    }
    orgpartval = orggrafptr->parttax[orgvertnum];
    bndgrafdat.parttax[bndvertnum] = orgpartval;  /* Record part for vertices of last level */
    bnddegrval = bndedgenum - bndgrafdat.s.verttax[bndvertnum];
    if (bnddegrval < (orggrafptr->s.vendtax[orgvertnum] - orggrafptr->s.verttax[orgvertnum])) { /* If vertex is connected to rest of part */
      bndgrafdat.s.edgetax[bndedgenum ++] = bndvertnnd + (Gnum) orgpartval; /* Add anchor edge to proper anchor vertex                    */
      bndeancnbr ++;
      bnddegrval ++;                              /* One more (anchor) edge added to this vertex */
    }
    if (bnddegrmax < bnddegrval)
      bnddegrmax = bnddegrval;
  }
  bndgrafdat.parttax[bndvertnnd]     = 0;         /* Set parts of anchor vertices */
  bndgrafdat.parttax[bndvertnnd + 1] = 1;

  bndgrafdat.s.verttax[bndvertnnd] = bndedgenum;  /* Mark end of regular edge array and start of first anchor edge array */
  bndedgetmp = bndedgenum + bndeancnbr;
#ifdef SCOTCH_DEBUG_VGRAPH2
  if ((bndedgetmp - 1) >= (bndedgenbr + orggrafptr->s.baseval)) {
    errorPrint ("vgraphSeparateBd: internal error (8)");
    vgraphExit (&bndgrafdat);
    memFree    (queudat.qtab);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VGRAPH2 */
  bndgrafdat.s.edgenbr =
  bndgrafdat.s.edlosum = bndedgetmp - orggrafptr->s.baseval; /* Since edge load array not kept       */
  bndgrafdat.s.verttax[bndvertnnd + 2] = bndedgetmp; /* Mark end of edge array with anchor vertices  */
  for (bndvertnum = bndvlvlnum; bndvertnum < bndvertnnd; bndvertnum ++) { /* Fill anchor edge arrays */
    Gnum                orgvertnum;

    orgvertnum = bndvnumtax[bndvertnum];
    if (bndgrafdat.s.verttax[bndvertnum + 1] > bndgrafdat.s.verttax[bndvertnum]) { /* If vertex is not isolated */
      Gnum                bndvertend;

      bndvertend = bndgrafdat.s.edgetax[bndgrafdat.s.verttax[bndvertnum + 1] - 1]; /* Get last neighbor of its edge sub-array */
      if (bndvertend >= bndvertnnd) {             /* If it is an anchor          */
        if (bndvertend == bndvertnnd)             /* Add edge from proper anchor */
          bndgrafdat.s.edgetax[bndedgenum ++] = bndvertnum;
        else
          bndgrafdat.s.edgetax[-- bndedgetmp] = bndvertnum;
      }
    }
  }
  bndgrafdat.s.verttax[bndvertnnd + 1] = bndedgenum; /* Mark end of edge array of first anchor and start of second */
#ifdef SCOTCH_DEBUG_VGRAPH2
  if (bndedgenum != bndedgetmp) {
    errorPrint ("vgraphSeparateBd: internal error (9)");
    vgraphExit (&bndgrafdat);
    memFree    (queudat.qtab);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VGRAPH2 */
  if (bnddegrmax < (bndgrafdat.s.verttax[bndvertnnd + 1] - bndgrafdat.s.verttax[bndvertnnd]))
    bnddegrmax = (bndgrafdat.s.verttax[bndvertnnd + 1] - bndgrafdat.s.verttax[bndvertnnd]);
  if (bnddegrmax < (bndgrafdat.s.verttax[bndvertnnd + 2] - bndgrafdat.s.verttax[bndvertnnd + 1]))
    bnddegrmax = (bndgrafdat.s.verttax[bndvertnnd + 2] - bndgrafdat.s.verttax[bndvertnnd + 1]);
  bndgrafdat.s.degrmax = bnddegrmax;
  bndgrafdat.s.edgetax = (Gnum *) memRealloc ((void *) (bndgrafdat.s.edgetax + orggrafptr->s.baseval), bndgrafdat.s.edgenbr * sizeof (Gnum)) - orggrafptr->s.baseval;

  bndgrafdat.frontab = queudat.qtab + bndgrafdat.s.vertnbr; /* Recycle end of queue array and part of index array as band frontier array */
  for (fronnum = 0, bndvertnum = orggrafptr->s.baseval; /* Fill band frontier array with first vertex indices as they make the separator */
       fronnum < orggrafptr->fronnbr; fronnum ++, bndvertnum ++)
    bndgrafdat.frontab[fronnum] = bndvertnum;

  bndgrafdat.compload[0] = orggrafptr->compload[0];
  bndgrafdat.compload[1] = orggrafptr->compload[1];
  bndgrafdat.compload[2] = orggrafptr->compload[2];
  bndgrafdat.comploaddlt = orggrafptr->comploaddlt;
  bndgrafdat.compsize[0] = bndvertnbr - bndcompsize1 - orggrafptr->fronnbr + 1; /* "+ 1" for anchor vertices */
  bndgrafdat.compsize[1] = bndcompsize1 + 1;
  bndgrafdat.fronnbr     = orggrafptr->fronnbr;
  bndgrafdat.levlnum     = orggrafptr->levlnum;

#ifdef SCOTCH_DEBUG_VGRAPH2
  if ((graphCheck (&bndgrafdat.s) != 0) ||        /* Check band graph consistency */
      (vgraphCheck (&bndgrafdat)  != 0)) {
    errorPrint ("vgraphSeparateBd: inconsistent band graph data");
    bndgrafdat.frontab = NULL;                    /* Do not free frontab as it is not allocated */
    vgraphExit (&bndgrafdat);
    memFree    (queudat.qtab);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VGRAPH2 */

  if (vgraphSeparateSt (&bndgrafdat, paraptr->strat) != 0) { /* Apply strategy to band graph */
    errorPrint ("vgraphSeparateBd: cannot separate band graph");
    bndgrafdat.frontab = NULL;                    /* Do not free frontab as it is not allocated */
    vgraphExit (&bndgrafdat);
    memFree    (queudat.qtab);
    return     (1);
  }
  if ((bndgrafdat.parttax[bndvertnnd]     != 0) || /* If band graph was too small and anchors changed parts, apply strategy on full graph */
      (bndgrafdat.parttax[bndvertnnd + 1] != 1)) {
    bndgrafdat.frontab = NULL;                    /* Do not free frontab as it is not allocated */
    vgraphExit (&bndgrafdat);
    memFree    (queudat.qtab);
    return     (vgraphSeparateSt (orggrafptr, paraptr->strat));
  }

  orggrafptr->compload[0] = bndgrafdat.compload[0];
  orggrafptr->compload[1] = bndgrafdat.compload[1];
  orggrafptr->compload[2] = bndgrafdat.compload[2];
  orggrafptr->comploaddlt = bndgrafdat.comploaddlt;
  orggrafptr->compsize[0] = orggrafptr->compsize[0] - (bndvertnbr - bndcompsize1 - orggrafptr->fronnbr) + bndgrafdat.compsize[0] - 1; /* "- 1" for anchors */
  orggrafptr->compsize[1] = orggrafptr->compsize[1] - bndcompsize1 + bndgrafdat.compsize[1] - 1;
  orggrafptr->fronnbr     = bndgrafdat.fronnbr;

  for (bndvertnum = bndgrafdat.s.baseval; bndvertnum < bndvertnnd; bndvertnum ++) /* Update part array of full graph */
    orggrafptr->parttax[bndvnumtax[bndvertnum]] = bndgrafdat.parttax[bndvertnum];
  for (fronnum = 0; fronnum < bndgrafdat.fronnbr; fronnum ++) /* Update frontier array of full graph */
    orggrafptr->frontab[fronnum] = bndgrafdat.s.vnumtax[bndgrafdat.frontab[fronnum]];

  bndgrafdat.frontab = NULL;                      /* Do not free frontab as it is not allocated */
  vgraphExit (&bndgrafdat);                       /* Free band graph structures                 */
  memFree    (queudat.qtab);

#ifdef SCOTCH_DEBUG_VGRAPH2
  if (vgraphCheck (orggrafptr) != 0) {
    errorPrint ("vgraphSeparateBd: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VGRAPH2 */

  return (0);
}
