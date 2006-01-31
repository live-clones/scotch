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
/**   NAME       : vgraph_separate_es.h                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the edge-separation-based node      **/
/**                separation module.                      **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 24 oct 1996     **/
/**                                 to   : 07 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 18 aug 2004     **/
/**                                 to     19 aug 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Separator type. +*/

typedef enum VgraphSeparateEsWidth_ {
  VGRAPHSEPAESWIDTHTHIN,                          /*+ Thin vertex separator +*/
  VGRAPHSEPAESWIDTHFAT                            /*+ Fat vertex separator  +*/
} VgraphSeparateEsWidth;

/*+ This structure holds the method parameters. +*/

typedef struct VgraphSeparateEsParam_ {
  Strat *                   strat;                /*+ Edge bipartitioning strategy used +*/
  VgraphSeparateEsWidth     widtval;              /*+ Separator width                   +*/
} VgraphSeparateEsParam;

/*+ These are the type of subgraphs vertices
    belong to, used to represent the Dulmage-
    Mendelsohn decomposition.
    TRICK: item numbers have been carefully
    chosen, so that one can easily sort out
    vertices that belong to (HR u SC u VC)
    and to (HR u SR u VC).                    +*/

typedef enum VgraphSeparateEsType_ {
  VGRAPHSEPAESTYPEHC     = 0x0000,
  VGRAPHSEPAESTYPEVR     = 0x0001,
  VGRAPHSEPAESTYPEHRSCVC = 0x0002,                /* Bit mask for testing */
  VGRAPHSEPAESTYPESC     = 0x0003,
  VGRAPHSEPAESTYPEHRSRVC = 0x0004,                /* Bit mask for testing */
  VGRAPHSEPAESTYPESR     = 0x0005,
  VGRAPHSEPAESTYPEHR     = 0x0006,
  VGRAPHSEPAESTYPEVC     = 0x0007
} VgraphSeparateEsType;

#define VGRAPHSEPAESTYPEBITC        1             /* Bit index for VGRAPHSEPAESTYPEHRSCVC */
#define VGRAPHSEPAESTYPEBITR        2             /* Bit index for VGRAPHSEPAESTYPEHRSRVC */

/*+ Vertex traversal flag. +*/

typedef enum VgraphSeparateEsTrav_ {
  VGRAPHSEPAESTRAVFREE = 0,                       /*+ Vertex not traversed                                     +*/
  VGRAPHSEPAESTRAVUSED,                           /*+ Vertex traversed by search for free rows                 +*/
  VGRAPHSEPAESTRAVDRTY                            /*+ Vertex traversed by backtracking search for free columns +*/
} VgraphSeparateEsTrav;

/*
**  The function prototypes.
*/

#ifndef VGRAPH_SEPARATE_ES
#define static
#endif

static int                  vgraphSeparateEsCover (const Graph * const, const Gnum, Gnum * const, Gnum * const);
static int                  vgraphSeparateEsCoverAugment (const Gnum * restrict const, const Gnum, Gnum * restrict const, VgraphSeparateEsTrav * restrict const, const Gnum * restrict const, const Gnum * restrict const, const Gnum * restrict const, const Gnum);
static void                 vgraphSeparateEsCoverCol (const Gnum * restrict const, VgraphSeparateEsType * restrict const, const Gnum * restrict const, const Gnum * restrict const, const Gnum * restrict const, const Gnum);
static void                 vgraphSeparateEsCoverRow (const Gnum * restrict const, VgraphSeparateEsType * restrict const, const Gnum * restrict const, const Gnum * restrict const, const Gnum * restrict const, const Gnum);

int                         vgraphSeparateEs    (Vgraph * const, const VgraphSeparateEsParam * const);

#undef static
