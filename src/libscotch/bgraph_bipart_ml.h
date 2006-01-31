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
/**   NAME       : bgraph_bipart_ml.h                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Luca SCARANO (v3.1)                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the multi-level graph bipartitio-   **/
/**                ning routines.                          **/
/**                                                        **/
/**   DATES      : # Version 3.1  : from : 24 oct 1995     **/
/**                                 to     03 jul 1996     **/
/**                # Version 3.2  : from : 20 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 12 dec 2003     **/
/**                                 to     20 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct BgraphBipartMlParam_ {
  INT                       coarnbr;              /*+ Minimum number of vertices   +*/
  double                    coarrat;              /*+ Coarsening ratio             +*/
  GraphCoarsenType          matchtype;            /*+ Edge matching function type  +*/
  Strat *                   stratlow;             /*+ Strategy at lowest level     +*/
  Strat *                   stratasc;             /*+ Strategy at ascending levels +*/
} BgraphBipartMlParam;

/*
**  The function prototypes.
*/

#ifndef BGRAPH_BIPART_ML
#define static
#endif

static int                  bgraphBipartMlCoarsen (const Bgraph * const, Bgraph * restrict const, GraphCoarsenMulti * restrict * const, const BgraphBipartMlParam * const);
static int                  bgraphBipartMlUncoarsen (Bgraph * restrict const, const Bgraph * const, const GraphCoarsenMulti * const);

int                         bgraphBipartMl      (Bgraph * restrict const, const BgraphBipartMlParam * const);
static int                  bgraphBipartMl2     (Bgraph * restrict const, const BgraphBipartMlParam * const);

#undef static
