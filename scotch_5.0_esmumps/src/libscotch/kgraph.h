/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
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
