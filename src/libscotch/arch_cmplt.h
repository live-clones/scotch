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
/**   NAME       : arch_cmplt.h                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the complete graph target           **/
/**                architecture functions.                 **/
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
/**                                 to     24 jul 1995     **/
/**                # Version 3.1  : from : 11 jun 1996     **/
/**                                 to     11 jun 1996     **/
/**                # Version 3.2  : from : 20 sep 1996     **/
/**                                 to     13 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 09 jan 2004     **/
/**                                 to     09 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The complete graph definitions. +*/

typedef struct ArchCmplt_ {
  Anum                      numnbr;               /*+ Number of vertices +*/
} ArchCmplt;

typedef struct ArchCmpltDom_ {
  Anum                      nummin;               /*+ Minimum vertex number +*/
  Anum                      numnbr;               /*+ Number of vertices    +*/
} ArchCmpltDom;

/*
**  The function prototypes.
*/

#ifndef ARCH_CMPLT
#define static
#endif

int                         archCmpltArchLoad   (ArchCmplt * restrict const, FILE * restrict const);
int                         archCmpltArchSave   (const ArchCmplt * const, FILE * restrict const);
ArchDomNum                  archCmpltDomNum     (const ArchCmplt * const, const ArchCmpltDom * const);
int                         archCmpltDomTerm    (const ArchCmplt * const, ArchCmpltDom * restrict const, const ArchDomNum);
Anum                        archCmpltDomSize    (const ArchCmplt * const, const ArchCmpltDom * const);
Anum                        archCmpltDomDist    (const ArchCmplt * const, const ArchCmpltDom * const, const ArchCmpltDom * const);
int                         archCmpltDomFrst    (const ArchCmplt * const, ArchCmpltDom * const);
int                         archCmpltDomLoad    (const ArchCmplt * const, ArchCmpltDom * const, FILE * const);
int                         archCmpltDomSave    (const ArchCmplt * const, const ArchCmpltDom * const, FILE * const);
int                         archCmpltDomBipart  (const ArchCmplt * const, const ArchCmpltDom * const, ArchCmpltDom * restrict const, ArchCmpltDom * restrict const);

#undef static
