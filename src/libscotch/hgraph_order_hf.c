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
/**   NAME       : hgraph_order_hf.c                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module orders a subgraph using     **/
/**                the block-oriented Halo Approximate     **/
/**                (Multiple) Minimum Fill algorithm,      **/
/**                with super-variable accounting          **/
/**                R2HAMDf4 v2.0).                         **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 15 may 2001     **/
/**                                 to   : 23 nov 2001     **/
/**                # Version 4.0  : from : 10 jan 2003     **/
/**                                 to   : 24 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HGRAPH_ORDER_HF

#include "module.h"
#include "common.h"
#include "graph.h"
#include "order.h"
#include "hgraph.h"
#include "hall_order_hf.h"
#include "hall_order_hx.h"
#include "hgraph_order_hf.h"
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
hgraphOrderHf (
const Hgraph * restrict const             grafptr,
Order * restrict const                    ordeptr,
const Gnum                                ordenum, /*+ Zero-based ordering number +*/
OrderCblk * restrict const                cblkptr, /*+ Multiple column-block      +*/
const HgraphOrderHfParam * restrict const paraptr)
{
  Gnum                nbbuck;
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
  Gnum * restrict     headtab;                    /* Head array : nbbuck = 2 * n                 */
  Gnum                ncmpa;
  Gnum                n;                          /* Number of nodes to order (with halo or not) */
  int                 o;

  if (grafptr->s.vertnbr < paraptr->colmin)       /* If graph is too small, order simply */
    return (hgraphOrderSi (grafptr, ordeptr, ordenum, cblkptr));

  n      = grafptr->s.vertnbr;
  nbbuck = n * 2;
  iwlen  = (Gnum) ((double) grafptr->s.edgenbr * HGRAPHORDERHFCOMPRAT) + 32;
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
                     &nexttab, (size_t) (n     * sizeof (Gnum)),
                     &headtab, (size_t) ((nbbuck + 2) * sizeof (Gnum)), NULL) == NULL) {
    errorPrint  ("hgraphOrderHf: out of memory");
    return      (1);
  }

  hgraphOrderHxFill (grafptr, petab, lentab, iwtab, elentab, &pfree);

  hallOrderHfR2hamdf4 (n, 0, nbbuck, iwlen, petab, pfree, /* No elements here */
                       lentab, iwtab, nvartab, elentab, lasttab, &ncmpa,
                       leaftab, secntab, nexttab, frsttab, headtab);
  if (ncmpa < 0) {
    errorPrint ("hgraphOrderHf: internal error");
    memFree    (petab);                           /* Free group leader */
    return     (1);
  }

  o = hallOrderHxBuild (grafptr->s.baseval, n, grafptr->vnohnbr,
                        grafptr->s.vnumtax, ordeptr, cblkptr,
                        nvartab - grafptr->s.baseval,
                        lentab  - grafptr->s.baseval,
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
