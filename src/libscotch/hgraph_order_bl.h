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
/**   NAME       : hgraph_order_bl.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the block splitting algorithm.      **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 24 jun 2002     **/
/**                                 to     24 jun 2002     **/
/**                # Version 4.0  : from : 26 jun 2002     **/
/**                                 to     29 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderBlParam_ {
  Strat * restrict          strat;                /*+ Ordering strategy    +*/
  long                      cblkmin;              /*+ Block splitting size +*/
} HgraphOrderBlParam;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_BL
#define static
#endif

int                         hgraphOrderBl       (const Hgraph * const, Order * const, const Gnum, OrderCblk * const, const HgraphOrderBlParam * const);

#undef static
