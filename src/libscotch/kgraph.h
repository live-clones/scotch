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
/**   NAME       : kgraph.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a static mapper.                **/
/**                These lines are the data declarations   **/
/**                for the k-way graph partitoning struc-  **/
/**                tures and routines.                     **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 12 sep 1997     **/
/**                                 to     26 may 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     12 mar 1999     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to     16 feb 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The graph structure. +*/

typedef struct Kgraph_ {
  Graph                     s;                    /*+ Source graph                      +*/
  Mapping                   m;                    /*+ Current mapping of graph vertices +*/
  int                       fronnbr;              /*+ Number of frontier vertices       +*/
  Gnum * restrict           frontab;              /*+ Array of frontier vertex numbers  +*/
  Gnum * restrict           comploadavg;          /*+ Array of target average loads     +*/
  Gnum * restrict           comploaddlt;          /*+ Array of target imbalances        +*/
  Gnum                      commload;             /*+ Communication load                +*/
} Kgraph;

/*
**  The function prototypes.
*/

#ifndef KGRAPH
#define static
#endif

int                         kgraphInit          (Kgraph * const, const Graph * restrict const, const Mapping * restrict const);
void                        kgraphExit          (Kgraph * const, Mapping * restrict const);

#undef static
