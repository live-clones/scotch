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
/**   NAME       : amk_m2.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the target architecture file    **/
/**                for bidimensional mesh graphs.          **/
/**                Here are the data declaration for the   **/
/**                target machine architecture functions.  **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 21 apr 1994     **/
/**                                 to   : 22 apr 1994     **/
/**                # Version 2.0  : from : 12 jul 1994     **/
/**                                 to   : 13 nov 1994     **/
/**                # Version 2.0  : from : 18 sep 1995     **/
/**                                 to   : 19 sep 1995     **/
/**                # Version 3.1  : from : 30 may 1996     **/
/**                                 to   : 30 may 1996     **/
/**                # Version 3.2  : from : 31 may 1997     **/
/**                                 to   : 02 jun 1997     **/
/**                # Version 4.0  : from : 09 feb 2004     **/
/**                                 to   : 09 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/** File name aliases. **/

#define C_FILENBR                   1             /* Number of files in list                */
#define C_FILEARGNBR                1             /* Number of files which can be arguments */

#define C_filenamearcout            C_fileTab[0].name /* Architecture output file name */

#define C_filepntrarcout            C_fileTab[0].pntr /* Architecture output file */

/*
**  The type and structure definitions.
*/

/** The method type. **/

typedef enum C_MethType_ {
  C_METHNESTED,                                   /*+ Nested decomposition  +*/
  C_METHONEWAY                                    /*+ One-way decomposition +*/
} C_MethType;

/*
**  The function prototypes.
*/

void                        C_termBipart        (ArchMesh2 *, ArchMesh2Dom *, unsigned int, unsigned int *, unsigned int *, int (*) ());
int                         C_methBipartOne     (const ArchMesh2 * const, const ArchMesh2Dom * const, ArchMesh2Dom * restrict const, ArchMesh2Dom * restrict const);

/*
**  The macro definitions.
*/

#ifndef abs
#define abs(a)                      (((a) >= 0) ? (a) : -(a))
#endif /* abs */

#define C_termDist(x0,y0,x1,y1)     ((unsigned int) (abs ((int) (x0) - (int) (x1)) + \
                                                     abs ((int) (y0) - (int) (y1))))
