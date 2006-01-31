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
/**   NAME       : hmesh_order_gp.h                        **/
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
/**                # Version 4.0  : from : 04 nov 2002     **/
/**                                 to   : 01 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HmeshOrderGpParam_ {
  long                      passnbr;              /*+ Number of passes to do +*/
} HmeshOrderGpParam;

/*+ Complementary vertex structure. +*/

typedef struct HmeshOrderGpVertex_ {
  Gnum                      passnum;              /*+ Number of pass when vertex selected   +*/
  Gnum                      vertdist;             /*+ Current distance from diameter vertex +*/
} HmeshOrderGpVertex;

/*+ Neighbor queue. +*/

typedef struct HmeshOrderGpQueue_ {
  Gnum *                    head;                 /*+ Head of distance queue  +*/
  Gnum *                    tail;                 /*+ Tail of distance queue  +*/
  Gnum *                    qtab;                 /*+ Array of queue elements +*/
} HmeshOrderGpQueue;

/*
**  The function prototypes.
*/

#ifndef HMESH_ORDER_GP
#define static
#endif

int                         hmeshOrderGp        (const Hmesh * const, Order * const, const Gnum, OrderCblk * const, const HmeshOrderGpParam * restrict const);

#undef static

/*
**  The macro definitions.
*/

#define hmeshOrderGpQueueFlush(queue)     ((queue)->head = (queue)->tail = (queue)->qtab)
#define hmeshOrderGpQueueEmpty(queue)     ((queue)->head <= (queue)->tail)
#define hmeshOrderGpQueuePut(queue,vnum)  (* ((queue)->head ++) = (vnum))
#define hmeshOrderGpQueueGet(queue)       (* ((queue)->tail ++))
