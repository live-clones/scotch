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
/**   NAME       : hmesh_order_bl.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module resizes block data using    **/
/**                the block splitting post-processing     **/
/**                algorithm.                              **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 28 sep 2002     **/
/**                                 to     09 feb 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH_ORDER_BL

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "order.h"
#include "mesh.h"
#include "hmesh.h"
#include "hmesh_order_bl.h"
#include "hmesh_order_st.h"

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
hmeshOrderBl (
const Hmesh * restrict const              meshptr,
Order * restrict const                    ordeptr,
const Gnum                                ordenum, /*+ Zero-based ordering number +*/
OrderCblk * restrict const                cblkptr, /*+ Single column-block        +*/
const HmeshOrderBlParam * restrict const  paraptr)
{
  Gnum                cblknbr;                    /* Number of old column blocks before splitting */
  Gnum                cblknum;                    /* Number of current column block               */

  if (paraptr->cblkmin <= 0) {
    errorPrint ("hmeshOrderBl: invalid minimum block size");
    return     (1);
  }

  if (hmeshOrderSt (meshptr, ordeptr, ordenum, cblkptr, paraptr->strat) != 0) /* Perform ordering strategy */
    return (1);

  if (cblkptr->cblktab == NULL) {                 /* If single column block    */
    if (cblkptr->vnodnbr < (2 * paraptr->cblkmin)) /* If block cannot be split */
	return (0);

    cblknbr = cblkptr->vnodnbr / paraptr->cblkmin; /* Get new number of blocks */

    if ((cblkptr->cblktab = (OrderCblk *) memAlloc (cblknbr * sizeof (OrderCblk))) == NULL) {
      errorPrint ("hgraphOrderBl: out of memory");
      return     (1);
    }
    ordeptr->cblknbr += cblknbr - 1;
    cblkptr->cblknbr  = cblknbr;

    for (cblknum = 0; cblknum < cblknbr; cblknum ++) {
      cblkptr->cblktab[cblknum].typeval = ORDERCBLKOTHR;
      cblkptr->cblktab[cblknum].vnodnbr = ((cblkptr->vnodnbr + cblknbr - 1) - cblknum) / cblknbr;
      cblkptr->cblktab[cblknum].cblknbr = 0;
      cblkptr->cblktab[cblknum].cblktab = NULL;
    }
  }
  else {                                          /* Block already partitioned */
    for (cblknum = 0; cblknum < cblkptr->cblknbr; cblknum ++) {
      if (hmeshOrderBl (meshptr, ordeptr, ordenum, cblkptr->cblktab + cblknum, paraptr) != 0)
        return (1);
    }
  }

  return (0);
}
