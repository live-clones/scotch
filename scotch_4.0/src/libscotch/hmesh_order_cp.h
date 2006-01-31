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
/**   NAME       : hmesh_order_cp.h                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the mesh compression algorithm.     **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 08 feb 2004     **/
/**                                 to     08 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/** Prime number for hashing vertex numbers. **/

#define HMESHORDERCPHASHPRIME       17            /* Prime number */

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HmeshOrderCpParam_ {
  double                    comprat;              /*+ Compression ratio threshold            +*/
  Strat * restrict          stratcpr;             /*+ Compressed submesh ordering strategy   +*/
  Strat * restrict          stratunc;             /*+ Uncompressed submesh ordering strategy +*/
} HmeshOrderCpParam;

/*+ This structure holds fine neighbor hashing data. +*/

typedef struct HmeshOrderCpHash_ {
  Gnum                      vnodnum;              /*+ Origin node vertex (i.e. pass) number +*/
  Gnum                      velmnum;              /*+ Adjacent end element vertex number    +*/
} HmeshOrderCpHash;

/*+ This structure holds coarse neighbor mate data. +*/

typedef struct HgraphOrderCpMate_ {
  Gnum                      coarvertend;          /*+ Adjacent coarse end vertex number +*/
  Gnum                      finevertend;          /*+ Adjacent end vertex number        +*/
} HgraphOrderCpMate;

/*
**  The function prototypes.
*/

#ifndef HMESH_ORDER_CP
#define static
#endif

int                         hmeshOrderCp        (const Hmesh * const, Order * const, const Gnum, OrderCblk * const, const HmeshOrderCpParam * const);

static Gnum                 hmeshOrderCpTree    (const Gnum * const, const Gnum * const, OrderCblk * const, Gnum);

#undef static
