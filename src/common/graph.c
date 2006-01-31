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
/**   NAME       : graph.c                                 **/
/**                                                        **/
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the general purpose     **/
/**                routines for the graph structure.       **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 24 aug 1998     **/
/**                                 to     24 sep 1998     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH

#include "common.h"
#include "graph.h"

/********************************/
/*                              */
/* The graph handling routines. */
/*                              */
/********************************/

/* This routine initializes the given
** graph structure.
** It returns:
** - 0  : in all cases.
*/

int
graphInit (
Graph * const               grafptr)              /*+ Graph structure to initialize +*/
{
  memSet (grafptr, 0, sizeof (Graph));

  return (0);
}

/* This routine frees the contents
** of the given graph.
** It returns:
** - VOID  : in all cases.
*/

void
graphExit (
Graph * const               grafptr)              /*+ Graph structure to free +*/
{
  if (grafptr->verttab != NULL)
    memFree (grafptr->verttab);
  if (grafptr->velotab != NULL)
    memFree (grafptr->velotab);
  if (grafptr->vlbltab != NULL)
    memFree (grafptr->vlbltab);
  if (grafptr->edgetab != NULL)
    memFree (grafptr->edgetab);
}
