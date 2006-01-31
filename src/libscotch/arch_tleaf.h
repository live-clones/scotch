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
/**   NAME       : arch_tleaf.h                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the tree-leaf pseudo-graph target   **/
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
/**                                 to     16 aug 1995     **/
/**                # Version 3.1  : from : 20 jul 1996     **/
/**                                 to     23 jul 1996     **/
/**                # Version 3.2  : from : 10 oct 1996     **/
/**                                 to     14 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 10 dec 2003     **/
/**                                 to     10 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/** The Tree-Leaf graph definitions. **/

typedef struct ArchTleaf_ {
  Anum                      leafdep;              /*+ Maximum leaf depth                      +*/
  Anum                      clusdep;              /*+ Depth before reaching complete clusters +*/
  Anum                      linkval;              /*+ Value of extra-cluster links            +*/
} ArchTleaf;

typedef struct ArchTleafDom_ {
  Anum                      leaflvl;              /*+ Current leaf depth +*/
  Anum                      leafnum;              /*+ Leaf number        +*/
} ArchTleafDom;

/*
**  The function prototypes.
*/

#ifndef ARCH_TLEAF
#define static
#endif

int                         archTleafArchLoad   (ArchTleaf * restrict const, FILE * restrict const);
int                         archTleafArchSave   (const ArchTleaf * const, FILE * restrict const);
ArchDomNum                  archTleafDomNum     (const ArchTleaf * const, const ArchTleafDom * const);
int                         archTleafDomTerm    (const ArchTleaf * const, ArchTleafDom * restrict const, const ArchDomNum);
Anum                        archTleafDomSize    (const ArchTleaf * const, const ArchTleafDom * const);
Anum                        archTleafDomDist    (const ArchTleaf * const, const ArchTleafDom * const, const ArchTleafDom * const);
int                         archTleafDomFrst    (const ArchTleaf * const, ArchTleafDom * restrict const);
int                         archTleafDomLoad    (const ArchTleaf * const, ArchTleafDom * restrict const, FILE * restrict const);
int                         archTleafDomSave    (const ArchTleaf * const, const ArchTleafDom * const, FILE * restrict const);
int                         archTleafDomBipart  (const ArchTleaf * const, const ArchTleafDom * const, ArchTleafDom * restrict const, ArchTleafDom * restrict const);

#undef static
