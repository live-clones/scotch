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
/**   NAME       : bgraph.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the job control routines of the     **/
/**                Dual Recursive Bipartitioning method.   **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 23 mar 1993     **/
/**                                 to     12 may 1993     **/
/**                # Version 1.3  : from : 06 apr 1994     **/
/**                                 to     09 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     04 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to     30 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 15 nov 1995     **/
/**                                 to     15 nov 1995     **/
/**                # Version 3.2  : from : 24 aug 1996     **/
/**                                 to     03 nov 1997     **/
/**                # Version 3.3  : from : 01 dec 1998     **/
/**                                 to     02 dec 1998     **/
/**                # Version 4.0  : from : 18 dec 2001     **/
/**                                 to     31 aug 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Graph option flags. +*/

#define BGRAPHFREEPART              (GRAPHBITSNOTUSED) /* Free part array          */
#define BGRAPHFREEFRON              (GRAPHBITSNOTUSED << 1) /* Free frontier array */

/*+ The bipartition graph structure. +*/

typedef struct Bgraph_ {
  Graph                     s;                    /*+ Source graph data                         +*/
  Gnum * restrict           veextax;              /*+ Vertex external gain array if moved to 1  +*/
  GraphPart * restrict      parttax;              /*+ Array of parts for every vertex           +*/
  Gnum * restrict           frontab;              /*+ Array of frontier vertex numbers          +*/
  Gnum                      fronnbr;              /*+ Number of frontier vertices               +*/
  Gnum                      compload0;            /*+ Load in part 0 (strategy variable)        +*/
  Gnum                      compload0avg;         /*+ Average load of part 0                    +*/
  Gnum                      compload0dlt;         /*+ Difference from the average               +*/
  Gnum                      compsize0;            /*+ Number of vertices in part 0              +*/
  Gnum                      commload;             /*+ Communication load                        +*/
  Gnum                      commloadextn0;        /*+ Communication load if all moved to part 0 +*/
  Gnum                      commgainextn;         /*+ External gain if all swapped              +*/
  Gnum                      commgainextn0;        /*+ External gain if all swapped from part 0  +*/
  Anum                      domdist;              /*+ Distance between subdomains               +*/
  Gnum                      domwght[2];           /*+ Weights of the two subdomains             +*/
  uint                      levlnum;              /*+ Coarsening level (uint since parameter)   +*/
} Bgraph;

/*+ The save graph structure. +*/

typedef struct BgraphStore_ {
  Gnum                      fronnbr;              /*+ Number of frontier nodes      +*/
  Gnum                      compload0dlt;         /*+ Difference from the average   +*/
  Gnum                      compsize0;            /*+ Number of vertices in part 0  +*/
  Gnum                      commload;             /*+ Communication load            +*/
  Gnum                      commgainextn;         /*+ External gain if all swapped  +*/
  byte *                    datatab;              /*+ Variable-sized data array     +*/
} BgraphStore;

/*
**  The function prototypes.
*/

#ifndef BGRAPH
#define static
#endif

#ifdef MAPPING_H
int                         bgraphInit          (Bgraph * const, const Graph * const, const Graph * const, const Mapping * const, const ArchDom[]);
#endif /* MAPPING_H */
void                        bgraphInit2         (Bgraph * const, const Anum, const Anum, const Anum);
#ifdef MAPPING_H
int                         bgraphInit3         (Bgraph * const, const Graph * const, const Mapping * const, const ArchDom[]);
#endif /* MAPPING_H */
void                        bgraphExit          (Bgraph * restrict const);
void                        bgraphSwal          (Bgraph * restrict const);
void                        bgraphZero          (Bgraph * restrict const);
int                         bgraphCheck         (const Bgraph * restrict const);

int                         bgraphStoreInit     (const Bgraph * const, BgraphStore * const);
void                        bgraphStoreExit     (BgraphStore * const);
void                        bgraphStoreSave     (const Bgraph * const , BgraphStore * const);
void                        bgraphStoreUpdt     (Bgraph * const, const BgraphStore * const);

#undef static
