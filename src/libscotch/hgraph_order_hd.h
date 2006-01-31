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
/**   NAME       : hgraph_order_hd.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the block-oriented Halo       **/
/**                Approximate (Multiple) Minimum Degree   **/
/**                ordering routine.                       **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 09 aug 1998     **/
/**                                 to   : 18 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 19 oct 1998     **/
/**                # Version 4.0  : from : 14 jan 2003     **/
/**                                 to   : 24 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

#define HGRAPHORDERHDCOMPRAT        1.2L          /*+ Compression ratio +*/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderHdParam_ {
  long                      colmin;               /*+ Minimum number of columns +*/
  long                      colmax;               /*+ Maximum number of columns +*/
  double                    fillrat;              /*+ Fill-in ratio             +*/
} HgraphOrderHdParam;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_HD
#define static
#endif

int                         hgraphOrderHd       (const Hgraph * restrict const, Order * restrict const, const Gnum, OrderCblk * restrict const, const HgraphOrderHdParam * restrict const);

#undef static
