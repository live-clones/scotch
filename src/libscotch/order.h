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
/**   NAME       : order.h                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data           **/
/**                declarations for the generic ordering   **/
/**                structure.                              **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 19 oct 1996     **/
/**                                 to   : 21 aug 1998     **/
/**                # Version 4.0  : from : 19 dec 2001     **/
/**                                 to     28 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ Ordering option flags. +*/

#define ORDERNONE                   0x0000        /* No options set                 */
#define ORDERFREEPERI               0x0001        /* Free inverse permutation array */

/*
**  The type and structure definitions.
*/

/*+ Column block tree cell type. +*/

typedef enum OrderCblkType_ {
  ORDERCBLKNEDI,                                  /*+ Nested dissection separator node +*/
  ORDERCBLKOTHR                                   /*+ Other ordering node              +*/
} OrderCblkType;

/*+ Column-block tree cell. Each cell
    defines a column block, which is either
    a separator or a leaf built by nested
    dissection, or a super-variable built
    by minimum-degree algorithms. Column
    blocks are given in ascending order
    within sub-arrays, for proper infix
    traversal.                              +*/

typedef struct OrderCblk_ {
  OrderCblkType                 typeval;          /*+ Type of tree node                  +*/
  Gnum                          vnodnbr;          /*+ Number of node vertices in subtree +*/
  Gnum                          cblknbr;          /*+ Number of descendent column blocks +*/
  struct OrderCblk_ * restrict  cblktab;          /*+ Sub-array of column-blocks         +*/
} OrderCblk;

/*+ Ordering structure. A block ordering is
    defined by its inverse permutation peritab
    and by the tree of permuted ordering blocks,
    which, once flattened, defines the blocks
    of the ordering. For the sake of coherence
    between orderings that have been produced
    either from graphs or meshes, all ordering
    values are based at baseval.                 +*/

typedef struct Order_ {
  int                       flagval;              /*+ Flag value                          +*/
  Gnum                      baseval;              /*+ Base value for structures           +*/
  Gnum                      vnodnbr;              /*+ Number of node vertices             +*/
  Gnum                      cblknbr;              /*+ Number of column blocks             +*/
  OrderCblk                 cblktre;              /*+ Root of column block tree           +*/
  Gnum * restrict           peritab;              /*+ Inverse permutation array [vnodnbr] +*/
} Order;

/*
**  The function prototypes.
*/

#ifndef ORDER
#define static
#endif

int                         orderInit           (Order * const, const Gnum, const Gnum, Gnum * const);
void                        orderExit           (Order * const);
static void                 orderExit2          (OrderCblk * const, const Gnum);
int                         orderLoad           (Order * restrict const, const Gnum * restrict const, FILE * restrict const);
int                         orderSave           (const Order * restrict const, const Gnum * restrict const, FILE * restrict const);
int                         orderSaveMap        (const Order * const, const Gnum * restrict const, FILE * restrict const);
int                         orderSaveTree       (const Order * const, const Gnum * restrict const, FILE * restrict const);
void                        orderPeri           (const Gnum * const, const Gnum, const Gnum, Gnum * const, const Gnum);
void                        orderRang           (const Order * const, Gnum * const);
static void                 orderRang2          (Gnum ** const, Gnum * const, const OrderCblk * const);
void                        orderTree           (const Order * restrict const, Gnum * restrict const);
static void                 orderTree2          (Gnum * restrict const, Gnum * restrict const, const OrderCblk * restrict const, Gnum);
int                         orderCheck          (const Order * const);

#undef static
