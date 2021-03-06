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
/**   NAME       : graph_grid.h                            **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These are the data declarations for the **/
/**                3D finite difference and finite element **/
/**                graph-making routines.                  **/
/**                                                        **/
/**   DATES      : # Version 1.0  : from : 24 sep 2002     **/
/**                                 to     25 sep 2002     **/
/**                                                        **/
/************************************************************/

#define GRAPH_GRID_H

/*
**  The type and structure definitions.
*/

/*+ The grid neighbor structure. It is used to
    record the current vertex and its current
    neighbor.                                  +*/

typedef struct GraphGridNghb_ {
  INT                       baseval;              /*+ Grid base value +*/
  INT                       xnbr;                 /*+ Grid size       +*/
  INT                       ynbr;
  INT                       znbr;
  INT                       xnum;                 /*+ Coordinates of current vertex +*/
  INT                       ynum;
  INT                       znum;
  INT                       xpos;                 /*+ Coordinates of current neighbor +*/
  INT                       ypos;
  INT                       zpos;
  INT                       xmin;                 /*+ Extents of neighbor area +*/
  INT                       ymin;
  INT                       zmin;
  INT                       xmax;
  INT                       ymax;
  INT                       zmax;
} GraphGridNghb;

/*
**  The function prototypes.
*/

#ifndef GRAPH_GRID
#define static
#endif

int                         graphBuildGrid2D    (Graph * const grafptr, const INT xnbr, const INT ynbr, const INT baseval);
int                         graphBuildGrid2E    (Graph * const grafptr, const INT xnbr, const INT ynbr, const INT baseval);
int                         graphBuildGrid3D    (Graph * const grafptr, const INT xnbr, const INT ynbr, const INT znbr, const INT baseval);
int                         graphBuildGrid3E    (Graph * const grafptr, const INT xnbr, const INT ynbr, const INT znbr, const INT baseval);

INT                         graphGrid2DDegr     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid2DFrst     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid2DNext     (GraphGridNghb * ngbptr);
int                         graphGrid2DSize     (INT * const vertptr, INT * const edgeptr, INT * const degrptr, const INT xnbr, const INT ynbr);

INT                         graphGrid2EDegr     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid2EFrst     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid2ENext     (GraphGridNghb * ngbptr);
int                         graphGrid2ESize     (INT * const vertptr, INT * const edgeptr, INT * const degrptr, const INT xnbr, const INT ynbr);

INT                         graphGrid3DDegr     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid3DFrst     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid3DNext     (GraphGridNghb * ngbptr);
int                         graphGrid3DSize     (INT * const vertptr, INT * const edgeptr, INT * const degrptr, const INT xnbr, const INT ynbr, const INT znbr);

INT                         graphGrid3EDegr     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid3EFrst     (GraphGridNghb * ngbptr, const INT vertnum);
INT                         graphGrid3ENext     (GraphGridNghb * ngbptr);
int                         graphGrid3ESize     (INT * const vertptr, INT * const edgeptr, INT * const degrptr, const INT xnbr, const INT ynbr, const INT znbr);

#undef static
