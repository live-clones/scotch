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
/**   NAME       : geom.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the geometrical data handling       **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 13 dec 1998     **/
/**                                 to     15 dec 1998     **/
/**                # Version 3.4  : from : 10 oct 1999     **/
/**                                 to     12 oct 1999     **/
/**                # Version 4.0  : from : 24 nov 2001     **/
/**                                 to     18 jan 2004     **/
/**                                                        **/
/************************************************************/

#define GEOM_H

/*
**  The type and structure definitions.
*/

/*+ Geometrical graph structure. +*/

typedef struct Geom_ {
  int                       dimnnbr;              /*+ Geometry type (1, 2, or 3D) +*/
  double * restrict         geomtab;              /*+ Geometrical vertex array    +*/
} Geom;

/*
**  The function prototypes.
*/

#ifndef GEOM
#define static
#endif

int                         geomInit            (Geom * restrict const);
void                        geomExit            (Geom * restrict const);

#undef static
