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
/**   NAME       : arch_torus.h                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the tori graph target architecture  **/
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
/**                # Version 4.0  : from : 05 nov 2003     **/
/**                                 to     05 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The 2D-torus definitions. +*/

typedef struct ArchTorus2_ {
  Anum                      c[2];                 /*+ Mesh dimensions +*/
} ArchTorus2;

typedef struct ArchTorus2Dom_ {
  Anum                      c[2][2];              /*+ Inclusive X and Y coordinates +*/
} ArchTorus2Dom;

/*+ The 3D-torus definitions. +*/

typedef struct ArchTorus3_ {
  Anum                      c[3];                 /*+ Mesh dimensions +*/
} ArchTorus3;

typedef struct ArchTorus3Dom_ {
  Anum                      c[3][2];              /*+ Inclusive X, Y, and Z coordinates +*/
} ArchTorus3Dom;

/*
**  The function prototypes.
*/

#ifndef ARCH_TORUS
#define static
#endif

int                         archTorus2ArchLoad  (ArchTorus2 * restrict const, FILE * restrict const);
int                         archTorus2ArchSave  (const ArchTorus2 * const, FILE * restrict const);
ArchDomNum                  archTorus2DomNum    (const ArchTorus2 * const, const ArchTorus2Dom * const);
int                         archTorus2DomTerm   (const ArchTorus2 * const, ArchTorus2Dom * restrict const, const ArchDomNum);
Anum                        archTorus2DomSize   (const ArchTorus2 * const, const ArchTorus2Dom * const);
Anum                        archTorus2DomDist   (const ArchTorus2 * const, const ArchTorus2Dom * const, const ArchTorus2Dom * const);
int                         archTorus2DomFrst   (const ArchTorus2 * const, ArchTorus2Dom * const);
int                         archTorus2DomLoad   (const ArchTorus2 * const, ArchTorus2Dom * const, FILE * restrict const);
int                         archTorus2DomSave   (const ArchTorus2 * const, const ArchTorus2Dom * const, FILE * restrict const);
int                         archTorus2DomBipart (const ArchTorus2 * const, const ArchTorus2Dom * const, ArchTorus2Dom * restrict const, ArchTorus2Dom * restrict const);
int                         archTorus2DomBipartO (const ArchTorus2 * const, const ArchTorus2Dom * const, ArchTorus2Dom * restrict const, ArchTorus2Dom * restrict const);
int                         archTorus2DomBipartU (const ArchTorus2 * const, const ArchTorus2Dom * const, ArchTorus2Dom * restrict const, ArchTorus2Dom * restrict const);

int                         archTorus3ArchLoad  (ArchTorus3 * restrict const, FILE * restrict const);
int                         archTorus3ArchSave  (const ArchTorus3 * const, FILE * restrict const);
ArchDomNum                  archTorus3DomNum    (const ArchTorus3 * const, const ArchTorus3Dom * const);
int                         archTorus3DomTerm   (const ArchTorus3 * const, ArchTorus3Dom * restrict const, const ArchDomNum);
Anum                        archTorus3DomSize   (const ArchTorus3 * const, const ArchTorus3Dom * const);
Anum                        archTorus3DomDist   (const ArchTorus3 * const, const ArchTorus3Dom * const, const ArchTorus3Dom * const);
int                         archTorus3DomFrst   (const ArchTorus3 * const, ArchTorus3Dom * const);
int                         archTorus3DomLoad   (const ArchTorus3 * const, ArchTorus3Dom * const, FILE * restrict const);
int                         archTorus3DomSave   (const ArchTorus3 * const, const ArchTorus3Dom * const, FILE * restrict const);
int                         archTorus3DomBipart (const ArchTorus3 * const, const ArchTorus3Dom * const, ArchTorus3Dom * restrict const, ArchTorus3Dom * restrict const);

#undef static
