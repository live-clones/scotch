/* Copyright 2007 INRIA
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
/**   NAME       : library_dgraph_f.c                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the distributed source graph handling   **/
/**                routines of the libSCOTCH library.      **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 04 sep 2006     **/
/**                                 to     05 aug 2007     **/
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
/* for the graph handling routines.   */
/*                                    */
/**************************************/

/*
**
*/

FORTRAN (                                       \
SCOTCHFDGRAPHINIT, scotchfdgraphinit, (         \
SCOTCH_Dgraph * const       grafptr,            \
const MPI_Comm * const      commptr,            \
int * const                 revaptr),           \
(grafptr, commptr, revaptr))
{
  *revaptr = SCOTCH_dgraphInit (grafptr, *commptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFDGRAPHEXIT, scotchfdgraphexit, (         \
SCOTCH_Dgraph * const       grafptr),           \
(grafptr))
{
  SCOTCH_dgraphExit (grafptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFDGRAPHSAVE, scotchfdgraphsave, (         \
SCOTCH_Dgraph * const       grafptr,            \
int * const                 fileptr,            \
int * const                 revaptr),           \
(grafptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFDGRAPHSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFDGRAPHSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_dgraphSave (grafptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFDGRAPHSIZE, scotchfdgraphsize, (         \
const SCOTCH_Dgraph * const grafptr,            \
SCOTCH_Num * const          vertglbptr,         \
SCOTCH_Num * const          vertlocptr,         \
SCOTCH_Num * const          edgeglbptr,         \
SCOTCH_Num * const          edgelocptr),        \
(grafptr, vertglbptr, vertlocptr, edgeglbptr, edgelocptr))
{
  SCOTCH_dgraphSize (grafptr, vertglbptr, vertlocptr, edgeglbptr, edgelocptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFDGRAPHDATA, scotchfdgraphdata, (         \
const SCOTCH_Dgraph * const grafptr,            \
const SCOTCH_Num * const    indxptr,            \
SCOTCH_Num * const          baseptr,            \
SCOTCH_Num * const          vertglbptr,         \
SCOTCH_Num * const          vertlocptr,         \
SCOTCH_Num * const          vertlocptz,         \
SCOTCH_Num * const          vertgstptr,         \
SCOTCH_Num * const          vertlocidx,         \
SCOTCH_Num * const          vendlocidx,         \
SCOTCH_Num * const          velolocidx,         \
SCOTCH_Num * const          vlbllocidx,         \
SCOTCH_Num * const          edgeglbptr,         \
SCOTCH_Num * const          edgelocptr,         \
SCOTCH_Num * const          edgelocptz,         \
SCOTCH_Num * const          edgelocidx,         \
SCOTCH_Num * const          edgegstidx,         \
SCOTCH_Num * const          edlolocidx,         \
int * const                 comm),              \
(grafptr, indxptr, baseptr,                     \
 vertglbptr, vertlocptr, vertlocptz,            \
 vertgstptr, vertlocidx, vendlocidx,            \
 velolocidx, vlbllocidx, edgeglbptr,            \
 edgelocptr, edgelocptz, edgelocidx,            \
 edgegstidx, edlolocidx, comm))
{
  SCOTCH_Num *        vertloctab;                 /* Pointer to graph arrays */
  SCOTCH_Num *        vendloctab;
  SCOTCH_Num *        veloloctab;
  SCOTCH_Num *        vlblloctab;
  SCOTCH_Num *        edgeloctab;
  SCOTCH_Num *        edgegsttab;
  SCOTCH_Num *        edloloctab;

  SCOTCH_dgraphData (grafptr, baseptr, vertglbptr, vertlocptr, vertlocptz, vertgstptr,
                     &vertloctab, &vendloctab, &veloloctab, &vlblloctab,
                     edgeglbptr, edgelocptr, edgelocptz,
                     &edgeloctab, &edgegsttab, &edloloctab, (MPI_Comm * const) comm);
  *vertlocidx = (vertloctab - indxptr) + 1;       /* Add 1 since Fortran indices start at 1 */
  *vendlocidx = (vendloctab - indxptr) + 1;
  *velolocidx = (veloloctab != NULL) ? (veloloctab - indxptr) + 1 : *vertlocidx;
  *vlbllocidx = (vlblloctab != NULL) ? (vlblloctab - indxptr) + 1 : *vertlocidx;
  *edgelocidx = (edgeloctab - indxptr) + 1;
  *edgegstidx = (edgegsttab != NULL) ? (edgegsttab - indxptr) + 1 : *vertlocidx;
  *edlolocidx = (edloloctab != NULL) ? (edloloctab - indxptr) + 1 : *vertlocidx;
}
