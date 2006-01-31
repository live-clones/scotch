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
/**   NAME       : kgraph_map_st.h                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the strategy and method       **/
/**                tables and the generic entry point for  **/
/**                the graph multipartitioning methods.    **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 15 oct 1996     **/
/**                                 to     29 sep 1997     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     09 dec 1998     **/
/**                # Version 4.0  : from : 12 jan 2004     **/
/**                                 to     12 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type definitions.
*/

/*+ Method types. +*/

typedef enum KgraphMapStMethodType_ {
  KGRAPHMAPSTMETHRB = 0,                          /*+ Dual Recursive Bipartitioning +*/
  KGRAPHMAPSTMETHNBR                              /*+ Number of methods             +*/
} KgraphMapStMethodType;

/*
**  The external declarations.
*/

extern StratTab             kgraphmapststratab;

/*
**  The function prototypes.
*/

#ifndef KGRAPH_MAP_ST
#define static
#endif

int                         kgraphMapSt         (Kgraph * restrict const, const Strat * restrict const);

#undef static
