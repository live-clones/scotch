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
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the source graph    **/
/**                functions.                              **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 01 dec 1992     **/
/**                                 to     18 may 1993     **/
/**                # Version 1.3  : from : 30 apr 1994     **/
/**                                 to     18 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     31 oct 1994     **/
/**                # Version 3.0  : from : 07 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 28 nov 1995     **/
/**                                 to     08 jun 1996     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     15 sep 1998     **/
/**                # Version 3.3  : from : 22 sep 1998     **/
/**                                 to     31 dec 1998     **/
/**                # Version 4.0  : from : 24 nov 2001     **/
/**                                 to     22 apr 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH

#include "module.h"
#include "common.h"
#include "graph.h"

/****************************************/
/*                                      */
/* These routines handle source graphs. */
/*                                      */
/****************************************/

/* This routine initializes a source graph
** structure.
** It returns:
** - 0  : in all cases.
*/

int
graphInit (
Graph * const               grafptr)
{
  memSet (grafptr, 0, sizeof (Graph));            /* Initialize graph fields     */
  grafptr->flagval = GRAPHFREETABS;               /* By default, free all arrays */

  return (0);
}

/* This routine frees a source graph structure.
** It returns:
** - VOID  : in all cases.
*/

void
graphExit (
Graph * const               grafptr)
{
  graphFree (grafptr);                            /* Free graph data */

#ifdef SCOTCH_DEBUG_GRAPH2
  memSet (grafptr, ~0, sizeof (Graph));           /* Purge graph fields */
#endif /* SCOTCH_DEBUG_GRAPH2 */
}

/* This routine frees the graph data.
** It returns:
** - VOID  : in all cases.
*/

void
graphFree (
Graph * const               grafptr)
{
  if (((grafptr->flagval & GRAPHFREEEDGE) != 0) && /* If edgetab must be freed */
      (grafptr->edgetax != NULL))                 /* And if it exists          */
    memFree (grafptr->edgetax + grafptr->baseval); /* Free it                  */

  if ((grafptr->flagval & GRAPHFREEVERT) != 0) {  /* If verttab/vendtab must be freed                            */
    if ((grafptr->vendtax != NULL) &&             /* If vendtax is distinct from verttab                         */
        (grafptr->vendtax != grafptr->verttax + 1) && /* (if vertex arrays grouped, vendtab not distinct anyway) */
        ((grafptr->flagval & GRAPHVERTGROUP) == 0))
      memFree (grafptr->vendtax + grafptr->baseval); /* Then free vendtax                                 */
    if (grafptr->verttax != NULL)                 /* Free verttab anyway, as it is the array group leader */
      memFree (grafptr->verttax + grafptr->baseval);
  }
  if ((grafptr->flagval & GRAPHFREEVNUM) != 0) {  /* If vnumtab must be freed         */
    if ((grafptr->vnumtax != NULL) &&             /* And is not in vertex array group */
        ((grafptr->flagval & GRAPHVERTGROUP) == 0))
      memFree (grafptr->vnumtax + grafptr->baseval);
  }
  if ((grafptr->flagval & GRAPHFREEOTHR) != 0) {  /* If other arrays must be freed */
    if ((grafptr->velotax != NULL) &&             /* Free graph tables             */
        ((grafptr->flagval & GRAPHVERTGROUP) == 0))
      memFree (grafptr->velotax + grafptr->baseval);
    if (grafptr->vlbltax != NULL)
      memFree (grafptr->vlbltax + grafptr->baseval);
    if ((grafptr->edlotax != NULL) && 
        ((grafptr->flagval & GRAPHEDGEGROUP) == 0))
      memFree (grafptr->edlotax + grafptr->baseval);
  }

#ifdef SCOTCH_DEBUG_GRAPH2
  memSet (grafptr, ~0, sizeof (Graph));           /* Purge graph fields */
#endif /* SCOTCH_DEBUG_GRAPH2 */
}
