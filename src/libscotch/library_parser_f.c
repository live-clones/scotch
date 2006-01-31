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
/**   NAME       : library_parser_f.c                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the Fortran API for the  **/
/**                strategy handling routines of the       **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 17 jan 2004     **/
/**                                 to     17 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "parser.h"
#include "scotch.h"

/***************************************/
/*                                     */
/* These routines are the Fortran API  */
/* for the ordering handling routines. */
/*                                     */
/***************************************/

FORTRAN (                                       \
SCOTCHFSTRATINIT, scotchfstratinit, (           \
SCOTCH_Strat * const        stratptr,           \
int * const                 revaptr),           \
(stratptr, revaptr))
{
  *revaptr = SCOTCH_stratInit (stratptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFSTRATEXIT, scotchfstratexit, (           \
SCOTCH_Strat * const        stratptr),          \
(stratptr))
{
  SCOTCH_stratExit (stratptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFSTRATSAVE, scotchfstratsave, (           \
const SCOTCH_Strat * const  stratptr,           \
int * const                 fileptr,            \
int * const                 revaptr),           \
(stratptr, fileptr, revaptr))
{
  FILE *              stream;                     /* Stream to build from handle */
  int                 filenum;                    /* Duplicated handle           */
  int                 o;

  if ((filenum = dup (*fileptr)) < 0) {           /* If cannot duplicate file descriptor */
    errorPrint ("SCOTCHFSTRATSAVE: cannot duplicate handle");

    *revaptr = 1;                                 /* Indicate error */
    return;
  }
  if ((stream = fdopen (filenum, "w+")) == NULL) { /* Build stream from handle */
    errorPrint ("SCOTCHFSTRATSAVE: cannot open output stream");
    close      (filenum);
    *revaptr = 1;
    return;
  }

  o = SCOTCH_stratSave (stratptr, stream);

  fclose (stream);                                /* This closes filenum too */

  *revaptr = o;
}
