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
/**   NAME       : vgraph_separate_ml.h                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the multi-level vertex separation   **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 28 oct 1997     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 13 dec 2001     **/
/**                                 to     02 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct VgraphSeparateMlParam_ {
  INT                       coarnbr;             /*+ Minimum number of vertices   +*/
  double                    coarrat;             /*+ Coarsening ratio             +*/
  GraphCoarsenType          coartype;            /*+ Edge matching function type  +*/
  Strat *                   stratlow;            /*+ Strategy at lowest level     +*/
  Strat *                   stratasc;            /*+ Strategy at ascending levels +*/
} VgraphSeparateMlParam;

/*
**  The function prototypes.
*/

#ifndef VGRAPH_SEPARATE_ML
#define static
#endif

static int                  vgraphSeparateMlCoarsen (const Vgraph * const, Vgraph * const, GraphCoarsenMulti * restrict * const, const VgraphSeparateMlParam * const);
static int                  vgraphSeparateMlUncoarsen (Vgraph * const, const Vgraph * const, const GraphCoarsenMulti * restrict const);

int                         vgraphSeparateMl    (Vgraph * const, const VgraphSeparateMlParam * const);
static int                  vgraphSeparateMl2   (Vgraph * const, const VgraphSeparateMlParam * const);

#undef static
