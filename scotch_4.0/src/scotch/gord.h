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
/**   NAME       : gord.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a sparse matrix graph orderer.  **/
/**                This module contains the data declara-  **/
/**                tions for the main routine.             **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 24 aug 1996     **/
/**                                 to   : 20 oct 1997     **/
/**                # Version 3.3  : from : 07 oct 1998     **/
/**                                 to   : 31 may 1999     **/
/**                # Version 4.0  : from : 11 dec 2002     **/
/**                                 to   : 27 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   5             /* Number of files in list                */
#define C_FILEARGNBR                3             /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* Source graph input file name */
#define C_filenameordout            C_fileTab[1].name /* Ordering output file name    */
#define C_filenamelogout            C_fileTab[2].name /* Log file name                */
#define C_filenamemapout            C_fileTab[3].name /* Separator mapping file name  */
#define C_filenametreout            C_fileTab[4].name /* Separator tree file name     */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* Source graph input file */
#define C_filepntrordout            C_fileTab[1].pntr /* Ordering output file    */
#define C_filepntrlogout            C_fileTab[2].pntr /* Log file                */
#define C_filepntrmapout            C_fileTab[3].pntr /* Separator mapping file  */
#define C_filepntrtreout            C_fileTab[4].pntr /* Separator tre file      */

/*+ Process flags. +*/

#define C_FLAGNONE                  0x0000        /* No flags                   */
#define C_FLAGMAPOUT                0x0001        /* Output mapping data        */
#define C_FLAGTREOUT                0x0002        /* Output separator tree data */
#define C_FLAGVERBSTR               0x0004        /* Output strategy string     */
#define C_FLAGVERBTIM               0x0008        /* Output timing information  */
