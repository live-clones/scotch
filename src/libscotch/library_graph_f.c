/* Copyright 2004,2007,2010,2018,2021,2023,2024 IPB, Universite de Bordeaux, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
**
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
**
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
**
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
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
/**                                 to   : 15 nov 2001     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to   : 17 mar 2005     **/
/**                # Version 5.0  : from : 11 jul 2007     **/
/**                                 to   : 11 jul 2007     **/
/**                # Version 5.1  : from : 27 mar 2010     **/
/**                                 to   : 15 apr 2010     **/
/**                # Version 6.0  : from : 20 apr 2018     **/
/**                                 to   : 25 apr 2018     **/
/**                # Version 6.1  : from : 15 mar 2021     **/
/**                                 to   : 15 mar 2021     **/
/**                # Version 7.0  : from : 21 jan 2023     **/
/**                                 to   : 09 aug 2024     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "module.h"
#include "common.h"
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

SCOTCH_FORTRAN (                      \
GRAPHINIT, graphinit, (               \
SCOTCH_Graph * const        grafptr,  \
int * const                 revaptr), \
(grafptr, revaptr))
{
  *revaptr = SCOTCH_graphInit (grafptr);
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GRAPHEXIT, graphexit, (               \
SCOTCH_Graph * const        grafptr), \
(grafptr))
{
  SCOTCH_graphExit (grafptr);
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GRAPHFREE, graphfree, (               \
SCOTCH_Graph * const        grafptr), \
(grafptr))
{
  SCOTCH_graphFree (grafptr);
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GRAPHSIZEOF, graphsizeof, (           \
int * const                 sizeptr), \
(sizeptr))
{
  *sizeptr = SCOTCH_graphSizeof ();
}

/* When an input stream is built from the given
** file handle, it is set as unbuffered, so as to
** allow for multiple stream reads from the same
** file handle. If it were buffered, too many
** input characters would be read on the first
** block read.
*/

SCOTCH_FORTRAN (                      \
GRAPHLOAD, graphload, (               \
SCOTCH_Graph * const        grafptr,  \
int * const                 fileptr,  \
const SCOTCH_Num * const    baseval,  \
const SCOTCH_Num * const    flagval,  \
int * const                 revaptr), \
(grafptr, fileptr, baseval, flagval, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint (STRINGIFY (SCOTCH_NAME_PUBLICFU (GRAPHLOAD)) ": cannot duplicate handle");
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "r")) == NULL) { /* Build stream from handle */
    errorPrint (STRINGIFY (SCOTCH_NAME_PUBLICFU (GRAPHLOAD)) ": cannot open input stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }
  setbuf (stream, NULL);                          /* Do not buffer on input */

  o = SCOTCH_graphLoad (grafptr, stream, *baseval, *flagval);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GRAPHSAVE, graphsave, (               \
const SCOTCH_Graph * const  grafptr,  \
int * const                 fileptr,  \
int * const                 revaptr), \
(grafptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint (STRINGIFY (SCOTCH_NAME_PUBLICFU (GRAPHSAVE)) ": cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w")) == NULL) { /* Build stream from handle */
    errorPrint (STRINGIFY (SCOTCH_NAME_PUBLICFU (GRAPHSAVE)) ": cannot open output stream");
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

SCOTCH_FORTRAN (                      \
GRAPHBUILD, graphbuild, (             \
SCOTCH_Graph * const        grafptr,  \
const SCOTCH_Num * const    baseval,  \
const SCOTCH_Num * const    vertnbr,  \
const SCOTCH_Num * const    verttab,  \
const SCOTCH_Num * const    vendtab,  \
const SCOTCH_Num * const    velotab,  \
const SCOTCH_Num * const    vlbltab,  \
const SCOTCH_Num * const    edgenbr,  \
const SCOTCH_Num * const    edgetab,  \
const SCOTCH_Num * const    edlotab,  \
int * const                 revaptr), \
(grafptr, baseval, vertnbr, verttab, vendtab,     \
 velotab, vlbltab, edgenbr, edgetab, edlotab,     \
 revaptr))
{
  *revaptr = SCOTCH_graphBuild (grafptr, *baseval, *vertnbr, verttab, vendtab,
                                velotab, vlbltab, *edgenbr, edgetab, edlotab);
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GRAPHSIZE, graphsize, (               \
const SCOTCH_Graph * const  grafptr,  \
SCOTCH_Num * const          vertptr,  \
SCOTCH_Num * const          edgeptr), \
(grafptr, vertptr, edgeptr))
{
  SCOTCH_graphSize (grafptr, vertptr, edgeptr);
}

/*
**
*/

SCOTCH_FORTRAN (                              \
GRAPHDATA, graphdata, (                       \
const SCOTCH_Graph * const  grafptr,          \
const SCOTCH_Num * const    indxptr,          \
SCOTCH_Num * const          baseptr,          \
SCOTCH_Num * const          vertptr,          \
SCOTCH_Idx * const          vertidx,          \
SCOTCH_Idx * const          vendidx,          \
SCOTCH_Idx * const          veloidx,          \
SCOTCH_Idx * const          vlblidx,          \
SCOTCH_Num * const          edgeptr,          \
SCOTCH_Idx * const          edgeidx,          \
SCOTCH_Idx * const          edloidx),         \
(grafptr, indxptr, baseptr,                   \
 vertptr, vertidx, vendidx, veloidx, vlblidx, \
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
  *vertidx = (SCOTCH_Idx) (verttab - indxptr) + 1; /* Add 1 since Fortran indices start at 1 */
  *vendidx = (SCOTCH_Idx) (vendtab - indxptr) + 1;
  *veloidx = (velotab != NULL) ? (SCOTCH_Idx) (velotab - indxptr) + 1 : *vertidx;
  *vlblidx = (vlbltab != NULL) ? (SCOTCH_Idx) (vlbltab - indxptr) + 1 : *vertidx;
  *edgeidx = (SCOTCH_Idx) (edgetab - indxptr) + 1;
  *edloidx = (edlotab != NULL) ? (SCOTCH_Idx) (edlotab - indxptr) + 1 : *edgeidx;
}

/*
**
*/

SCOTCH_FORTRAN (                              \
GRAPHSTAT, graphstat, (                       \
const SCOTCH_Graph * const  grafptr,          \
SCOTCH_Num * const          velominptr,       \
SCOTCH_Num * const          velomaxptr,       \
SCOTCH_Num * const          velosumptr,       \
double *                    veloavgptr,       \
double *                    velodltptr,       \
SCOTCH_Num * const          degrminptr,       \
SCOTCH_Num * const          degrmaxptr,       \
double *                    degravgptr,       \
double *                    degrdltptr,       \
SCOTCH_Num * const          edlominptr,       \
SCOTCH_Num * const          edlomaxptr,       \
SCOTCH_Num * const          edlosumptr,       \
double *                    edloavgptr,       \
double *                    edlodltptr),      \
(grafptr, velominptr, velomaxptr, velosumptr, \
 veloavgptr, velodltptr, degrminptr,          \
 degrmaxptr, degravgptr, degrdltptr,          \
 edlominptr, edlomaxptr, edlosumptr,          \
 edloavgptr, edlodltptr))
{
  SCOTCH_graphStat (grafptr,
                    velominptr, velomaxptr, velosumptr, veloavgptr, velodltptr,
                    degrminptr, degrmaxptr, degravgptr, degrdltptr,
                    edlominptr, edlomaxptr, edlosumptr, edloavgptr, edlodltptr);
}
