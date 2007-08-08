/* Copyright 2004,2007 INRIA
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
