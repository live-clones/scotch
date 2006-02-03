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
/**   NAME       : bgraph_bipart_gg.h                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Luca SCARANO (v3.1)                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the function       **/
/**                declarations for the greedy graph       **/
/**                growing bipartitioning method.          **/
/**                                                        **/
/**   DATES      : # Version 3.1  : from : 07 jan 1996     **/
/**                                 to     29 apr 1996     **/
/**                # Version 3.2  : from : 20 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 09 jan 2004     **/
/**                                 to     09 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ System-defined constants. +*/

#define BGRAPHBIPARTGGGAINTABLSUBBITS 1

#define BGRAPHBIPARTGGSTATEFREE     ((GainLink *) 0) /*+ Vertex in initial state (TRICK: must be 0) +*/
#define BGRAPHBIPARTGGSTATEUSED     ((GainLink *) 1) /*+ Swapped vertex                             +*/
#define BGRAPHBIPARTGGSTATELINK     ((GainLink *) 2) /*+ Currently in gain table if higher          +*/

/*
**  The type and structure definitions.
*/

/** Method parameters. **/

typedef struct BgraphBipartGgParam_ {
  INT                       passnbr;              /*+ Number of passes to do +*/
} BgraphBipartGgParam;

/*+ The complementary vertex structure. For
    trick reasons, the gain table data structure
    must be the first field of the structure.    +*/

typedef struct BgraphBipartGgVertex_ {
  GainLink                  gainlink;             /*+ Gain link: FIRST                       +*/
  Gnum                      commgain0;            /*+ Gain if vertex and neighbors in part 0 +*/
  Gnum                      commgain;             /*+ Gain value                             +*/
} BgraphBipartGgVertex;

/*
**  The function prototypes.
*/

#ifndef BGRAPH_BIPART_GG
#define static
#endif

int                         bgraphBipartGg      (Bgraph * restrict const, const BgraphBipartGgParam * const);

#undef static
