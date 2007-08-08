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
** $Id: graph_induce.c 71 2006-02-28 16:01:15Z pelegrin $
*/
/************************************************************/
/**                                                        **/
/**   NAME       : graph_induce.c                          **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the source graph    **/
/**                subgraph-making functions.              **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 01 dec 1992     **/
/**                                 to     18 may 1993     **/
/**                # Version 1.3  : from : 30 apr 1994     **/
/**                                 to     18 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     31 oct 1994     **/
/**                # Version 3.0  : from : 07 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 28 nov 1995     **/
/**                                 to     08 jun 1996     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     17 sep 1998     **/
/**                # Version 4.0  : from : 28 nov 2001     **/
/**                                 to     17 apr 2006     **/
/**                                                        **/
/**   NOTES      : # Several algorithms, such as the       **/
/**                  active graph building routine of      **/
/**                  bgraphInit2, assume that, for every   **/
/**                  vertex, remaining edges will be kept  **/
/**                  in the same order as in the original  **/
/**                  graph. This must be enforced.         **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH
#define GRAPH_INDUCE

#include "module.h"
#include "common.h"
#include "graph.h"
#include "graph_induce.h"

/****************************************/
/*                                      */
/* These routines handle source graphs. */
/*                                      */
/****************************************/

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
graphInduceList (
const Graph * restrict const    orggrafptr,
const VertList * restrict const indlistptr,
Graph * restrict const          indgrafptr)
{
  Gnum * restrict     orgindxtax;                 /* Based access to vertex translation array       */
  Gnum                indvertnbr;                 /* Number of vertices in induced graph            */
  Gnum                indvertnum;                 /* Number of current vertex in induced graph      */
  Gnum * restrict     indedgetab;                 /* Pointer to pre-allocated edge array            */
  Gnum                indedgenbr;                 /* (Approximate) number of edges in induced graph */

  memSet (indgrafptr, 0, sizeof (Graph));         /* Initialize graph fields */
  indgrafptr->flagval = GRAPHFREETABS | GRAPHVERTGROUP | GRAPHEDGEGROUP;
  indgrafptr->baseval = orggrafptr->baseval;

  indvertnbr = indlistptr->vnumnbr;

  if (orggrafptr->velotax != NULL) {
    if (memAllocGroup ((void **)
          &indgrafptr->verttax, (size_t) ((indvertnbr + 1) * sizeof (Gnum)),
          &indgrafptr->vnumtax, (size_t) ( indvertnbr      * sizeof (Gnum)),
          &indgrafptr->velotax, (size_t) ( indvertnbr      * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("graphInduceList: out of memory (1)");
      return     (1);                             /* Nothing to free because group allocation failed */
    }
    indgrafptr->velotax -= indgrafptr->baseval;
  }
  else {
    if (memAllocGroup ((void **)
          &indgrafptr->verttax, (size_t) ((indvertnbr + 1) * sizeof (Gnum)),
          &indgrafptr->vnumtax, (size_t) ( indvertnbr      * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("graphInduceList: out of memory (2)");
      return     (1);
    }
  }
  indgrafptr->verttax -= indgrafptr->baseval;     /* Adjust base of arrays */
  indgrafptr->vnumtax -= indgrafptr->baseval;
  indgrafptr->vertnbr  = indvertnbr;
  indgrafptr->vertnnd  = indvertnbr + indgrafptr->baseval;

  indedgenbr = indvertnbr * orggrafptr->degrmax;  /* Compute upper bound of number of edges */
  if (indedgenbr > orggrafptr->edgenbr)
    indedgenbr = orggrafptr->edgenbr;
  if (orggrafptr->edlotax != NULL)                /* If graph has edge weights */
    indedgenbr *= 2;                              /* Account for edge weights  */

  if (memAllocGroup ((void *)
        &indedgetab, (size_t) (indedgenbr          * sizeof (Gnum)), /* Pre-allocate space for edgetab (and edlotab)          */
        &orgindxtax, (size_t) (orggrafptr->vertnbr * sizeof (Gnum)), NULL) == NULL) { /* orgindxtab is at the end of the heap */
    errorPrint ("graphInduceList: out of memory (3)");
    graphExit  (indgrafptr);
    return     (1);
  }

/* TODO: Vertex list can be kept as it is the one of *graphOrderNd */

  memCpy (indgrafptr->vnumtax + indgrafptr->baseval, /* Copy vertex number array from list */
          indlistptr->vnumtab, indvertnbr * sizeof (Gnum));

  memSet (orgindxtax, ~0, orggrafptr->vertnbr * sizeof (Gnum)); /* Preset index array */
  orgindxtax -= orggrafptr->baseval;

  for (indvertnum = indgrafptr->baseval, indedgenbr = 0; /* Fill index array */
       indvertnum < indgrafptr->baseval + indvertnbr; indvertnum ++) {
    Gnum                orgvertnum;

    orgvertnum = indgrafptr->vnumtax[indvertnum];

    orgindxtax[orgvertnum] = indvertnum;          /* Mark selected vertices */
    indedgenbr += orggrafptr->vendtax[orgvertnum] - orggrafptr->verttax[orgvertnum];
  }

  return (graphInduce2 (orggrafptr, indgrafptr, indvertnbr, indedgenbr, indedgetab, orgindxtax));
}

/* This routine builds the graph induced
** by the original graph and the vector
** of selected vertices.
** The induced vnumtab array is the list of
** selected vertices if the original graph
** does not have a vnumtab, or the proper
** subset of the original vnumtab else.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
graphInducePart (
const Graph * restrict const  orggrafptr,         /* Pointer to original graph             */
const GraphPart * const       orgparttax,         /* Based array of vertex partition flags */
const Gnum                    indpartnbr,         /* Number of vertices in selected part   */
const GraphPart               indpartval,         /* Partition value of vertices to keep   */
Graph * restrict const        indgrafptr)         /* Pointer to induced subgraph           */
{
  Gnum * restrict             orgindxtab;         /* Original to induced vertex translation array   */
  Gnum * restrict             orgindxtax;         /* Based access to vertex translation array       */
  Gnum                        indvertnum;         /* Number of current vertex in induced graph      */
  Gnum * restrict             indedgetab;         /* Pointer to pre-allocated edge array            */
  Gnum                        indedgenbr;         /* (Approximate) number of edges in induced graph */
  Gnum                        orgvertnum;

  memSet (indgrafptr, 0, sizeof (Graph));         /* Initialize graph fields  */
  indgrafptr->flagval = GRAPHFREETABS | GRAPHVERTGROUP | GRAPHEDGEGROUP;
  indgrafptr->baseval = orggrafptr->baseval;

  indedgenbr = indpartnbr * orggrafptr->degrmax;  /* Compute upper bound of number of edges */
  if (indedgenbr > orggrafptr->edgenbr)
    indedgenbr = orggrafptr->edgenbr;
  if (orggrafptr->edlotax != NULL)                /* If graph has edge weights */
    indedgenbr *= 2;                              /* Account for edge weights  */

  if (orggrafptr->velotax != NULL) {
    if (memAllocGroup ((void **)
          &indgrafptr->verttax, (size_t) ((indpartnbr + 1) * sizeof (Gnum)),
          &indgrafptr->vnumtax, (size_t) ( indpartnbr      * sizeof (Gnum)),
          &indgrafptr->velotax, (size_t) ( indpartnbr      * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("graphInducePart: out of memory (1)");
      return     (1);                             /* Nothing to free because group allocation failed */
    }
    indgrafptr->velotax -= indgrafptr->baseval;
  }
  else {
    if (memAllocGroup ((void **)
          &indgrafptr->verttax, (size_t) ((indpartnbr + 1) * sizeof (Gnum)),
          &indgrafptr->vnumtax, (size_t) ( indpartnbr      * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("graphInducePart: out of memory (2)");
      return     (1);
    }
  }
  indgrafptr->verttax -= indgrafptr->baseval;     /* Adjust base of arrays */
  indgrafptr->vnumtax -= indgrafptr->baseval;
  indgrafptr->vertnbr  = indpartnbr;
  indgrafptr->vertnnd  = indpartnbr + indgrafptr->baseval;

  if (memAllocGroup ((void *)
        &indedgetab, (size_t) (indedgenbr          * sizeof (Gnum)), /* Pre-allocate space for edgetab (and edlotab)          */
        &orgindxtab, (size_t) (orggrafptr->vertnbr * sizeof (Gnum)), NULL) == NULL) { /* orgindxtab is at the end of the heap */
    errorPrint ("graphInducePart: out of memory (3)");
    graphExit  (indgrafptr);
    return     (1);
  }
  orgindxtax = orgindxtab - orggrafptr->baseval;

  for (indvertnum = indgrafptr->baseval, indedgenbr = 0, orgvertnum = orggrafptr->baseval; /* Fill index array */
       orgvertnum < orggrafptr->vertnnd; orgvertnum ++) {
    if (orgparttax[orgvertnum] == indpartval) {   /* If vertex should be kept */
      orgindxtax[orgvertnum] = indvertnum;        /* Mark selected vertex     */
      indgrafptr->vnumtax[indvertnum] = orgvertnum;
      indedgenbr += orggrafptr->vendtax[orgvertnum] - orggrafptr->verttax[orgvertnum];
      indvertnum ++;                              /* One more induced vertex created */
    }
    else
      orgindxtax[orgvertnum] = ~0;
  }
#ifdef SCOTCH_DEBUG_GRAPH2
  if ((indvertnum - indgrafptr->baseval) != indpartnbr) {
    errorPrint ("graphInducePart: inconsistent data");
    memFree    (indedgetab);
    graphExit  (indgrafptr);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_GRAPH2 */

  return (graphInduce2 (orggrafptr, indgrafptr, indpartnbr, indedgenbr, indedgetab, orgindxtax));
}

/* This routine finalizes the building
** of the induced subgraph.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
int
graphInduce2 (
const Graph * restrict const  orggrafptr,         /* Pointer to original graph                          */
Graph * restrict const        indgrafptr,         /* Pointer to induced graph                           */
const Gnum                    indvertnbr,         /* Number of vertices in induced graph                */
const Gnum                    indedgenbr,         /* (Upper bound of) number of edges in induced graph  */
Gnum * restrict const         indedgetab,         /* Pointer to pre-allocated edge and edge load arrays */
const Gnum * restrict const   orgindxtax)         /* Array of numbers of selected vertices              */
{
  Gnum                indvertnum;                 /* Current induced vertex number              */
  Gnum                indvelosum;                 /* Overall induced vertex load                */
  Gnum                indedgenum;                 /* Number of current induced edge             */
  Gnum                orgvertnum;                 /* Number of current vertex in original graph */
  Gnum                orgedgenum;                 /* Number of current edge in original graph   */

  if (orggrafptr->edlotax != NULL) {
    memOffset ((void *) indedgetab,
               &indgrafptr->edgetax, (size_t) (indedgenbr * sizeof (Gnum)),
               &indgrafptr->edlotax, (size_t) (indedgenbr * sizeof (Gnum)), NULL);
    indgrafptr->edgetax -= indgrafptr->baseval;
    indgrafptr->edlotax -= indgrafptr->baseval;
  }
  else
    indgrafptr->edgetax = indedgetab - indgrafptr->baseval;

  indgrafptr->velosum = 0;
  indgrafptr->degrmax = 0;
  for (indvertnum = indedgenum = indgrafptr->baseval, indvelosum = 0;
       indvertnum < indgrafptr->vertnnd; indvertnum ++) {
    orgvertnum = indgrafptr->vnumtax[indvertnum];
    indgrafptr->verttax[indvertnum] = indedgenum;
    if (indgrafptr->velotax != NULL) {            /* If graph has vertex weights */
      indvelosum +=                               /* Accumulate vertex loads     */
      indgrafptr->velotax[indvertnum] = orggrafptr->velotax[orgvertnum];
    }

    if (indgrafptr->edlotax != NULL) {            /* If graph has edge weights */
      for (orgedgenum = orggrafptr->verttax[orgvertnum];
           orgedgenum < orggrafptr->vendtax[orgvertnum]; orgedgenum ++) {
        if (orgindxtax[orggrafptr->edgetax[orgedgenum]] != ~0) { /* If edge should be kept */
          indgrafptr->edgetax[indedgenum] = orgindxtax[orggrafptr->edgetax[orgedgenum]];
          indgrafptr->edlotax[indedgenum] = orggrafptr->edlotax[orgedgenum];
          indedgenum ++;
        }
      }
    }
    else {
      for (orgedgenum = orggrafptr->verttax[orgvertnum];
           orgedgenum < orggrafptr->vendtax[orgvertnum]; orgedgenum ++) {
        if (orgindxtax[orggrafptr->edgetax[orgedgenum]] != ~0) { /* If edge should be kept */
          indgrafptr->edgetax[indedgenum] = orgindxtax[orggrafptr->edgetax[orgedgenum]];
          indedgenum ++;
        }
      }
    }
    if (indgrafptr->degrmax < (indedgenum - indgrafptr->verttax[indvertnum]))
      indgrafptr->degrmax = (indedgenum - indgrafptr->verttax[indvertnum]);
  }
  indgrafptr->verttax[indvertnum] = indedgenum;   /* Mark end of edge array                      */
  indgrafptr->vendtax  = indgrafptr->verttax + 1; /* Use compact representation of vertex arrays */
  indgrafptr->vertnbr = indvertnum - indgrafptr->baseval;
  indgrafptr->vertnnd = indvertnum;
  indgrafptr->velosum = (indgrafptr->velotax == NULL) ? indgrafptr->vertnbr : indvelosum;
  indgrafptr->edgenbr = indedgenum - indgrafptr->baseval; /* Set actual number of edges */

  if (indgrafptr->edlotax != NULL) {              /* Re-allocate arrays and delete orgindxtab             */
    size_t              indedlooftval;            /* Offset of edge load array with respect to edge array */

    indedlooftval = indgrafptr->edlotax - indgrafptr->edgetax;
    indgrafptr->edgetax  = memRealloc (indgrafptr->edgetax + indgrafptr->baseval, (indedlooftval + indgrafptr->edgenbr) * sizeof (Gnum));
    indgrafptr->edgetax -= indgrafptr->baseval;
    indgrafptr->edlotax  = indgrafptr->edgetax + indedlooftval; /* Use old index into old array as new index */
  }
  else {
    indgrafptr->edgetax  = memRealloc (indgrafptr->edgetax + indgrafptr->baseval, indgrafptr->edgenbr * sizeof (Gnum));
    indgrafptr->edgetax -= indgrafptr->baseval;
  }

  if (orggrafptr->vnumtax != NULL) {              /* Adjust vnumtax */
    for (indvertnum = indgrafptr->baseval; indvertnum < indgrafptr->vertnnd; indvertnum ++)
      indgrafptr->vnumtax[indvertnum] = orggrafptr->vnumtax[indgrafptr->vnumtax[indvertnum]];
  }

#ifdef SCOTCH_DEBUG_GRAPH2
  if (graphCheck (indgrafptr) != 0) {             /* Check graph consistency */
    errorPrint ("graphInduce2: inconsistent graph data");
    graphExit  (indgrafptr);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_GRAPH2 */

  return (0);
}
