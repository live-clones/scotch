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
/**   NAME       : library_geom.c                          **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the geom     **/
/**                graph handling routines of the          **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 10 oct 1999     **/
/**                                 to     01 nov 2001     **/
/**                # Version 4.0  : from : 18 dec 2001     **/
/**                                 to     19 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "geom.h"
#include "graph.h"
#include "scotch.h"

/****************************************/
/*                                      */
/* These routines are the C API for the */
/* graph geometry handling routines.    */
/*                                      */
/****************************************/

/*+ This routine initializes the opaque
*** geom structure used to handle graph
*** geometry in the Scotch library.
*** It returns:
*** - 0   : if the initialization succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_geomInit (
SCOTCH_Geom * const         geomptr)
{
  if (sizeof (SCOTCH_Num) != sizeof (Gnum)) {
    errorPrint ("SCOTCH_geomInit: internal error (1)");
    return     (1);
  }
  if (sizeof (SCOTCH_Geom) < sizeof (Geom)) {
    errorPrint ("SCOTCH_geomInit: internal error (2)");
    return     (1);
  }

  return (geomInit ((Geom *) geomptr));
}

/*+ This routine frees the contents of the
*** given opaque geometry structure.
*** It returns:
*** - VOID  : in all cases.
+*/

void
SCOTCH_geomExit (
SCOTCH_Geom * const         geomptr)
{
  geomExit ((Geom *) geomptr);
}

/*+ This routine accesses all of the geometry data.
*** NULL pointers on input indicate unwanted
*** data. NULL pointers on output indicate
*** unexisting arrays.
*** It returns:
*** - VOID  : in all cases.
+*/

void
SCOTCH_geomData (
const SCOTCH_Geom * const   geomptr,              /* Geometry structure to read */
SCOTCH_Num * const          dimnptr,              /* Number of dimensions       */
double ** const             geomtab)              /* Geometry array [vertnbr]   */
{
  const Geom *        srcgeomptr;                 /* Pointer to source geometry structure */

  srcgeomptr = (const Geom *) geomptr;

  if (dimnptr != NULL)
    *dimnptr = srcgeomptr->dimnnbr;
  if (geomtab != NULL)
    *geomtab = (double *) srcgeomptr->geomtab;
}
