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
/**   NAME       : hgraph.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the source graph    **/
/**                functions.                              **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 17 jan 2002     **/
/**                                 to     01 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HGRAPH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "hgraph.h"

/****************************************/
/*                                      */
/* These routines handle source graphs. */
/*                                      */
/****************************************/

/* This routine initializes a source graph
** structure.
** It returns:
** - 0  : in all cases.
*/

int
hgraphInit (
Hgraph * restrict const     grafptr)
{
  memSet (grafptr, 0, sizeof (Hgraph));           /* Initialize graph fields     */
  grafptr->s.flagval = GRAPHFREETABS;             /* By default, free all arrays */

  return (0);
}

/* This routine frees a source graph structure.
** It returns:
** - VOID  : in all cases.
*/

void
hgraphExit (
Hgraph * restrict const     grafptr)
{
  hgraphFree (grafptr);
}

/* This routine frees a source graph structure.
** It returns:
** - VOID  : in all cases.
*/

void
hgraphFree (
Hgraph * restrict const     grafptr)
{
  if ((grafptr->vnhdtax != NULL)               && /* Free end vertex array for non-halo vertices */
      (grafptr->vnhdtax != grafptr->s.vendtax) &&
      ((grafptr->s.flagval & HGRAPHFREEVHND) != 0))
    memFree (grafptr->vnhdtax);

  graphFree (&grafptr->s);                        /* Free graph data */

#ifdef SCOTCH_DEBUG_HGRAPH2
  memSet (grafptr, ~0, sizeof (Hgraph));          /* Purge graph fields */
#endif /* SCOTCH_DEBUG_HGRAPH2 */
}

/* This routine checks the consistency
** of the given halo graph.
** It returns:
** - 0   : if graph data are consistent.
** - !0  : on error.
*/

int
hgraphCheck (
const Hgraph * restrict const grafptr)
{
  Gnum                vertnum;                    /* Number of current vertex */
  Gnum                edgenum;                    /* Number of current edge   */

  if (graphCheck (&grafptr->s) != 0) {
    errorPrint ("hgraphCheck: invalid graph structure in halo graph");
    return     (1);
  }

  if ((grafptr->vnohnbr < 0)                                        ||
      (grafptr->vnohnbr > grafptr->s.vertnbr)                       ||
      (grafptr->vnohnnd != (grafptr->vnohnbr + grafptr->s.baseval)) ||
      (grafptr->vnlosum > grafptr->s.velosum)                       ||
      (grafptr->enohnbr > grafptr->s.edgenbr)) {
    errorPrint ("hgraphCheck: invalid halo graph parameters");
    return     (1);
  }

  for (vertnum = grafptr->s.baseval; vertnum < grafptr->vnohnnd; vertnum ++) { /* For all non-halo vertices */
    if ((grafptr->vnhdtax[vertnum] < grafptr->s.verttax[vertnum]) ||
        (grafptr->vnhdtax[vertnum] > grafptr->s.vendtax[vertnum])) {
      errorPrint ("hgraphCheck: invalid non-halo end vertex array");
      return     (1);
    }
  }

  for ( ; vertnum < grafptr->s.vertnnd; vertnum ++) { /* For all halo vertices */
    for (edgenum = grafptr->s.verttax[vertnum]; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
      if (grafptr->s.edgetax[edgenum] >= grafptr->vnohnnd) { /* If two halo vertices connected together */
        errorPrint ("hgraphCheck: halo vertices should not be connected together");
        return     (1);
      }
    }
  }

  return (0);
}
