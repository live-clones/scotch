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
/**   NAME       : vgraph.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the separator      **/
/**                handling routines.                      **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 24 aug 1996     **/
/**                                 to     03 nov 1997     **/
/**                # Version 4.0  : from : 12 dec 2001     **/
/**                                 to     08 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VGRAPH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "vgraph.h"

/*************************/
/*                       */
/* These routines handle */
/* separator graphs.     */
/*                       */
/*************************/

/* This routine frees the contents
** of the given active graph.
** It returns:
** - VOID  : in all cases.
*/

void
vgraphExit (
Vgraph * const              grafptr)
{
  if (grafptr->parttax != NULL)
    memFree (grafptr->parttax + grafptr->s.baseval);
  if (grafptr->frontab != NULL)
    memFree (grafptr->frontab);

  graphFree (&grafptr->s);                        /* Free source graph */

#ifdef SCOTCH_DEBUG_VGRAPH2
  memSet (grafptr, ~0, sizeof (Vgraph));
#endif /* SCOTCH_DEBUG_VGRAPH2 */
}

/* This routine moves all of the graph
** vertices to the first part.
** It returns:
** - VOID  : in all cases.
*/

void
vgraphZero (
Vgraph * const              grafptr)
{
  memSet (grafptr->parttax + grafptr->s.baseval, 0, grafptr->s.vertnbr * sizeof (GraphPart)); /* Set all vertices to part 0 */

  grafptr->compload[0] = grafptr->s.velosum;      /* No frontier vertices */
  grafptr->compload[1] =
  grafptr->compload[2] = 0;
  grafptr->comploaddlt = grafptr->s.velosum;
  grafptr->compsize[0] = grafptr->s.vertnbr;
  grafptr->compsize[1] =
  grafptr->fronnbr     = 0;
}
