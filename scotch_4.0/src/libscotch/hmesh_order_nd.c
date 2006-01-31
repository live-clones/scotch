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
/**   NAME       : hmesh_order_nd.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module orders mesh nodes using the **/
/**                nested dissection algorithm.            **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 06 jan 2002     **/
/**                                 to     05 jan 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH_ORDER_ND

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "order.h"
#include "mesh.h"
#include "hmesh.h"
#include "hmesh_order_nd.h"
#include "hmesh_order_st.h"
#include "vmesh.h"
#include "vmesh_separate_st.h"

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
hmeshOrderNd (
const Hmesh * restrict const      meshptr,
Order * restrict const            ordeptr,
const Gnum                        ordenum,
OrderCblk * restrict const        cblkptr,
const HmeshOrderNdParam * const   paraptr)
{
  Hmesh                     indmeshdat;           /* Induced halo mesh data */
  Vmesh                     nspmeshdat;           /* Node separation mesh   */
  Gnum                      vertnbr;
  int                       o;

  if (hmeshMesh (meshptr, &nspmeshdat.m) != 0) {
    errorPrint ("hmeshOrderNd: cannot create node separation mesh");
    return     (1);
  }
  nspmeshdat.ecmpsize[0] = nspmeshdat.m.velmnbr;
  nspmeshdat.ecmpsize[1] = 0;
  nspmeshdat.ncmpload[0] = nspmeshdat.m.vnlosum;
  nspmeshdat.ncmpload[1] = 0;
  nspmeshdat.ncmpload[2] = 0;
  nspmeshdat.ncmploaddlt = nspmeshdat.m.vnlosum;
  nspmeshdat.ncmpsize[0] = nspmeshdat.m.vnodnbr;
  nspmeshdat.ncmpsize[1] = 0;
  nspmeshdat.fronnbr     = 0;
  nspmeshdat.levlnum     = meshptr->levlnum;

  vertnbr = nspmeshdat.m.velmnbr + nspmeshdat.m.vnodnbr;
  if (memAllocGroup ((void **)
                      &nspmeshdat.parttax, (size_t) (vertnbr * sizeof (GraphPart)),
                      &nspmeshdat.frontab, (size_t) (vertnbr * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("hmeshOrderNd: out of memory (1)");
    return     (1);
  }
  memSet (nspmeshdat.parttax, 0, vertnbr * sizeof (GraphPart)); /* Set all vertices to part 0 */
  nspmeshdat.parttax -= nspmeshdat.m.baseval;

  if (vmeshSeparateSt (&nspmeshdat, paraptr->sepstrat) != 0) { /* Separate mesh */
    vmeshExit (&nspmeshdat);
    return    (1);
  }
printf ("HN Sepa\tsize=%ld\tload=%ld\tbal=%ld\n", (long) nspmeshdat.fronnbr, (long) nspmeshdat.ncmpload[2], (long) nspmeshdat.ncmploaddlt);

  if ((nspmeshdat.ncmpsize[0] == 0) ||            /* If could not separate more */
      (nspmeshdat.ncmpsize[1] == 0)) {
    vmeshExit (&nspmeshdat);

    return (hmeshOrderSt (meshptr, ordeptr, ordenum, cblkptr, paraptr->ordstratlea)); /* Order this leaf */
  }

  cblkptr->typeval = ORDERCBLKNEDI;               /* Node becomes a nested dissection node */
  if ((cblkptr->cblktab = (OrderCblk *) memAlloc (3 * sizeof (OrderCblk))) == NULL) {
    errorPrint ("hmeshOrderNd: out of memory (2)");
    vmeshExit  (&nspmeshdat);
    return     (1);
  }
  cblkptr->cblktab[0].typeval = ORDERCBLKOTHR;    /* Build column blocks */
  cblkptr->cblktab[0].vnodnbr = nspmeshdat.ncmpsize[0];
  cblkptr->cblktab[0].cblknbr = 0;
  cblkptr->cblktab[0].cblktab = NULL;
  cblkptr->cblktab[1].typeval = ORDERCBLKOTHR;
  cblkptr->cblktab[1].vnodnbr = nspmeshdat.ncmpsize[1];
  cblkptr->cblktab[1].cblknbr = 0;
  cblkptr->cblktab[1].cblktab = NULL;
  cblkptr->cblktab[2].vnodnbr = nspmeshdat.ncmpsize[2];
  cblkptr->cblktab[2].cblknbr = 0;
  cblkptr->cblktab[2].cblktab = NULL;

  if (nspmeshdat.ncmpsize[2] != 0) {              /* If separator not empty         */
    cblkptr->cblknbr  = 3;                        /* It is a three-cell tree node   */
    ordeptr->cblknbr += 2;                        /* Two more column blocks created */

    cblkptr->cblktab[2].typeval = ORDERCBLKOTHR;
    cblkptr->cblktab[2].vnodnbr = nspmeshdat.ncmpsize[2];
    cblkptr->cblktab[2].cblknbr = 0;
    cblkptr->cblktab[2].cblktab = NULL;

    if (meshInduceSepa (&nspmeshdat.m, nspmeshdat.parttax, nspmeshdat.fronnbr, nspmeshdat.frontab, &indmeshdat.m) != 0) {
      errorPrint ("hmeshOrderNd: cannot build induced subgraph (1)");
      memFree    (nspmeshdat.frontab);            /* Free remaining space */
      return     (1);
    }
    indmeshdat.vnohnbr = indmeshdat.m.vnodnbr;    /* Fill halo mesh structure of non-halo mesh */
    indmeshdat.vnohnnd = indmeshdat.m.vnodnnd;
    indmeshdat.vehdtax = indmeshdat.m.vendtax;
    indmeshdat.vnhlsum = indmeshdat.m.vnlosum;
    indmeshdat.enohnbr = indmeshdat.m.edgenbr;
    indmeshdat.levlnum = meshptr->levlnum;        /* Separator mesh is at level of original mesh */

    o = hmeshOrderSt (&indmeshdat, ordeptr, ordenum + nspmeshdat.ncmpsize[0] + nspmeshdat.ncmpsize[1],
                      cblkptr->cblktab + 2, paraptr->ordstratsep);
    hmeshExit (&indmeshdat);
  }
  else {                                          /* Separator is empty            */
    cblkptr->cblknbr = 2;                         /* It is a two-cell tree node    */
    ordeptr->cblknbr ++;                          /* One more column block created */
    o = 0;                                        /* Everything went fine till now */
  }
  if (o == 0) {
    if (hmeshInducePart (meshptr, nspmeshdat.parttax, 0, nspmeshdat.ecmpsize[0],
                         nspmeshdat.ncmpsize[0], nspmeshdat.ncmpsize[2], &indmeshdat) != 0) {
      errorPrint ("hmeshOrderNd: cannot build induced subgraph (2)");
      memFree    (nspmeshdat.frontab);            /* Free remaining space */
      return     (1);
    }
    o = hmeshOrderNd (&indmeshdat, ordeptr, ordenum, cblkptr->cblktab, paraptr);
    hmeshExit (&indmeshdat);
  }
  if (o == 0) {
    if (hmeshInducePart (meshptr, nspmeshdat.parttax, 1, nspmeshdat.ecmpsize[1],
                         nspmeshdat.ncmpsize[1], nspmeshdat.ncmpsize[2], &indmeshdat) != 0) {
      errorPrint ("hmeshOrderNd: cannot build induced subgraph (3)");
      memFree    (nspmeshdat.frontab);            /* Free remaining space */
      return     (1);
    }
    o = hmeshOrderNd (&indmeshdat, ordeptr, ordenum + nspmeshdat.ncmpsize[0], cblkptr->cblktab + 1, paraptr);
    hmeshExit (&indmeshdat);
  }

  vmeshExit (&nspmeshdat);

  return (0);
}
