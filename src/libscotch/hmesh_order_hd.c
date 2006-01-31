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
/**   NAME       : hmesh_order_hd.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module orders a submesh using      **/
/**                the block-oriented Halo Approximate     **/
/**                (Multiple) Minimum Degree algorithm,    **/
/**                with super-variable accounting          **/
/**                R2HAMDf4 v2.0).                         **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 10 dec 2003     **/
/**                                 to   : 24 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH_ORDER_HD

#include "module.h"
#include "common.h"
#include "graph.h"
#include "order.h"
#include "mesh.h"
#include "hmesh.h"
#include "hall_order_hd.h"
#include "hall_order_hx.h"
#include "hmesh_order_hd.h"
#include "hmesh_order_hx.h"
#include "hmesh_order_si.h"

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
hmeshOrderHd (
const Hmesh * restrict const              meshptr,
Order * restrict const                    ordeptr,
const Gnum                                ordenum,
OrderCblk * restrict const                cblkptr, /*+ Single column-block +*/
const HmeshOrderHdParam * restrict const  paraptr)
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
  Gnum                n;                          /* Number of nodes to order */
  int                 o;

  n = meshptr->m.velmnbr + meshptr->m.vnodnbr;

  if (n < paraptr->colmin)                        /* If graph is too small, order simply */
    return (hmeshOrderSi (meshptr, ordeptr, ordenum, cblkptr));

  iwlen  = (Gnum) ((double) meshptr->m.edgenbr * HMESHORDERHDCOMPRAT) + 32;
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
    errorPrint  ("hmeshOrderHd: out of memory");
    return      (1);
  }

  hmeshOrderHxFill (meshptr, petab, lentab, iwtab, elentab, &pfree);

  hallOrderHdHalmd (n, meshptr->m.velmnbr, iwlen, petab, pfree,
                    lentab, iwtab, nvartab, elentab, lasttab, &ncmpa,
                    leaftab, secntab, nexttab, frsttab);

  if (ncmpa < 0) {
    errorPrint ("hmeshOrderHd: internal error");
    memFree    (petab);                           /* Free group leader */
    return     (1);
  }

  o = hallOrderHxBuild (meshptr->m.baseval, n, meshptr->vnohnbr,
                        (meshptr->m.vnumtax == NULL) ? NULL : meshptr->m.vnumtax + (meshptr->m.vnodbas - meshptr->m.baseval), /* Point to node part of vnumtab array */
                        ordeptr, cblkptr,
                        nvartab - meshptr->m.baseval,
                        lentab  - meshptr->m.baseval,
                        petab   - meshptr->m.baseval,
                        frsttab - meshptr->m.baseval,
                        nexttab - meshptr->m.baseval,
                        secntab - meshptr->m.baseval,
                        iwtab   - meshptr->m.baseval,
                        elentab - meshptr->m.baseval,
                        ordeptr->peritab + ordenum, /* Use given inverse permutation as inverse permutation space, never based */
                        leaftab,
                        paraptr->colmin, paraptr->colmax, (float) paraptr->fillrat);

  memFree (petab);                                /* Free group leader */

  return (o);
}
