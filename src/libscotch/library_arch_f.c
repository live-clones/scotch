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
/**   NAME       : library_arch_f.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the source graph handling routines of   **/
/**                the libSCOTCH library.                  **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 02 dec 1999     **/
/**                                 to     15 nov 2001     **/
/**                # Version 4.0  : from : 13 jan 2004     **/
/**                                 to     13 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "scotch.h"

/*************************************/
/*                                   */
/* These routines are the Fortran    */
/* API for the architecture handling */
/* routines.                         */
/*                                   */
/*************************************/

/*
**
*/

FORTRAN (                                       \
SCOTCHFARCHINIT, scotchfarchinit, (             \
SCOTCH_Arch * const         archptr,            \
int * const                 revaptr),           \
(archptr, revaptr))
{
  *revaptr = SCOTCH_archInit (archptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFARCHEXIT, scotchfarchexit, (             \
SCOTCH_Arch * const         archptr),           \
(archptr))
{
  SCOTCH_archExit (archptr);
}

/* When an input stream is built from the given
** file handle, it is set as unbuffered, so as to
** allow for multiple stream reads from the same
** file handle. If it were buffered, too many
** input characters would be read on the first
** block read.
*/

FORTRAN (                                       \
SCOTCHFARCHLOAD, scotchfarchload, (             \
SCOTCH_Arch * const         archptr,            \
int * const                 fileptr,            \
int * const                 revaptr),           \
(archptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFARCHLOAD: cannot duplicate handle");
    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "r+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFARCHLOAD: cannot open input stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }
  setbuf (stream, NULL);                          /* Do not buffer on input */

  o = SCOTCH_archLoad (archptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFARCHSAVE, scotchfarchsave, (             \
const SCOTCH_Arch * const   archptr,            \
int * const                 fileptr,            \
int * const                 revaptr),           \
(archptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFARCHSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFARCHSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_archSave (archptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFARCHCMPLT, scotchfarchcmplt, (           \
SCOTCH_Arch * const         archptr,            \
const SCOTCH_Num * const    archnbr,            \
int * const                 revaptr),           \
(archptr, archnbr, revaptr))
{
  *revaptr = SCOTCH_archCmplt (archptr, *archnbr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFARCHNAME, scotchfarchname, (             \
const SCOTCH_Arch * const   archptr,            \
char * const                chartab,            \
int * const                 charptr),           \
(archptr, chartab, charptr))
{
  char *              nameptr;                    /* Name string */

  nameptr = SCOTCH_archName (archptr);            /* Get architecture name  */
  strncpy (chartab, nameptr, *charptr);           /* Copy architecture name */
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFARCHSIZE, scotchfarchsize, (             \
const SCOTCH_Arch * const   archptr,            \
int * const                 sizeptr),           \
(archptr, sizeptr))
{
  *sizeptr = SCOTCH_archSize (archptr);
}
