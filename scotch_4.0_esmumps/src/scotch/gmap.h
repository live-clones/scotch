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
/**   NAME       : gmap.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a graph static mapper.          **/
/**                These lines are the data declaration    **/
/**                for the main routine.                   **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 05 jan 1993     **/
/**                                 to     12 may 1993     **/
/**                # Version 1.3  : from : 09 apr 1994     **/
/**                                 to     30 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     08 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to     09 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     15 aug 1995     **/
/**                # Version 3.1  : from : 07 nov 1995     **/
/**                                 to     10 nov 1995     **/
/**                # Version 3.2  : from : 04 oct 1996     **/
/**                                 to     18 jul 1997     **/
/**                # Version 3.3  : from : 07 oct 1998     **/
/**                                 to   : 31 may 1999     **/
/**                # Version 4.0  : from : 16 jan 2004     **/
/**                                 to   : 16 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   4             /* Number of files in list                */
#define C_FILEARGNBR                4             /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* Source graph input file name        */
#define C_filenametgtinp            C_fileTab[1].name /* Target architecture input file name */
#define C_filenamemapout            C_fileTab[2].name /* Mapping result output file name     */
#define C_filenamelogout            C_fileTab[3].name /* Log file name                       */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* Source graph input file        */
#define C_filepntrtgtinp            C_fileTab[1].pntr /* Target architecture input file */
#define C_filepntrmapout            C_fileTab[2].pntr /* Mapping result output file     */
#define C_filepntrlogout            C_fileTab[3].pntr /* Log file                       */

/*+ Process flags. +*/

#define C_FLAGNONE                  0x0000        /* No flags      */
#define C_FLAGVERBSTR               0x0001        /* Verbose flags */
#define C_FLAGVERBTIM               0x0002        /* Verbose flags */
#define C_FLAGVERBMAP               0x0004
