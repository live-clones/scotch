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
/**   NAME       : hgraph_order_cp.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the graph compression         **/
/**                ordering routine.                       **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 29 aug 1998     **/
/**                                 to   : 09 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to   : 03 jan 1999     **/
/**                # Version 4.0  : from : 01 jan 2003     **/
/**                                 to   : 01 jan 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ Prime number for hashing vertex numbers. +*/

#define HGRAPHORDERCPHASHPRIME      17            /* Prime number */

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HgraphOrderCpParam_ {
  double                    comprat;              /*+ Compression ratio threshold             +*/
  Strat * restrict          stratcpr;             /*+ Compressed subgraph ordering strategy   +*/
  Strat * restrict          stratunc;             /*+ Uncompressed subgraph ordering strategy +*/
} HgraphOrderCpParam;

/*+ This structure holds fine neighbor hashing data. +*/

typedef struct HgraphOrderCpHash_ {
  Gnum                      vertnum;              /*+ Origin vertex (i.e. pass) number +*/
  Gnum                      vertend;              /*+ Adjacent end vertex number       +*/
} HgraphOrderCpHash;

/*+ This structure holds coarse neighbor mate data. +*/

typedef struct HgraphOrderCpMate_ {
  Gnum                      coarvertend;          /*+ Adjacent coarse end vertex number +*/
  Gnum                      finevertend;          /*+ Adjacent end vertex number        +*/
} HgraphOrderCpMate;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_CP
#define static
#endif

int                         hgraphOrderCp       (const Hgraph * const, Order * const, const Gnum, OrderCblk * const, const HgraphOrderCpParam * const);

static Gnum                 hgraphOrderCpTree   (const Gnum * const, const Gnum * const, OrderCblk * const, Gnum);

#undef static
