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
/**   NAME       : order_scotch.c                          **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These are the general purpose routines  **/
/**                used to interface PaStiX with the       **/
/**                libSCOTCH graph and mesh ordering       **/
/**                library.                                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 20 aug 1998     **/
/**                                 to     18 may 1999     **/
/**                # Version 1.0  : from : 21 mar 2003     **/
/**                                 to     21 mar 2003     **/
/**                # Version 2.0  : from : 25 oct 2003     **/
/**                                 to     25 oct 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ORDER_SCOTCH

#include "common.h"
#include "scotch.h"

/**********************************************/
/*                                            */
/* Error handling routines. These routines    */
/* redirect error messages from the libSCOTCH */
/* library to the PaStiX error processing     */
/* routines.                                  */
/*                                            */
/**********************************************/

/* This routine prints an error message with
** a variable number of arguments, as printf ()
** does, and exits.
** It returns:
** - EXIT  : in all cases.
*/

void
SCOTCHerrorPrint (
const char * const          errstr,               /*+ printf-like variable argument list +*/
...)
{
  va_list             errlist;                    /* Argument list of the call */
  char                errbuf[2048];               /* Error buffer              */
  int                 errlen;                     /* Length of header          */

  strcpy (errbuf, "orderXxxxScotch: ");
  errlen = strlen (errbuf);

  va_start   (errlist, errstr);                   /* Open variable-argument list */
#ifdef X_ARCHi586_pc_linux2
  vsnprintf  (errbuf + errlen, 2047 - errlen, errstr, errlist); /* Write result to buffer */
#else
  vsprintf   (errbuf + errlen, errstr, errlist);  /* Write result to buffer */
#endif /* X_ARCHi586_pc_linux2 */
  va_end     (errlist);                           /* Close variable-argument list */
  errbuf[2047] = '\0';                            /* Set end of string            */
  errorPrint (errbuf);                            /* Print arguments              */
}

/* This routine prints a warning message with
** a variable number of arguments, as printf ()
** does.
** It returns:
** - VOID  : in all cases.
*/

void
SCOTCHerrorPrintW (
const char * const          errstr,               /*+ printf-like variable argument list +*/
...)
{
  va_list             errlist;                    /* Argument list of the call */
  char                errbuf[2048];               /* Error buffer              */
  int                 errlen;                     /* Length of header          */

  strcpy (errbuf, "orderXxxxScotch: ");
  errlen = strlen (errbuf);

  va_start    (errlist, errstr);                  /* Open variable-argument list  */
#ifdef X_ARCHi586_pc_linux2
  vsnprintf   (errbuf + errlen, 2047 - errlen, errstr, errlist); /* Write result to buffer */
#else
  vsprintf    (errbuf + errlen, errstr, errlist); /* Write result to buffer */
#endif /* X_ARCHi586_pc_linux2 */
  va_end      (errlist);                          /* Close variable-argument list */
  errbuf[2047] = '\0';                            /* Set end of string            */
  errorPrintW (errbuf);                           /* Print arguments              */
}
