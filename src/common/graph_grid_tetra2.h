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
/**   NAME       : graph_grid_tetra2.h                     **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These are the data declarations for the **/
/**                2D non-parallepipedic tetrahedral-      **/
/**                filled graph-making routines.           **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 15 oct 2000     **/
/**                                 to     15 oct 2000     **/
/**                # Version 1.0  : from : 03 jun 2002     **/
/**                                 to     03 jun 2002     **/
/**                                                        **/
/************************************************************/

#define GRAPH_GRID_TETRA2_H

#ifdef CXREF_DOC
#ifndef COMMON_H
#include "common.h"
#endif /* COMMON_H */
#ifndef GRAPH_H
#include "graph.h"
#endif /* GRAPH_H */
#endif /* CXREF_DOC */

/*
**  The type and structure definitions.
*/

/*+ The grid neighbor structure. It is used to
    record the current vertex and its current
    neighbor.                                  +*/

typedef struct GraphGridTetra2Nghb_ {
  INT                       baseval;              /*+ Grid base value +*/
  INT                       xnbr;                 /*+ Grid size       +*/
  INT                       ynbr;
  INT                       xnum;                 /*+ Coordinates of current vertex +*/
  INT                       ynum;
  INT                       xpos;                 /*+ Coordinates of current neighbor +*/
  INT                       ypos;
  INT                       vertval;              /*+ End vertex type +*/
} GraphGridTetra2Nghb;

/*
**  The function prototypes.
*/

#ifndef GRAPH_GRID_TETRA2
#define static
#endif

int                         graphBuildGrid2C    (Graph * const grafptr, const INT xnbr, const INT ynbr, const INT baseval);

INT                         graphGrid2CDegr     (GraphGridTetra2Nghb * ngbptr, const INT vertnum);
INT                         graphGrid2CFrst     (GraphGridTetra2Nghb * ngbptr, const INT vertnum);
INT                         graphGrid2CNext     (GraphGridTetra2Nghb * ngbptr);
int                         graphGrid2CSize     (INT * const vertptr, INT * const edgeptr, INT * const degrptr, const INT xnbr, const int ynbr);

#undef static
