/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
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
