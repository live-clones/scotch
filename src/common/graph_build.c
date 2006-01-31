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
/**   NAME       : graph_build.c                           **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These module holds the generic graph    **/
/**                building routine.                       **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 15 oct 2000     **/
/**                                 to     15 oct 2000     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH

#include "common.h"
#include "graph.h"

/********************************/
/*                              */
/* The graph handling routines. */
/*                              */
/********************************/

/* This routine builds a graph
** structure from the given
** algorithmic graph definition.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
*/

int
graphBuild (
Graph * const               grafptr,              /*+ Graph to build                           +*/
const INT                   vertnbr,              /*+ Number of vertices                       +*/
const INT                   edgenbr,              /*+ Number of arcs                           +*/
const INT                   degrmax,              /*+ (Upper bound of) maximum degree          +*/
const INT                   baseval,              /*+ Base value                               +*/
void * const                nghbptr,              /*+ Pointer to adjacency structure           +*/
INT                         nghbfrstfunc (void * const, const INT), /*+ Returns first neighbor +*/
INT                         nghbnextfunc (void * const)) /*+ Returns next neighbor             +*/
{
  INT *               verttax;
  INT *               edgetax;
  INT                 vertnum;
  INT                 edgenum;
  INT                 vertend;

  grafptr->baseval = baseval;                     /* Set structure fields */
  grafptr->vertnbr = vertnbr;
  grafptr->edgenbr = edgenbr;
  if (((grafptr->verttab = (INT *) memAlloc ((grafptr->vertnbr + 1) * sizeof (INT))) == NULL) ||
      ((grafptr->edgetab = (INT *) memAlloc (grafptr->edgenbr       * sizeof (INT))) == NULL)) {
    errorPrint ("graphBuild: out of memory");
    if (grafptr->verttab != NULL)
      memFree (grafptr->verttab);
    return (1);
  }
  grafptr->velotab = NULL;                        /* Constant number of DOFs per vertex */
  grafptr->veloval = 1;                           /* All vertices have weight 1         */
  grafptr->vlbltab = NULL;                        /* No vertex labels                   */
  grafptr->degrmax = degrmax;                     /* Upper bound on maximum degree      */

  verttax = grafptr->verttab - baseval;
  edgetax = grafptr->edgetab - baseval;

  for (vertnum = edgenum = baseval; vertnum < grafptr->vertnbr + baseval; vertnum ++) {
    verttax[vertnum] = edgenum;

    if ((vertend = nghbfrstfunc (nghbptr, vertnum)) >= baseval) {
#ifdef GRAPH_DEBUG
      if (edgenum >= (edgenbr + baseval)) {
        errorPrint ("graphBuild: too many edges in graph (1)");
        graphExit  (grafptr);
        graphInit  (grafptr);
        return     (1);
      }
#endif /* GRAPH_DEBUG */
      edgetax[edgenum ++] = vertend;
      while ((vertend = nghbnextfunc (nghbptr)) >= baseval) {
#ifdef GRAPH_DEBUG
        if (edgenum >= (edgenbr + baseval)) {
          errorPrint ("graphBuild: too many edges in graph (2)");
          graphExit  (grafptr);
          graphInit  (grafptr);
          return     (1);
        }
#endif /* GRAPH_DEBUG */
        edgetax[edgenum ++] = vertend;
      }
    }
  }
  verttax[vertnum] = edgenum;                     /* Set end of array */

  grafptr->edgenbr = edgenum - baseval;           /* Use exact number of arcs */

#ifdef GRAPH_DEBUG
  if (graphCheck (grafptr) != 0) {                /* Check graph consistency */
    errorPrint ("graphBuild: inconsistent graph data");
    graphExit  (grafptr);
    graphInit  (grafptr);
    return     (1);
  }
#endif /* GRAPH_DEBUG */

  return (0);
}
