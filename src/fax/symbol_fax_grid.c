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
/**   NAME       : symbol_fax_grid.c                       **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These are the entry points and neighbor **/
/**                routines for finite difference and      **/
/**                finite elements grids.                  **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 21 apr 1999     **/
/**                                 to     01 may 1999     **/
/**                # Version 1.0  : from : 01 jun 2002     **/
/**                                 to     03 jun 2002     **/
/**                # Version 1.1  : from : 26 jun 2002     **/
/**                                 to     26 jun 2002     **/
/**                # Version 1.2  : from : 24 sep 2002     **/
/**                                 to     25 sep 2002     **/
/**                # Version 3.0  : from : 02 mar 2004     **/
/**                                 to     02 mar 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_FAX_GRID

#include "common.h"
#include "scotch.h"
#include "graph.h"
#include "graph_grid.h"
#include "symbol.h"
#include "order.h"
#include "fax.h"

/*+ This routine computes the block symbolic
*** factorization of the 2D finite difference
*** grid whose characteristics and ordering
*** are given as parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolFaxGrid2D (
SymbolMatrix * const        symbptr,              /*+ Symbolic block matrix [based] +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const Order * const         ordeptr)              /*+ Matrix ordering               +*/
{
  INT                 gridvertnbr;                /* Number of vertices           */
  INT                 gridedgenbr;                /* Number of edges              */
  INT                 griddegrmax;                /* Maximum degree (not used)    */
  GraphGridNghb       gridnghbdat;                /* Neighbor recording structure */
  int                 o;

  graphGrid2DSize (&gridvertnbr, &gridedgenbr, &griddegrmax, /* Compute grid parameters */
                   xnbr, ynbr);

  gridnghbdat.baseval = baseval;                  /* Initialize neighbor definition structure */
  gridnghbdat.xnbr    = xnbr;
  gridnghbdat.ynbr    = ynbr;

  o = symbolFax (symbptr, gridvertnbr, gridedgenbr, baseval, &gridnghbdat,
                 (INT (*) (void * const, const INT)) graphGrid2DFrst,
                 (INT (*) (void * const)) graphGrid2DNext,
                 (INT (*) (void * const, const INT)) graphGrid2DDegr, ordeptr);

#ifdef FAX_DEBUG
  if (symbolCheck (symbptr) != 0) {
    errorPrint ("symbolFaxGrid2D: internal error");
    return     (1);
  }
#endif /* FAX_DEBUG */

  return (o);
}

/*+ This routine computes the block symbolic
*** factorization of the 3D finite difference
*** grid whose characteristics and ordering
*** are given as parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolFaxGrid3D (
SymbolMatrix * const        symbptr,              /*+ Symbolic block matrix [based] +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   znbr,                 /*+ Z dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const Order * const         ordeptr)              /*+ Matrix ordering               +*/
{
  INT                 gridvertnbr;                /* Number of vertices           */
  INT                 gridedgenbr;                /* Number of edges              */
  INT                 griddegrmax;                /* Maximum degree (not used)    */
  GraphGridNghb       gridnghbdat;                /* Neighbor recording structure */
  int                 o;

  graphGrid3DSize (&gridvertnbr, &gridedgenbr, &griddegrmax, /* Compute grid parameters */
                   xnbr, ynbr, znbr);

  gridnghbdat.baseval = baseval;                  /* Initialize neighbor definition structure */
  gridnghbdat.xnbr    = xnbr;
  gridnghbdat.ynbr    = ynbr;
  gridnghbdat.znbr    = znbr;

  o = symbolFax (symbptr, gridvertnbr, gridedgenbr, baseval, &gridnghbdat,
                 (INT (*) (void * const, const INT)) graphGrid3DFrst,
                 (INT (*) (void * const)) graphGrid3DNext,
                 (INT (*) (void * const, const INT)) graphGrid3DDegr, ordeptr);

#ifdef FAX_DEBUG
  if (symbolCheck (symbptr) != 0) {
    errorPrint ("symbolFaxGrid3D: internal error");
    return     (1);
  }
#endif /* FAX_DEBUG */

  return (o);
}

/*+ This routine computes the block symbolic
*** factorization of the 2D finite element
*** grid whose characteristics and ordering
*** are given as parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolFaxGrid2E (
SymbolMatrix * const        symbptr,              /*+ Symbolic block matrix [based] +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const Order * const         ordeptr)              /*+ Matrix ordering               +*/
{
  INT                 gridvertnbr;                /* Number of vertices           */
  INT                 gridedgenbr;                /* Number of edges              */
  INT                 griddegrmax;                /* Maximum degree (not used)    */
  GraphGridNghb       gridnghbdat;                /* Neighbor recording structure */
  int                 o;

  graphGrid2DSize (&gridvertnbr, &gridedgenbr, &griddegrmax, /* Compute grid parameters */
                   xnbr, ynbr);

  gridnghbdat.baseval = baseval;                  /* Initialize neighbor definition structure */
  gridnghbdat.xnbr    = xnbr;
  gridnghbdat.ynbr    = ynbr;

  o = symbolFax (symbptr, gridvertnbr, gridedgenbr, baseval, &gridnghbdat,
                 (INT (*) (void * const, const INT)) graphGrid2EFrst,
                 (INT (*) (void * const)) graphGrid2ENext,
                 (INT (*) (void * const, const INT)) graphGrid2EDegr, ordeptr);

#ifdef FAX_DEBUG
  if (symbolCheck (symbptr) != 0) {
    errorPrint ("symbolFaxGrid2E: internal error");
    return     (1);
  }
#endif /* FAX_DEBUG */

  return (o);
}

/*+ This routine computes the block symbolic
*** factorization of the 3D finite element
*** grid whose characteristics and ordering
*** are given as parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolFaxGrid3E (
SymbolMatrix * const        symbptr,              /*+ Symbolic block matrix [based] +*/
const INT                   xnbr,                 /*+ X dimension                   +*/
const INT                   ynbr,                 /*+ Y dimension                   +*/
const INT                   znbr,                 /*+ Z dimension                   +*/
const INT                   baseval,              /*+ Base value                    +*/
const Order * const         ordeptr)              /*+ Matrix ordering               +*/
{
  INT                 gridvertnbr;                /* Number of vertices           */
  INT                 gridedgenbr;                /* Number of edges              */
  INT                 griddegrmax;                /* Maximum degree (not used)    */
  GraphGridNghb       gridnghbdat;                /* Neighbor recording structure */
  int                 o;

  graphGrid3ESize (&gridvertnbr, &gridedgenbr, &griddegrmax, /* Compute grid parameters */
                   xnbr, ynbr, znbr);

  gridnghbdat.baseval = baseval;                  /* Initialize neighbor definition structure */
  gridnghbdat.xnbr    = xnbr;
  gridnghbdat.ynbr    = ynbr;
  gridnghbdat.znbr    = znbr;

  o = symbolFax (symbptr, gridvertnbr, gridedgenbr, baseval, &gridnghbdat,
                 (INT (*) (void * const, const INT)) graphGrid3EFrst,
                 (INT (*) (void * const)) graphGrid3ENext,
                 (INT (*) (void * const, const INT)) graphGrid3EDegr, ordeptr);

#ifdef FAX_DEBUG
  if (symbolCheck (symbptr) != 0) {
    errorPrint ("symbolFaxGrid3E: internal error");
    return     (1);
  }
#endif /* FAX_DEBUG */

  return (o);
}
