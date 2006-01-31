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
/**   NAME       : hmesh.c                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the halo source     **/
/**                mesh functions.                         **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 12 sep 2002     **/
/**                                 to     10 feb 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "hmesh.h"

/****************************************/
/*                                      */
/* These routines handle source meshes. */
/*                                      */
/****************************************/

/* This routine frees a source halo mesh structure.
** It returns:
** - VOID  : in all cases.
*/

void
hmeshExit (
Hmesh * const               meshptr)
{
  if ((meshptr->vehdtax != NULL) &&               /* Exit halo mesh data */
      (meshptr->vehdtax != (meshptr->m.vendtax + (meshptr->m.baseval - meshptr->m.velmbas))) &&
      ((meshptr->m.flagval & MESHVERTGROUP) == 0))
    memFree (meshptr->vehdtax + meshptr->m.velmbas);
  meshExit (&meshptr->m);                         /* Exit mesh data */

#ifdef SCOTCH_DEBUG_HMESH2
  memSet (meshptr, ~0, sizeof (Hmesh));           /* Purge halo mesh fields */
#endif /* SCOTCH_DEBUG_HMESH2 */
}

/* This routine sets the base of the given
** halo mesh to the given base value, and
** returns the old base value.
** It returns:
** - old base value : in all cases.
*/

Gnum
hmeshBase (
Hmesh * const               meshptr,
const Gnum                  baseval)
{
  Gnum                baseold;                    /* Old base value  */
  Gnum                baseadj;                    /* Base adjustment */
  Gnum                velmnum;

  if (meshptr->m.baseval == baseval)              /* If nothing to do */
    return (baseval);

  baseold = meshptr->m.baseval;                   /* Record old base value */
  baseadj = baseval - baseold;                    /* Compute adjustment    */

  meshBase (&meshptr->m, baseval);                /* Change base of mesh */

  for (velmnum = meshptr->m.velmbas; velmnum < meshptr->m.velmnnd; velmnum ++)
    meshptr->vehdtax[velmnum] += baseadj;         /* Change base of array */

  meshptr->vnohnnd += baseadj;
  meshptr->vehdtax -= baseadj;

  return (baseold);                               /* Return old base value */
}
