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
/**   NAME       : gtst.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the declarations     **/
/**                for the source graph analyzer.          **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 31 oct 1993     **/
/**                                 to     31 oct 1993     **/
/**                # Version 3.2  : from : 03 jun 1997     **/
/**                                 to   : 03 jun 1997     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to   : 19 oct 1998     **/
/**                # Version 4.0  : from : 10 sep 2003     **/
/**                                 to   : 10 sep 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/** File name aliases. **/

#define C_FILENBR                   2             /* Number of files in list                */
#define C_FILEARGNBR                2             /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* Source graph input file name */
#define C_filenamedatout            C_fileTab[1].name /* Statistics output file name  */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* Source graph input file */
#define C_filepntrdatout            C_fileTab[1].pntr /* Statistics output file  */
