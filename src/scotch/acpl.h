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
/**   NAME       : acpl.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the target archi-  **/
/**                tecture compilation function defini-    **/
/**                tions.                                  **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 12 nov 1994     **/
/**                                 to   : 12 nov 1994     **/
/**                # Version 3.0  : from : 06 jul 1995     **/
/**                                 to   : 06 jul 1995     **/
/**                # Version 3.2  : from : 24 sep 1996     **/
/**                                 to   : 01 jun 1997     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 02 oct 1998     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   2             /* Number of files in list                */
#define C_FILEARGNBR                2             /* Number of files which can be arguments */

#define C_filenametgtinp            C_fileTab[0].name /* Target architecture input file name  */
#define C_filenametgtout            C_fileTab[1].name /* Target architecture output file name */

#define C_filepntrtgtinp            C_fileTab[0].pntr /* Target architecture input file  */
#define C_filepntrtgtout            C_fileTab[1].pntr /* Target architecture output file */
