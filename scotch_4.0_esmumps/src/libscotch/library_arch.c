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
/**   NAME       : library_arch.c                          **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the target   **/
/**                architecture handling routines of the   **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 18 aug 1998     **/
/**                                 to     18 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     29 mar 1999     **/
/**                # Version 3.4  : from : 01 nov 2001     **/
/**                                 to     01 nov 2001     **/
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
#include "arch_cmplt.h"
#include "scotch.h"

/***************************************/
/*                                     */
/* These routines are the C API for    */
/* the architecture handling routines. */
/*                                     */
/***************************************/

/*+ This routine initializes the opaque
*** architecture structure used to handle
*** target architectures in the Scotch library.
*** It returns:
*** - 0   : if the initialization succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_archInit (
SCOTCH_Arch * const         archptr)
{
  if (sizeof (SCOTCH_Num) != sizeof (Anum)) {
    errorPrint ("SCOTCH_archInit: internal error (1)");
    return     (1);
  }
  if (sizeof (SCOTCH_Arch) < sizeof (Arch)) {
    errorPrint ("SCOTCH_archInit: internal error (2)");
    return     (1);
  }

  return (archInit ((Arch *) archptr));
}

/*+ This routine frees the contents of the
*** given opaque architecture structure.
*** It returns:
*** - VOID  : in all cases.
+*/

void
SCOTCH_archExit (
SCOTCH_Arch * const         archptr)
{
  archExit ((Arch *) archptr);
}

/*+ This routine loads the given opaque
*** architecture structure with the data of
*** the given stream.
*** It returns:
*** - 0   : if the loading succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_archLoad (
SCOTCH_Arch * const         archptr,
FILE * const                stream)
{
  return (archLoad ((Arch *) archptr, stream));
}

/*+ This routine saves the given opaque
*** architecture structure to the given
*** stream.
*** It returns:
*** - 0   : if the saving succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_archSave (
const SCOTCH_Arch * const   archptr,
FILE * const                stream)
{
  return (archSave ((Arch *) archptr, stream));
}

/*+ This routine fills the contents of the given
*** opaque target structure so as to yield a
*** complete graph with the given number of vertices.
*** It returns:
*** - 0   : if the computation succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_archCmplt (
SCOTCH_Arch * const         archptr,
const SCOTCH_Num            archnbr)
{
  Arch *              tgtarchptr;
  ArchCmplt *         tgtarchcmpptr;

  if (sizeof (SCOTCH_Num) != sizeof (Gnum)) {
    errorPrint ("SCOTCH_archCmplt: internal error (1)");
    return     (1);
  }

  tgtarchptr    = (Arch *) archptr;
  tgtarchcmpptr = (ArchCmplt *) (&tgtarchptr->data);

  tgtarchptr->class     = archClass ("cmplt");
  tgtarchcmpptr->numnbr = (Gnum) archnbr;

  return (0);
}

/*+ This routine returns the name of the
*** given target architecture.
*** It returns:
*** - !NULL  : pointer to the name of the
***            target architecture.
+*/

char *
SCOTCH_archName (
const SCOTCH_Arch * const   archptr)
{
  return (archName ((const Arch * const) archptr));
}

/*+ This routine returns the size of the
*** given target architecture.
*** It returns:
*** - !0  : size of the target architecture.
+*/

SCOTCH_Num
SCOTCH_archSize (
const SCOTCH_Arch * const   archptr)
{
  ArchDom             domdat;

  archDomFrst ((Arch *) archptr, &domdat);        /* Get first domain     */
  return (archDomSize ((Arch *) archptr, &domdat)); /* Return domain size */
}
