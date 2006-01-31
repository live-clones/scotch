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
/**   NAME       : library_arch_build_f.c                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the target architecture building        **/
/**                routine of the libSCOTCH library.       **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 17 mar 2005     **/
/**                                 to     17 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "graph.h"
#include "graph_io.h"
#include "scotch.h"

/****************************************/
/*                                      */
/* These routines are the Fortran API   */
/* for the target architecture building */
/* routines.                            */
/*                                      */
/****************************************/

/*
**
*/

FORTRAN (                                           \
SCOTCHFSTRATGRAPHBIPART, scotchfstratgraphbipart, ( \
SCOTCH_Strat * const        stratptr,               \
const char * const          string,                 \
int * const                 revaptr,                \
const int                   strnbr),                \
(stratptr, string, revaptr, strnbr))
{
  char * restrict     strtab;                     /* Pointer to null-terminated string */

  if ((strtab = (char *) memAlloc (strnbr + 1)) == NULL) { /* Allocate temporary space */
    errorPrint ("SCOTCHFSTRATGRAPHBIPART: out of memory (1)");
    *revaptr = 1;
  }
  memCpy (strtab, string, strnbr);                /* Copy string contents */
  strtab[strnbr] = '\0';                          /* Terminate string     */

  *revaptr = SCOTCH_stratGraphBipart (stratptr, strtab); /* Call original routine */

  memFree (strtab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFARCHBUILD, scotchfarchbuild, (           \
SCOTCH_Arch * const         archptr,            \
SCOTCH_Graph * const        grafptr,            \
const SCOTCH_Num * const    listnbr,            \
const SCOTCH_Num * const    listptr,            \
SCOTCH_Strat * const        stratptr,           \
int * const                 revaptr),           \
(archptr, grafptr, listnbr, listptr, stratptr, revaptr))
{
  *revaptr = SCOTCH_archBuild (archptr, grafptr, *listnbr, listptr, stratptr);
}
