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
/**   NAME       : library_mesh_f.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the source mesh handling routines of    **/
/**                the libSCOTCH library.                  **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 23 sep 2002     **/
/**                                 to     13 dec 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "scotch.h"

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the mesh handling routines.    */
/*                                    */
/**************************************/

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHINIT, scotchfmeshinit, (             \
SCOTCH_Mesh * const         meshptr,            \
int * const                 revaptr),           \
(meshptr, revaptr))
{
  *revaptr = SCOTCH_meshInit (meshptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHEXIT, scotchfmeshexit, (             \
SCOTCH_Mesh * const         meshptr),           \
(meshptr))
{
  SCOTCH_meshExit (meshptr);
}

/* When an input stream is built from the given
** file handle, it is set as unbuffered, so as to
** allow for multiple stream reads from the same
** file handle. If it were buffered, too many
** input characters would be read on the first
** block read.
*/

FORTRAN (                                       \
SCOTCHFMESHLOAD, scotchfmeshload, (             \
SCOTCH_Mesh * const         meshptr,            \
int * const                 fileptr,            \
const SCOTCH_Num * const    baseptr,            \
int * const                 revaptr),           \
(meshptr, fileptr, baseptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFMESHLOAD: cannot duplicate handle");
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFMESHLOAD: cannot open input stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }
  setbuf (stream, NULL);                          /* Do not buffer on input */

  o = SCOTCH_meshLoad (meshptr, stream, *baseptr);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHSAVE, scotchfmeshsave, (             \
const SCOTCH_Mesh * const   meshptr,            \
int * const                 fileptr,            \
int * const                 revaptr),           \
(meshptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFMESHSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFMESHSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_meshSave (meshptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHBUILD, scotchfmeshbuild, (           \
SCOTCH_Mesh * const         meshptr,            \
const SCOTCH_Num * const    velmbas,            \
const SCOTCH_Num * const    vnodbas,            \
const SCOTCH_Num * const    velmnbr,            \
const SCOTCH_Num * const    vnodnbr,            \
const SCOTCH_Num * const    verttab,            \
const SCOTCH_Num * const    vendtab,            \
const SCOTCH_Num * const    velotab,            \
const SCOTCH_Num * const    vnlotab,            \
const SCOTCH_Num * const    vlbltab,            \
const SCOTCH_Num * const    edgenbr,            \
const SCOTCH_Num * const    edgetab,            \
int * const                 revaptr),           \
(meshptr, velmbas, vnodbas, velmnbr, vnodnbr,   \
 verttab, vendtab, velotab, vnlotab, vlbltab,   \
 edgenbr, edgetab, revaptr))
{
  *revaptr = SCOTCH_meshBuild (meshptr, *velmnbr, *vnodnbr, *velmbas, *vnodbas,
                               verttab, vendtab, velotab, vnlotab, vlbltab,
                               *edgenbr, edgetab);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHCHECK, scotchfmeshcheck, (           \
const SCOTCH_Mesh * const   meshptr,            \
int * const                 revaptr),           \
(meshptr, revaptr))
{
  *revaptr = SCOTCH_meshCheck (meshptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHSIZE, scotchfmeshsize, (             \
const SCOTCH_Mesh * const  meshptr,             \
SCOTCH_Num * const          velmnbr,            \
SCOTCH_Num * const          vnodnbr,            \
SCOTCH_Num * const          edgenbr),           \
(meshptr, velmnbr, vnodnbr, edgenbr))
{
  SCOTCH_meshSize (meshptr, velmnbr, vnodnbr, edgenbr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFMESHDATA, scotchfmeshdata, (             \
const SCOTCH_Mesh * const   meshptr,            \
const SCOTCH_Num * const    indxptr,            \
SCOTCH_Num * const          velmbas,            \
SCOTCH_Num * const          vnodbas,            \
SCOTCH_Num * const          velmnbr,            \
SCOTCH_Num * const          vnodnbr,            \
SCOTCH_Num * const          vertidx,            \
SCOTCH_Num * const          vendidx,            \
SCOTCH_Num * const          veloidx,            \
SCOTCH_Num * const          vnloidx,            \
SCOTCH_Num * const          vlblidx,            \
SCOTCH_Num * const          edgenbr,            \
SCOTCH_Num * const          edgeidx,            \
SCOTCH_Num * const          degrnbr),           \
(meshptr, indxptr, velmbas, vnodbas, velmnbr, vnodnbr, \
 vertidx, vendidx, veloidx, vnloidx, vlblidx,          \
 edgenbr, edgeidx, degrnbr))
{
  SCOTCH_Num *        verttab;                    /* Pointer to mesh arrays */
  SCOTCH_Num *        vendtab;
  SCOTCH_Num *        velotab;
  SCOTCH_Num *        vnlotab;
  SCOTCH_Num *        vlbltab;
  SCOTCH_Num *        edgetab;

  SCOTCH_meshData (meshptr, velmbas, vnodbas, velmnbr, vnodnbr,
                   &verttab, &vendtab, &velotab, &vnlotab, &vlbltab,
                   edgenbr, &edgetab, degrnbr);
  *vertidx = (verttab - indxptr) + 1;             /* Add 1 since Fortran indices start at 1 */
  *vendidx = (vendtab - indxptr) + 1;
  *veloidx = (velotab != NULL) ? (velotab - indxptr) + 1 : *vertidx;
  *vnloidx = (vnlotab != NULL) ? (vnlotab - indxptr) + 1 : *vertidx;
  *vlblidx = (vlbltab != NULL) ? (vlbltab - indxptr) + 1 : *vertidx;
  *edgeidx = (edgetab - indxptr) + 1;
}
