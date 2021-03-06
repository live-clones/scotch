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
/**   NAME       : library_graph_map_f.c                   **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the Fortran API for the  **/
/**                mapping routines of the libSCOTCH       **/
/**                library.                                **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 02 dec 1999     **/
/**                                 to     15 nov 2001     **/
/**                # Version 4.0  : from : 12 jan 2004     **/
/**                                 to     12 dec 2005     **/
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
/* for the mapping routines.          */
/*                                    */
/**************************************/

/* String lengths are passed at the very
** end of the argument list.
*/

FORTRAN (                                       \
SCOTCHFSTRATGRAPHMAP, scotchfstratgraphmap, (   \
SCOTCH_Strat * const        stratptr,           \
const char * const          string,             \
int * const                 revaptr,            \
const int                   strnbr),            \
(stratptr, string, revaptr, strnbr))
{
  char * restrict     strtab;                     /* Pointer to null-terminated string */

  if ((strtab = (char *) memAlloc (strnbr + 1)) == NULL) { /* Allocate temporary space */
    errorPrint ("SCOTCHFSTRATGRAPHMAP: out of memory (1)");
    *revaptr = 1;
  }
  memCpy (strtab, string, strnbr);                /* Copy string contents */
  strtab[strnbr] = '\0';                          /* Terminate string     */

  *revaptr = SCOTCH_stratGraphMap (stratptr, strtab);  /* Call original routine */

  memFree (strtab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHMAPINIT, scotchfgraphmapinit, (     \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Mapping * const      mapptr,             \
const SCOTCH_Arch * const   archptr,            \
SCOTCH_Num * const          maptab,             \
int * const                 revaptr),           \
(grafptr, mapptr, archptr, maptab, revaptr))
{
  *revaptr = SCOTCH_graphMapInit (grafptr, mapptr, archptr, maptab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHMAPEXIT, scotchfgraphmapexit, (     \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Mapping * const      mapptr),            \
(grafptr, mapptr))
{
  SCOTCH_graphMapExit (grafptr, mapptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHMAPLOAD, scotchfgraphmapload, (     \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Mapping * const      mapptr,             \
int * const                 fileptr,            \
int * const                 revaptr),           \
(grafptr, mapptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHMAPLOAD: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHMAPLOAD: cannot open input stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_graphMapLoad (grafptr, mapptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHMAPSAVE, scotchfgraphmapsave, (     \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Mapping * const      mapptr,             \
int * const                 fileptr,            \
int * const                 revaptr),           \
(grafptr, mapptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHMAPSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHMAPSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_graphMapSave (grafptr, mapptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                         \
SCOTCHFGRAPHMAPCOMPUTE, scotchfgraphmapcompute, ( \
const SCOTCH_Graph * const  grafptr,              \
SCOTCH_Mapping * const      mapptr,               \
const SCOTCH_Strat * const  stratptr,             \
int * const                 revaptr),             \
(grafptr, mapptr, stratptr, revaptr))
{
  *revaptr = SCOTCH_graphMapCompute (grafptr, mapptr, stratptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHMAP, scotchfgraphmap, (             \
const SCOTCH_Graph * const  grafptr,            \
const SCOTCH_Arch * const   archptr,            \
const SCOTCH_Strat * const  stratptr,           \
SCOTCH_Num * const          maptab,             \
int * const                 revaptr),           \
(grafptr, archptr, stratptr, maptab, revaptr))
{
  *revaptr = SCOTCH_graphMap (grafptr, archptr, stratptr, maptab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHPART, scotchfgraphpart, (           \
const SCOTCH_Graph * const  grafptr,            \
const SCOTCH_Num * const    partptr,            \
const SCOTCH_Strat * const  stratptr,           \
SCOTCH_Num * const          maptab,             \
int * const                 revaptr),           \
(grafptr, partptr, stratptr, maptab, revaptr))
{
  *revaptr = SCOTCH_graphPart (grafptr, *partptr, stratptr, maptab);
}
