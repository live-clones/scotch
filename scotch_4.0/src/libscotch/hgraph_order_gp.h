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
/**   NAME       : hgraph_order_gp.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the Gibbs-Poole-Stockmeyer    **/
/**                node ordering routine.                  **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 31 oct 1996     **/
/**                                 to   : 27 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 02 oct 1998     **/
/**                # Version 4.0  : from : 03 feb 2002     **/
/**                                 to   : 01 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderGpParam_ {
  long                      passnbr;              /*+ Number of passes to do +*/
} HgraphOrderGpParam;

/*+ Complementary vertex structure. +*/

typedef struct HgraphOrgerGpVertex_ {
  Gnum                      passnum;              /*+ Number of pass when vertex selected   +*/
  Gnum                      vertdist;             /*+ Current distance from diameter vertex +*/
} HgraphOrderGpVertex;

/*+ Neighbor queue. +*/

typedef struct HgraphOrgerGpQueue_ {
  Gnum * restrict           head;                 /*+ Head of distance queue  +*/
  Gnum * restrict           tail;                 /*+ Tail of distance queue  +*/
  Gnum * restrict           qtab;                 /*+ Array of queue elements +*/
} HgraphOrgerGpQueue;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_GP
#define static
#endif

int                         hgraphOrderGp       (const Hgraph * const, Order * const, const Gnum, OrderCblk * const, const HgraphOrderGpParam * restrict const);

#undef static

/*
**  The macro definitions.
*/

#define hgraphOrderGpQueueFlush(queue)    ((queue)->head = (queue)->tail = (queue)->qtab)
#define hgraphOrderGpQueueEmpty(queue)    ((queue)->head <= (queue)->tail)
#define hgraphOrderGpQueuePut(queue,vnum) (* ((queue)->head ++) = (vnum))
#define hgraphOrderGpQueueGet(queue)      (* ((queue)->tail ++))
