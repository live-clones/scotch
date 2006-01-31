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
/**   NAME       : arch_deco.h                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the decomposition-defined target    **/
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
/**                # Version 3.1  : from : 20 jul 1996     **/
/**                                 to     20 jul 1996     **/
/**                # Version 3.2  : from : 11 sep 1996     **/
/**                                 to     28 sep 1998     **/
/**                # Version 4.0  : from : 29 nov 2003     **/
/**                                 to     14 jun 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ Decomposition architecture flags. +*/

#define ARCHDECONONE                0x0000        /* No options set */

#define ARCHDECOFREE                0x0001        /* Free arrays    */

/*
**  The type and structure definitions.
*/

/*+ The decomposition-described terminal vertex definition. +*/

typedef struct ArchDecoTermVert_ {
  ArchDomNum                labl;                 /*+ Number for terminals, or ARCHDOMNOTTERM +*/
  Anum                      wght;                 /*+ Weight of the domain (processor load)   +*/
  Anum                      num;                  /*+ Number of the terminal                  +*/
} ArchDecoTermVert;

/** The decomposition-described architecture definitions. **/

typedef struct ArchDecoVert_ {
  ArchDomNum                labl;                 /*+ Smallest number of included terminal  +*/
  Anum                      size;                 /*+ Number of processors in the domain    +*/
  Anum                      wght;                 /*+ Weight of the domain (processor load) +*/
} ArchDecoVert;

typedef struct ArchDeco_ {
  int                       flagval;              /*+ Flag value                 +*/
  Anum                      domtermnbr;           /*+ Number of terminal domains +*/
  Anum                      domvertnbr;           /*+ Number of domains          +*/
  ArchDecoVert *            domverttab;           /*+ Table of domain "vertices" +*/
  Anum  *                   domdisttab;           /*+ Table of domain distances  +*/
} ArchDeco;

typedef struct ArchDecoDom_ {
  Anum                      num;                  /*+ Domain number in the decomposition +*/
} ArchDecoDom;

/*
**  The function prototypes.
*/

#ifndef ARCH_DECO
#define static
#endif

int                         archDecoArchBuild   (ArchDeco * const, const Anum, const Anum, const ArchDecoTermVert * const, const Anum  * const);
int                         archDecoArchLoad    (ArchDeco * const, FILE * restrict const);
int                         archDecoArchSave    (const ArchDeco * const, FILE * restrict const);
int                         archDecoArchFree    (ArchDeco * const);
Anum                        archDecoArchSize    (ArchDeco * const, const Anum);
Anum                        archDecoArchDist    (ArchDeco * const, const Anum, const Anum);
Anum                        archDecoArchDistE   (ArchDeco * const, const Anum, const Anum);
ArchDomNum                  archDecoDomNum      (const ArchDeco * const, const ArchDecoDom * const);
int                         archDecoDomTerm     (const ArchDeco * const, ArchDecoDom * restrict const, const ArchDomNum);
Anum                        archDecoDomSize     (const ArchDeco * const, const ArchDecoDom * const);
Anum                        archDecoDomWght     (const ArchDeco * const, const ArchDecoDom * const);
Anum                        archDecoDomDist     (const ArchDeco * const, const ArchDecoDom * const, const ArchDecoDom * const);
int                         archDecoDomFrst     (const ArchDeco * const, ArchDecoDom * restrict const);
int                         archDecoDomLoad     (const ArchDeco * const, ArchDecoDom * restrict const, FILE * restrict const);
int                         archDecoDomSave     (const ArchDeco * const, const ArchDecoDom * const, FILE * restrict const);
int                         archDecoDomBipart   (const ArchDeco * const, const ArchDecoDom * const, ArchDecoDom * restrict const, ArchDecoDom * restrict const);

#undef static

/*
**  The macro definitions.
*/

#define archDecoArchSize(d,i)       ((d)->domverttab[(i) - 1].size)
#define archDecoArchDist(d,i,j)     ((d)->domdisttab[((i) >= (j)) ? (((i) - 1) * ((i) - 2)) / 2 + (j) - 1 \
                                                                  : (((j) - 1) * ((j) - 2)) / 2 + (i) - 1])
#define archDecoArchDistE(d,i,j)    (((i) == (j)) ? 0 : archDecoArchDist ((d), (i), (j)))
