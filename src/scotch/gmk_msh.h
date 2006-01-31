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
/**   NAME       : gmk_msh.h                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a mesh-to-graph converter.      **/
/**                This module contains the data declara-  **/
/**                tions for the main module.              **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 21 jan 2004     **/
/**                                 to   : 21 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines
*/

/*+ File name aliases. +*/

#define C_FILENBR                   2            /* Number of files in list                */
#define C_FILEARGNBR                2            /* Number of files which can be arguments */

#define C_filenamemshinp            C_fileTab[0].name /* External graph input file name  */
#define C_filenamegrfout            C_fileTab[1].name /* Source graph output file name   */

#define C_filepntrmshinp            C_fileTab[0].pntr /* External graph input file  */
#define C_filepntrgrfout            C_fileTab[1].pntr /* Source graph output file   */

/*
**  The type and structure definitions.
*/

/*+ This structure defines the method array element. +*/

typedef struct C_Format_ {
  char                      code;                /* Format type code */
  int                    (* func) ();            /* Function to call */
} C_Format;
