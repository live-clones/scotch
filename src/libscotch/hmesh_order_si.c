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
/**   NAME       : hmesh_order_si.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module orders halo mesh vertices   **/
/**                using the natural order.                **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 01 jan 2002     **/
/**                                 to     27 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH_ORDER_SI

#include "module.h"
#include "common.h"
#include "graph.h"
#include "order.h"
#include "mesh.h"
#include "hmesh.h"
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
hmeshOrderSi (
const Hmesh * restrict const  meshptr,
Order * restrict const        ordeptr,
const Gnum                    ordenum,
OrderCblk * restrict const    cblkptr)            /*+ Single column-block +*/
{
  Gnum                vnodnum;
  Gnum                ordeval;

  if (meshptr->m.vnumtax == NULL) {               /* If mesh is original mesh (no halo) */
#ifdef SCOTCH_DEBUG_ORDER2
    if (meshptr->m.vnodnbr != ordeptr->vnodnbr) {
      errorPrint ("hmeshOrderSi: invalid permutation bounds");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_ORDER2 */
    for (vnodnum = ordeptr->baseval, ordeval = ordenum;
         vnodnum < ordeptr->baseval + ordeptr->vnodnbr; vnodnum ++, ordeval ++) {
      ordeptr->peritab[ordeval] = vnodnum;
    }
  }
  else {                                          /* Mesh is not original mesh */
    for (vnodnum = meshptr->m.vnodbas, ordeval = ordenum;
         vnodnum < meshptr->vnohnnd; vnodnum ++, ordeval ++) {
      ordeptr->peritab[ordeval] = meshptr->m.vnumtax[vnodnum];
#ifdef SCOTCH_DEBUG_ORDER2
      if ((ordeptr->peritab[ordeval] <   ordeptr->baseval) ||
          (ordeptr->peritab[ordeval] >= (ordeptr->baseval + ordeptr->vnodnbr))) {
        errorPrint ("hmeshOrderSi: invalid permutation index");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_ORDER2 */
    }
  }

  return (0);
}
