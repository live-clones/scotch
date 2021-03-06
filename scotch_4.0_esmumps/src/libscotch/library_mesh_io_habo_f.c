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
/**   NAME       : library_mesh_io_habo_f.c                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the Fortran API for the  **/
/**                mesh i/o routines of the libSCOTCH      **/
/**                library.                                **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 24 nov 2005     **/
/**                                 to     24 nov 2005     **/
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

FORTRAN (                                           \
SCOTCHFMESHGEOMLOADHABO, scotchfmeshgeomloadhabo, ( \
SCOTCH_Mesh * const         meshptr,                \
SCOTCH_Geom * const         geomptr,                \
const int * const           filegrfptr,             \
const int * const           filegeoptr,             \
const char * const          dataptr, /* No use */   \
int * const                 revaptr,                \
const int                   datanbr),               \
(meshptr, geomptr, filegrfptr, filegeoptr, dataptr, revaptr, datanbr))
{
  FILE *              filegrfstream;              /* Streams to build from handles */
  FILE *              filegeostream;
  int                 filegrfnum;                 /* Duplicated handle */
  int                 filegeonum;
  int                 o;

  if ((filegrfnum = dup (*filegrfptr)) < 0) {     /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFMESHGEOMLOADHABO: cannot duplicate handle (1)");
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((filegeonum = dup (*filegeoptr)) < 0) {     /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFMESHGEOMLOADHABO: cannot duplicate handle (2)");
    close      (filegrfnum);
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((filegrfstream = fdopen (filegrfnum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFMESHGEOMLOADHABO: cannot open input stream (1)");
    close      (filegrfnum);
    close      (filegeonum);
    *revaptr = 1;
    return;
  }
  if ((filegeostream = fdopen (filegeonum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFMESHGEOMLOADHABO: cannot open input stream (2)");
    fclose     (filegrfstream);
    close      (filegeonum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_meshGeomLoadHabo (meshptr, geomptr, filegrfstream, filegeostream, NULL);

  fclose (filegrfstream);                         /* This closes file descriptors too */
  fclose (filegeostream);

  *revaptr = o;
}
