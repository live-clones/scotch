/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : library_arch.c                          **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the target   **/
/**                architecture handling routines of the   **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 18 aug 1998     **/
/**                                 to   : 18 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 29 mar 1999     **/
/**                # Version 3.4  : from : 01 nov 2001     **/
/**                                 to   : 01 nov 2001     **/
/**                # Version 4.0  : from : 13 jan 2004     **/
/**                                 to   : 13 jan 2004     **/
/**                # Version 5.0  : from : 12 sep 2007     **/
/**                                 to   : 12 sep 2007     **/
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
  tgtarchcmpptr = (ArchCmplt *) (void *) (&tgtarchptr->data);

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
