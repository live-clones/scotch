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
/**   NAME       : hgraph_order_hx.c                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains service routines   **/
/**                for the hgraphOrderH{d|f} ordering      **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 23 jan 2004     **/
/**                                 to   : 28 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HGRAPH_ORDER_HX

#include "module.h"
#include "common.h"
#include "graph.h"
#include "hgraph.h"
#include "hgraph_order_hx.h"

/***********************************/
/*                                 */
/* These are the service routines. */
/*                                 */
/***********************************/

/* This routine fills the input arrays for
** the graph ordering routines.
** It returns:
** - void  : in all cases.
*/

void
hgraphOrderHxFill (
const Hgraph * restrict const             grafptr,
Gnum * restrict const                     petab,
Gnum * restrict const                     lentab,
Gnum * restrict const                     iwtab,
Gnum * restrict const                     elentab,
Gnum * restrict const                     pfreptr)
{
  Gnum * restrict             petax;
  Gnum * restrict             iwtax;
  Gnum * restrict             lentax;
  Gnum * restrict             elentax;
  Gnum                        vertadj;            /* Index adjustment for vertices */
  Gnum                        vertnum;
  Gnum                        vertnew;
  Gnum                        edgenew;

  petax   = petab - 1;                            /* Base HAMF arrays at base 1 */
  iwtax   = iwtab - 1;
  lentax  = lentab - 1;
  elentax = elentab - 1;

  vertadj = 1 - grafptr->s.baseval;
  for (vertnum = grafptr->s.baseval, vertnew = edgenew = 1; /* Process non-halo vertices */
       vertnum < grafptr->vnohnnd; vertnum ++, vertnew ++) {
    Gnum                      degrval;
    Gnum                      edgenum;

    degrval = grafptr->s.vendtax[vertnum] - grafptr->s.verttax[vertnum];
    petax[vertnew]   = edgenew;
    lentax[vertnew]  = degrval;
    elentax[vertnew] = degrval;

    for (edgenum = grafptr->s.verttax[vertnum];
         edgenum < grafptr->s.vendtax[vertnum]; edgenum ++, edgenew ++)
      iwtax[edgenew] = grafptr->s.edgetax[edgenum] + vertadj;
  }
  for ( ; vertnum < grafptr->s.vertnnd; vertnum ++, vertnew ++) { /* Process halo vertices */
    Gnum                      degrval;
    Gnum                      edgenum;

    degrval = grafptr->s.verttax[vertnum] - grafptr->s.vendtax[vertnum];
    petax[vertnew]   = edgenew;
    lentax[vertnew]  = (degrval != 0) ? degrval : (-1 - grafptr->s.vertnbr);
    elentax[vertnew] = 0;

    for (edgenum = grafptr->s.verttax[vertnum];
         edgenum < grafptr->s.vendtax[vertnum]; edgenum ++, edgenew ++)
      iwtax[edgenew] = grafptr->s.edgetax[edgenum] + vertadj;
  }

  *pfreptr = edgenew;                             /* Set index to first free area */
}
