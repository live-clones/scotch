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
/**   NAME       : atst.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Target architecture graph analyzer.     **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 17 may 1994     **/
/**                                 to   : 17 may 1994     **/
/**                # Version 2.0  : from : 11 nov 1994     **/
/**                                 to   : 11 nov 1994     **/
/**                # Version 3.0  : from : 05 jul 1995     **/
/**                                 to   : 05 jul 1995     **/
/**                # Version 3.2  : from : 01 jun 1997     **/
/**                                 to   : 01 jun 1997     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/** File name aliases. **/

#define C_FILENBR                   2             /* Number of files in list                */
#define C_FILEARGNBR                2             /* Number of files which can be arguments */


#define C_filenametgtinp            C_fileTab[0].name /* Target graph input file name */
#define C_filenamelogout            C_fileTab[1].name /* Statistics output file name  */

#define C_filepntrtgtinp            C_fileTab[0].pntr /* Target graph input file */
#define C_filepntrlogout            C_fileTab[1].pntr /* Statistics output file  */
