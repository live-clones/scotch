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
/**   NAME       : arch_hcub.h                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the hypercube graph target          **/
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
/**                                 to     09 aug 1995     **/
/**                # Version 3.1  : from : 11 jun 1996     **/
/**                                 to     11 jun 1996     **/
/**                # Version 3.2  : from : 21 sep 1996     **/
/**                                 to     13 may 1998     **/
/**                # Version 4.0  : from : 11 nov 2003     **/
/**                                 to     11 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The binary hypercube definitions. +*/

typedef struct ArchHcub_ {
  Anum                      dimmax;               /*+ Number of hypercube dimensions +*/
} ArchHcub;

typedef struct ArchHcubDom_ {
  Anum                      dimcur;               /*+ Current dimension to be set +*/
  Anum                      bitset;               /*+ Bit set of set dimensions   +*/
} ArchHcubDom;

/*
**  The function prototypes.
*/

#ifndef ARCH_HCUB
#define static
#endif

int                         archHcubArchLoad    (ArchHcub * restrict const, FILE * restrict const);
int                         archHcubArchSave    (const ArchHcub * const, FILE * restrict const);
ArchDomNum                  archHcubDomNum      (const ArchHcub * const, const ArchHcubDom * const);
int                         archHcubDomTerm     (const ArchHcub * const, ArchHcubDom * restrict const, const ArchDomNum);
Anum                        archHcubDomSize     (const ArchHcub * const, const ArchHcubDom * const);
Anum                        archHcubDomDist     (const ArchHcub * const, const ArchHcubDom * const, const ArchHcubDom * const);
int                         archHcubDomFrst     (const ArchHcub * const, ArchHcubDom * restrict const);
int                         archHcubDomLoad     (const ArchHcub * const, ArchHcubDom * restrict const, FILE * restrict const);
int                         archHcubDomSave     (const ArchHcub * const, const ArchHcubDom * const, FILE * restrict const);
int                         archHcubDomBipart   (const ArchHcub * const, const ArchHcubDom * const, ArchHcubDom * restrict const, ArchHcubDom * restrict const);

#undef static
