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
/**   NAME       : order_scotch_graph.c                    **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This is the interface module with the   **/
/**                libSCOTCH matrix ordering library.      **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 20 aug 1998     **/
/**                                 to     18 may 1999     **/
/**                # Version 1.0  : from : 18 mar 2003     **/
/**                                 to     21 jan 2004     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     04 jan 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ORDER_GRAPH

#include "common.h"
#include "scotch.h"
#include "graph.h"
#include "order.h"

/****************************/
/*                          */
/* Graph ordering routines. */
/*                          */
/****************************/

/*+ This routine orders the given
*** graph using the Emilio default
*** ordering strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderGraph (
Order * restrict const        ordeptr,            /*+ Ordering to compute   +*/
const Graph * restrict const  grafptr)            /*+ Graph matrix to order +*/
{
  return (orderGraphList (ordeptr, grafptr, 0, NULL));
}

/*+ This routine orders the subgraph of
*** the given graph induced by the given
*** vertex list, using the Emilio default
*** ordering strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderGraphList (
Order * restrict const        ordeptr,            /*+ Ordering to compute        +*/
const Graph * restrict const  grafptr,            /*+ Graph matrix to order      +*/
const INT                     listnbr,            /*+ Number of vertices in list +*/
const INT * restrict const    listtab)              /*+ Vertex list array          +*/
{
  return (orderGraphListStrat (ordeptr, grafptr, listnbr, listtab,
                               "c{rat=0.7,cpr=n{sep=/(vert>120)?(m{vert=50,low=h{pass=10},asc=f{bal=0.2}}f{bal=0.1})|m{vert=50,low=h{pass=10},asc=f{bal=0.1}};,ole=f{cmin=0,cmax=10000,frat=0.08},ose=g},unc=n{sep=/(vert>120)?m{vert=50,low=h{pass=10},asc=f{bal=0.1}}|m{vert=50,low=h{pass=10},asc=f{bal=0.1}};,ole=f{cmin=0,cmax=10000,frat=0.08},ose=g}}"));
}

/*+ This routine orders the given
*** graph using the given ordering
*** strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderGraphStrat (
Order * restrict const        ordeptr,            /*+ Ordering to compute   +*/
const Graph * restrict const  grafptr,            /*+ Graph matrix to order +*/
const char * restrict const   stratptr)           /*+ Ordering strategy     +*/
{
  return (orderGraphListStrat (ordeptr, grafptr, 0, NULL, stratptr));
}

/*+ This routine orders the subgraph of
*** the given graph induced by the given
*** vertex list, using the given ordering
*** strategy.
*** It returns:
*** - 0   : if ordering succeeded.
*** - !0  : on error.
+*/

int
orderGraphListStrat (
Order * restrict const        ordeptr,            /*+ Ordering to compute        +*/
const Graph * restrict const  grafptr,            /*+ Graph matrix to order      +*/
const INT                     listnbr,            /*+ Number of vertices in list +*/
const INT * restrict const    listtab,            /*+ Vertex list array          +*/
const char * restrict const   stratptr)           /*+ Ordering strategy          +*/
{
  SCOTCH_Strat        scotstrat;                  /* Scotch ordering strategy */
  INT                 baseval;
  INT                 vertnbr;
  INT                 edgenbr;
  int                 o;

  if (sizeof (INT) != sizeof (SCOTCH_Num)) {      /* Check integer consistency */
    errorPrint ("orderGraphListStrat: inconsistent integer types");
    return     (1);
  }

  SCOTCH_graphData (grafptr, &baseval, &vertnbr, NULL, NULL, NULL, NULL, &edgenbr, NULL, NULL);

  if (((ordeptr->permtab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->peritab = (INT *) memAlloc ( vertnbr      * sizeof (INT))) == NULL) ||
      ((ordeptr->rangtab = (INT *) memAlloc ((vertnbr + 1) * sizeof (INT))) == NULL)) {
    errorPrint ("orderGraphListStrat: out of memory");
    orderExit  (ordeptr);
    orderInit  (ordeptr);
    return     (1);
  }

  SCOTCH_stratInit (&scotstrat);                  /* Initialize default ordering strategy */

  o = SCOTCH_stratGraphOrder (&scotstrat, stratptr);
  if (o == 0)
    o = SCOTCH_graphOrderList (grafptr,           /* Compute graph ordering */
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
      (ordeptr->rangtab[ordeptr->cblknbr] != baseval + vertnbr) ||
      (orderCheck (ordeptr) != 0)) {
    errorPrint ("orderGraphListStrat: invalid ordering");
  }
#endif /* ORDER_DEBUG */

  ordeptr->rangtab = memRealloc (ordeptr->rangtab, (ordeptr->cblknbr + 1) * sizeof (INT));

  return (0);
}
