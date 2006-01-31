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
/**   NAME       : hgraph_order_hd.c                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module orders a separator using    **/
/**                the block-oriented Halo Approximate     **/
/**                (Multiple) Minimum Degree algorithm,    **/
/**                with super-variable accounting (HaloAMD **/
/**                v2.0).                                  **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 09 aug 1998     **/
/**                                 to     18 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 05 jan 1999     **/
/**                # Version 4.0  : from : 14 jan 2003     **/
/**                                 to   : 23 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HGRAPH_ORDER_HD

#include "module.h"
#include "common.h"
#include "graph.h"
#include "order.h"
#include "hgraph.h"
#include "hall_order_hx.h"
#include "hall_order_hd.h"
#include "hgraph_order_hd.h"
#include "hgraph_order_hx.h"
#include "hgraph_order_si.h"

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the ordering.
** It returns:
** - 0   : if the ordering could be computed.
** - !0  : on error.
*/

int
hgraphOrderHd (
const Hgraph * restrict const             grafptr,
Order * restrict const                    ordeptr,
const Gnum                                ordenum, /*+ Zero-based ordering number +*/
OrderCblk * restrict const                cblkptr, /*+ Multiple column-block      +*/
const HgraphOrderHdParam * restrict const paraptr)
{
  Gnum * restrict     petab;
  Gnum                pfree;
  Gnum                iwlen;
  Gnum * restrict     iwtab;
  Gnum * restrict     lentab;
  Gnum * restrict     nvartab;
  Gnum * restrict     elentab;
  Gnum * restrict     lasttab;
  Gnum * restrict     leaftab;
  Gnum * restrict     secntab;                    /* Array of index to first secondary variable */
  Gnum * restrict     nexttab;                    /* Array of index of next principal variable  */
  Gnum * restrict     frsttab;
  Gnum                ncmpa;
  Gnum                n;                          /* Number of nodes to order (with halo or not) */
  int                 o;

  if (grafptr->s.vertnbr < paraptr->colmin)       /* If graph is too small, order simply */
    return (hgraphOrderSi (grafptr, ordeptr, ordenum, cblkptr));

  n     = grafptr->s.vertnbr;
  iwlen = (Gnum) ((double) grafptr->s.edgenbr * HGRAPHORDERHDCOMPRAT) + 32;
  if (iwlen < n)                                  /* Prepare to re-use array */
    iwlen = n;

  if (memAllocGroup ((void **)
                     &petab,   (size_t) (n     * sizeof (Gnum)),
                     &iwtab,   (size_t) (iwlen * sizeof (Gnum)),
                     &lentab,  (size_t) (n     * sizeof (Gnum)),
                     &nvartab, (size_t) (n     * sizeof (Gnum)),
                     &elentab, (size_t) (n     * sizeof (Gnum)),
                     &lasttab, (size_t) (n     * sizeof (Gnum)),
                     &leaftab, (size_t) (n     * sizeof (Gnum)),
                     &frsttab, (size_t) (n     * sizeof (Gnum)),
                     &secntab, (size_t) (n     * sizeof (Gnum)),
                     &nexttab, (size_t) (n     * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("hgraphOrderHd: out of memory");
    return     (1);
  }

  hgraphOrderHxFill (grafptr, petab, lentab, iwtab, elentab, &pfree);

  hallOrderHdHalmd (n, 0, iwlen, petab, pfree,    /* No elements here */
                    lentab, iwtab, nvartab, elentab, lasttab, &ncmpa,
                    leaftab, secntab, nexttab, frsttab);
  if (ncmpa < 0) {
    errorPrint ("hgraphOrderHd: internal error");
    memFree    (petab);                           /* Free group leader */
    return     (1);
  }

  o = hallOrderHxBuild (grafptr->s.baseval, n, grafptr->vnohnbr,
                        grafptr->s.vnumtax, ordeptr, cblkptr,
                        nvartab - grafptr->s.baseval,
                        lentab - grafptr->s.baseval,
                        petab   - grafptr->s.baseval,
                        frsttab - grafptr->s.baseval,
                        nexttab - grafptr->s.baseval,
                        secntab - grafptr->s.baseval,
                        iwtab   - grafptr->s.baseval,
                        elentab - grafptr->s.baseval,
                        ordeptr->peritab + ordenum, /* Use given inverse permutation as inverse permutation space, never based */
                        leaftab,
                        paraptr->colmin, paraptr->colmax, (float) paraptr->fillrat);

  memFree (petab);                                /* Free group leader */

  return (o);
}
