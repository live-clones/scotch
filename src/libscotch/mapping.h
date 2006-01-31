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
/**   NAME       : mapping.h                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the declarations for    **/
/**                the mapping handling routines.          **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 15 dec 1992     **/
/**                                 to     01 apr 1993     **/
/**                # Version 1.0  : from : 04 oct 1993     **/
/**                                 to     06 oct 1993     **/
/**                # Version 1.1  : from : 15 oct 1993     **/
/**                                 to     15 oct 1993     **/
/**                # Version 1.3  : from : 09 apr 1994     **/
/**                                 to     11 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     02 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to     18 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     04 jul 1995     **/
/**                # Version 3.1  : from : 30 oct 1995     **/
/**                                 to     06 jun 1996     **/
/**                # Version 3.2  : from : 23 aug 1996     **/
/**                                 to     26 may 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     30 mar 1999     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to     13 nov 2005     **/
/**                                                        **/
/************************************************************/

#define MAPPING_H

/*
**  The defines.
*/

/*+ Ordering option flags. +*/

#define MAPNONE                     0x0000        /* No options set       */
#define MAPFREEPART                 0x0001        /* Free partition array */

/*
**  The type definitions.
*/

/*+ This structure defines an (eventually
    partial) mapping of a source graph to
    a target architecture.                +*/

typedef struct Mapping_ {
  int                       flagval;              /*+ Flag value                    +*/
  Gnum                      baseval;              /*+ Base value for structures     +*/
  Gnum                      vertnbr;              /*+ Number of vertices in mapping +*/
  Anum * restrict           parttax;              /*+ Mapping array [vertnbr]       +*/
  Anum                      domnmax;              /*+ Maximum number of domains     +*/
  Anum                      domnnbr;              /*+ Current number of domains     +*/
  ArchDom * restrict        domntab;              /*+ Array of domains [termmax]    +*/
  Arch                      archdat;              /*+ Architecture data             +*/
} Mapping;

/*+ The target architecture sort structure, used
    to sort vertices by increasing label value.  +*/

typedef struct MappingSort_ {
  Anum                      labl;                 /*+ Target architecture vertex label +*/
  Anum                      peri;                 /*+ Inverse permutation              +*/
} MappingSort;

/*
**  The function prototypes.
*/

#ifndef MAPPING
#define static
#endif

int                         mapInit             (Mapping * restrict const, const Gnum, const Gnum, const Arch * restrict const, Gnum * restrict const);
void                        mapExit             (Mapping * const);
int                         mapLoad             (Mapping * restrict const, const Gnum * restrict const, FILE * restrict const);
int                         mapSave             (const Mapping * restrict const, const Gnum * restrict const, FILE * restrict const);
int                         mapView             (const Mapping * restrict const, const Graph * restrict const, FILE * const);

#undef static

/*
**  The macro definitions.
*/

#define mapDomain(map,idx)          (&((map)->domntab[(map)->parttax[(idx)]]))
