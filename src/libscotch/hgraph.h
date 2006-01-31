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
/**   NAME       : hgraph.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the source halo graph structure.    **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 02 jan 2002     **/
/**                                 to     30 apr 2004     **/
/**                                                        **/
/************************************************************/

#define HGRAPH_H

/*
**  The defines.
*/

/*+ Graph option flags. +*/

#define HGRAPHFREEVHND              0x0400        /* Free vnhdtab array */
#define HGRAPHFREETABS              (GRAPHFREETABS | HGRAPHFREEVHND)

/*
**  The type and structure definitions.
*/

/*+ Halo graph structure. +*/

typedef struct Hgraph_ {
  Graph                     s;                    /*+ Source graph                                                   +*/
  Gnum                      vnohnbr;              /*+ Number of non-halo vertices                                    +*/
  Gnum                      vnohnnd;              /*+ Based number of first halo vertex in graph (s.vertnnd if none) +*/
  Gnum * restrict           vnhdtax;              /*+ End vertex array for non-halo vertices [vnohnbr, based]        +*/
  Gnum                      vnlosum;              /*+ Sum of vertex loads for non-halo vertices only (<= s.velosum)  +*/
  Gnum                      enohnbr;              /*+ Number of non-halo edges                                       +*/
  Gnum                      levlnum;              /*+ Nested dissection level                                        +*/
} Hgraph;

/*
**  The function prototypes.
*/

#ifndef HGRAPH
#define static
#endif

int                         hgraphInit          (Hgraph * const);
void                        hgraphExit          (Hgraph * const);
void                        hgraphFree          (Hgraph * const);
Gnum                        hgraphBase          (Hgraph * const, const Gnum);
int                         hgraphInduceList    (const Hgraph * const, const VertList * const, const Gnum, Hgraph * const);
int                         hgraphCheck         (const Hgraph *);

#undef static
