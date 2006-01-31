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
/**   NAME       : gotst.h                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a graph scalar factorizer.      **/
/**                This module contains the data declara-  **/
/**                tions for the main module.              **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 27 jan 2004     **/
/**                                 to   : 27 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines
*/

/*+ File name aliases. +*/

#define C_FILENBR                   3            /* Number of files in list                */
#define C_FILEARGNBR                3            /* Number of files which can be arguments */

#define C_filenamegrfinp            C_fileTab[0].name /* Graph input file name    */
#define C_filenamesrcout            C_fileTab[1].name /* Ordering input file name */
#define C_filenamedatout            C_fileTab[2].name /* Output data file name    */

#define C_filepntrgrfinp            C_fileTab[0].pntr /* Graph input file     */
#define C_filepntrordinp            C_fileTab[1].pntr /* Ordering output file */
#define C_filepntrdatout            C_fileTab[2].pntr /* Output data file     */

/*
**  The type and structure definitions.
*/

typedef struct C_FactorNode_ {
  struct C_FactorNode_ *    linkdad;              /*+ Father of node    +*/
  struct C_FactorNode_ *    linkson;              /*+ First son of node +*/
  struct C_FactorNode_ *    linkbro;              /*+ Brother of node   +*/
} C_FactorNode;

/*
**  The function prototypes.
*/

static int                  C_factorView        (const SCOTCH_Num, SCOTCH_Num * restrict, const SCOTCH_Num, SCOTCH_Num * restrict, SCOTCH_Num * restrict, const SCOTCH_Num, FILE * restrict const);
static void                 C_factorView2       (C_FactorNode * restrict const, SCOTCH_Num, SCOTCH_Num * restrict const, SCOTCH_Num * restrict const, SCOTCH_Num * restrict const, SCOTCH_Num * restrict const, double * restrict const, double * restrict const);
static int                  C_smbfct            (SCOTCH_Num, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *, SCOTCH_Num *);
