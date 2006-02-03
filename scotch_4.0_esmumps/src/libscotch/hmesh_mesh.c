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
/**   NAME       : hmesh_mesh.c                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the mesh un-       **/
/**                haloing routine.                        **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 28 apr 2004     **/
/**                                 to     11 may 2004     **/
/**                                                        **/
/**   NOTES      : # From a given halo mesh is created a   **/
/**                  non-halo mesh. When nodes are         **/
/**                  numbered after elements, halo nodes   **/
/**                  are simply removed. When nodes are    **/
/**                  numbered before elements, halo nodes  **/
/**                  are turned into empty elements such   **/
/**                  that the numbering of vertices        **/
/**                  remains continuous, without holes.    **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH_MESH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "hgraph.h"
#include "mesh.h"
#include "hmesh.h"

/***************************************/
/*                                     */
/* The non-halo mesh building routine. */
/*                                     */
/***************************************/

/* This routine builds a non-halo mesh from
** the given halo mesh.
** It returns:
** - 0  : if the non-halo mesh has been successfully built.
** - 1  : on error.
*/

int
hmeshMesh (
const Hmesh * restrict const  hmshptr,            /*+ Original halo mesh +*/
Mesh * restrict const         meshptr)            /*+ Mesh to build      +*/
{
  meshptr->baseval = hmshptr->m.baseval;
  meshptr->veisnbr = hmshptr->m.veisnbr + hmshptr->veihnbr; /* Add halo isolated elements to isolated elements */
  meshptr->vnodnbr = hmshptr->vnohnbr;
  meshptr->vnodbas = hmshptr->m.vnodbas;
  meshptr->vnodnnd = hmshptr->vnohnbr + hmshptr->m.vnodbas;
  meshptr->verttax = hmshptr->m.verttax;
  meshptr->velotax = hmshptr->m.velotax;
  meshptr->vnlotax = hmshptr->m.vnlotax;          /* Use non-halo part of node vertex load array, if any */
  meshptr->velosum = hmshptr->m.velosum;
  meshptr->vnlosum = hmshptr->vnhlsum;
  meshptr->vnumtax = hmshptr->m.vnumtax;          /* The same for vnumtab                                */
  meshptr->vlbltax = NULL;
  meshptr->edgenbr = hmshptr->enohnbr;
  meshptr->edgetax = hmshptr->m.edgetax;
  meshptr->degrmax = hmshptr->m.degrmax;

  if (hmshptr->vnohnbr == hmshptr->m.vnodnbr) {   /* If halo mesh does not have any halo      */
    meshptr->flagval = MESHNONE;                  /* Just create a clone of the original mesh */
    meshptr->velmnbr = hmshptr->m.velmnbr;
    meshptr->velmbas = hmshptr->m.velmbas;
    meshptr->velmnnd = hmshptr->m.velmnnd;
    meshptr->vendtax = hmshptr->m.vendtax;
    return (0);
  }

  meshptr->flagval = MESHFREEVEND;
  if (hmshptr->m.velmbas <= hmshptr->m.vnodbas) { /* If elements numbered before nodes */
    if ((meshptr->vendtax = memAlloc ((hmshptr->m.velmnbr + hmshptr->vnohnbr) * sizeof (Gnum))) == NULL) { /* Do not keep halo nodes at end of array */
      errorPrint ("hmeshHgraph: out of memory (1)");
      return     (1);
    }
    memCpy (meshptr->vendtax, hmshptr->vehdtax + hmshptr->m.velmbas, hmshptr->m.velmnbr * sizeof (Gnum));
    memCpy (meshptr->vendtax + hmshptr->m.velmnbr, hmshptr->m.vendtax + hmshptr->m.vnodbas, hmshptr->vnohnbr * sizeof (Gnum));

    meshptr->velmnbr = hmshptr->m.velmnbr;
    meshptr->velmbas = hmshptr->m.velmbas;
    meshptr->velmnnd = hmshptr->m.velmnnd;
  }
  else {                                          /* If nodes numbered before elements */
    if ((meshptr->vendtax = memAlloc ((hmshptr->m.velmnbr + hmshptr->m.vnodnbr) * sizeof (Gnum))) == NULL) { /* Turn halo nodes into empty elements */
      errorPrint ("hmeshHgraph: out of memory (2)");
      return     (1);
    }
    memCpy (meshptr->vendtax, hmshptr->m.vendtax + hmshptr->m.baseval, hmshptr->vnohnbr * sizeof (Gnum)); /* Copy non-halo node part */
    memCpy (meshptr->vendtax + hmshptr->vnohnbr, hmshptr->m.verttax + hmshptr->vnohnnd, hmshptr->m.vnodnbr - hmshptr->vnohnbr * sizeof (Gnum)); /* Create empty fake element part */
    memCpy (meshptr->vendtax + hmshptr->m.vnodnbr, hmshptr->vehdtax + hmshptr->m.velmbas, hmshptr->m.velmnbr * sizeof (Gnum));

    meshptr->velmnbr = hmshptr->m.velmnbr + hmshptr->m.vnodnbr - hmshptr->vnohnbr; /* Turn halo node vertices into element vertices */
    meshptr->velmbas = hmshptr->vnohnnd;
    meshptr->velmnnd = hmshptr->m.velmnnd;
  }
  meshptr->vendtax -= meshptr->baseval;

#ifdef SCOTCH_DEBUG_HMESH2
  if (meshCheck (meshptr) != 0) {
    errorPrint ("hmeshMesh: internal error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_HMESH2 */

  return (0);
}
