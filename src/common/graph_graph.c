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
/**   NAME       : graph_graph.c                           **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These module holds the array graph      **/
/**                building routine.                       **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 14 oct 2003     **/
/**                                 to     22 jan 2004     **/
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
** arrays.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
*/

int
graphBuildGraph (
Graph * const               grafptr,              /*+ Graph to build                             +*/
const INT                   vertnbr,              /*+ Number of vertices                         +*/
const INT                   edgenbr,              /*+ Number of arcs                             +*/
const INT                   degrmax,              /*+ (Upper bound of) maximum degree            +*/
const INT                   baseval,              /*+ Base value                                 +*/
INT * restrict              verttab,              /*+ Vertex array                               +*/
INT * restrict              velotab,              /*+ Array of vertex weights (DOFs) if not NULL +*/
const INT                   veloval,              /*+ Number of DOFs per node if velotab == NULL +*/
INT * restrict              edgetab)              /*+ Edge array                                 +*/
{
  grafptr->baseval = baseval;                     /* Set structure fields */
  grafptr->vertnbr = vertnbr;
  grafptr->edgenbr = edgenbr;
  grafptr->verttab = verttab;
  grafptr->velotab = velotab;
  grafptr->veloval = veloval;
  grafptr->vlbltab = NULL;                        /* No vertex labels */
  grafptr->edgetab = edgetab;
  grafptr->degrmax = degrmax;

#ifdef GRAPH_DEBUG
  if (graphCheck (grafptr) != 0) {                /* Check graph consistency */
    errorPrint ("graphBuildGraph: inconsistent graph data");
    return     (1);
  }
#endif /* GRAPH_DEBUG */

  return (0);
}
