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
/**   NAME       : library_mesh_io_scot.c                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the Scotch   **/
/**                geometry and mesh handling routines o f **/
/**                the libSCOTCH library.                  **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 19 jan 2004     **/
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
#include "mesh.h"
#include "scotch.h"

/*************************************/
/*                                   */
/* These routines are the C API for  */
/* the Scotch mesh and geometry      */
/* handling routines.                */
/*                                   */
/*************************************/

/*+ This routine loads the given opaque mesh
*** structure with the data of the given stream.
*** - 0   : if loading succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_meshGeomLoadScot (
SCOTCH_Mesh * restrict const  meshptr,
SCOTCH_Geom * restrict const  geomptr,
FILE * const                  filesrcptr,
FILE * const                  filegeoptr,
const char * const            dataptr)            /* No use */
{
  return (meshGeomLoadScot ((Mesh *) meshptr, (Geom *) geomptr, filesrcptr, filegeoptr, NULL));
}

/*+ This routine saves the contents of the given
*** opaque mesh structure to the given stream.
*** It returns:
*** - 0   : if the saving succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_meshGeomSaveScot (
const SCOTCH_Mesh * restrict const  meshptr,
const SCOTCH_Geom * restrict const  geomptr,
FILE * const                        filesrcptr,
FILE * const                        filegeoptr,
const char * const                  dataptr)      /* No use */
{
  return (meshGeomSaveScot ((Mesh *) meshptr, (Geom *) geomptr, filesrcptr, filegeoptr, NULL));
}
