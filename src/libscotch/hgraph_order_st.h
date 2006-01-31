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
/**   NAME       : hgraph_order_st.h                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data           **/
/**                declarations for the main graph         **/
/**                ordering routine.                       **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 31 oct 1996     **/
/**                                 to   : 29 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     07 sep 2001     **/
/**                # Version 4.0  : from : 29 dec 2001     **/
/**                                 to   : 15 jan 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type definitions.
*/

/*+ Method types. +*/

typedef enum HgraphOrderStMethodType_ {
  HGRAPHORDERSTMETHBL = 0,                        /*+ Block splitting post-processing       +*/
  HGRAPHORDERSTMETHCP,                            /*+ Graph compression                     +*/
  HGRAPHORDERSTMETHGP,                            /*+ Gibbs-Poole-Stockmeyer                +*/
  HGRAPHORDERSTMETHHD,                            /*+ Block Halo Approximate Minimum Degree +*/
  HGRAPHORDERSTMETHHF,                            /*+ Block Halo Approximate Minimum Fill   +*/
  HGRAPHORDERSTMETHND,                            /*+ Nested Dissection                     +*/
  HGRAPHORDERSTMETHSI,                            /*+ Simple                                +*/
  HGRAPHORDERSTMETHNBR                            /*+ Number of methods                     +*/
} HgraphOrderStMethodType;

/*
**  The external declarations.
*/

extern StratTab             hgraphorderststratab;

/*
**  The function prototypes.
*/

#ifndef HGRAPH_ORDER_ST
#define static
#endif

int                         hgraphOrderSt       (const Hgraph * restrict const, Order * restrict const, const Gnum, OrderCblk * restrict const, const Strat * const);

#undef static
