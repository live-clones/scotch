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
/**   NAME       : vgraph.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for vertex separation routines.   **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 24 aug 1996     **/
/**                                 to   : 17 oct 1997     **/
/**                # Version 3.3  : from : 13 mar 1999     **/
/**                                 to   : 13 mar 1999     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to   : 07 jan 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Active graph structure. +*/

typedef struct Vgraph_ {
  Graph                     s;                    /*+ Source graph                                       +*/
  GraphPart * restrict      parttax;              /*+ Based part array: 0,1: part; 2: separator          +*/
  Gnum                      compload[3];          /*+ Size of both parts and separator                   +*/
  Gnum                      comploaddlt;          /*+ Load difference between both parts                 +*/
  Gnum                      compsize[2];          /*+ Number of vertices in parts (separator is fronnbr) +*/
  Gnum                      fronnbr;              /*+ Number of frontier vertices; TRICK: compsize[2]    +*/
  Gnum * restrict           frontab;              /*+ Array of frontier vertex numbers                   +*/
  Gnum                      levlnum;              /*+ Nested dissection or coarsening level              +*/
} Vgraph;

/*+ The graph separator storing structure. +*/

typedef struct VgraphStore_ {
  Gnum                      fronnbr;              /*+ Number of frontier nodes     +*/
  Gnum                      comploaddlt;          /*+ Difference from the average  +*/
  Gnum                      compload[2];          /*+ Load in both parts           +*/
  Gnum                      compsize0;            /*+ Number of vertices in part 0 +*/
  byte *                    datatab;              /*+ Variable-sized data array    +*/
} VgraphStore;

/*
**  The function prototypes.
*/

#ifndef VGRAPH
#define static
#endif

void                        vgraphExit          (Vgraph * const);
void                        vgraphZero          (Vgraph * const);
int                         vgraphCheck         (const Vgraph * const);

int                         vgraphStoreInit     (const Vgraph * const, VgraphStore * const);
void                        vgraphStoreExit     (VgraphStore * const);
void                        vgraphStoreSave     (const Vgraph * const , VgraphStore * const);
void                        vgraphStoreUpdt     (Vgraph * const, const VgraphStore * const);

#undef static
