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
/**   NAME       : amk_grf.h                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Decomposition architecture builder.     **/
/**                These lines are the data declarations   **/
/**                for the program routines.               **/
/**                                                        **/
/**   DATES      : # Version 3.0  : from : 06 jul 1995     **/
/**                                 to   : 02 oct 1995     **/
/**                # Version 3.1  : from : 26 mar 1996     **/
/**                                 to   : 26 mar 1996     **/
/**                # Version 3.2  : from : 23 apr 1997     **/
/**                                 to   : 02 jun 1997     **/
/**                # Version 3.3  : from : 15 may 1999     **/
/**                                 to   : 15 may 1999     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                 3              /* Number of files in list                */
#define C_FILEARGNBR              2              /* Number of files which can be arguments */

#define C_filenamegrfinp          C_fileTab[0].name /* Source graph input file name  */
#define C_filenametgtout          C_fileTab[1].name /* Architecture output file name */
#define C_filenamevrtinp          C_fileTab[2].name /* Vertex list input file name   */

#define C_filepntrgrfinp          C_fileTab[0].pntr /* Source graph input file  */
#define C_filepntrtgtout          C_fileTab[1].pntr /* Architecture output file */
#define C_filepntrvrtinp          C_fileTab[2].pntr /* Vertex list input file   */

/*+ Process flags. +*/

#define C_FLAGVRTINP              0x0001         /* Input vertex list */

#define C_FLAGNONE                0x0000          /* Default flags */

/*
**  The type and structure definitions.
*/

/*+ The sort structure, used to sort graph vertices by label. +*/

typedef struct C_VertSort_ {
  SCOTCH_Num                labl;                 /*+ Vertex label  +*/
  SCOTCH_Num                num;                  /*+ Vertex number +*/
} C_VertSort;
