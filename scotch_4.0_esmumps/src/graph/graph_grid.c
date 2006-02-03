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
/**   NAME       : graph_grid.c                            **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the grid-making         **/
/**                routines for the graph structure.       **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 02 sep 1998     **/
/**                                 to     22 apr 1999     **/
/**                # Version 1.0  : from : 24 sep 2002     **/
/**                                 to     25 sep 2002     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     28 feb 2004     **/
/**                                                        **/
/**   NOTES      : # Grids are numbered such that:         **/
/**                                                        **/
/**                  0-----> X+                            **/
/**                  |                                     **/
/**                  |                                     **/
/**                  V Y+                                  **/
/**                          v = y * xnbr + x              **/
/**                                                        **/
/**                                                        **/
/**                     ^ Z+                               **/
/**                    /                                   **/
/**                   /                                    **/
/**                  0-----> X+                            **/
/**                  |                                     **/
/**                  |                                     **/
/**                  V Y+                                  **/
/**                          v = (z * ynbr + y) * xnbr + x **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH_GRID

#include "common.h"
#include "scotch.h"
#include "graph.h"
#include "graph_grid.h"

/***************************************/
/*                                     */
/* 2D finite difference grid routines. */
/*                                     */
/***************************************/

/* These routines define a 2D
** finite difference graph.
*/

INT
graphGrid2DDegr (
GraphGridNghb * const       nghbptr,
const INT                   vertnum)
{
  return (4);
}

INT
graphGrid2DFrst (
GraphGridNghb * const       nghbptr,
INT                         vertnum)
{
  INT                 verttmp;

  verttmp       = vertnum - nghbptr->baseval;
  nghbptr->xnum = verttmp % nghbptr->xnbr;
  nghbptr->ynum = verttmp / nghbptr->xnbr;

  nghbptr->xpos = nghbptr->xnum;                  /* Current neighbor is (0,-1) */
  nghbptr->ypos = nghbptr->ynum - 1;
  nghbptr->xmin = 0;                              /* Serves to hold vertex index */

  return ((nghbptr->ypos < 0)                     /* If first neighbor is invalid */
          ? graphGrid2DNext (nghbptr)             /* Compute next valid neighbor  */
          : (nghbptr->ypos *                      /* Else return neighbor number  */
             nghbptr->xnbr + nghbptr->xpos + nghbptr->baseval));
}

INT
graphGrid2DNext (
GraphGridNghb * const       nghbptr)
{
  switch (nghbptr->xmin) {                        /* Avoids writing gotos */
    case 0 :                                      /* Current is (0,-1)    */
      nghbptr->xmin ++;
      nghbptr->ypos = nghbptr->ynum;
      nghbptr->xpos = nghbptr->xnum - 1;
      if (nghbptr->xpos >= 0)
        break;
    case 1 :                                      /* Current is (-1,0) */
      nghbptr->xmin ++;
      nghbptr->xpos = nghbptr->xnum + 1;
      if (nghbptr->xpos < nghbptr->xnbr)
        break;
    case 2 :                                      /* Current is (+1,0) */
      nghbptr->xmin ++;
      nghbptr->xpos = nghbptr->xnum;
      nghbptr->ypos = nghbptr->ynum + 1;
      if (nghbptr->ypos < nghbptr->ynbr)
        break;
    default :                                     /* Current is (0,+1) */
      return (nghbptr->baseval - 1);
  }

  return (nghbptr->ypos *
          nghbptr->xnbr + nghbptr->xpos + nghbptr->baseval);
}

int
graphGrid2DSize (
INT * const                 vertptr,
INT * const                 edgeptr,
INT * const                 degrptr,
const INT                   xnbr,
const INT                   ynbr)
{
  *vertptr = xnbr * ynbr;                         /* Compute grid parameters */
  *edgeptr = xnbr * ynbr * 4 - (xnbr + ynbr) * 2;
  *degrptr = 4;

  return (0);
}

/* This routine builds a finite difference graph
** in shape of a 2D grid of the given size.
** It returns:
** - 0  : in all cases.
*/

int
graphBuildGrid2D (
Graph * const               grafptr,              /*+ Graph to build +*/
const INT                   xnbr,                 /*+ X dimension    +*/
const INT                   ynbr,                 /*+ Y dimension    +*/
const INT                   baseval)              /*+ Base value     +*/
{
  GraphGridNghb               nghbdat;            /* Neighbor data */
  INT                         vertnbr;
  INT                         edgenbr;
  INT                         degrmax;

  graphGrid2DSize (&vertnbr, &edgenbr, &degrmax, xnbr, ynbr); /* Get graph parameters */

  nghbdat.baseval = baseval;                      /* Initialize neighbor structure */
  nghbdat.xnbr    = xnbr;
  nghbdat.ynbr    = ynbr;

  return (graphBuild (                            /* Build graph */
            grafptr,
            vertnbr,
            edgenbr,
            baseval,
            (void *) &nghbdat,
            (INT (*) (void * const, const INT)) graphGrid2DFrst,
            (INT (*) (void * const))            graphGrid2DNext));
}

/***************************************/
/*                                     */
/* 3D finite difference grid routines. */
/*                                     */
/***************************************/

/* These routines define a 3D
** finite difference graph.
*/

INT
graphGrid3DDegr (
GraphGridNghb * const       nghbptr,
const INT                   vertnum)
{
  return (6);
}

INT
graphGrid3DFrst (
GraphGridNghb * const       nghbptr,
INT                         vertnum)
{
  INT                 verttmp;

  verttmp       = vertnum - nghbptr->baseval;
  nghbptr->xnum = verttmp % nghbptr->xnbr;
  verttmp       = verttmp / nghbptr->xnbr;
  nghbptr->ynum = verttmp % nghbptr->ynbr;
  nghbptr->znum = verttmp / nghbptr->ynbr;

  nghbptr->xpos = nghbptr->xnum;                  /* Current neighbor is (0,0,-1) */
  nghbptr->ypos = nghbptr->ynum;
  nghbptr->zpos = nghbptr->znum - 1;
  nghbptr->xmin = 0;                              /* Serves as vertex index */

  return ((nghbptr->zpos < 0)                     /* If first neighbor is invalid */
          ? graphGrid3DNext (nghbptr)             /* Compute next valid neighbor  */
          : ((nghbptr->zpos                  *    /* Else return neighbor number  */
              nghbptr->ynbr + nghbptr->ypos) *
              nghbptr->xnbr + nghbptr->xpos + nghbptr->baseval));
}

INT
graphGrid3DNext (
GraphGridNghb * const       nghbptr)
{
  switch (nghbptr->xmin) {                        /* Avoids writing gotos */
    case 0 :                                      /* Current is (0,0,-1)  */
      nghbptr->xmin ++;
      nghbptr->zpos = nghbptr->znum;
      nghbptr->ypos = nghbptr->ynum - 1;
      if (nghbptr->ypos >= 0)
        break;
    case 1 :                                      /* Current is (0,-1,0) */
      nghbptr->xmin ++;
      nghbptr->ypos = nghbptr->ynum;
      nghbptr->xpos = nghbptr->xnum - 1;
      if (nghbptr->xpos >= 0)
        break;
    case 2 :                                      /* Current is (-1,0,0) */
      nghbptr->xmin ++;
      nghbptr->xpos = nghbptr->xnum + 1;
      if (nghbptr->xpos < nghbptr->xnbr)
        break;
    case 3 :                                      /* Current is (+1,0,0) */
      nghbptr->xmin ++;
      nghbptr->xpos = nghbptr->xnum;
      nghbptr->ypos = nghbptr->ynum + 1;
      if (nghbptr->ypos < nghbptr->ynbr)
        break;
    case 4 :                                      /* Current is (0,+1,0) */
      nghbptr->xmin ++;
      nghbptr->ypos = nghbptr->ynum;
      nghbptr->zpos = nghbptr->znum + 1;
      if (nghbptr->zpos < nghbptr->znbr)
        break;
    default :                                     /* Current is (0,0,+1) */
      return (nghbptr->baseval - 1);
  }

  return ((nghbptr->zpos                  *
           nghbptr->ynbr + nghbptr->ypos) *
           nghbptr->xnbr + nghbptr->xpos + nghbptr->baseval);
}

int
graphGrid3DSize (
INT * const                 vertptr,
INT * const                 edgeptr,
INT * const                 degrptr,
const INT                   xnbr,
const INT                   ynbr,
const INT                   znbr)
{
  *vertptr = xnbr * ynbr * znbr;                  /* Compute grid parameters */
  *edgeptr = xnbr * ynbr * znbr * 6 - (xnbr * ynbr + xnbr * znbr + ynbr * znbr) * 2;
  *degrptr = 6;

  return (0);
}

/* This routine builds a finite difference graph
** in shape of a 3D grid of the given size.
** It returns:
** - 0  : in all cases.
*/

int
graphBuildGrid3D (
Graph * const               grafptr,              /*+ Graph to build +*/
const INT                   xnbr,                 /*+ X dimension    +*/
const INT                   ynbr,                 /*+ Y dimension    +*/
const INT                   znbr,                 /*+ Z dimension    +*/
const INT                   baseval)              /*+ Base value     +*/
{
  GraphGridNghb               nghbdat;            /* Neighbor data */
  INT                         vertnbr;
  INT                         edgenbr;
  INT                         degrmax;

  graphGrid3DSize (&vertnbr, &edgenbr, &degrmax, xnbr, ynbr, znbr); /* Get graph parameters */

  nghbdat.baseval = baseval;                      /* Initialize neighbor structure */
  nghbdat.xnbr    = xnbr;
  nghbdat.ynbr    = ynbr;
  nghbdat.znbr    = znbr;

  return (graphBuild (                            /* Build graph */
            grafptr,
            vertnbr,
            edgenbr,
            baseval,
            (void *) &nghbdat,
            (INT (*) (void * const, const INT)) graphGrid3DFrst,
            (INT (*) (void * const))            graphGrid3DNext));
}

/************************************/
/*                                  */
/* 2D finite element grid routines. */
/*                                  */
/************************************/

/* These routines define a
** 2D finite element graph.
*/

INT
graphGrid2EDegr (
GraphGridNghb * const       nghbptr,
const INT                   vertnum)
{
  return (8);
}

INT
graphGrid2EFrst (
GraphGridNghb * const       nghbptr,
INT                         vertnum)
{
  INT                 verttmp;

  verttmp       = vertnum - nghbptr->baseval;
  nghbptr->xnum = verttmp % nghbptr->xnbr;
  nghbptr->ynum = verttmp / nghbptr->xnbr;

  nghbptr->xmin = nghbptr->xnum - 1;
  nghbptr->ymin = nghbptr->ynum - 1;
  nghbptr->xmax = nghbptr->xnum + 1;
  nghbptr->ymax = nghbptr->ynum + 1;

  if (nghbptr->xmin < 0)
    nghbptr->xmin = 0;
  if (nghbptr->ymin < 0)
    nghbptr->ymin = 0;
  if (nghbptr->xmax >= nghbptr->xnbr)
    nghbptr->xmax = nghbptr->xnbr - 1;
  if (nghbptr->ymax >= nghbptr->ynbr)
    nghbptr->ymax = nghbptr->ynbr - 1;

  nghbptr->xpos = nghbptr->xmin - 1;              /* Trick: process (0,0) loop case */
  nghbptr->ypos = nghbptr->ymin;                  /* By calling graphGrid2ENext()   */

  return (graphGrid2ENext (nghbptr));
}

INT
graphGrid2ENext (
GraphGridNghb * const       nghbptr)
{
  do {
    if (++ nghbptr->xpos > nghbptr->xmax) {
      nghbptr->xpos = nghbptr->xmin;
      if (++ nghbptr->ypos > nghbptr->ymax)
        return (nghbptr->baseval - 1);
    }
  } while ((nghbptr->xpos == nghbptr->xnum) &&    /* Skip loop edge */
           (nghbptr->ypos == nghbptr->ynum));

  return (nghbptr->ypos *
          nghbptr->xnbr + nghbptr->xpos + nghbptr->baseval);
}

int
graphGrid2ESize (
INT * const                 vertptr,
INT * const                 edgeptr,
INT * const                 degrptr,
const INT                   xnbr,
const INT                   ynbr)
{
  *vertptr = xnbr * ynbr;                         /* Compute grid parameters */
  *edgeptr = xnbr * ynbr * 8 - (xnbr + ynbr) * 6 + 4;
  *degrptr = 8;

  return (0);
}

/* This routine builds a finite element graph
** in shape of a 2D grid of the given size.
** It returns:
** - 0  : in all cases.
*/

int
graphBuildGrid2E (
Graph * const               grafptr,              /*+ Graph to build +*/
const INT                   xnbr,                 /*+ X dimension    +*/
const INT                   ynbr,                 /*+ Y dimension    +*/
const INT                   baseval)              /*+ Base value     +*/
{
  GraphGridNghb               nghbdat;            /* Neighbor data */
  INT                         vertnbr;
  INT                         edgenbr;
  INT                         degrmax;

  graphGrid2ESize (&vertnbr, &edgenbr, &degrmax, xnbr, ynbr); /* Get graph parameters */

  nghbdat.baseval = baseval;                      /* Initialize neighbor structure */
  nghbdat.xnbr    = xnbr;
  nghbdat.ynbr    = ynbr;

  return (graphBuild (                            /* Build graph */
            grafptr,
            vertnbr,
            edgenbr,
            baseval,
            (void *) &nghbdat,
            (INT (*) (void * const, const INT)) graphGrid2EFrst,
            (INT (*) (void * const))            graphGrid2ENext));
}

/************************************/
/*                                  */
/* 3D finite element grid routines. */
/*                                  */
/************************************/

/* These routines define a 3D
** finite element graph.
*/

INT
graphGrid3EDegr (
GraphGridNghb * const       nghbptr,
const INT                   vertnum)
{
  return (26);
}

INT
graphGrid3EFrst (
GraphGridNghb * const       nghbptr,
INT                         vertnum)
{
  INT                 verttmp;

  verttmp       = vertnum - nghbptr->baseval;
  nghbptr->xnum = verttmp % nghbptr->xnbr;
  verttmp       = verttmp / nghbptr->xnbr;
  nghbptr->ynum = verttmp % nghbptr->ynbr;
  nghbptr->znum = verttmp / nghbptr->ynbr;

  nghbptr->xmin = nghbptr->xnum - 1;
  nghbptr->ymin = nghbptr->ynum - 1;
  nghbptr->zmin = nghbptr->znum - 1;
  nghbptr->xmax = nghbptr->xnum + 1;
  nghbptr->ymax = nghbptr->ynum + 1;
  nghbptr->zmax = nghbptr->znum + 1;

  if (nghbptr->xmin < 0)
    nghbptr->xmin = 0;
  if (nghbptr->ymin < 0)
    nghbptr->ymin = 0;
  if (nghbptr->zmin < 0)
    nghbptr->zmin = 0;
  if (nghbptr->xmax >= nghbptr->xnbr)
    nghbptr->xmax = nghbptr->xnbr - 1;
  if (nghbptr->ymax >= nghbptr->ynbr)
    nghbptr->ymax = nghbptr->ynbr - 1;
  if (nghbptr->zmax >= nghbptr->znbr)
    nghbptr->zmax = nghbptr->znbr - 1;

  nghbptr->xpos = nghbptr->xmin - 1;              /* Trick: process (0,0,0) loop case */
  nghbptr->ypos = nghbptr->ymin;                  /* By calling graphGrid3ENext()     */
  nghbptr->zpos = nghbptr->zmin;

  return (graphGrid3ENext (nghbptr));
}

INT
graphGrid3ENext (
GraphGridNghb * const       nghbptr)
{
  do {
    if (++ nghbptr->xpos > nghbptr->xmax) {
      nghbptr->xpos = nghbptr->xmin;
      if (++ nghbptr->ypos > nghbptr->ymax) {
        nghbptr->ypos = nghbptr->ymin;
        if (++ nghbptr->zpos > nghbptr->zmax)
          return (nghbptr->baseval - 1);
      }
    }
  } while ((nghbptr->xpos == nghbptr->xnum) &&    /* Skip loop edge */
           (nghbptr->ypos == nghbptr->ynum) &&
           (nghbptr->zpos == nghbptr->znum));

  return ((nghbptr->zpos                  *
           nghbptr->ynbr + nghbptr->ypos) *
           nghbptr->xnbr + nghbptr->xpos + nghbptr->baseval);
}

int
graphGrid3ESize (
INT * const                 vertptr,
INT * const                 edgeptr,
INT * const                 degrptr,
const INT                   xnbr,
const INT                   ynbr,
const INT                   znbr)
{
  if ((xnbr < 2) || (ynbr < 2) || (znbr < 2)) {
    errorPrint ("graphGrid3ESize: all dimension values must be greater than 1");
    return     (1);
  }

  *vertptr = xnbr * ynbr * znbr;                  /* Compute grid parameters */
  *edgeptr = 26 * xnbr * ynbr * znbr - (xnbr * ynbr + xnbr * znbr + ynbr * znbr) * 18 + (xnbr + ynbr + znbr) * 12 - 8;
  *degrptr = 26;

  return (0);
}

/* This routines builds a finite element
** graph in shape of a 3D grid of the
** given size.
** It returns:
** - 0  : in all cases.
*/

int
graphBuildGrid3E (
Graph * const               grafptr,              /*+ Graph to build +*/
const INT                   xnbr,                 /*+ X dimension    +*/
const INT                   ynbr,                 /*+ Y dimension    +*/
const INT                   znbr,                 /*+ Z dimension    +*/
const INT                   baseval)              /*+ Base value     +*/
{
  GraphGridNghb               nghbdat;            /* Neighbor data */
  INT                         vertnbr;
  INT                         edgenbr;
  INT                         degrmax;

  graphGrid3ESize (&vertnbr, &edgenbr, &degrmax,  xnbr, ynbr, znbr); /* Get graph parameters */

  nghbdat.baseval = baseval;                      /* Initialize neighbor structure */
  nghbdat.xnbr    = xnbr;
  nghbdat.ynbr    = ynbr;
  nghbdat.znbr    = znbr;

  return (graphBuild (                            /* Build graph */
            grafptr,
            vertnbr,
            edgenbr,
            baseval,
            (void *) &nghbdat,
            (INT (*) (void * const, const INT)) graphGrid3EFrst,
            (INT (*) (void * const))            graphGrid3ENext));
}
