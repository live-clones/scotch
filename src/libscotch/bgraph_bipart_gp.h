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
/**   NAME       : bgraph_bipart_gp.h                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the Gibbs, Poole, and Stockmeyer    **/
/**                bipartitioning algorithm.               **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     28 oct 1994     **/
/**                # Version 3.2  : from : 21 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 01 nov 2003     **/
/**                                 to     20 aug 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Method parameters. +*/

typedef struct BgraphBipartGpParam_ {
  INT                       passnbr;              /*+ Number of passes to do +*/
} BgraphBipartGpParam;

/*+ Complementary vertex structure. +*/

typedef struct BgraphBipartGpVertex_ {
  Gnum                      passnum;              /*+ Number of pass when vertex selected   +*/
  Gnum                      distval;              /*+ Current distance from diameter vertex +*/
} BgraphBipartGpVertex;

/*+ Neighbor queue. +*/

typedef struct BgraphBipartGpQueue_ {
  Gnum                      headnum;              /*+ Head of distance queue  +*/
  Gnum                      tailnum;              /*+ Tail of distance queue  +*/
  Gnum * restrict           queutab;              /*+ Array of queue elements +*/
} BgraphBipartGpQueue;

/*
**  The function prototypes.
*/

#ifndef BGRAPH_BIPART_GP
#define static
#endif

int                         bgraphBipartGp      (Bgraph * restrict const, const BgraphBipartGpParam * const);

#undef static

/*
**  The macro definitions.
*/

#define bgraphBipartGpQueueFlush(queue) ((queue)->headnum = (queue)->tailnum = 0)
#define bgraphBipartGpQueueEmpty(queue) ((queue)->headnum <= (queue)->tailnum)
#define bgraphBipartGpQueuePut(queue,vnum) ((queue)->queutab[(queue)->headnum ++] = (vnum))
#define bgraphBipartGpQueueGet(queue) ((queue)->queutab[(queue)->tailnum ++])
