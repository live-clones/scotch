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
/**   NAME       : arch_vcmplt.h                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the variable-sized complete graph   **/
/**                target architecture functions.          **/
/**                                                        **/
/**   DATES      : # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     09 aug 1995     **/
/**                # Version 3.1  : from : 20 jul 1996     **/
/**                                 to     20 jul 1996     **/
/**                # Version 3.2  : from : 15 oct 1996     **/
/**                                 to     14 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 3.4  : from : 08 nov 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 05 nov 2003     **/
/**                                 to     05 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The variable-sized complete graph bipartitioning definitions. +*/

typedef struct ArchVcmplt_ {
  int                       padding;              /*+ No data needed +*/
} ArchVcmplt;

typedef struct ArchVcmpltDom_ {
  Anum                      termnum;              /*+ Terminal number +*/
} ArchVcmpltDom;

/*
**  The function prototypes.
*/

#ifndef ARCH_VCMPLT
#define static
#endif

ArchDomNum                  archVcmpltDomNum    (const ArchVcmplt * const, const ArchVcmpltDom * const);
int                         archVcmpltDomTerm   (const ArchVcmplt * const, ArchVcmpltDom * restrict const, const ArchDomNum);
Anum                        archVcmpltDomSize   (const ArchVcmplt * const, const ArchVcmpltDom * const);
Anum                        archVcmpltDomDist   (const ArchVcmplt * const, const ArchVcmpltDom * const, const ArchVcmpltDom * const);
int                         archVcmpltDomFrst   (const ArchVcmplt * const, ArchVcmpltDom * const);
int                         archVcmpltDomLoad   (const ArchVcmplt * const, ArchVcmpltDom * const, FILE * const);
int                         archVcmpltDomSave   (const ArchVcmplt * const, const ArchVcmpltDom * const, FILE * const);
int                         archVcmpltDomBipart (const ArchVcmplt * const, const ArchVcmpltDom * const, ArchVcmpltDom * restrict const, ArchVcmpltDom * restrict const);

#undef static
