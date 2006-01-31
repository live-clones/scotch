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
/**   NAME       : graph_grid_tetra2.c                     **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These are the graph-making routines for **/
/**                the 2D non-parallepipedic tetrahedral-  **/
/**                filled grids.                           **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 15 oct 2000     **/
/**                                 to     15 oct 2000     **/
/**                # Version 1.0  : from : 03 jun 2002     **/
/**                                 to     03 jun 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH_GRID_TETRA2

#include "common.h"
#include "graph.h"
#include "graph_grid_tetra2.h"

/**************************************/
/*                                    */
/* Routines for 2D triangular grids   */
/* with complete upppermost Y layers. */
/*                                    */
/**************************************/

/*+ This routine computes the parameters
*** of the 2D triangular grid with complete
*** uppermost Y layers whose characteristics
*** are given as parameters.
*** The number of arcs is computed as such:
*** - complete top: 2 [arcs] *
***                   (2 * x) * (2 * x - 1) / 2 [complete]
*** - horizontal: 2 [arcs] *
***                 (y - 2) [layers] * (x - 1) [grid edges]
*** - vertical: 2 [arcs] *
***               x [layers] * (y - 2) [grid edges]
*** - diagonal: 2 [arcs] *
***               (x - 1) * (y - 2) [up-right]
*** for an overall total of:
***   2 * (x * (2 * x - 1) + (y - 2) * (3 * x - 2))
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

INT
graphGrid2CDegr (
GraphGridTetra2Nghb * const nghbptr,
const INT                   vertnum)
{
  INT                 verttmp;
  INT                 ynum;

  verttmp = vertnum - nghbptr->baseval;
  ynum    = verttmp / nghbptr->xnbr;

  if (ynum == (nghbptr->ynbr - 1))                /* Last layer */
    return (2 * nghbptr->xnbr - 1);
  if (nghbptr->ynum == (nghbptr->ynbr - 2))       /* Before-last layer */
    return (2 * nghbptr->xnbr + 1);
  return (6);                                     /* Other layers */
}

INT
graphGrid2CFrst (
GraphGridTetra2Nghb * const nghbptr,
const INT                   vertnum)
{
  INT                 verttmp;

  verttmp       = vertnum - nghbptr->baseval;
  nghbptr->xnum = verttmp % nghbptr->xnbr;
  nghbptr->ynum = verttmp / nghbptr->xnbr;

  if (nghbptr->ynum == (nghbptr->ynbr - 1))       /* If belongs to last layer        */
    nghbptr->vertval = 9;                         /* Directly enter clique loops     */
  else if (nghbptr->ynum == (nghbptr->ynbr - 2))  /* If belongs to before-last layer */
    nghbptr->vertval = 7;
  else if (nghbptr->ynum == 0)                    /* If belongs to first layer */
    nghbptr->vertval = 2;
  else                                            /* If belongs to regular layers */
    nghbptr->vertval = 0;

  return (graphGrid2CNext (nghbptr));             /* Compute next valid neighbor */
}

INT
graphGrid2CNext (
GraphGridTetra2Nghb * const nghbptr)
{
  INT                 xpos;
  INT                 ypos;

  switch (nghbptr->vertval) {                     /* Avoids writing gotos           */
    case 0 :                                      /* Regular vertex: first neighbor */
      nghbptr->vertval = 1;
      xpos = nghbptr->xnum - 1;
      ypos = nghbptr->ynum - 1;
      if (xpos >= 0)
        break;
    case 1 :                                      /* Regular vertex: current is (-1,-1) */
      nghbptr->vertval = 2;
      xpos = nghbptr->xnum;
      ypos = nghbptr->ynum - 1;
      break;
    case 2 :                                      /* Regular vertex: first neighbor */
      nghbptr->vertval = 3;
      xpos = nghbptr->xnum - 1;
      ypos = nghbptr->ynum;
      if (xpos >= 0)
        break;
    case 3 :                                      /* Regular vertex: current is (-1,0) */
      nghbptr->vertval = 4;
      xpos = nghbptr->xnum + 1;
      ypos = nghbptr->ynum;
      if (xpos < nghbptr->xnbr)
        break;
    case 4 :                                      /* Regular vertex: current is (+1,0) */
      nghbptr->vertval = 5;
      xpos = nghbptr->xnum;
      ypos = nghbptr->ynum + 1;
      break;
    case 5 :                                      /* Regular vertex: current is (0,+1) */
      nghbptr->vertval = 6;
      xpos = nghbptr->xnum + 1;
      ypos = nghbptr->ynum + 1;
      if (xpos < nghbptr->xnbr)
        break;
    case 6 :                                      /* End of neighbors for regular vertex */
      return (nghbptr->baseval - 1);

    case 7 :                                      /* Before-last layer: first neighbor */
      nghbptr->vertval = 8;
      xpos = nghbptr->xnum - 1;
      ypos = nghbptr->ynum - 1;
      if (xpos >= 0)
        break;
    case 8 :                                      /* Before-last layer: current is (-1,-1) */
      nghbptr->vertval = 9;
      xpos = nghbptr->xnum;
      ypos = nghbptr->ynum - 1;
      break;
    case 9 :                                      /* Initialization of clique loop                   */
      nghbptr->vertval = 10;                      /* Go to clique loop next time                     */
      nghbptr->xpos    = -1;                      /* Pre-compute first neighbor in before-last layer */
      nghbptr->ypos    = nghbptr->ynbr - 2;
    case 10 :                                     /* Clique layers before vertex */
      if (++ nghbptr->xpos >= nghbptr->xnbr) {
        nghbptr->xpos = 0;
        nghbptr->ypos ++;
      }
      if ((nghbptr->xpos != nghbptr->xnum) ||     /* Skip loop edge */
          (nghbptr->ypos != nghbptr->ynum)) {
        xpos = nghbptr->xpos;
        ypos = nghbptr->ypos;
        break;
      }
      nghbptr->vertval = 11;
    case 11 :                                     /* Clique layers after vertex */
      if (++ nghbptr->xpos >= nghbptr->xnbr) {
        nghbptr->xpos = 0;
        if (++ nghbptr->ypos >= nghbptr->ynbr)
          return (nghbptr->baseval - 1);
      }
      xpos = nghbptr->xpos;
      ypos = nghbptr->ypos;
      break;
  }

  return (ypos * nghbptr->xnbr + xpos + nghbptr->baseval);
}

int
graphGrid2CSize (
INT * const                 vertptr,
INT * const                 edgeptr,
INT * const                 degrptr,
const INT                   xnbr,
const INT                   ynbr)
{
  if ((xnbr < 3) || (ynbr < 3)) {
    errorPrint ("graphGrid2CSize: dimension values must both be greater than 2");
    return     (1);
  }

  *vertptr = xnbr * ynbr;                         /* Compute grid parameters */
  *edgeptr = 2 * ((2 * xnbr - 1) * xnbr +
                  (ynbr - 2) * (3 * xnbr - 2));
  *degrptr = (xnbr * 2) + 1;                      /* For a vertex of the before-last layer */

  return (0);
}

/*+ This routine builds the 2D triangular
*** grid with complete uppermost Y layers
*** whose characteristics are given as
*** parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
graphBuildGrid2C (
Graph * const               grafptr,              /*+ Graph to build +*/
const INT                   xnbr,                 /*+ X dimension    +*/
const INT                   ynbr,                 /*+ Y dimension    +*/
const INT                   baseval)              /*+ Base value     +*/
{
  GraphGridTetra2Nghb         nghbdat;            /* Neighbor data */
  INT                         vertnbr;
  INT                         edgenbr;
  INT                         degrmax;

  if (graphGrid2CSize (&vertnbr, &edgenbr, &degrmax, /* Get graph parameters */
                       xnbr, ynbr) != 0) {
    errorPrint ("graphBuildGrid2C: invalid parameters");
    return     (1);
  }

  nghbdat.baseval = baseval;                      /* Initialize neighbor structure */
  nghbdat.xnbr    = xnbr;
  nghbdat.ynbr    = ynbr;

  return (graphBuild (                            /* Build graph */
            grafptr,
            vertnbr,
            edgenbr,
            degrmax,
            baseval,
            (void *) &nghbdat,
            (int (*) (void * const, const INT)) graphGrid2CFrst,
            (int (*) (void * const))            graphGrid2CNext));
}
