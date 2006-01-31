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
/**   NAME       : mesh.c                                  **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the general purpose     **/
/**                routines for the mesh structure.        **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 19 oct 2003     **/
/**                                 to     24 oct 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MESH

#include "common.h"
#include "mesh.h"

/*******************************/
/*                             */
/* The mesh handling routines. */
/*                             */
/*******************************/

/* This routine initializes the given
** mesh structure.
** It returns:
** - 0  : in all cases.
*/

int
meshInit (
Mesh * restrict const       meshptr)              /*+ Mesh structure to initialize +*/
{
  memSet (meshptr, 0, sizeof (Mesh));

  return (0);
}

/* This routine frees the contents
** of the given graph.
** It returns:
** - VOID  : in all cases.
*/

void
meshExit (
Mesh * restrict const       meshptr)              /*+ Mesh structure to free +*/
{
  if (meshptr->verttab != NULL)
    memFree (meshptr->verttab);
  if (meshptr->velotab != NULL)
    memFree (meshptr->velotab);
  if (meshptr->vlbltab != NULL)
    memFree (meshptr->vlbltab);
  if (meshptr->edgetab != NULL)
    memFree (meshptr->edgetab);
}

/* This routine frees the contents
** of the given graph.
** It returns:
** - VOID  : in all cases.
*/

void
meshFree (
Mesh * restrict const       meshptr)              /*+ Mesh structure to free +*/
{
  meshExit (meshptr);

  memSet (meshptr, 0, sizeof (Mesh));
}
