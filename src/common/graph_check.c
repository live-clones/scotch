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
/**   NAME       : graph_check.c                           **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines hold the graph checking     **/
/**                routine.                                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 17 apr 2002     **/
/**                                 to     17 apr 2002     **/
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

/* This routine sets the base of the
** given graph structure to the given
** base value.
** It returns:
** - VOID  : in all cases.
*/

int
graphCheck (
const Graph * const         grafptr)              /*+ Graph structure +*/
{
  INT                 baseval;                    /* Base value                        */
  INT * restrict      verttax;                    /* Based access to vertex array      */
  INT                 vertnum;                    /* Number of current vertex          */
  INT                 vertmax;                    /* Maximum vertex index              */
  INT * restrict      velotax;                    /* Based access to vertex load array */
  INT * restrict      edgetax;                    /* Based access to edge array        */
  INT                 edgenum;                    /* Number of current edge            */
  INT                 edgeend;                    /* Number of end vertex edge         */
  INT                 edgemax;                    /* Maximum edge index [+1]           */
  INT                 degrmax;                    /* Maximum degree                    */

  baseval = grafptr->baseval;
  verttax = grafptr->verttab - baseval;
  vertmax = grafptr->vertnbr + (baseval - 1);
  velotax = (grafptr->velotab == NULL) ? NULL : grafptr->velotab - baseval;
  edgetax = grafptr->edgetab - baseval;
  edgemax = grafptr->edgenbr + baseval;

  degrmax = 0;
  for (vertnum = baseval; vertnum <= vertmax; vertnum ++) {
    if ((verttax[vertnum]     < baseval)          ||
        (verttax[vertnum + 1] < verttax[vertnum]) ||
        (verttax[vertnum + 1] > edgemax)) {
      errorPrint ("graphCheck: invalid vertex array");
      return     (1);
    }

    if ((verttax[vertnum + 1] - verttax[vertnum]) > degrmax)
      degrmax = (verttax[vertnum + 1] - verttax[vertnum]);

    for (edgenum = verttax[vertnum]; edgenum < verttax[vertnum + 1]; edgenum ++) {
      if ((edgetax[edgenum] < baseval) ||         /* If invalid edge end */
          (edgetax[edgenum] > vertmax)) {
        errorPrint ("graphCheck: invalid edge array");
        return     (1);
      }
      if (edgetax[edgenum] == vertnum) {          /* Loops not allowed */
        errorPrint ("graphCheck: loops not allowed");
        return     (1);
      }
      for (edgeend = verttax[edgetax[edgenum]];   /* Search for matching arc */
           (edgeend < verttax[edgetax[edgenum] + 1]) && (edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if (edgeend >= verttax[edgetax[edgenum] + 1]) {
        errorPrint ("graphCheck: arc data do not match");
        return     (1);
      }
      for (edgeend ++;                            /* Search for duplicate arcs */
           (edgeend < verttax[edgetax[edgenum] + 1]) && (edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if (edgeend < verttax[edgetax[edgenum] + 1]) {
        errorPrint ("graphCheck: duplicate arc");
        return     (1);
      }
    }
    if (velotax != NULL) {
      if (velotax[vertnum] < 1) {                 /* If non-strictly positive loads */
        errorPrint ("graphCheck: invalid vertex load array");
        return     (1);
      }
    }
  }
  if (grafptr->degrmax < degrmax) {
    errorPrint ("graphCheck: invalid maximum degree");
    return     (1);
  }

  return (0);
}
