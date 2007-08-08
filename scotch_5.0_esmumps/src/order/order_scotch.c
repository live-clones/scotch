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
