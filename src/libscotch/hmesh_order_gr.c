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
/**   NAME       : hmesh_order_gr.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module orders a halo mesh by       **/
/**                turning it into a graph and using a     **/
/**                graph separation strategy.              **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 30 nov 2003     **/
/**                                 to     27 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH_ORDER_GR

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "order.h"
#include "hgraph.h"
#include "hgraph_order_st.h"
#include "mesh.h"
#include "hmesh.h"
#include "hmesh_order_gr.h"

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
hmeshOrderGr (
const Hmesh * restrict const    meshptr,
Order * restrict const          ordeptr,
const Gnum                      ordenum,
OrderCblk * restrict const      cblkptr,
const HmeshOrderGrParam * const paraptr)
{
  Hgraph                            grafdat;
  int                               o;

  hgraphInit (&grafdat);
  if (hmeshHgraph (meshptr, &grafdat) != 0) {     /* Build halo graph with improper vnumtab */
    errorPrint ("hmeshOrderGr: cannot build halo graph");
    return     (1);
  }

  if ((o = hgraphOrderSt (&grafdat, ordeptr, ordenum, cblkptr, paraptr->stratptr)) != 0)
    errorPrint ("hmeshOrderGr: cannot order graph");

  hgraphFree (&grafdat);                          /* Free graph structure */

  return (o);
}
