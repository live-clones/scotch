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
/**   NAME       : vgraph_separate_gg.h                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the the greedy graph growing        **/
/**                vertex separation method.               **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 14 nov 1997     **/
/**                                 to     15 jul 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 19 dec 2001     **/
/**                                 to     09 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ System-defined constants. +*/

#define VGRAPHSEPAGGSUBBITS         4

#define VGRAPHSEPAGGSTATEPART0      ((GainLink *) 0) /*+ Vertex in part 0 (initial state)  +*/
#define VGRAPHSEPAGGSTATEPART1      ((GainLink *) 1) /*+ Vertex in part 1                  +*/
#define VGRAPHSEPAGGSTATEPART2      ((GainLink *) 2) /*+ Vertex in part 2, chained         +*/
#define VGRAPHSEPAGGSTATELINK       ((GainLink *) 3) /*+ Currently in gain table if higher +*/

/*
**  The type and structure definitions.
*/

/*+ Method parameters. +*/

typedef struct VgraphSeparateGgParam_ {
  INT                       passnbr;              /*+ Number of passes to do +*/
} VgraphSeparateGgParam;

/*+ The complementary vertex structure. For
    trick reasons, the gain table data structure
    must be the first field of the structure.    +*/

typedef struct VgraphSeparateGgVertex_ {
  GainLink                  gainlink;             /*+ Gain link: FIRST              +*/
  Gnum                      compgain2;            /*+ Computation gain in separator +*/
} VgraphSeparateGgVertex;

/*
**  The function prototypes.
*/

#ifndef VGRAPH_SEPARATE_GG
#define static
#endif

int                         vgraphSeparateGg    (Vgraph * restrict const, const VgraphSeparateGgParam * restrict const);

#undef static
