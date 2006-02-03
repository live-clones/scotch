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
/**   NAME       : graph_coarsen.h                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the source graph coarsening         **/
/**                functions.                              **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 02 dec 1992     **/
/**                                 to     18 may 1993     **/
/**                # Version 1.3  : from : 30 apr 1994     **/
/**                                 to     18 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     18 aug 1994     **/
/**                # Version 3.0  : from : 07 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 28 nov 1995     **/
/**                                 to     28 nov 1995     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     17 sep 1998     **/
/**                # Version 4.0  : from : 13 dec 2001     **/
/**                                 to     05 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/** Prime number for cache-friendly perturbations. **/

#define GRAPHCOARPERTPRIME          179           /* Prime number */

/** Prime number for hashing vertex numbers. **/

#define GRAPHCOARHASHPRIME          1049          /* Prime number */

/*
**  The type and structure definitions.
*/

/*+ Here are the edge matching function types for coarsening. +*/

typedef enum GraphCoarsenType_ {
  GRAPHCOARHEM,                                   /*+ Heavy-edge matching         +*/
  GRAPHCOARSCN,                                   /*+ Scanning (first) matching   +*/
  GRAPHCOARCSC,                                   /*+ Crystal scanning matching   +*/
  GRAPHCOARCHE,                                   /*+ Crystal heavy-edge matching +*/
  GRAPHCOARNBR                                    /*+ Number of matching types    +*/
} GraphCoarsenType;

/*+ The multinode table element, which contains
    pairs of based indices of collapsed vertices.
    Both values are equal for uncollapsed vertices.
    As the base values of the fine and coarse graphs
    may be different, the values of the collapsed
    vertices are set with respect to the base value
    of the fine graph.                               +*/

typedef struct GraphCoarsenMulti_  {
  Gnum                      vertnum[2];           /*+ Numbers of the collapsed vertices of a multinode +*/
} GraphCoarsenMulti;

/*+ A table made of such elements is used during
    coarsening to build the edge array of the new
    graph, after the labeling of the vertices.    +*/

typedef struct GraphCoarsenHash_ {
  Gnum                      vertorgnum;           /*+ Origin vertex (i.e. pass) number +*/
  Gnum                      vertendnum;           /*+ Other end vertex number          +*/
  Gnum                      edgenum;              /*+ Number of corresponding edge     +*/
} GraphCoarsenHash;

/*
**  The function prototypes.
*/

#ifndef GRAPH_COARSEN
#define static
#endif

int                         graphCoarsen        (const Graph * restrict const, Graph * restrict const, GraphCoarsenMulti * restrict * const, const Gnum, const double, const GraphCoarsenType);

static void                 graphCoarsenEdgeLl  (const Graph * const, const Gnum * const, const GraphCoarsenMulti * restrict const, Graph * const, GraphCoarsenHash * const, const Gnum);
static void                 graphCoarsenEdgeLu  (const Graph * const, const Gnum * const, const GraphCoarsenMulti * restrict const, Graph * const, GraphCoarsenHash * const, const Gnum);

static Gnum                 graphCoarsenMatchHy (const Graph * const, Gnum *, const Gnum, const Gnum);
static Gnum                 graphCoarsenMatchSc (const Graph * const, Gnum *, const Gnum, const Gnum);
static Gnum                 graphCoarsenMatchCs (const Graph * const, Gnum *, const Gnum, const Gnum);
static Gnum                 graphCoarsenMatchCh (const Graph * const, Gnum *, const Gnum, const Gnum);

#undef static
