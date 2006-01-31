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
/**   NAME       : arch_vhcub.h                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the variable-sized hypercube        **/
/**                target architecture functions.          **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 08 nov 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 04 nov 2003     **/
/**                                 to     04 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The variable-sized hypercube bipartitioning definitions. +*/

typedef struct ArchVhcub_ {
  int                       padding;              /*+ No data needed +*/
} ArchVhcub;

typedef struct ArchVhcubDom_ {
  Anum                      termlvl;              /*+ Terminal depth  +*/
  Anum                      termnum;              /*+ Terminal number +*/
} ArchVhcubDom;

/*
**  The function prototypes.
*/

#ifndef ARCH_VHCUB
#define static
#endif

ArchDomNum                  archVhcubDomNum     (const ArchVhcub * const, const ArchVhcubDom * const);
int                         archVhcubDomTerm    (const ArchVhcub * const, ArchVhcubDom * restrict const, const ArchDomNum);
Anum                        archVhcubDomSize    (const ArchVhcub * const, const ArchVhcubDom * const);
Anum                        archVhcubDomDist    (const ArchVhcub * const, const ArchVhcubDom * const, const ArchVhcubDom * const);
int                         archVhcubDomFrst    (const ArchVhcub * const, ArchVhcubDom * const);
int                         archVhcubDomLoad    (const ArchVhcub * const, ArchVhcubDom * const, FILE * const);
int                         archVhcubDomSave    (const ArchVhcub * const, const ArchVhcubDom * const, FILE * const);
int                         archVhcubDomBipart  (const ArchVhcub * const, const ArchVhcubDom * const, ArchVhcubDom * restrict const, ArchVhcubDom * restrict const);

#undef static
