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
/**   NAME       : hgraph_order_si.c                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module orders halo graph vertices  **/
/**                using a simple method.                  **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 01 nov 1996     **/
/**                                 to     21 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     02 oct 1998     **/
/**                # Version 4.0  : from : 19 dec 2001     **/
/**                                 to     11 dec 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HGRAPH_ORDER_SI

#include "module.h"
#include "common.h"
#include "graph.h"
#include "order.h"
#include "hgraph.h"
#include "hgraph_order_si.h"

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the ordering.
** It returns:
** - 0   : if the ordering could be computed.
** - !0  : on error.
*/

int
hgraphOrderSi (
const Hgraph * restrict const   grafptr,
Order * restrict const          ordeptr,
const Gnum                      ordenum,          /*+ Zero-based ordering number +*/
OrderCblk * restrict const      cblkptr)          /*+ Single column-block        +*/
{
  Gnum                vertnum;
  Gnum                vnumnum;

  if (grafptr->s.vnumtax == NULL) {               /* If graph is original graph */
    for (vertnum = grafptr->s.baseval, vnumnum = ordenum;
         vertnum < grafptr->vnohnnd; vertnum ++, vnumnum ++)
      ordeptr->peritab[vnumnum] = vertnum;
  }
  else {                                          /* Graph is not original graph */
    for (vertnum = grafptr->s.baseval, vnumnum = ordenum;
         vertnum < grafptr->vnohnnd; vertnum ++, vnumnum ++)
      ordeptr->peritab[vnumnum] = grafptr->s.vnumtax[vertnum];
  }

  return (0);
}
