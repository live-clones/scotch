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
/**   NAME       : graph_grid_tetra3.c                     **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These are the graph-making routines for **/
/**                the 3D non-parallepipedic tetrahedral-  **/
/**                filled grids.                           **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 16 oct 2000     **/
/**                                 to     16 oct 2000     **/
/**                # Version 1.0  : from : 03 jun 2002     **/
/**                                 to     03 jun 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH_GRID_TETRA3

#include "common.h"
#include "graph.h"
#include "graph_grid_tetra3.h"

/**************************************/
/*                                    */
/* Routines for 3D triangular grids   */
/* with complete upppermost Y layers. */
/*                                    */
/**************************************/

/*+ This routine computes the parameters
*** of the 3D triangular grid with complete
*** uppermost Y layers whose characteristics
*** are given as parameters.
*** All the arcs within a given brick are
*** listed below:
***   (1,2) (1,3) (1,4) (1,5) (1,6) (1,8)
***   (2,1) (2,3) (2,6)
***   (3,1) (3,2) (3,4) (3,6) (3,7) (3,8)
***   (4,1) (4,3) (4,8)
***   (5,1) (5,6) (5,8)
***   (6,1) (6,2) (6,3) (6,5) (6,7) (6,8)
***   (7,3) (7,6) (7,8)
***   (8,1) (8,3) (8,4) (8,5) (8,6) (8,7)
*** Placing the brick in the 8 quadrant
*** positions available yields the
*** following displacements:
*** center at vertex 1:
***   2 ( 1,  0,  0)
***   3 ( 1,  1,  0)
***   4 ( 0,  1,  0)
***   5 ( 0,  0,  1)
***   6 ( 1,  0,  1)
***   8 ( 0,  1,  1)
*** center at vertex 2:
***   1 (-1,  0,  0)
***   3 ( 0,  1,  0)
***   4 (-1,  1,  0)
***   5 (-1,  0,  1)
***   7 ( 0,  1,  1)
*** center at vertex 3:
***   1 (-1, -1,  0)
***   2 ( 0, -1,  0)
***   4 (-1,  0,  0)
***   6 ( 0, -1,  1)
***   7 ( 0,  0,  1)
***   8 (-1,  0,  1)
*** center at vertex 4:
***   1 ( 0, -1,  0)
***   3 ( 1,  0,  0)
***   8 ( 0,  0,  1)
*** center at vertex 5:
***   1 ( 0,  0, -1)
***   6 ( 1,  0,  0)
***   8 ( 0,  1,  0)
*** center at vertex 6:
***   1 (-1,  0, -1)
***   2 ( 0,  0, -1)
***   3 ( 0,  1, -1)
***   5 (-1,  0,  0)
***   7 ( 0,  1,  0)
***   8 (-1,  1,  0)
*** center at vertex 7:
***   3 ( 0,  0, -1)
***   6 ( 0, -1,  0)
***   8 (-1,  0,  0)
*** center at vertex 8:
***   1 ( 0, -1, -1)
***   3 ( 1,  0, -1)
***   4 ( 0,  0, -1)
***   5 ( 0, -1,  0)
***   6 ( 1, -1,  0)
***   7 ( 1,  0,  0)
*** Merging and removing duplicates
*** yields the following list, sorted
*** by increasing Z displacement:
***   ( 0,  0, -1)
***   ( 0,  1, -1)
***   ( 0, -1, -1)
***   ( 1,  0, -1)
***   (-1,  0, -1)
***   (-1, -1,  0)
***   ( 0, -1,  0)
***   ( 1, -1,  0)
***   (-1,  0,  0)
***   ( 1,  0,  0)
***   (-1,  1,  0)
***   ( 0,  1,  0)
***   ( 1,  1,  0)
***   ( 0, -1,  1)
***   (-1,  0,  1)
***   ( 0,  0,  1)
***   ( 1,  0,  1)
***   ( 0,  1,  1)
*** for an overall total of 18 arcs per regular vertex.
*** The number of arcs is upper-bounded as such:
*** - complete top: 2 [arcs] *
***                   (2 * x * y) * (2 * x * y - 1) / 2 [complete]
*** - first layer: (5 + 8) * x * y [upper bound]
*** - before-last layer, to previous: 5 * x * y [upper bound]
*** - normal layers: 18 * (z - 3) * x * y [upper bound]
*** The over-all total number of arcs is thus upper-bounded as:
***   2 * (x * y) * ((2 * x * y - 1) + 9 * (z - 2))
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

INT
graphGrid3CDegr (
GraphGridTetra3Nghb * const nghbptr,
const INT                   vertnum)
{
  INT                 verttmp;
  INT                 znum;

  verttmp = vertnum - nghbptr->baseval;
  verttmp = verttmp / nghbptr->xnbr;
  znum    = verttmp / nghbptr->ynbr;

  if (znum == (nghbptr->znbr - 1))                /* Last layer */
    return (nghbptr->xnbr * nghbptr->ynbr * 2 - 1);
  if (znum == (nghbptr->znbr - 2))                /* Before-last layer */
    return (nghbptr->xnbr * nghbptr->ynbr * 2 + 4);
  return (18);
}

INT
graphGrid3CFrst (
GraphGridTetra3Nghb * const nghbptr,
INT                         vertnum)
{
  INT                 verttmp;

  verttmp       = vertnum - nghbptr->baseval;
  nghbptr->xnum = verttmp % nghbptr->xnbr;
  verttmp       = verttmp / nghbptr->xnbr;
  nghbptr->ynum = verttmp % nghbptr->ynbr;
  nghbptr->znum = verttmp / nghbptr->ynbr;

  if (nghbptr->znum == (nghbptr->znbr - 1))       /* If belongs to last layer         */
    nghbptr->vertval = 4;                         /* Directly enter clique loops      */
  else if (nghbptr->znum == (nghbptr->znbr - 2)) { /* If belongs to before-last layer */
    nghbptr->vertval = 3;
    nghbptr->vertidx = -1;
  }
  else if (nghbptr->znum == 0) {                  /* If belongs to first layer */
    nghbptr->vertval = 1;
    nghbptr->vertidx = 4;
  }
  else {                                          /* If belongs to regular layers */
    nghbptr->vertval = 0;
    nghbptr->vertidx = -1;
  }

  return (graphGrid3CNext (nghbptr));             /* Compute next valid neighbor */
}

static int                  graphGrid3CNghbTab[][2] = { {  0,  0 /*, -1 */},
                                                        {  0,  1 /*, -1 */},
                                                        {  0, -1 /*, -1 */},
                                                        {  1,  0 /*, -1 */},
                                                        { -1,  0 /*, -1 */},
                                                        { -1, -1 /*,  0 */},
                                                        {  0, -1 /*,  0 */},
                                                        {  1, -1 /*,  0 */},
                                                        { -1,  0 /*,  0 */},
                                                        {  1,  0 /*,  0 */},
                                                        { -1,  1 /*,  0 */},
                                                        {  0,  1 /*,  0 */},
                                                        {  1,  1 /*,  0 */},
                                                        {  0, -1 /*,  1 */},
                                                        { -1,  0 /*,  1 */},
                                                        {  0,  0 /*,  1 */},
                                                        {  1,  0 /*,  1 */},
                                                        {  0,  1 /*,  1 */} };


INT
graphGrid3CNext (
GraphGridTetra3Nghb * const nghbptr)
{
  INT                 xpos;
  INT                 ypos;
  INT                 zpos;

  switch (nghbptr->vertval) {                     /* Avoids writing gotos  */
    case 0 :                                      /* Regular vertex: dz=-1 */
      zpos = nghbptr->znum - 1;
      while (++ nghbptr->vertidx < 5) {
        xpos = nghbptr->xnum + graphGrid3CNghbTab[nghbptr->vertidx][0];
        if ((xpos < 0) || (xpos >= nghbptr->xnbr))
          continue;
        ypos = nghbptr->ynum + graphGrid3CNghbTab[nghbptr->vertidx][1];
        if ((ypos >= 0) && (ypos < nghbptr->ynbr))
          goto endswitch;
      }
      nghbptr->vertval = 1;
    case 1 :                                      /* Regular vertex: dz=0 */
      zpos = nghbptr->znum;
      while (++ nghbptr->vertidx < 13) {
        xpos = nghbptr->xnum + graphGrid3CNghbTab[nghbptr->vertidx][0];
        if ((xpos < 0) || (xpos >= nghbptr->xnbr))
          continue;
        ypos = nghbptr->ynum + graphGrid3CNghbTab[nghbptr->vertidx][1];
        if ((ypos >= 0) && (ypos < nghbptr->ynbr))
          goto endswitch;
      }
      nghbptr->vertval = 2;
    case 2 :                                      /* Regular vertex: dz=+1 */
      zpos = nghbptr->znum + 1;
      while (++ nghbptr->vertidx < 18) {
        xpos = nghbptr->xnum + graphGrid3CNghbTab[nghbptr->vertidx][0];
        if ((xpos < 0) || (xpos >= nghbptr->xnbr))
          continue;
        ypos = nghbptr->ynum + graphGrid3CNghbTab[nghbptr->vertidx][1];
        if ((ypos >= 0) && (ypos < nghbptr->ynbr))
          goto endswitch;
      }
      return (nghbptr->baseval - 1);
    case 3 :                                      /* Before-last layer: dz=-1 */
      zpos = nghbptr->znum - 1;
      while (++ nghbptr->vertidx < 5) {
        xpos = nghbptr->xnum + graphGrid3CNghbTab[nghbptr->vertidx][0];
        if ((xpos < 0) || (xpos >= nghbptr->xnbr))
          continue;
        ypos = nghbptr->ynum + graphGrid3CNghbTab[nghbptr->vertidx][1];
        if ((ypos >= 0) && (ypos < nghbptr->ynbr))
          goto endswitch;
      }
    case 4 :                                      /* Preparation to clique layers before vertex      */
      nghbptr->vertval = 5;                       /* Go to clique loop next time                     */
      nghbptr->xpos    = -1;                      /* Pre-compute first neighbor in before-last layer */
      nghbptr->ypos    = 0;
      nghbptr->zpos    = nghbptr->znbr - 2;
    case 5 :                                      /* Clique layers before vertex */
      if (++ nghbptr->xpos >= nghbptr->xnbr) {
        nghbptr->xpos = 0;
        if (++ nghbptr->ypos >= nghbptr->ynbr) {
          nghbptr->ypos = 0;
          nghbptr->zpos ++;
        }
      }
      if ((nghbptr->xpos != nghbptr->xnum) ||     /* Skip loop edge */
          (nghbptr->ypos != nghbptr->ynum) ||
          (nghbptr->zpos != nghbptr->znum)) {
        xpos = nghbptr->xpos;
        ypos = nghbptr->ypos;
        zpos = nghbptr->zpos;
        break;
      }
      nghbptr->vertval = 6;
    case 6 :                                      /* Clique layers after vertex */
      if (++ nghbptr->xpos >= nghbptr->xnbr) {
        nghbptr->xpos = 0;
        if (++ nghbptr->ypos >= nghbptr->ynbr) {
          nghbptr->ypos = 0;
          if (++ nghbptr->zpos >= nghbptr->znbr)
            return (nghbptr->baseval - 1);
        }
      }
      xpos = nghbptr->xpos;
      ypos = nghbptr->ypos;
      zpos = nghbptr->zpos;
      break;
  }

endswitch:
  return ((zpos * nghbptr->ynbr + ypos) * nghbptr->xnbr + xpos + nghbptr->baseval);
}

int
graphGrid3CSize (
INT * const                 vertptr,
INT * const                 edgeptr,
INT * const                 degrptr,
const INT                   xnbr,
const INT                   ynbr,
const INT                   znbr)
{
  if ((xnbr < 3) || (ynbr < 3) || (znbr < 3)) {
    errorPrint ("graphGrid3CSize: dimension values must both be greater than 2");
    return     (1);
  }

  *vertptr = xnbr * ynbr * znbr;                  /* Compute grid parameters */
  *edgeptr = 2 * (xnbr * ynbr) *
             ((2 * xnbr * ynbr - 1) + 9 * (znbr - 2));
  *degrptr = (xnbr * ynbr * 2) + 4;               /* For a vertex of the before-last layer */

  return (0);
}

/*+ This routine builds the 3D triangular
*** grid with complete uppermost Y layers
*** whose characteristics are given as
*** parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
graphBuildGrid3C (
Graph * const               grafptr,              /*+ Graph to build +*/
const INT                   xnbr,                 /*+ X dimension    +*/
const INT                   ynbr,                 /*+ Y dimension    +*/
const INT                   znbr,                 /*+ Z dimension    +*/
const INT                   baseval)              /*+ Base value     +*/
{
  GraphGridTetra3Nghb         nghbdat;            /* Neighbor data */
  INT                         vertnbr;
  INT                         edgenbr;
  INT                         degrmax;

  if (graphGrid3CSize (&vertnbr, &edgenbr, &degrmax, /* Get graph parameters */
                       xnbr, ynbr, znbr) != 0) {
    errorPrint ("graphBuildGrid3C: invalid parameters");
    return     (1);
  }

  nghbdat.baseval = baseval;                      /* Initialize neighbor structure */
  nghbdat.xnbr    = xnbr;
  nghbdat.ynbr    = ynbr;
  nghbdat.znbr    = znbr;

  return (graphBuild (                            /* Build graph */
            grafptr,
            vertnbr,
            edgenbr,
            degrmax,
            baseval,
            (void *) &nghbdat,
            (int (*) (void * const, const INT)) graphGrid3CFrst,
            (int (*) (void * const))            graphGrid3CNext));
}
