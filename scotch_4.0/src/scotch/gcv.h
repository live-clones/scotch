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
/**   NAME       : gcv.h                                   **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a graph file converter.         **/
/**                This module contains the data declara-  **/
/**                tions for the main module.              **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 02 apr 1993     **/
/**                                 to     02 apr 1993     **/
/**                # Version 2.0  : from : 28 oct 1994     **/
/**                                 to     16 nov 1994     **/
/**                # Version 3.0  : from : 08 sep 1995     **/
/**                                 to     17 sep 1995     **/
/**                # Version 3.1  : from : 22 mar 1996     **/
/**                                 to     22 mar 1996     **/
/**                # Version 3.2  : from : 04 oct 1996     **/
/**                                 to     04 mar 1997     **/
/**                # Version 3.3  : from : 06 oct 1998     **/
/**                                 to   : 06 oct 1998     **/
/**                # Version 3.4  : from : 13 oct 1999     **/
/**                                 to   : 14 oct 1999     **/
/**                # Version 4.0  : from : 29 nov 2003     **/
/**                                 to   : 29 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines
*/

/*+ File name aliases. +*/

#define C_FILENBR                   3            /* Number of files in list                */
#define C_FILEARGNBR                3            /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* External graph input file name  */
#define C_filenamesrcout            C_fileTab[1].name /* Source graph output file name   */
#define C_filenamegeoout            C_fileTab[2].name /* Source graph geometry file name */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* External graph input file  */
#define C_filepntrsrcout            C_fileTab[1].pntr /* Source graph output file   */
#define C_filepntrgeoout            C_fileTab[2].pntr /* Source graph geometry file */

/*
**  The type and structure definitions.
*/

/*+ This structure defines the method array element. +*/

typedef struct C_Format_ {
  char                      code;                /* Format type code */
  int                    (* func) ();            /* Function to call */
} C_Format;
