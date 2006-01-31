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
/**   NAME       : library_graph_order_f.c                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the Fortran API for the  **/
/**                graph ordering routines of the          **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 02 feb 2000     **/
/**                                 to     15 nov 2001     **/
/**                # Version 4.0  : from : 02 feb 2002     **/
/**                                 to     13 dec 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "scotch.h"

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the ordering routines.         */
/*                                    */
/**************************************/

FORTRAN (                                       \
SCOTCHFGRAPHORDERINIT, scotchfgraphorderinit, ( \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Ordering * const     ordeptr,            \
SCOTCH_Num * const          permtab,            \
SCOTCH_Num * const          peritab,            \
SCOTCH_Num * const          cblkptr,            \
SCOTCH_Num * const          rangtab,            \
SCOTCH_Num * const          treetab,            \
int * const                 revaptr),           \
(grafptr, ordeptr, permtab, peritab,            \
 cblkptr, rangtab, treetab, revaptr))
{
  *revaptr = SCOTCH_graphOrderInit (grafptr, ordeptr, permtab, peritab, cblkptr, rangtab, treetab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHORDEREXIT, scotchfgraphorderexit, ( \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Ordering * const     ordeptr),           \
(grafptr, ordeptr))
{
  SCOTCH_graphOrderExit (grafptr, ordeptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHORDERSAVE, scotchfgraphordersave, ( \
const SCOTCH_Graph * const    grafptr,          \
const SCOTCH_Ordering * const ordeptr,          \
int * const                   fileptr,          \
int * const                   revaptr),         \
(grafptr, ordeptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHORDERSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHORDERSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_graphOrderSave (grafptr, ordeptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                         \
SCOTCHFSTRATGRAPHORDER, scotchfstratgraphorder, ( \
SCOTCH_Strat * const        stratptr,             \
const char * const          string,               \
int * const                 revaptr,              \
const int                   strnbr),              \
(stratptr, string, revaptr, strnbr))
{
  char * restrict     strtab;                     /* Pointer to null-terminated string */

  if ((strtab = (char *) memAlloc (strnbr + 1)) == NULL) { /* Allocate temporary space */
    errorPrint ("SCOTCHFSTRATGRAPHORDER: out of memory (1)");
    *revaptr = 1;
  }
  memCpy (strtab, string, strnbr);                /* Copy string contents */
  strtab[strnbr] = '\0';                          /* Terminate string     */

  *revaptr = SCOTCH_stratGraphOrder (stratptr, strtab); /* Call original routine */

  memFree (strtab);                               /* Prevent compiler warnings */
}

/*
**
*/

FORTRAN (                                             \
SCOTCHFGRAPHORDERCOMPUTE, scotchfgraphordercompute, ( \
const SCOTCH_Graph * const  grafptr,                  \
SCOTCH_Ordering * const     ordeptr,                  \
const SCOTCH_Strat * const  stratptr,                 \
int * const                 revaptr),                 \
(grafptr, ordeptr, stratptr, revaptr))
{
  *revaptr = SCOTCH_graphOrderCompute (grafptr, ordeptr, stratptr);
}

/*
**
*/

FORTRAN (                                                     \
SCOTCHFGRAPHORDERCOMPUTELIST, scotchfgraphordercomputelist, ( \
const SCOTCH_Graph * const  grafptr,                          \
SCOTCH_Ordering * const     ordeptr,                          \
const SCOTCH_Num *          listptr,                          \
const SCOTCH_Num * const    listtab,                          \
const SCOTCH_Strat * const  stratptr,                         \
int * const                 revaptr),                         \
(grafptr, ordeptr, listptr, listtab, stratptr, revaptr))
{
  *revaptr = SCOTCH_graphOrderComputeList (grafptr, ordeptr, *listptr, listtab, stratptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHORDER, scotchfgraphorder, (         \
const SCOTCH_Graph * const  grafptr,            \
const SCOTCH_Strat * const  stratptr,           \
SCOTCH_Num * const          permtab,            \
SCOTCH_Num * const          peritab,            \
SCOTCH_Num * const          cblkptr,            \
SCOTCH_Num * const          rangtab,            \
SCOTCH_Num * const          treetab,            \
int * const                 revaptr),           \
(grafptr, stratptr, permtab, peritab,           \
 cblkptr, rangtab, treetab, revaptr))
{
  *revaptr = SCOTCH_graphOrder (grafptr, stratptr, permtab, peritab, cblkptr, rangtab, treetab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHORDERLIST, scotchfgraphorderlist, ( \
const SCOTCH_Graph * const  grafptr,            \
const SCOTCH_Num * const    listptr,            \
const SCOTCH_Num * const    listtab,            \
const SCOTCH_Strat * const  stratptr,           \
SCOTCH_Num * const          permtab,            \
SCOTCH_Num * const          peritab,            \
SCOTCH_Num * const          cblkptr,            \
SCOTCH_Num * const          rangtab,            \
SCOTCH_Num * const          treetab,            \
int * const                 revaptr),           \
(grafptr, listptr, listtab, stratptr,           \
 permtab, peritab, cblkptr, rangtab, treetab, revaptr))
{
  *revaptr = SCOTCH_graphOrderList (grafptr, *listptr, listtab, stratptr, permtab, peritab, cblkptr, rangtab, treetab);
}

/*
**
*/

FORTRAN (                                         \
SCOTCHFGRAPHORDERCHECK, scotchfgraphordercheck, ( \
const SCOTCH_Graph * const    grafptr,            \
const SCOTCH_Ordering * const ordeptr,            \
int * const                   revaptr),           \
(grafptr, ordeptr, revaptr))
{
  *revaptr = SCOTCH_graphOrderCheck (grafptr, ordeptr);
}
