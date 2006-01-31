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
/**   NAME       : hgraph_order_hf.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the block-oriented Halo       **/
/**                Approximate (Multiple) Minimum Fill     **/
/**                graph ordering routine.                 **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 15 may 2001     **/
/**                                 to   : 15 may 2001     **/
/**                # Version 4.0  : from : 10 jan 2003     **/
/**                                 to   : 24 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

#define HGRAPHORDERHFCOMPRAT        1.2L          /*+ Compression ratio +*/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderHfParam_ {
  long                      colmin;               /*+ Minimum number of columns +*/
  long                      colmax;               /*+ Maximum number of columns +*/
  double                    fillrat;              /*+ Fill-in ratio             +*/
} HgraphOrderHfParam;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_HF
#define static
#endif

int                         hgraphOrderHf       (const Hgraph * const, Order * const, const Gnum, OrderCblk * const, const HgraphOrderHfParam * const);

#undef static
