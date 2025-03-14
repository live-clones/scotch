/* Copyright 2004,2007,2009,2010,2018,2024 IPB, Universite de Bordeaux, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
**
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
**
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
**
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
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
/**                # Version 5.1  : from : 01 oct 2009     **/
/**                                 to   : 04 nov 2010     **/
/**                # Version 6.0  : from : 07 jun 2018     **/
/**                                 to   : 07 jun 2018     **/
/**                # Version 7.0  : from : 11 sep 2024     **/
/**                                 to   : 11 sep 2024     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderGpParam_ {
  INT                       passnbr;              /*+ Number of passes to do +*/
} HgraphOrderGpParam;

/*+ Complementary vertex structure. +*/

typedef struct HgraphOrgerGpVertex_ {
  Gnum                      passnum;              /*+ Number of pass when vertex selected   +*/
  Gnum                      vertdist;             /*+ Current distance from diameter vertex +*/
} HgraphOrderGpVertex;

/*+ Neighbor queue. +*/

typedef struct HgraphOrgerGpQueue_ {
  Gnum *                    head;                 /*+ Head of distance queue  +*/
  Gnum *                    tail;                 /*+ Tail of distance queue  +*/
  Gnum *                    qtab;                 /*+ Array of queue elements +*/
} HgraphOrgerGpQueue;

/*
**  The function prototypes.
*/

int                         hgraphOrderGp       (Hgraph * const, Order * const, const Gnum, OrderCblk * const, const HgraphOrderGpParam * restrict const);

/*
**  The macro definitions.
*/

#define hgraphOrderGpQueueFlush(queue)    ((queue)->head = (queue)->tail = (queue)->qtab)
#define hgraphOrderGpQueueEmpty(queue)    ((queue)->head <= (queue)->tail)
#define hgraphOrderGpQueuePut(queue,vnum) (* ((queue)->head ++) = (vnum))
#define hgraphOrderGpQueueGet(queue)      (* ((queue)->tail ++))
