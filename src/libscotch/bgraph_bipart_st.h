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
/**   NAME       : bgraph_bipart_st.h                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the strategy and method       **/
/**                tables and the generic entry point for  **/
/**                the graph bipartitioning methods.       **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 08 oct 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 4.0  : from : 15 jan 2002     **/
/**                                 to     15 jan 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The type definitions.
*/

/** Method types. **/

typedef enum BgraphBipartStMethodType_ {
  BGRAPHBIPARTMETHEX = 0,                         /*+ Exactifying            +*/
  BGRAPHBIPARTMETHFM,                             /*+ Fiduccia-Mattheyses    +*/
  BGRAPHBIPARTMETHGG,                             /*+ Greedy Graph Growing   +*/
  BGRAPHBIPARTMETHGP,                             /*+ Gibbs-Poole-Stockmeyer +*/
  BGRAPHBIPARTMETHML,                             /*+ Multi-level (strategy) +*/
  BGRAPHBIPARTMETHZR,                             /*+ Move all to part zero  +*/
  BGRAPHBIPARTMETHNBR                             /*+ Number of methods      +*/
} BgraphBipartStMethodType;

/*
**  The external declarations.
*/

extern StratTab             bgraphbipartststratab;

/*
**  The function prototypes.
*/

#ifndef BGRAPH_BIPART_ST
#define static
#endif

int                         bgraphBipartSt      (Bgraph * restrict const, const Strat * restrict const);

#undef static
