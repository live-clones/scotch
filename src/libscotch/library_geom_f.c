/* Copyright 2004,2007,2010,2018,2021,2023,2024 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : library_geom_f.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the graph geometry handling routines    **/
/**                of the libSCOTCH library.               **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 15 jun 2005     **/
/**                                 to   : 22 dec 2005     **/
/**                # Version 5.1  : from : 15 apr 2010     **/
/**                                 to   : 15 apr 2010     **/
/**                # Version 6.0  : from : 11 apr 2018     **/
/**                                 to   : 25 apr 2018     **/
/**                # Version 6.1  : from : 15 mar 2021     **/
/**                                 to   : 15 mar 2021     **/
/**                # Version 7.0  : from : 21 jan 2023     **/
/**                                 to   : 09 aug 2024     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

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

SCOTCH_FORTRAN (                      \
GEOMINIT, geominit, (                 \
SCOTCH_Geom * const         geomptr,  \
int * const                 revaptr), \
(geomptr, revaptr))
{
  *revaptr = SCOTCH_geomInit (geomptr);
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GEOMEXIT, geomexit, (                 \
SCOTCH_Geom * const         geomptr), \
(geomptr))
{
  SCOTCH_geomExit (geomptr);
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GEOMSIZEOF, geomsizeof, (             \
int * const                 sizeptr), \
(sizeptr))
{
  *sizeptr = SCOTCH_geomSizeof ();
}

/*
**
*/

SCOTCH_FORTRAN (                      \
GEOMDATA, geomdata, (                 \
const SCOTCH_Geom * const   geomptr,  \
const double * const        indxptr,  \
SCOTCH_Num * const          dimnptr,  \
SCOTCH_Idx * const          geomidx), \
(geomptr, indxptr, dimnptr, geomidx))
{
  double *            geomtab;

  SCOTCH_geomData (geomptr, dimnptr, &geomtab);
  *geomidx = (SCOTCH_Idx) (geomtab - indxptr) + 1; /* Add 1 since Fortran indices start at 1 */
}
