/* Copyright INRIA 2004
**
** This file is part of the Scotch distribution.
**
** The Scotch distribution is libre/free software; you can
** redistribute it and/or modify it under the terms of the
** GNU Lesser General Public License as published by the
** Free Software Foundation; either version 2.1 of the
** License, or (at your option) any later version.
**
** The Scotch distribution is distributed in the hope that
** it will be useful, but WITHOUT ANY WARRANTY; without even
** the implied warranty of MERCHANTABILITY or FITNESS FOR A
** PARTICULAR PURPOSE. See the GNU Lesser General Public
** License for more details.
**
** You should have received a copy of the GNU Lesser General
** Public License along with the Scotch distribution; if not,
** write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
**
** $Id$
*/
/************************************************************/
/**                                                        **/
/**   NAME       : vgraph_separate_ml.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module separates an active         **/
/**                graph using a multi-level scheme.       **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 28 oct 1997     **/
/**                                 to     05 nov 1997     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 13 dec 2001     **/
/**                                 to     20 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VGRAPH_SEPARATE_ML

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "graph_coarsen.h"
#include "vgraph.h"
#include "vgraph_separate_ml.h"
#include "vgraph_separate_st.h"

/*********************************************/
/*                                           */
/* The coarsening and uncoarsening routines. */
/*                                           */
/*********************************************/

/* This routine builds a coarser graph from the
** graph that is given on input. The coarser
** graphs differ at this stage from classical
** active graphs as their internal gains are not
** yet computed.
** It returns:
** - 0  : if the coarse graph has been built.
** - 1  : if threshold achieved or on error.
*/

static
int
vgraphSeparateMlCoarsen (
const Vgraph * restrict const         finegrafptr, /*+ Finer graph                         +*/
Vgraph * restrict const               coargrafptr, /*+ Coarser graph to build              +*/
GraphCoarsenMulti * restrict * const  coarmultptr, /*+ Pointer to multinode table to build +*/
const VgraphSeparateMlParam * const   paraptr)    /*+ Method parameters                    +*/
{
  if (graphCoarsen (&finegrafptr->s, &coargrafptr->s, coarmultptr, paraptr->coarnbr, paraptr->coarrat, paraptr->coartype) != 0)
    return (1);                                   /* Return if coarsening failed */

  coargrafptr->parttax = NULL;                    /* Do not allocate partition data yet      */
  coargrafptr->frontab = finegrafptr->frontab;    /* Re-use frontier array for coarser graph */
  coargrafptr->levlnum = finegrafptr->levlnum + 1; /* Graph level is coarsening level        */

  return (0);
}

/* This routine propagates the separation of the
** coarser graph back to the finer graph, according
** to the multinode table of collapsed vertices.
** After the separation is propagated, it finishes
** to compute the parameters of the finer graph that
** were not computed at the coarsening stage.
** It returns:
** - 0   : if coarse graph data has been propagated to fine graph.
** - !0  : on error.
*/

static
int
vgraphSeparateMlUncoarsen (
Vgraph * restrict const                     finegrafptr, /*+ Finer graph     +*/
const Vgraph * restrict const               coargrafptr, /*+ Coarser graph   +*/
const GraphCoarsenMulti * restrict const    coarmulttax) /*+ Multinode array +*/
{
  Gnum                coarvertnum;                /* Number of current coarse vertex           */
  Gnum                finefronnbr;                /* Number of frontier vertices in fine graph */

  if (finegrafptr->parttax == NULL) {             /* If partition array not yet allocated */
    if ((finegrafptr->parttax = (GraphPart *) memAlloc (finegrafptr->s.vertnbr * sizeof (GraphPart))) == NULL) {
      errorPrint ("vgraphSeparateMlUncoarsen: out of memory");
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
    finegrafptr->parttax -= finegrafptr->s.baseval;
  }

  if (coargrafptr != NULL) {                      /* If coarser graph provided         */
    Gnum                finesize1;                /* Number of vertices in fine part 1 */

    finesize1 = coargrafptr->compsize[1];         /* Pre-allocate size */

    for (coarvertnum = coargrafptr->s.baseval, finefronnbr = 0;
         coarvertnum < coargrafptr->s.vertnnd; coarvertnum ++) {
      GraphPart           coarpartval;            /* Value of current multinode part */

      coarpartval = coargrafptr->parttax[coarvertnum];
      finegrafptr->parttax[coarmulttax[coarvertnum].vertnum[0]] = coarpartval;
      if (coarpartval != 2) {                     /* If vertex is not in separator */
        if (coarmulttax[coarvertnum].vertnum[0] !=
            coarmulttax[coarvertnum].vertnum[1]) {
          finegrafptr->parttax[coarmulttax[coarvertnum].vertnum[1]] = coarpartval;
          finesize1 += (Gnum) coarpartval;        /* One extra vertex created in part 1 if (coarpartval == 1) */
        }
      }
      else {                                      /* Vertex is in separator */
        finegrafptr->frontab[finefronnbr ++] = coarmulttax[coarvertnum].vertnum[0];
        if (coarmulttax[coarvertnum].vertnum[0] !=
            coarmulttax[coarvertnum].vertnum[1]) {
          finegrafptr->parttax[coarmulttax[coarvertnum].vertnum[1]] = coarpartval;
          finegrafptr->frontab[finefronnbr ++] = coarmulttax[coarvertnum].vertnum[1]; /* One extra vertex in separator */
        }
      }
    }

    finegrafptr->fronnbr     = finefronnbr;
    finegrafptr->compload[0] = coargrafptr->compload[0];
    finegrafptr->compload[1] = coargrafptr->compload[1];
    finegrafptr->compload[2] = coargrafptr->compload[2];
    finegrafptr->comploaddlt = coargrafptr->comploaddlt;
    finegrafptr->compsize[0] = finegrafptr->s.vertnbr - finefronnbr - finesize1;
    finegrafptr->compsize[1] = finesize1;
  }
  else                                            /* No coarse graph provided      */
    vgraphZero (finegrafptr);                     /* Assign all vertices to part 0 */

#ifdef SCOTCH_DEBUG_VGRAPH2
  if (vgraphCheck (finegrafptr) != 0) {
    errorPrint ("vgraphSeparateMlUncoarsen: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VGRAPH2 */

  return (0);
}

/* This routine recursively performs the
** separation recursion.
** It returns:
** - 0   : if separator could be computed.
** - !0  : on error.
*/

static
int
vgraphSeparateMl2 (
Vgraph * restrict const             grafptr,      /* Vertex-separation graph */
const VgraphSeparateMlParam * const paraptr)      /* Method parameters       */
{
  Vgraph                        coargrafdat;
  GraphCoarsenMulti * restrict  coarmulttax;
  int                           o;

  if (vgraphSeparateMlCoarsen (grafptr, &coargrafdat, &coarmulttax, paraptr) == 0) {
    if (((o = vgraphSeparateMl2         (&coargrafdat, paraptr))              == 0) &&
        ((o = vgraphSeparateMlUncoarsen (grafptr, &coargrafdat, coarmulttax)) == 0) &&
        ((o = vgraphSeparateSt          (grafptr, paraptr->stratasc))         != 0)) /* Apply ascending strategy */
      errorPrint ("vgraphSeparateMl2: cannot apply ascending strategy");
    coargrafdat.frontab = NULL;                   /* Prevent frontab of fine graph from being freed */
    vgraphExit (&coargrafdat);
  }
  else {                                          /* Cannot coarsen due to lack of memory or error */
    if (((o = vgraphSeparateMlUncoarsen (grafptr, NULL, NULL)) == 0) && /* Finalize graph          */
        ((o = vgraphSeparateSt          (grafptr, paraptr->stratlow)) != 0)) /* Apply low strategy */
      errorPrint ("vgraphSeparateMl2: cannot apply low strategy");
  }

  return (o);
}

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the muti-level separation.
** It returns:
** - 0 : if separator could be computed.
** - 1 : on error.
*/

int
vgraphSeparateMl (
Vgraph * const                      grafptr,      /*+ Vertex-separation graph +*/
const VgraphSeparateMlParam * const paraptr)      /*+ Method parameters       +*/
{
  Gnum                levlnum;                    /* Save value for graph level */
  int                 o;

  levlnum = grafptr->levlnum;                     /* Save graph level               */
  grafptr->levlnum = 0;                           /* Initialize coarsening level    */
  o = vgraphSeparateMl2 (grafptr, paraptr);       /* Perform multi-level separation */
  grafptr->levlnum = levlnum;                     /* Restore graph level            */

  return (o);
}
