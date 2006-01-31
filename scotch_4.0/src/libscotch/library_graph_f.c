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
/**   NAME       : library_graph_f.c                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the source graph handling routines of   **/
/**                the libSCOTCH library.                  **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 02 dec 1999     **/
/**                                 to     15 nov 2001     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
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

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the graph handling routines.   */
/*                                    */
/**************************************/

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHINIT, scotchfgraphinit, (           \
SCOTCH_Graph * const        grafptr,            \
int * const                 revaptr),           \
(grafptr, revaptr))
{
  *revaptr = SCOTCH_graphInit (grafptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHEXIT, scotchfgraphexit, (           \
SCOTCH_Graph * const        grafptr),           \
(grafptr))
{
  SCOTCH_graphExit (grafptr);
}

/* When an input stream is built from the given
** file handle, it is set as unbuffered, so as to
** allow for multiple stream reads from the same
** file handle. If it were buffered, too many
** input characters would be read on the first
** block read.
*/

FORTRAN (                                       \
SCOTCHFGRAPHLOAD, scotchfgraphload, (           \
SCOTCH_Graph * const        grafptr,            \
int * const                 fileptr,            \
const SCOTCH_Num * const    baseptr,            \
const SCOTCH_Num * const    flagptr,            \
int * const                 revaptr),           \
(grafptr, fileptr, baseptr, flagptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHLOAD: cannot duplicate handle");
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHLOAD: cannot open input stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }
  setbuf (stream, NULL);                          /* Do not buffer on input */

  o = SCOTCH_graphLoad (grafptr, stream, *baseptr, *flagptr);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHSAVE, scotchfgraphsave, (           \
const SCOTCH_Graph * const  grafptr,            \
int * const                 fileptr,            \
int * const                 revaptr),           \
(grafptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFGRAPHSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFGRAPHSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_graphSave (grafptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHBUILD, scotchfgraphbuild, (         \
SCOTCH_Graph * const        grafptr,            \
const SCOTCH_Num * const    baseptr,            \
const SCOTCH_Num * const    vertptr,            \
const SCOTCH_Num * const    verttab,            \
const SCOTCH_Num * const    vendtab,            \
const SCOTCH_Num * const    velotab,            \
const SCOTCH_Num * const    vlbltab,            \
const SCOTCH_Num * const    edgeptr,            \
const SCOTCH_Num * const    edgetab,            \
const SCOTCH_Num * const    edlotab,            \
int * const                 revaptr),           \
(grafptr, baseptr, vertptr, verttab, vendtab,   \
 velotab, vlbltab, edgeptr, edgetab, edlotab,   \
 revaptr))
{
  *revaptr = SCOTCH_graphBuild (grafptr, *baseptr, *vertptr, verttab, vendtab,
                                velotab, vlbltab, *edgeptr, edgetab, edlotab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHSIZE, scotchfgraphsize, (           \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Num * const          vertptr,            \
SCOTCH_Num * const          edgeptr),           \
(grafptr, vertptr, edgeptr))
{
  SCOTCH_graphSize (grafptr, vertptr, edgeptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHDATA, scotchfgraphdata, (           \
const SCOTCH_Graph * const  grafptr,            \
const SCOTCH_Num * const    indxptr,            \
SCOTCH_Num * const          baseptr,            \
SCOTCH_Num * const          vertptr,            \
SCOTCH_Num * const          vertidx,            \
SCOTCH_Num * const          vendidx,            \
SCOTCH_Num * const          veloidx,            \
SCOTCH_Num * const          vlblidx,            \
SCOTCH_Num * const          edgeptr,            \
SCOTCH_Num * const          edgeidx,            \
SCOTCH_Num * const          edloidx),           \
(grafptr, indxptr, baseptr,                     \
 vertptr, vertidx, vendidx, veloidx, vlblidx,   \
 edgeptr, edgeidx, edloidx))
{
  SCOTCH_Num *        verttab;                    /* Pointer to graph arrays */
  SCOTCH_Num *        vendtab;
  SCOTCH_Num *        velotab;
  SCOTCH_Num *        vlbltab;
  SCOTCH_Num *        edgetab;
  SCOTCH_Num *        edlotab;

  SCOTCH_graphData (grafptr, baseptr, vertptr, &verttab, &vendtab, &velotab, &vlbltab,
                    edgeptr, &edgetab, &edlotab);
  *vertidx = (verttab - indxptr) + 1;             /* Add 1 since Fortran indices start at 1 */
  *vendidx = (vendtab - indxptr) + 1;
  *veloidx = (velotab != NULL) ? (velotab - indxptr) + 1 : *vertidx;
  *vlblidx = (vlbltab != NULL) ? (vlbltab - indxptr) + 1 : *vertidx;
  *edgeidx = (edgetab - indxptr) + 1;
  *edloidx = (edlotab != NULL) ? (edlotab - indxptr) + 1 : *edgeidx;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHSTAT, scotchfgraphstat, (           \
const SCOTCH_Graph * const  grafptr,            \
SCOTCH_Num * const          velominptr,         \
SCOTCH_Num * const          velomaxptr,         \
SCOTCH_Num * const          velosumptr,         \
double *                    veloavgptr,         \
double *                    velodltptr,         \
SCOTCH_Num * const          degrminptr,         \
SCOTCH_Num * const          degrmaxptr,         \
double *                    degravgptr,         \
double *                    degrdltptr,         \
SCOTCH_Num * const          edlominptr,         \
SCOTCH_Num * const          edlomaxptr,         \
SCOTCH_Num * const          edlosumptr,         \
double *                    edloavgptr,         \
double *                    edlodltptr),        \
(grafptr, velominptr, velomaxptr, velosumptr,   \
 veloavgptr, velodltptr, degrminptr,            \
 degrmaxptr, degravgptr, degrdltptr,            \
 edlominptr, edlomaxptr, edlosumptr,            \
 edloavgptr, edlodltptr))
{
  SCOTCH_graphStat (grafptr,
                    velominptr, velomaxptr, velosumptr, veloavgptr, velodltptr,
                    degrminptr, degrmaxptr, degravgptr, degrdltptr,
                    edlominptr, edlomaxptr, edlosumptr, edloavgptr, edlodltptr);
}
