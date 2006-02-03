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
/**   NAME       : vmesh_store.c                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the save data      **/
/**                structure handling routines for node    **/
/**                separation meshes.                      **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 10 sep 2002     **/
/**                                 to   : 10 sep 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VMESH_STORE

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "vmesh.h"

/*********************************/
/*                               */
/* Store mesh handling routines. */
/*                               */
/*********************************/

/* This routine builds a save structure
** for the given node separation mesh.
** It returns:
** - 0   : if allocation succeeded.
** - !0  : on error.
*/

int
vmeshStoreInit (
const Vmesh * const         meshptr,
VmeshStore * const          storptr)
{
  Gnum                savsize;

  savsize = (meshptr->m.velmnbr + meshptr->m.vnodnbr) * (sizeof (GraphPart) + sizeof (Gnum)); /* Compute size for frontier and part arrays */

  if ((storptr->datatab = (byte *) memAlloc (savsize)) == NULL) { /* Allocate save structure */
    errorPrint ("vmeshStoreInit: out of memory");
    return     (1);
  }

  return (0);
}

/* This routine frees a mesh node
** separation save structure.
** It returns:
** - VOID  : in all cases.
*/

void
vmeshStoreExit (
VmeshStore * const          storptr)
{
  memFree (storptr->datatab);
#ifdef SCOTCH_DEBUG_VMESH2
  storptr->datatab = NULL;
#endif /* SCOTCH_DEBUG_VMESH2 */
}

/* This routine saves partition data from the
** given node separation mesh to the given
** save structure.
** It returns:
** - VOID  : in all cases.
*/

void
vmeshStoreSave (
const Vmesh * const         meshptr,
VmeshStore * const          storptr)
{
  byte *              parttab;                    /* Pointer to part data save area     */
  byte *              frontab;                    /* Pointer to frontier data save area */

  storptr->ecmpsize[0] = meshptr->ecmpsize[0];    /* Save partition parameters */
  storptr->ecmpsize[1] = meshptr->ecmpsize[1];
  storptr->ncmpload[0] = meshptr->ncmpload[0];
  storptr->ncmpload[1] = meshptr->ncmpload[1];
  storptr->ncmpload[2] = meshptr->ncmpload[2];
  storptr->ncmploaddlt = meshptr->ncmploaddlt;
  storptr->ncmpsize[0] = meshptr->ncmpsize[0];
  storptr->ncmpsize[1] = meshptr->ncmpsize[1];
  storptr->fronnbr     = meshptr->fronnbr;

  frontab = storptr->datatab;                     /* Compute data offsets within save structure */
  parttab = frontab + meshptr->fronnbr * sizeof (Gnum);

  memCpy (frontab, meshptr->frontab, meshptr->fronnbr * sizeof (Gnum));
  memCpy (parttab, meshptr->parttax + meshptr->m.baseval, (meshptr->m.velmnbr + meshptr->m.vnodnbr) * sizeof (GraphPart));
}

/* This routine updates partition data of the
** given active graph, using the given save graph.
** It returns:
** - VOID  : in all cases.
*/

void
vmeshStoreUpdt (
Vmesh * const               meshptr,
const VmeshStore * const    storptr)
{
  byte *              frontab;                    /* Pointer to frontier data save area */
  byte *              parttab;                    /* Pointer to part data save area     */

  meshptr->ecmpsize[0] = storptr->ecmpsize[0];    /* Load partition parameters */
  meshptr->ecmpsize[1] = storptr->ecmpsize[1];
  meshptr->ncmpload[0] = storptr->ncmpload[0];
  meshptr->ncmpload[1] = storptr->ncmpload[1];
  meshptr->ncmpload[2] = storptr->ncmpload[2];
  meshptr->ncmploaddlt = storptr->ncmploaddlt;
  meshptr->ncmpsize[0] = storptr->ncmpsize[0];
  meshptr->ncmpsize[1] = storptr->ncmpsize[1];
  meshptr->fronnbr     = storptr->fronnbr;

  frontab = storptr->datatab;                     /* Compute data offsets within save structure */
  parttab = frontab + storptr->fronnbr * sizeof (Gnum);

  memCpy (meshptr->frontab, frontab, storptr->fronnbr * sizeof (Gnum));
  memCpy (meshptr->parttax + meshptr->m.baseval, parttab, (meshptr->m.velmnbr + meshptr->m.vnodnbr) * sizeof (GraphPart));
}
