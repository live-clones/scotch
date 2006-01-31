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
/**   NAME       : arch_mesh.h                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the mesh graph target architecture  **/
/**                functions.                              **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 01 dec 1992     **/
/**                                 to   : 24 mar 1993     **/
/**                # Version 1.2  : from : 04 feb 1994     **/
/**                                 to   : 11 feb 1994     **/
/**                # Version 1.3  : from : 20 apr 1994     **/
/**                                 to   : 20 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to   : 12 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to   : 30 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     17 aug 1995     **/
/**                # Version 3.1  : from : 22 jul 1996     **/
/**                                 to     23 jul 1996     **/
/**                # Version 3.2  : from : 16 oct 1996     **/
/**                                 to     14 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 09 jan 2004     **/
/**                                 to     09 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The 2D-mesh definitions. +*/

typedef struct ArchMesh2_ {
  Anum                      c[2];                 /*+ Mesh dimensions +*/
} ArchMesh2;

typedef struct ArchMesh2Dom_ {
  Anum                      c[2][2];              /*+ Inclusive X and Y coordinates +*/
} ArchMesh2Dom;

/*+ The 3D-mesh definitions. +*/

typedef struct ArchMesh3_ {
  Anum                      c[3];                 /*+ Mesh dimensions +*/
} ArchMesh3;

typedef struct ArchMesh3Dom_ {
  Anum                      c[3][2];              /*+ Inclusive X, Y, and Z coordinates +*/
} ArchMesh3Dom;

/*
**  The function prototypes.
*/

#ifndef ARCH_MESH
#define static
#endif

int                         archMesh2ArchLoad   (ArchMesh2 * restrict const, FILE * restrict const);
int                         archMesh2ArchSave   (const ArchMesh2 * const, FILE * restrict const);
ArchDomNum                  archMesh2DomNum     (const ArchMesh2 * const, const ArchMesh2Dom * const);
int                         archMesh2DomTerm    (const ArchMesh2 * const, ArchMesh2Dom * restrict const, const ArchDomNum);
Anum                        archMesh2DomSize    (const ArchMesh2 * const, const ArchMesh2Dom * const);
Anum                        archMesh2DomDist    (const ArchMesh2 * const, const ArchMesh2Dom * const, const ArchMesh2Dom * const);
int                         archMesh2DomFrst    (const ArchMesh2 * const, ArchMesh2Dom * const);
int                         archMesh2DomLoad    (const ArchMesh2 * const, ArchMesh2Dom * const, FILE * restrict const);
int                         archMesh2DomSave    (const ArchMesh2 * const, const ArchMesh2Dom * const, FILE * restrict const);
int                         archMesh2DomBipart  (const ArchMesh2 * const, const ArchMesh2Dom * const, ArchMesh2Dom * restrict const, ArchMesh2Dom * restrict const);
int                         archMesh2DomBipartO (const ArchMesh2 * const, const ArchMesh2Dom * const, ArchMesh2Dom * restrict const, ArchMesh2Dom * restrict const);
int                         archMesh2DomBipartU (const ArchMesh2 * const, const ArchMesh2Dom * const, ArchMesh2Dom * restrict const, ArchMesh2Dom * restrict const);

int                         archMesh3ArchLoad   (ArchMesh3 * restrict const, FILE * restrict const);
int                         archMesh3ArchSave   (const ArchMesh3 * const, FILE * restrict const);
ArchDomNum                  archMesh3DomNum     (const ArchMesh3 * const, const ArchMesh3Dom * const);
int                         archMesh3DomTerm    (const ArchMesh3 * const, ArchMesh3Dom * restrict const, const ArchDomNum);
Anum                        archMesh3DomSize    (const ArchMesh3 * const, const ArchMesh3Dom * const);
Anum                        archMesh3DomDist    (const ArchMesh3 * const, const ArchMesh3Dom * const, const ArchMesh3Dom * const);
int                         archMesh3DomFrst    (const ArchMesh3 * const, ArchMesh3Dom * const);
int                         archMesh3DomLoad    (const ArchMesh3 * const, ArchMesh3Dom * const, FILE * restrict const);
int                         archMesh3DomSave    (const ArchMesh3 * const, const ArchMesh3Dom * const, FILE * restrict const);
int                         archMesh3DomBipart  (const ArchMesh3 * const, const ArchMesh3Dom * const, ArchMesh3Dom * restrict const, ArchMesh3Dom * restrict const);

#undef static
