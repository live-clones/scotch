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
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the source graph   **/
/**                checking function.                      **/
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
/**                                 to     15 sep 1998     **/
/**                # Version 3.3  : from : 22 sep 1998     **/
/**                                 to     31 dec 1998     **/
/**                # Version 4.0  : from : 24 nov 2001     **/
/**                                 to     22 apr 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH

#include "module.h"
#include "common.h"
#include "graph.h"

/****************************************/
/*                                      */
/* These routines handle source graphs. */
/*                                      */
/****************************************/

/* This routine checks the consistency
** of the given graph.
** It returns:
** - 0   : if graph data are consistent.
** - !0  : on error.
*/

int
graphCheck (
const Graph * const         grafptr)
{
  Gnum                vertnum;                    /* Number of current vertex  */
  Gnum                velosum;                    /* Sum of vertex loads       */
  Gnum                edgenum;                    /* Number of current edge    */
  Gnum                degrmax;                    /* Maximum degree            */

  if (grafptr->vertnbr != (grafptr->vertnnd - grafptr->baseval)) {
    errorPrint ("graphCheck: invalid vertex numbers");
    return     (1);
  }

  degrmax = 0;
  velosum = 0;
  for (vertnum = grafptr->baseval; vertnum < grafptr->vertnnd; vertnum ++) {
    if ((grafptr->verttax[vertnum] < grafptr->baseval)          ||
        (grafptr->vendtax[vertnum] < grafptr->verttax[vertnum])) {
      errorPrint ("graphCheck: invalid vertex arrays");
      return     (1);
    }

    if ((grafptr->vendtax[vertnum] - grafptr->verttax[vertnum]) > degrmax)
      degrmax = grafptr->vendtax[vertnum] - grafptr->verttax[vertnum];

    for (edgenum = grafptr->verttax[vertnum]; edgenum < grafptr->vendtax[vertnum]; edgenum ++) {
      Gnum                vertend;                /* Number of end vertex      */
      Gnum                edgeend;                /* Number of end vertex edge */

      vertend = grafptr->edgetax[edgenum];

      if ((vertend < grafptr->baseval) || (vertend >= grafptr->vertnnd)) { /* If invalid edge end */
        errorPrint ("graphCheck: invalid edge array");
        return     (1);
      }
      if (vertend == vertnum) {                   /* Loops not allowed */
        errorPrint ("graphCheck: loops not allowed");
        return     (1);
      }
      for (edgeend = grafptr->verttax[vertend];   /* Search for matching arc */
           (edgeend < grafptr->vendtax[vertend]) && (grafptr->edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if ((edgeend >= grafptr->vendtax[vertend]) ||
          ((grafptr->edlotax != NULL) && (grafptr->edlotax[edgenum] != grafptr->edlotax[edgeend]))) {
        errorPrint ("graphCheck: arc data do not match");
        return     (1);
      }
      for (edgeend ++;                            /* Search for duplicate arcs */
           (edgeend < grafptr->vendtax[vertend]) && (grafptr->edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if (edgeend < grafptr->vendtax[vertend]) {
        errorPrint ("graphCheck: duplicate arc");
        return     (1);
      }
    }
    if (grafptr->velotax != NULL) {
      if (grafptr->velotax[vertnum] < 1) {        /* If non-strictly positive loads */
        errorPrint ("graphCheck: invalid vertex load array");
        return     (1);
      }
      velosum += grafptr->velotax[vertnum];       /* Accumulate vertex loads */
    }
  }
  if (((grafptr->velotax != NULL) && (grafptr->velosum != velosum)) ||
      ((grafptr->velotax == NULL) && (grafptr->velosum != grafptr->vertnnd - grafptr->baseval))) {
    errorPrint ("graphCheck: invalid vertex load sum");
    return     (1);
  }
  if (grafptr->degrmax < degrmax) {
    errorPrint ("graphCheck: invalid maximum degree");
    return     (1);
  }

  return (0);
}
