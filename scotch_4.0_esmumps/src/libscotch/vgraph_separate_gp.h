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
/**   NAME       : vgraph_separate_gp.h                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the Gibbs, Poole, and Stockmeyer    **/
/**                graph separation algorithm.             **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 14 may 2004     **/
/**                                 to     17 may 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Method parameters. +*/

typedef struct VgraphSeparateGpParam_ {
  INT                       passnbr;              /*+ Number of passes to do +*/
} VgraphSeparateGpParam;

/*+ Complementary vertex structure. +*/

typedef struct VgraphSeparateGpVertex_ {
  Gnum                      passnum;              /*+ Number of pass when vertex selected   +*/
  Gnum                      distval;              /*+ Current distance from diameter vertex +*/
} VgraphSeparateGpVertex;

/*+ Neighbor queue. +*/

typedef struct VgraphSeparateGpQueue_ {
  Gnum                      headnum;              /*+ Head of distance queue  +*/
  Gnum                      tailnum;              /*+ Tail of distance queue  +*/
  Gnum * restrict           queutab;              /*+ Array of queue elements +*/
} VgraphSeparateGpQueue;

/*
**  The function prototypes.
*/

#ifndef VGRAPH_SEPARATE_GP
#define static
#endif

int                         vgraphSeparateGp    (Vgraph * restrict const, const VgraphSeparateGpParam * restrict const);

#undef static

/*
**  The macro definitions.
*/

#define vgraphSeparateGpQueueFlush(queue) ((queue)->headnum = (queue)->tailnum = 0)
#define vgraphSeparateGpQueueEmpty(queue) ((queue)->headnum <= (queue)->tailnum)
#define vgraphSeparateGpQueuePut(queue,vnum) ((queue)->queutab[(queue)->headnum ++] = (vnum))
#define vgraphSeparateGpQueueGet(queue) ((queue)->queutab[(queue)->tailnum ++])
