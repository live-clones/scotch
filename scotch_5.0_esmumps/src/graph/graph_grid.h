/* Copyright 2004,2007 INRIA
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
