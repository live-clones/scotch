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
/**   NAME       : library_parser.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the generic  **/
/**                strategy handling routines of the       **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 19 aug 1998     **/
/**                                 to     19 aug 1998     **/
/**   DATES      : # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     31 may 1999     **/
/**                # Version 3.4  : from : 01 nov 2001     **/
/**                                 to     01 nov 2001     **/
/**                # Version 4.0  : from : 23 dec 2001     **/
/**                                 to   : 23 dec 2001     **/
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

/************************************/
/*                                  */
/* These routines are the C API for */
/* the strategy handling routines.  */
/*                                  */
/************************************/

/* This routine initializes a strategy
** structure.
** It returns:
** - 0  : in all cases.
*/

int
SCOTCH_stratInit (
SCOTCH_Strat * const        stratptr)
{
  if (sizeof (SCOTCH_Strat) < sizeof (Strat *)) {
    errorPrint ("SCOTCH_stratInit: internal error (1)");
    return     (1);
  }

  *((Strat **) stratptr) = NULL;                  /* Initialize pointer to strategy */

  return (0);
}

/* This routine frees a strategy structure.
** It returns:
** - VOID  : in all cases.
*/

void
SCOTCH_stratExit (
SCOTCH_Strat * const        stratptr)
{
  if (*((Strat **) stratptr) != NULL)             /* If strategy is not null */
    stratExit (*((Strat **) stratptr));           /* Free strategy structure */
}

/* This routine outputs the contents of the
** given strategy to the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
SCOTCH_stratSave (
const SCOTCH_Strat * const  stratptr,
FILE * const                stream)
{
  return (stratSave (*((Strat **) stratptr), stream));
}
