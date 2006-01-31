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
/**   NAME       : library_geom_f.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the graph geometry handling routines    **/
/**                of the libSCOTCH library.               **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 15 jun 2005     **/
/**                                 to     22 dec 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "scotch.h"

/******************************************/
/*                                        */
/* These routines are the Fortran API for */
/* the graph geometry handling routines.  */
/*                                        */
/******************************************/

/*
**
*/

FORTRAN (                                       \
SCOTCHFGEOMINIT, scotchfgeominit, (             \
SCOTCH_Geom * const         geomptr,            \
int * const                 revaptr),           \
(geomptr, revaptr))
{
  *revaptr = SCOTCH_geomInit (geomptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGEOMEXIT, scotchfgeomexit, (             \
SCOTCH_Geom * const         geomptr),           \
(geomptr))
{
  SCOTCH_geomExit (geomptr);
}

/*
**
*/

FORTRAN (                                       \
SCOTCHFGEOMDATA, scotchfgeomdata, (             \
const SCOTCH_Geom * const   geomptr,            \
const double * const        indxptr,            \
SCOTCH_Num * const          dimnptr,            \
SCOTCH_Num * const          geomidx),           \
(geomptr, indxptr, dimnptr, geomidx))
{
  double *            geomtab;

  SCOTCH_geomData (geomptr, dimnptr, &geomtab);
  *geomidx = (geomtab - indxptr) + 1;             /* Add 1 since Fortran indices start at 1 */
}
