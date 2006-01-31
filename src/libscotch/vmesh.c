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
/**   NAME       : vmesh.c                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the separator      **/
/**                handling routines.                      **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 06 feb 2003     **/
/**                                 to     05 mar 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VMESH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "vmesh.h"

/*************************/
/*                       */
/* These routines handle */
/* separator meshes.     */
/*                       */
/*************************/

/* This routine frees the contents
** of the given active mesh.
** It returns:
** - VOID  : in all cases.
*/

void
vmeshExit (
Vmesh * const               meshptr)
{
  if (meshptr->parttax != NULL)                   /* Free leader of group (parttab + frontab) */
    memFree (meshptr->parttax + meshptr->m.baseval);

  meshFree (&meshptr->m);                         /* Free source mesh */

#ifdef SCOTCH_DEBUG_VMESH2
  memSet (meshptr, ~0, sizeof (Vmesh));
#endif /* SCOTCH_DEBUG_VMESH2 */
}

/* This routine moves all of the mesh
** elements to the first part.
** It returns:
** - VOID  : in all cases.
*/

void
vmeshZero (
Vmesh * const               meshptr)
{
  memSet (meshptr->parttax + meshptr->m.baseval, 0, (meshptr->m.velmnbr + meshptr->m.vnodnbr) * sizeof (GraphPart));

  meshptr->ecmpsize[0] = meshptr->m.velmnbr;
  meshptr->ecmpsize[1] = 0;
  meshptr->ncmpload[0] = meshptr->m.vnlosum;
  meshptr->ncmpload[1] =
  meshptr->ncmpload[2] = 0;
  meshptr->ncmploaddlt = meshptr->m.vnlosum;
  meshptr->ncmpsize[0] = meshptr->m.vnodnbr;
  meshptr->ncmpsize[1] =
  meshptr->ncmpsize[2] = 0;                       /* TRICK: frontier set here */
}
