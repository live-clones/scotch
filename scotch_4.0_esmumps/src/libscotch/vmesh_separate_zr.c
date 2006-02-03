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
/**   NAME       : vgraph_separate_zr.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module moves all of the vertices   **/
/**                to the first subdomain.                 **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 10 sep 2002     **/
/**                                 to     29 may 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VMESH_SEPARATE_ZR

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "vmesh.h"
#include "vmesh_separate_zr.h"

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine moves all of the mesh nodes
** and elements to the first part of the
** partition.
** It returns:
** - 0   : if the bipartitioning could be computed.
** - !0  : on error.
*/

int
vmeshSeparateZr (
Vmesh * restrict const      meshptr)
{
  if (meshptr->ncmpload[0] != meshptr->m.vnlosum) /* If not all vertices already in part zero */
    vmeshZero (meshptr);

  return (0);
}
