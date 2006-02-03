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
/**   NAME       : order_scotch_mesh.c                     **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This is the interface module with the   **/
/**                libSCOTCH matrix ordering library.      **/
/**                                                        **/
/**   DATES      : # Version 1.0  : from : 25 oct 2003     **/
/**                                 to     19 nov 2003     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     04 jan 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ORDER_MESH

#include "common.h"
#include "scotch.h"
#include "mesh.h"
#include "order.h"

/***************************/
/*                         */
/* Mesh ordering routines. */
/*                         */
/***************************/

/*+ This routine orders the given
*** mesh using the Emilio default
*** ordering strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderMesh (
Order * restrict const        ordeptr,            /*+ Ordering to compute  +*/
const Mesh * restrict const   meshptr)            /*+ Mesh matrix to order +*/
{
  return (orderMeshList (ordeptr, meshptr, 0, NULL));
}

/*+ This routine orders the submesh of
*** the given mesh induced by the given
*** node vertex list, using the Emilio
*** default ordering strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderMeshList (
Order * restrict const        ordeptr,            /*+ Ordering to compute        +*/
const Mesh * restrict const   meshptr,            /*+ Mesh matrix to order       +*/
const INT                     listnbr,            /*+ Number of vertices in list +*/
const INT * restrict const    listtab)            /*+ Vertex list array          +*/
{
  return (orderMeshListStrat (ordeptr, meshptr, listnbr, listtab,
                               "n{sep=/(vnod>20)?(m{vert=50,low=h{pass=10},asc=f{bal=0.2}}f{bal=0.1})|m{vert=50,low=h{pass=10},asc=f{bal=0.1}};,ole=s,ose=s}"));
  /*                               "n{sep=/(vnod>120)?(m{vert=50,low=h{pass=10},asc=f{bal=0.2}}f{bal=0.1})|m{vert=50,low=h{pass=10},asc=f{bal=0.1}};,ole=h{cmin=0,cmax=10000,frat=0.08},ose=g}")); */
}

/*+ This routine orders the given
*** mesh using the given ordering
*** strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderMeshStrat (
Order * restrict const      ordeptr,              /*+ Ordering to compute  +*/
const Mesh * const          meshptr,              /*+ Mesh matrix to order +*/
const char * const          stratptr)             /*+ Ordering strategy    +*/
{
  return (orderMeshListStrat (ordeptr, meshptr, 0, NULL, stratptr));
}

/*+ This routine orders the submesh of
*** the given mesh induced by the given
*** node vertex list, using the given
*** ordering strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderMeshListStrat (
Order * restrict const        ordeptr,            /*+ Ordering to compute        +*/
const Mesh * restrict const   meshptr,            /*+ Mesh matrix to order       +*/
const INT                     listnbr,            /*+ Number of vertices in list +*/
const INT * restrict const    listtab,            /*+ Vertex list array          +*/
const char * restrict const   stratptr)           /*+ Ordering strategy          +*/
{
  SCOTCH_Strat        scotstrat;                  /* Scotch ordering strategy */
  INT                 baseval;
  INT                 velmbas;
  INT                 vnodbas;
  INT                 vnodnbr;
  INT                 edgenbr;
  int                 o;

  if (sizeof (INT) != sizeof (SCOTCH_Num)) {      /* Check integer consistency */
    errorPrint ("orderMeshListStrat: inconsistent integer types");
    return     (1);
  }

  SCOTCH_meshData (meshptr, &velmbas, &vnodbas, NULL, &vnodnbr,
                   NULL, NULL, NULL, NULL, NULL, &edgenbr, NULL, NULL);
  baseval = MIN (velmbas, vnodbas);

  if (((ordeptr->permtab = (INT *) memAlloc ( vnodnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->peritab = (INT *) memAlloc ( vnodnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->rangtab = (INT *) memAlloc ((vnodnbr + 1) * sizeof (INT))) == NULL)) {
    errorPrint ("orderMeshListStrat: out of memory");
    orderExit  (ordeptr);
    orderInit  (ordeptr);
    return     (1);
  }

  SCOTCH_stratInit (&scotstrat);                  /* Initialize default ordering strategy */

  o = SCOTCH_stratMeshOrder (&scotstrat, stratptr);
  if (o == 0)
    o = SCOTCH_meshOrderList (meshptr,            /* Compute mesh ordering */
          (SCOTCH_Num) listnbr, (SCOTCH_Num *) listtab, &scotstrat,
          (SCOTCH_Num *) ordeptr->permtab,  (SCOTCH_Num *) ordeptr->peritab,
          (SCOTCH_Num *) &ordeptr->cblknbr, (SCOTCH_Num *) ordeptr->rangtab, NULL);


  SCOTCH_stratExit (&scotstrat);

  if (o != 0) {                                   /* If something failed in Scotch */
    orderExit (ordeptr);                          /* Free ordering arrays          */
    orderInit (ordeptr);
    return    (1);
  }

#ifdef ORDER_DEBUG
  if ((ordeptr->rangtab[0]                != baseval)           ||
      (ordeptr->rangtab[ordeptr->cblknbr] != baseval + vnodnbr) ||
      (orderCheck (ordeptr) != 0)) {
    errorPrint ("orderMeshListStrat: invalid ordering");
    return     (1);
  }
#endif /* ORDER_DEBUG */

  ordeptr->rangtab = memRealloc (ordeptr->rangtab, (ordeptr->cblknbr + 1) * sizeof (INT));

  return (0);
}
