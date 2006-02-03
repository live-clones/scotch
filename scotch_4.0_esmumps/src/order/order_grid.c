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
/**   NAME       : order_grid.c                            **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module builds orderings of grids   **/
/**                using nested dissection.                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 02 sep 1998     **/
/**                                 to     17 oct 2000     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     28 feb 2004     **/
/**                                                        **/
/**   NOTES      : # The numbering of grids is the same as **/
/**                  the one defined in graph_grid.c .     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ORDER_GRID

#include "common.h"
#include "order.h"
#include "order_grid.h"

/***************************/
/*                         */
/* Grid ordering routines. */
/*                         */
/***************************/

/* This routine builds a straight
** nested dissection ordering of the
** 2D grid whose characteristics are
** given as parameters.
** It returns:
** - 0  : in all cases.
*/

int
orderGrid2 (
Order * const               ordeptr,              /*+ Ordering to compute           +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const INT                   xlim,                 /*+ Minimum X dimension per block +*/
const INT                   ylim)                 /*+ Minimum Y dimension per block +*/
{
  INT *               permtnd;                    /* End of permutation array            */
  INT *               permptr;                    /* Pointer to current permutation cell */
  INT *               peritax;                    /* Based access for peritab            */
  INT *               rangptr;                    /* Pointer to current range array cell */
  INT                 ordenum;                    /* Number to set for current vertex    */
  INT                 vertnbr;                    /* Number of vertices                  */
  INT                 vertnum;                    /* Current vertex number               */

  vertnbr = xnbr * ynbr;
  if (((ordeptr->permtab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->peritab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->rangtab = (INT *) memAlloc ((vertnbr + 1) * sizeof (INT))) == NULL)) {
    errorPrint ("orderGrid2: out of memory");
    orderExit  (ordeptr);
    orderInit  (ordeptr);
    return     (1);
  }

  ordeptr->cblknbr = 0;                             /* Reset number of column blocks */
  rangptr          = ordeptr->rangtab;
  rangptr[0]       = baseval;                       /* Set initial range index          */
  ordenum          = baseval;                       /* Set final numbering              */
  graphOrderGrid22 (&ordeptr->cblknbr, &rangptr, ordeptr->permtab, /* Perform recursion */
                    &ordenum, xnbr, xlim, ylim, 0, 0, xnbr, ynbr, 0);

#ifdef ORDER_DEBUG
  if (ordenum != (baseval + vertnbr)) {
    errorPrint ("orderGrid2: internal error");
    return     (1);
  }
#endif /* ORDER_DEBUG */

  for (permptr = ordeptr->permtab, peritax = ordeptr->peritab - baseval, /* Build inverse permutation */
       permtnd = permptr + vertnbr, vertnum = baseval;
       permptr < permtnd;
       permptr ++, vertnum ++)
    peritax[*permptr] = vertnum;

#ifdef ORDER_DEBUG
  if ((ordeptr->rangtab[0]                != baseval)           ||
      (ordeptr->rangtab[ordeptr->cblknbr] != baseval + vertnbr) ||
      (orderCheck (ordeptr) != 0)) {
    errorPrint ("orderGrid2: invalid ordering");
  }
#endif /* ORDER_DEBUG */

  ordeptr->rangtab = memRealloc (ordeptr->rangtab, (ordeptr->cblknbr + 1) * sizeof (INT));

  return (0);
}

static
void
graphOrderGrid22 (
INT * const                 cblkptr,              /*+ Number of column blocks, to be set  +*/
INT ** const                rangptr,              /*+ Pointer to current range array cell +*/
INT * const                 permtab,              /*+ Permutation array [based]           +*/
INT * const                 ordeptr,              /*+ Pointer to current ordering value   +*/
const INT                   xnbr,                 /*+ X Dimension                         +*/
const INT                   xlim,                 /*+ Minimum X dimension per block       +*/
const INT                   ylim,                 /*+ Minimum Y dimension per block       +*/
const INT                   xmin,                 /*+ First X index for block             +*/
const INT                   ymin,                 /*+ First Y index for block             +*/
const INT                   xext,                 /*+ X dimension of block                +*/
const INT                   yext,                 /*+ Y dimension of block                +*/
int                         flag)                 /*+ Direction flag                      +*/
{
  INT                 xmed;                       /* Median X coordinate  */
  INT                 ymed;                       /* Median Y coordinate  */
  INT                 xnum;                       /* Current X coordinate */
  INT                 ynum;                       /* Current Y coordinate */

  if ((xext <= xlim) &&                           /* If minimum block size reached */
      (yext <= ylim)) {
    switch (flag & 3) {                           /* Fill according to direction */
      case 0 :                                    /* X+, Y+                      */
        for (ynum = 0; ynum < yext; ynum ++) {    /* Fill block                  */
          for (xnum = 0; xnum < xext; xnum ++)
            permtab[(ymin + ynum) * xnbr + xmin + xnum] = (*ordeptr) ++;
        }
        break;
      case 1 :                                    /* X-, Y+     */
        for (ynum = 0; ynum < yext; ynum ++) {    /* Fill block */
          for (xnum = xext - 1; xnum >= 0; xnum --)
            permtab[(ymin + ynum) * xnbr + xmin + xnum] = (*ordeptr) ++;
        }
        break;
      case 2 :                                    /* X+, Y-       */
        for (ynum = yext - 1; ynum >= 0; ynum --) { /* Fill block */
          for (xnum = 0; xnum < xext; xnum ++)
            permtab[(ymin + ynum) * xnbr + xmin + xnum] = (*ordeptr) ++;
        }
        break;
      case 3 :                                    /* X-, Y-       */
        for (ynum = yext - 1; ynum >= 0; ynum --) { /* Fill block */
          for (xnum = xext - 1; xnum >= 0; xnum --)
            permtab[(ymin + ynum) * xnbr + xmin + xnum] = (*ordeptr) ++;
        }
        break;
    }

    (*rangptr)[1] = (*rangptr)[0] + xext * yext;  /* Set column block size */
    (*rangptr) ++;                                /* One more column block */
    (*cblkptr) ++;
  }
  else {                                          /* Nested dissection is applied */
    if (xext > yext) {                            /* Cut X dimension              */
      xmed = xext / 2;                            /* Get median coordinate        */

      graphOrderGrid22 (cblkptr, rangptr, permtab, ordeptr, /* Perform recursion */
                        xnbr, xlim, ylim,
                        xmin, ymin, xmed, yext, flag & ~1);
      if ((xext - xmed - 1) > 0)
        graphOrderGrid22 (cblkptr, rangptr, permtab, ordeptr,
                          xnbr, xlim, ylim,
                          xmin + xmed + 1, ymin, (xext - xmed - 1), yext, flag | 1);
      graphOrderGrid22 (cblkptr, rangptr, permtab, ordeptr, /* Fill separator */
                        xnbr, xext, yext,
                        xmin + xmed, ymin, 1, yext, flag);
    }
    else {                                        /* Cut Y dimension       */
      ymed = yext / 2;                            /* Get median coordinate */

      graphOrderGrid22 (cblkptr, rangptr, permtab, ordeptr, /* Perform recursion */
                        xnbr, xlim, ylim,
                        xmin, ymin, xext, ymed, flag & ~2);
      if ((yext - ymed - 1) > 0)
        graphOrderGrid22 (cblkptr, rangptr, permtab, ordeptr,
                          xnbr, xlim, ylim,
                          xmin, ymin + ymed + 1, xext, (yext - ymed - 1), flag | 2);
      graphOrderGrid22 (cblkptr, rangptr, permtab, ordeptr, /* Fill separator */
                        xnbr, xext, yext,
                        xmin, ymin + ymed, xext, 1, flag);
    }
  }
}

/* This routine builds a straight
** nested dissection ordering of the mesh
** defined as a 2D triangular grid with
** complete uppermost Y layers. These two
** layers are ordered last.
** It returns:
** - 0  : in all cases.
*/

int
orderGrid2C (
Order * const               ordeptr,              /*+ Ordering to compute           +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const INT                   xlim,                 /*+ Minimum X dimension per block +*/
const INT                   ylim)                 /*+ Minimum Y dimension per block +*/
{
  INT *               permtnd;                    /* End of permutation array            */
  INT *               permptr;                    /* Pointer to current permutation cell */
  INT *               peritax;                    /* Based access for peritab            */
  INT *               rangptr;                    /* Pointer to current range array cell */
  INT                 ordenum;                    /* Number to set for current vertex    */
  INT                 vertnbr;                    /* Number of vertices                  */
  INT                 vertnum;                    /* Current vertex number               */

  vertnbr = xnbr * ynbr;
  if (((ordeptr->permtab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->peritab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->rangtab = (INT *) memAlloc ((vertnbr + 1) * sizeof (INT))) == NULL)) {
    errorPrint ("orderGrid2C: out of memory");
    orderExit  (ordeptr);
    orderInit  (ordeptr);
    return     (1);
  }

  ordeptr->cblknbr = 0;                             /* Reset number of column blocks */
  rangptr          = ordeptr->rangtab;
  rangptr[0]       = baseval;                       /* Set initial range index          */
  ordenum          = baseval;                       /* Set final numbering              */
  graphOrderGrid22 (&ordeptr->cblknbr, &rangptr, ordeptr->permtab, /* Perform recursion */
                    &ordenum, xnbr,
                    xlim, ylim, 0, 0, xnbr, ynbr - 2, 0);
  graphOrderGrid22 (&ordeptr->cblknbr, &rangptr, ordeptr->permtab, /* Order upper layers in ascending order */
                    &ordenum, xnbr,
                    xnbr, ynbr, 0, ynbr - 2, xnbr, 2, 0);

#ifdef ORDER_DEBUG
  if (ordenum != (baseval + vertnbr)) {
    errorPrint ("orderGrid2C: internal error");
    return     (1);
  }
#endif /* ORDER_DEBUG */

  for (permptr = ordeptr->permtab, peritax = ordeptr->peritab - baseval, /* Build inverse permutation */
       permtnd = permptr + vertnbr, vertnum = baseval;
       permptr < permtnd;
       permptr ++, vertnum ++)
    peritax[*permptr] = vertnum;

#ifdef ORDER_DEBUG
  if ((ordeptr->rangtab[0]                != baseval)           ||
      (ordeptr->rangtab[ordeptr->cblknbr] != baseval + vertnbr) ||
      (orderCheck (ordeptr) != 0)) {
    errorPrint ("orderGrid2C: invalid ordering");
  }
#endif /* ORDER_DEBUG */

  ordeptr->rangtab = memRealloc (ordeptr->rangtab, (ordeptr->cblknbr + 1) * sizeof (INT));

  return (0);
}

/* This routine builds a straight
** nested dissection ordering of the
** 3D grid whose characteristics are
** given as parameters.
** It returns:
** - 0  : in all cases.
*/

int
orderGrid3 (
Order * const               ordeptr,              /*+ Ordering to compute           +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   znbr,                 /*+ Z dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const INT                   xlim,                 /*+ Minimum X dimension per block +*/
const INT                   ylim,                 /*+ Minimum Y dimension per block +*/
const INT                   zlim)                 /*+ Minimum Z dimension per block +*/
{
  INT *               permtnd;                    /* End of permutation array            */
  INT *               permptr;                    /* Pointer to current permutation cell */
  INT *               peritax;                    /* Based access for peritab            */
  INT *               rangptr;                    /* Pointer to current range array cell */
  INT                 ordenum;                    /* Number to set for current vertex    */
  INT                 vertnbr;                    /* Number of vertices                  */
  INT                 vertnum;                    /* Current vertex number               */

  vertnbr = xnbr * ynbr * znbr;
  if (((ordeptr->permtab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->peritab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->rangtab = (INT *) memAlloc ((vertnbr + 1) * sizeof (INT))) == NULL)) {
    errorPrint ("orderGrid3: out of memory");
    orderExit  (ordeptr);
    orderInit  (ordeptr);
    return     (1);
  }

  ordeptr->cblknbr = 0;                             /* Reset number of column blocks */
  rangptr          = ordeptr->rangtab;
  rangptr[0]       = baseval;                       /* Set initial range index          */
  ordenum          = baseval;                       /* Set final numbering              */
  graphOrderGrid32 (&ordeptr->cblknbr, &rangptr, ordeptr->permtab, /* Perform recursion */
                    &ordenum, xnbr, ynbr,
                    xlim, ylim, zlim, 0, 0, 0, xnbr, ynbr, znbr, 0);

#ifdef ORDER_DEBUG
  if (ordenum != (baseval + vertnbr)) {
    errorPrint ("orderGrid3: internal error");
    return     (1);
  }
#endif /* ORDER_DEBUG */

  for (permptr = ordeptr->permtab, peritax = ordeptr->peritab - baseval, /* Build inverse permutation */
       permtnd = permptr + vertnbr, vertnum = baseval;
       permptr < permtnd;
       permptr ++, vertnum ++)
    peritax[*permptr] = vertnum;

#ifdef ORDER_DEBUG
  if ((ordeptr->rangtab[0]                != baseval)           ||
      (ordeptr->rangtab[ordeptr->cblknbr] != baseval + vertnbr) ||
      (orderCheck (ordeptr) != 0)) {
    errorPrint ("orderGrid3: invalid ordering");
  }
#endif /* ORDER_DEBUG */

  ordeptr->rangtab = memRealloc (ordeptr->rangtab, (ordeptr->cblknbr + 1) * sizeof (INT));

  return (0);
}

static
void
graphOrderGrid32 (
INT * const                 cblkptr,              /*+ Number of column blocks, to be set  +*/
INT ** const                rangptr,              /*+ Pointer to current range array cell +*/
INT * const                 permtab,              /*+ Permutation array [based]           +*/
INT * const                 ordeptr,              /*+ Pointer to current ordering value   +*/
const INT                   xnbr,                 /*+ X Dimension                         +*/
const INT                   ynbr,                 /*+ Y Dimension                         +*/
const INT                   xlim,                 /*+ Minimum X dimension per block       +*/
const INT                   ylim,                 /*+ Minimum Y dimension per block       +*/
const INT                   zlim,                 /*+ Minimum Z dimension per block       +*/
const INT                   xmin,                 /*+ First X index for block             +*/
const INT                   ymin,                 /*+ First Y index for block             +*/
const INT                   zmin,                 /*+ First Z index for block             +*/
const INT                   xext,                 /*+ X dimension of block                +*/
const INT                   yext,                 /*+ Y dimension of block                +*/
const INT                   zext,                 /*+ Z dimension of block                +*/
int                         flag)                 /*+ Direction flag                      +*/
{
  INT                 xmed;                       /* Median X coordinate  */
  INT                 ymed;                       /* Median Y coordinate  */
  INT                 zmed;                       /* Median Z coordinate  */
  INT                 xnum;                       /* Current X coordinate */
  INT                 ynum;                       /* Current Y coordinate */
  INT                 znum;                       /* Current Z coordinate */

  if ((xext <= xlim) &&                           /* If minimum block size reached */
      (yext <= ylim) &&
      (zext <= zlim)) {
    switch (flag & 7) {                           /* Fill according to direction */
      case 0 :                                    /* X+, Y+, Z+                  */
        for (znum = 0; znum < zext; znum ++) {    /* Fill block                  */
          for (ynum = 0; ynum < yext; ynum ++) {
            for (xnum = 0; xnum < xext; xnum ++)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
      case 1 :                                    /* X-, Y+, Z+ */
        for (znum = 0; znum < zext; znum ++) {    /* Fill block */
          for (ynum = 0; ynum < yext; ynum ++) {
            for (xnum = xext - 1; xnum >= 0; xnum --)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
      case 2 :                                    /* X+, Y-, Z+ */
        for (znum = 0; znum < zext; znum ++) {    /* Fill block */
          for (ynum = yext - 1; ynum >= 0; ynum --) {
            for (xnum = 0; xnum < xext; xnum ++)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
      case 3 :                                    /* X-, Y-, Z+ */
        for (znum = 0; znum < zext; znum ++) {    /* Fill block */
          for (ynum = yext - 1; ynum >= 0; ynum --) {
            for (xnum = xext - 1; xnum >= 0; xnum --)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
      case 4 :                                    /* X+, Y+, Z-   */
        for (znum = zext - 1; znum >= 0; znum --) { /* Fill block */
          for (ynum = 0; ynum < yext; ynum ++) {
            for (xnum = 0; xnum < xext; xnum ++)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
      case 5 :                                    /* X-, Y+, Z-   */
        for (znum = zext - 1; znum >= 0; znum --) { /* Fill block */
          for (ynum = 0; ynum < yext; ynum ++) {
            for (xnum = xext - 1; xnum >= 0; xnum --)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
      case 6 :                                    /* X+, Y-, Z-   */
        for (znum = zext - 1; znum >= 0; znum --) { /* Fill block */
          for (ynum = yext - 1; ynum >= 0; ynum --) {
            for (xnum = 0; xnum < xext; xnum ++)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
      case 7 :                                    /* X-, Y-, Z-   */
        for (znum = zext - 1; znum >= 0; znum --) { /* Fill block */
          for (ynum = yext - 1; ynum >= 0; ynum --) {
            for (xnum = xext - 1; xnum >= 0; xnum --)
              permtab[((zmin + znum) * ynbr + (ymin + ynum)) * xnbr + xmin + xnum] = (*ordeptr) ++;
          }
        }
        break;
    }
    (*rangptr)[1] = (*rangptr)[0] + xext * yext * zext; /* Set column block size */
    (*rangptr) ++;                                /* One more column block       */
    (*cblkptr) ++;
  }
  else {                                          /* Nested dissection is applied */
    if ((xext > yext) &&                          /* Cut X dimension              */
        (xext > zext)) {
      xmed = xext / 2;                            /* Get median coordinate */

      graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr, /* Perform recursion */
                        xnbr, ynbr, xlim, ylim, zlim,
                        xmin, ymin, zmin, xmed, yext, zext, flag & ~1);
      if ((xext - xmed - 1) > 0)
        graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr,
                          xnbr, ynbr, xlim, ylim, zlim,
                          xmin + xmed + 1, ymin, zmin, (xext - xmed - 1), yext, zext, flag | 1);
      graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr, /* Fill separator */
                        xnbr, ynbr, xext, yext, zext,
                        xmin + xmed, ymin, zmin, 1, yext, zext, flag);
    }
    else if (yext > zext) {                       /* Cut Y dimension       */
      ymed = yext / 2;                            /* Get median coordinate */

      graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr, /* Perform recursion */
                        xnbr, ynbr, xlim, ylim, zlim,
                        xmin, ymin, zmin, xext, ymed, zext, flag & ~2);
      if ((yext - ymed - 1) > 0)
        graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr,
                          xnbr, ynbr, xlim, ylim, zlim,
                          xmin, ymin + ymed + 1, zmin, xext, (yext - ymed - 1), zext, flag | 2);
      graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr, /* Fill separator */
                        xnbr, ynbr, xext, yext, zext,
                        xmin, ymin + ymed, zmin, xext, 1, zext, flag);
    }
    else {                                        /* Cut Z dimension       */
      zmed = zext / 2;                            /* Get median coordinate */

      graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr, /* Perform recursion */
                        xnbr, ynbr, xlim, ylim, zlim,
                        xmin, ymin, zmin, xext, yext, zmed, flag & ~4);
      if ((zext - zmed - 1) > 0)
        graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr,
                          xnbr, ynbr, xlim, ylim, zlim,
                          xmin, ymin, zmin + zmed + 1, xext, yext, (zext - zmed - 1), flag | 4);
      graphOrderGrid32 (cblkptr, rangptr, permtab, ordeptr, /* Fill separator */
                        xnbr, ynbr, xext, yext, zext,
                        xmin, ymin, zmin + zmed, xext, yext, 1, flag);
    }
  }
}

/* This routine builds a straight
** nested dissection ordering of the mesh
** defined as a 3D tetrahedral grid with
** complete uppermost Z layers. These two
** layers are ordered last.
** It returns:
** - 0  : in all cases.
*/

int
orderGrid3C (
Order * const               ordeptr,              /*+ Ordering to compute           +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   znbr,                 /*+ Z dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const INT                   xlim,                 /*+ Minimum X dimension per block +*/
const INT                   ylim,                 /*+ Minimum Y dimension per block +*/
const INT                   zlim)                 /*+ Minimum Z dimension per block +*/
{
  INT *               permtnd;                    /* End of permutation array            */
  INT *               permptr;                    /* Pointer to current permutation cell */
  INT *               peritax;                    /* Based access for peritab            */
  INT *               rangptr;                    /* Pointer to current range array cell */
  INT                 ordenum;                    /* Number to set for current vertex    */
  INT                 vertnbr;                    /* Number of vertices                  */
  INT                 vertnum;                    /* Current vertex number               */

  vertnbr = xnbr * ynbr * znbr;
  if (((ordeptr->permtab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->peritab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->rangtab = (INT *) memAlloc ((vertnbr + 1) * sizeof (INT))) == NULL)) {
    errorPrint ("orderGrid3C: out of memory");
    orderExit  (ordeptr);
    orderInit  (ordeptr);
    return     (1);
  }

  ordeptr->cblknbr = 0;                             /* Reset number of column blocks */
  rangptr          = ordeptr->rangtab;
  rangptr[0]       = baseval;                       /* Set initial range index          */
  ordenum          = baseval;                       /* Set final numbering              */
  graphOrderGrid32 (&ordeptr->cblknbr, &rangptr, ordeptr->permtab, /* Perform recursion */
                    &ordenum, xnbr, ynbr,
                    xlim, ylim, zlim, 0, 0, 0, xnbr, ynbr, znbr - 2, 0);
  graphOrderGrid32 (&ordeptr->cblknbr, &rangptr, ordeptr->permtab, /* Order upper layer */
                    &ordenum, xnbr, ynbr,
                    xnbr, ynbr, znbr, 0, 0, znbr - 2, xnbr, ynbr, 2, 0);
#ifdef ORDER_DEBUG
  if (ordenum != (baseval + vertnbr)) {
    errorPrint ("orderGrid3C: internal error");
    return     (1);
  }
#endif /* ORDER_DEBUG */

  for (permptr = ordeptr->permtab, peritax = ordeptr->peritab - baseval, /* Build inverse permutation */
       permtnd = permptr + vertnbr, vertnum = baseval;
       permptr < permtnd;
       permptr ++, vertnum ++)
    peritax[*permptr] = vertnum;

#ifdef ORDER_DEBUG
  if ((ordeptr->rangtab[0]                != baseval)           ||
      (ordeptr->rangtab[ordeptr->cblknbr] != baseval + vertnbr) ||
      (orderCheck (ordeptr) != 0)) {
    errorPrint ("orderGrid3C: invalid ordering");
  }
#endif /* ORDER_DEBUG */

  ordeptr->rangtab = memRealloc (ordeptr->rangtab, (ordeptr->cblknbr + 1) * sizeof (INT));

  return (0);
}
