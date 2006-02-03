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
/**   NAME       : gmk_m2.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the data declara-    **/
/**                tions for the bidimensional mesh source **/
/**                graph building program.                 **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 30 oct 1994     **/
/**                                 to     08 nov 1994     **/
/**                # Version 3.0  : from : 11 jul 1995     **/
/**                                 to     11 jul 1995     **/
/**                # Version 3.2  : from : 03 jun 1997     **/
/**                                 to   : 03 jun 1997     **/
/**                # Version 3.3  : from : 06 oct 1998     **/
/**                                 to   : 06 oct 1998     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   2             /* Number of files in list                */
#define C_FILEARGNBR                1             /* Number of files which can be arguments */

#define C_filenamesrcout            C_fileTab[0].name /* Source graph output file name   */
#define C_filenamegeoout            C_fileTab[1].name /* Geometry graph output file name */

#define C_filepntrsrcout            C_fileTab[0].pntr /* Source graph output file   */
#define C_filepntrgeoout            C_fileTab[1].pntr /* Geometry graph output file */

/*+ Process flags. +*/

#define C_FLAGGEOOUT                0x0001        /* Output the geometry graph                            */
#define C_FLAGTORUS                 0x0002        /* Build a torus rather than a mesh                     */
#define C_FLAGELEM                  0x0004        /* Build a 8-neighbor grid rather than a 4-neighbor one */

#define C_FLAGDEFAULT               0x0000        /* Default flags */
