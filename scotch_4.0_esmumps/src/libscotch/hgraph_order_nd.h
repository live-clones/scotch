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
/**   NAME       : hgraph_order_nd.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the halo graph nested dissection    **/
/**                ordering algorithm.                     **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 17 oct 1996     **/
/**                                 to   : 18 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     13 mar 1999     **/
/**                # Version 4.0  : from : 03 jan 2002     **/
/**                                 to     24 apr 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderNdParam_ {
  Strat * restrict          sepstrat;             /*+ Separation strategy         +*/
  Strat * restrict          ordstratlea;          /*+ Leaf ordering strategy      +*/
  Strat * restrict          ordstratsep;          /*+ Separator ordering strategy +*/
} HgraphOrderNdParam;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_ND
#define static
#endif

int                         hgraphOrderNd       (const Hgraph * const, Order * const, const Gnum, OrderCblk * const, const HgraphOrderNdParam * const);

#undef static
