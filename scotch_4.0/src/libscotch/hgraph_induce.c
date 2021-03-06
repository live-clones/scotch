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
/**   NAME       : hgraph_induce.c                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the halo source     **/
/**                graph subgraph-making functions.        **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 02 jan 2002     **/
/**                                 to     25 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HGRAPH
#define HGRAPH_INDUCE

#include "module.h"
#include "common.h"
#include "graph.h"
#include "hgraph.h"
#include "hgraph_induce.h"

/*********************************************/
/*                                           */
/* These routines handle halo source graphs. */
/*                                           */
/*********************************************/

/* This routine builds the graph induced
** by the original graph and the list of
** selected vertices.
** The induced vnumtab array is the list
** array if the original graph does not have
** a vnumtab, or the proper subset of the
** original vnumtab else.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
hgraphInduceList (
const Hgraph * restrict const     orggrafptr,     /* Pointer to original graph                 */
const VertList * restrict const   orglistptr,     /* Pointer to vertex list                    */
const Gnum                        orghalonbr,     /* Upper bound of number of vertices in halo */
Hgraph * restrict const           indgrafptr)     /* Pointer to induced subgraph               */
{
  Gnum * restrict     orgindxtax;                 /* Original to induced vertex number translation   */
  Gnum * restrict     indedgetab;                 /* Origin of induced graph edge arrays             */
  Gnum                indvertnbr;                 /* Number of vertices in induced graph             */
  Gnum                indvertnum;                 /* Number of current vertex in induced graph       */
  Gnum                indedgenbr;                 /* (Approximate) number of edges in induced graph  */
  Gnum                indedgesiz;                 /* (Approximate) size of edge and edge load arrays */

/* TODO: Ne pas allouer/copier vnumtax si elle reste copie conforme de orglistptr->vnumtab (premier graphe) */

  memSet (indgrafptr, 0, sizeof (Hgraph));        /* Pre-initialize graph fields */

  indgrafptr->s.flagval = GRAPHFREETABS | GRAPHVERTGROUP | GRAPHEDGEGROUP;
  indgrafptr->s.baseval = orggrafptr->s.baseval;

  indvertnbr = orglistptr->vnumnbr + orghalonbr;  /* Compute upper bound on number of vertices */

  {
    void *              dataptr;

    if (orggrafptr->s.velotax != NULL) {
      dataptr = memAllocGroup ((void **)
                               &indgrafptr->s.verttax, (size_t) ((indvertnbr + 1)     * sizeof (Gnum)),
                               &indgrafptr->vnhdtax,   (size_t) ( orglistptr->vnumnbr * sizeof (Gnum)), /* Put closest to beginning of array because no padding after */
                               &indgrafptr->s.velotax, (size_t) ( indvertnbr          * sizeof (Gnum)),
                               &indgrafptr->s.vnumtax, (size_t) ( indvertnbr          * sizeof (Gnum)), NULL);
      indgrafptr->s.velotax -= indgrafptr->s.baseval;
    }
    else {
      dataptr = memAllocGroup ((void **)
                               &indgrafptr->s.verttax, (size_t) ((indvertnbr + 1)     * sizeof (Gnum)),
                               &indgrafptr->vnhdtax,   (size_t) ( orglistptr->vnumnbr * sizeof (Gnum)),
                               &indgrafptr->s.vnumtax, (size_t) ( indvertnbr          * sizeof (Gnum)), NULL);
    }
    if (dataptr == NULL) {
      errorPrint ("hgraphInduceList: out of memory (1)"); /* Allocate induced graph structure */
      return     (1);
    }
  }
  memCpy (indgrafptr->s.vnumtax, orglistptr->vnumtab, orglistptr->vnumnbr * sizeof (Gnum)); /* Copy vertex number array from list */

  indgrafptr->s.verttax -= indgrafptr->s.baseval;
  indgrafptr->s.vnumtax -= indgrafptr->s.baseval;
  indgrafptr->vnhdtax   -= indgrafptr->s.baseval;
  indgrafptr->vnohnbr    = orglistptr->vnumnbr;
  indgrafptr->vnohnnd    = orglistptr->vnumnbr + indgrafptr->s.baseval;

  indedgenbr = indvertnbr * orggrafptr->s.degrmax; /* Compute upper bound of number of edges */
  if (indedgenbr > orggrafptr->s.edgenbr)
    indedgenbr = orggrafptr->s.edgenbr;
  indedgesiz = (orggrafptr->s.edlotax != NULL) ? indedgenbr * 2 : indedgenbr; /* Account for edge load array size if graph has edge weights */

  if (memAllocGroup ((void **)                    /* If cannot allocate edge arrays with approximation */
                     &indedgetab, (size_t) (indedgesiz            * sizeof (Gnum)),
                     &orgindxtax, (size_t) (orggrafptr->s.vertnbr * sizeof (Gnum)), NULL) == NULL) {
    indedgenbr = hgraphInduce3 (orggrafptr, orglistptr); /* Count real number of edges */

    indedgesiz = (orggrafptr->s.edlotax != NULL) ? indedgenbr * 2 : indedgenbr; /* Account for edge load array size if graph has edge weights */

    if ((indedgenbr < 0) ||                       /* If cannot compute real number of edges          */
        (memAllocGroup ((void **)                 /* Or cannot allocate edge arrays with real values */
                       &indedgetab, (size_t) (indedgesiz            * sizeof (Gnum)),
                       &orgindxtax, (size_t) (orggrafptr->s.vertnbr * sizeof (Gnum)), NULL) == NULL)) {
      errorPrint ("hgraphInduceList: out of memory (2)");
      hgraphExit (indgrafptr);
      return     (1);
    }
  }
  memSet (orgindxtax, ~0, orggrafptr->s.vertnbr * sizeof (Gnum)); /* Preset index array */
  orgindxtax -= orggrafptr->s.baseval;            /* Base access to orgindxtab          */

  for (indvertnum = indgrafptr->s.baseval; indvertnum < indgrafptr->vnohnnd; indvertnum ++) /* For all non-halo vertices */
    orgindxtax[indgrafptr->s.vnumtax[indvertnum]] = indvertnum; /* Mark selected vertices */

  return (hgraphInduce2 (orggrafptr, orgindxtax, indgrafptr, indedgenbr, indedgetab));
}

/* This routine finalizes the building of the
** halo graph induced by the original halo graph.
** Vertices belonging to the old halo remain to
** be numbered.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
int
hgraphInduce2 (
const Hgraph * restrict const   orggrafptr,       /* Pointer to original graph                      */
Gnum * restrict const           orgindxtax,       /* Array of numbers of selected vertices          */
Hgraph * restrict const         indgrafptr,       /* Pointer to induced graph                       */
const Gnum                      indedgenbr,       /* (Approximate) number of edges in induced graph */
Gnum * restrict const           indedgetab)       /* Pointer to pre-allocated space for edge arrays */
{
  void * restrict     indedgetnd;                 /* End of compacted edge array                 */
  Gnum                indvertnum;                 /* Current vertex number in induced halo graph */

  indedgetnd = memOffset (indedgetab, &indgrafptr->s.edgetax, (size_t) (indedgenbr * sizeof (Gnum)), NULL);
  indgrafptr->s.edgetax = indedgetab - indgrafptr->s.baseval;
#ifdef SCOTCH_DEBUG_HGRAPH2
  if (indedgetnd > (void *) (orgindxtax + orggrafptr->s.baseval)) {
    errorPrint ("hgraphInduce2: invalid edge array size (1)");
    hgraphExit (indgrafptr);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_HGRAPH2 */
  if (orggrafptr->s.edlotax != NULL) {
    size_t              indedlooftval;            /* Offset of edge load array with respect to edge array */

    indedgetnd = memOffset (indedgetnd, &indgrafptr->s.edlotax, (size_t) (indedgenbr * sizeof (Gnum)), NULL);
    indgrafptr->s.edlotax -= indgrafptr->s.baseval;
#ifdef SCOTCH_DEBUG_HGRAPH2
    if (indedgetnd > (void *) (orgindxtax + orggrafptr->s.baseval)) {
      errorPrint ("hgraphInduce2: invalid edge array size (2)");
      hgraphExit (indgrafptr);                    /* Indedgetab is now freed as part of indgrafptr */
      return     (1);
    }
#endif /* SCOTCH_DEBUG_HGRAPH2 */
    hgraphInduce2L (orggrafptr, orgindxtax, indgrafptr);

    indedlooftval = indgrafptr->s.edlotax - indgrafptr->s.edgetax;
    memReallocGroup ((void *) indedgetab,         /* Implicitely free orgindxtab */
                     &indgrafptr->s.edgetax, (size_t) (indedgenbr            * sizeof (Gnum)), /* Keep first offset as estimated number of edges */
                     &indgrafptr->s.edlotax, (size_t) (indgrafptr->s.edgenbr * sizeof (Gnum)), /* Use real number of edges for second array      */
                     NULL);
    indgrafptr->s.edgetax -= indgrafptr->s.baseval;
    indgrafptr->s.edlotax  = indgrafptr->s.edgetax + indedlooftval; /* Use old index into old array as new index */
  }
  else {
    hgraphInduce2U (orggrafptr, orgindxtax, indgrafptr);

    indgrafptr->s.edgetax  = memRealloc ((void *) indedgetab, indgrafptr->s.edgenbr * sizeof (Gnum)); /* Use real number of edges, implicitely free orgindxtab */
    indgrafptr->s.edgetax -= indgrafptr->s.baseval;
  }
  indgrafptr->s.vendtax = indgrafptr->s.verttax + 1; /* Use compact representation of arrays */
  indgrafptr->levlnum   = orggrafptr->levlnum + 1; /* Induced subgraph is one level below    */

  if (orggrafptr->s.vnumtax != NULL) {            /* Adjust vnumtax */
    for (indvertnum = indgrafptr->s.baseval; indvertnum < indgrafptr->s.vertnnd; indvertnum ++)
      indgrafptr->s.vnumtax[indvertnum] = orggrafptr->s.vnumtax[indgrafptr->s.vnumtax[indvertnum]];
  }

#ifdef SCOTCH_DEBUG_HGRAPH2
  if (hgraphCheck (indgrafptr) != 0) {            /* Check graph consistency */
    errorPrint ("hgraphInduce2: inconsistent graph data");
    hgraphExit (indgrafptr);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_HGRAPH2 */

  return (0);
}

#define HGRAPHINDUCE2U
#define HGRAPHINDUCE2NAME           hgraphInduce2U
#define HGRAPHINDUCE2EDLOINIT(e)
#include "hgraph_induce_edge.c"
#undef HGRAPHINDUCE2NAME
#undef HGRAPHINDUCE2EDLOINIT
#undef HGRAPHINDUCE2U

#define HGRAPHINDUCE2L
#define HGRAPHINDUCE2NAME           hgraphInduce2L
#define HGRAPHINDUCE2EDLOINIT(e)    indgrafptr->s.edlotax[e] = orggrafptr->s.edlotax[orgvertend]
#include "hgraph_induce_edge.c"
#undef HGRAPHINDUCE2NAME
#undef HGRAPHINDUCE2EDLOINIT
#undef HGRAPHINDUCE2L

/* This routine computes the exact number of edges
** required to build the induced halo subgraph. It
** is used when larger approximations lead to an
** out-of-memory error message. As a side effect,
** yet unnumbered halo vertices of the induced
** subgraph are numbered and the induced halo graph
** data are updated accordingly.
** It returns:
** - >=0  : number of edges in induced halo graph.
** - -1   : if out of memory (this is helpless).
*/

static
Gnum
hgraphInduce3 (
const Hgraph * restrict const   orggrafptr,       /* Pointer to original graph */
const VertList * restrict const orglistptr)       /* Pointer to vertex list    */
{
  Gnum                indedgenbr;                 /* Revised number of edges in induced halo graph */
  Gnum                indvertnum;                 /* Current vertex number in induced halo graph   */
  Gnum * restrict     orgindxtax;                 /* Array of numbers of selected vertices         */

  if ((orgindxtax = memAlloc (orggrafptr->s.vertnbr * sizeof (Gnum))) == NULL)
    return (-1);
  memSet (orgindxtax, ~0, orggrafptr->s.vertnbr * sizeof (Gnum)); /* Preset index array */
  orgindxtax -= orggrafptr->s.baseval;            /* Base access to orgindxtab          */

  for (indvertnum = 0; indvertnum < orglistptr->vnumnbr; indvertnum ++) /* For all vertices in list */
    orgindxtax[orglistptr->vnumtab[indvertnum]] = indvertnum; /* Mark selected vertices             */

  for (indvertnum = 0, indedgenbr = 0;            /* For all vertices in list */
       indvertnum < orglistptr->vnumnbr; indvertnum ++) {
    Gnum                orgvertnum;               /* Current vertex number in original halo graph */
    Gnum                orgedgenum;               /* Current edge number in original halo graph   */

    orgvertnum = orglistptr->vnumtab[indvertnum]; /* Get number of original vertex */
    indedgenbr += orggrafptr->s.vendtax[orgvertnum] - orggrafptr->s.verttax[orgvertnum]; /* Add degree of original vertex */

    for (orgedgenum = orggrafptr->s.verttax[orgvertnum]; /* For all neighbors of original halo vertex */
         orgedgenum < orggrafptr->s.vendtax[orgvertnum]; orgedgenum ++) {
      if (orgindxtax[orggrafptr->s.edgetax[orgedgenum]] == ~0) /* If neighbor is halo vertex */
        indedgenbr ++;                            /* Count for the arc once more             */
    }
  }

  memFree (orgindxtax + orggrafptr->s.baseval);

  return (indedgenbr);
}
