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
/**   NAME       : graph_base.c                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the graph base     **/
/**                changing routine.                       **/
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

/* This routine sets the base of the given
** graph to the given base value, and returns
** the old base value.
** It returns:
** - old base value : in all cases.
*/

Gnum
graphBase (
Graph * const               grafptr,
const Gnum                  baseval)
{
  Gnum                baseold;                    /* Old base value  */
  Gnum                baseadj;                    /* Base adjustment */
  Gnum                vertnum;
  Gnum                edgenum;

  if (grafptr->baseval == baseval)                /* If nothing to do */
    return (baseval);

  baseold = grafptr->baseval;                     /* Record old base value */
  baseadj = baseval - baseold;                    /* Compute adjustment    */

  for (vertnum = grafptr->baseval; vertnum < grafptr->vertnnd; vertnum ++) {
    for (edgenum = grafptr->verttax[vertnum]; edgenum < grafptr->vendtax[vertnum]; edgenum ++)
      grafptr->edgetax[edgenum] += baseadj;
    grafptr->verttax[vertnum] += baseadj;
  }
  if (grafptr->vendtax != grafptr->verttax + 1) { /* If distinct vertex end array */
    for (vertnum = grafptr->baseval; vertnum < grafptr->vertnnd; vertnum ++)
      grafptr->vendtax[vertnum] += baseadj;
  }
  else                                            /* If same vertex end array (of size +1) */
    grafptr->verttax[grafptr->vertnnd] += baseadj; /* Adjust last entry of verttax         */

  grafptr->verttax -= baseadj;                    /* Adjust array accesses */
  grafptr->vendtax -= baseadj;
  grafptr->edgetax -= baseadj;

  if (grafptr->vnumtax != NULL)
    grafptr->vnumtax -= baseadj;
  if (grafptr->vlbltax != NULL)
    grafptr->vlbltax -= baseadj;
  if (grafptr->edlotax != NULL)
    grafptr->edlotax -= baseadj;

  grafptr->baseval  = baseval;                    /* Set new base value */
  grafptr->vertnnd += baseadj;

  return (baseold);                               /* Return old base value */
}
