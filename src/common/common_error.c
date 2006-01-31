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
/**   NAME       : common_error.c                          **/
/**                                                        **/
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module handles errors.             **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 08 may 1998     **/
/**                                 to     02 oct 1998     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_ERROR

#include "common.h"

/********************************/
/*                              */
/* The error handling routines. */
/*                              */
/********************************/

static char                 errorProgName[32] = "";

/* This routine sets the program name for
** error reporting.
** It returns:
** - VOID  : in all cases.
*/

void
errorProg (
const char * const          progstr)              /*+ Program name +*/
{
  strncpy (errorProgName, progstr, 29);
  errorProgName[29] = '\0';
  strcat  (errorProgName, ": ");

}

/* This routine prints an error message with
** a variable number of arguments, as printf ()
** does, and exits.
** It returns:
** - EXIT  : in all cases.
*/

void
errorPrint (
const char * const          errstr,               /*+ printf-like variable argument list */
...)
{
  va_list             errlist;                    /* Argument list of the call */

  fprintf  (stderr, "%sERROR: ", errorProgName);
  va_start (errlist, errstr);
  vfprintf (stderr, errstr, errlist);             /* Print arguments */
  va_end   (errlist);
  fprintf  (stderr, "\n");
  fflush   (stderr);                              /* In case it has been set to buffered mode */
}

/* This routine prints a warning message with
** a variable number of arguments, as printf ()
** does.
** It returns:
** - VOID  : in all cases.
*/

void
errorPrintW (
const char * const          errstr,               /*+ printf-like variable argument list */
...)
{
  va_list             errlist;                    /* Argument list of the call */

  fprintf  (stderr, "%sWARNING: ", errorProgName);
  va_start (errlist, errstr);
  vfprintf (stderr, errstr, errlist);             /* Print arguments */
  va_end   (errlist);
  fprintf  (stderr, "\n");
  fflush   (stderr);                              /* In case it has been set to buffered mode */
}
