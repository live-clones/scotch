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
/**   NAME       : library_mesh_order_f.c                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the Fortran API for the  **/
/**                mesh ordering routines of the libSCOTCH **/
/**                library.                                **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 14 jan 2004     **/
/**                                 to     20 dec 2005     **/
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
SCOTCHFSTRATMESHORDER, scotchfstratmeshorder, ( \
SCOTCH_Strat * const        stratptr,           \
const char * const          string,             \
int * const                 revaptr,            \
const int                   strnbr),            \
(stratptr, string, revaptr, strnbr))
{
  char * restrict     strtab;                     /* Pointer to null-terminated string */

  if ((strtab = (char *) memAlloc (strnbr + 1)) == NULL) { /* Allocate temporary space */
    errorPrint ("SCOTCHFSTRATMESHORDER: out of memory (1)");
    *revaptr = 1;
  }
  memCpy (strtab, string, strnbr);                /* Copy string contents */
  strtab[strnbr] = '\0';                          /* Terminate string     */

  *revaptr = SCOTCH_stratMeshOrder (stratptr, strtab); /* Call original routine */

  memFree (strtab);                               /* Prevent compiler warnings */
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHORDERINIT, scotchfmeshorderinit, (   \
const SCOTCH_Mesh * const   meshptr,            \
SCOTCH_Ordering * const     ordeptr,            \
SCOTCH_Num * const          permtab,            \
SCOTCH_Num * const          peritab,            \
SCOTCH_Num * const          cblkptr,            \
SCOTCH_Num * const          rangtab,            \
SCOTCH_Num * const          treetab,            \
int * const                 revaptr),           \
(meshptr, ordeptr, permtab, peritab,            \
 cblkptr, rangtab, treetab, revaptr))
{
  *revaptr = SCOTCH_meshOrderInit (meshptr, ordeptr, permtab, peritab, cblkptr, rangtab, treetab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHORDEREXIT, scotchfmeshorderexit, (   \
const SCOTCH_Mesh * const   meshptr,            \
SCOTCH_Ordering * const     ordeptr),           \
(meshptr, ordeptr))
{
  SCOTCH_meshOrderExit (meshptr, ordeptr);
}

/*
**
*/

FORTRAN (                                     \
SCOTCHFMESHORDERSAVE, scotchfmeshordersave, ( \
const SCOTCH_Mesh * const     meshptr,        \
const SCOTCH_Ordering * const ordeptr,        \
int * const                   fileptr,        \
int * const                   revaptr),       \
(meshptr, ordeptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFMESHORDERSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFMESHORDERSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_meshOrderSave (meshptr, ordeptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                           \
SCOTCHFMESHORDERCOMPUTE, scotchfmeshordercompute, ( \
const SCOTCH_Mesh * const   meshptr,                \
SCOTCH_Ordering * const     ordeptr,                \
const SCOTCH_Strat * const  stratptr,               \
int * const                 revaptr),               \
(meshptr, ordeptr, stratptr, revaptr))
{
  *revaptr = SCOTCH_meshOrderCompute (meshptr, ordeptr, stratptr);
}

/*
**
*/

FORTRAN (                                                   \
SCOTCHFMESHORDERCOMPUTELIST, scotchfmeshordercomputelist, ( \
const SCOTCH_Mesh * const   meshptr,                        \
SCOTCH_Ordering * const     ordeptr,                        \
const SCOTCH_Num *          listptr,                        \
const SCOTCH_Num * const    listtab,                        \
const SCOTCH_Strat * const  stratptr,                       \
int * const                 revaptr),                       \
(meshptr, ordeptr, listptr, listtab, stratptr, revaptr))
{
  *revaptr = SCOTCH_meshOrderComputeList (meshptr, ordeptr, *listptr, listtab, stratptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHORDER, scotchfmeshorder, (           \
const SCOTCH_Mesh * const  meshptr,             \
const SCOTCH_Strat * const  stratptr,           \
SCOTCH_Num * const          permtab,            \
SCOTCH_Num * const          peritab,            \
SCOTCH_Num * const          cblkptr,            \
SCOTCH_Num * const          rangtab,            \
SCOTCH_Num * const          treetab,            \
int * const                 revaptr),           \
(meshptr, stratptr, permtab, peritab,           \
 cblkptr, rangtab, treetab, revaptr))
{
  *revaptr = SCOTCH_meshOrder (meshptr, stratptr, permtab, peritab, cblkptr, rangtab, treetab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHORDERLIST, scotchfmeshorderlist, (   \
const SCOTCH_Mesh * const   meshptr,            \
const SCOTCH_Num * const    listptr,            \
const SCOTCH_Num * const    listtab,            \
const SCOTCH_Strat * const  stratptr,           \
SCOTCH_Num * const          permtab,            \
SCOTCH_Num * const          peritab,            \
SCOTCH_Num * const          cblkptr,            \
SCOTCH_Num * const          rangtab,            \
SCOTCH_Num * const          treetab,            \
int * const                 revaptr),           \
(meshptr, listptr, listtab, stratptr,           \
 permtab, peritab, cblkptr, rangtab, treetab, revaptr))
{
  *revaptr = SCOTCH_meshOrderList (meshptr, *listptr, listtab, stratptr, permtab, peritab, cblkptr, rangtab, treetab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHORDERCHECK, scotchfmeshordercheck, ( \
const SCOTCH_Mesh * const   meshptr,            \
SCOTCH_Ordering * const     ordeptr,            \
int * const                 revaptr),           \
(meshptr, ordeptr, revaptr))
{
  *revaptr = SCOTCH_meshOrderCheck (meshptr, ordeptr);
}
