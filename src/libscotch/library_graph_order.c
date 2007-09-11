/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
*/
/************************************************************/
/**                                                        **/
/**   NAME       : library_graph_order.c                   **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the graph    **/
/**                ordering routines of the libSCOTCH      **/
/**                library.                                **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 19 aug 1998     **/
/**                                 to     22 aug 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     27 mar 1999     **/
/**                # Version 4.0  : from : 29 jan 2002     **/
/**                                 to     08 sep 2006     **/
/**                # Version 5.0  : from : 19 dec 2006     **/
/**                                 to     04 aug 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "order.h"
#include "hgraph.h"
#include "hgraph_order_st.h"
#include "library_order.h"
#include "scotch.h"

/************************************/
/*                                  */
/* These routines are the C API for */
/* the graph ordering routines.     */
/*                                  */
/************************************/

/*+ This routine parses the given
*** graph ordering strategy.
*** It returns:
*** - 0   : if string successfully scanned.
*** - !0  : on error.
+*/

int
SCOTCH_stratGraphOrder (
SCOTCH_Strat * const        stratptr,
const char * const          string)
{
  if (*((Strat **) stratptr) != NULL)
    stratExit (*((Strat **) stratptr));

  if ((*((Strat **) stratptr) = stratInit (&hgraphorderststratab, string)) == NULL) {
    errorPrint ("SCOTCH_stratGraphOrder: error in ordering strategy");
    return     (1);
  }

  return (0);
}

/*+ This routine initializes an API ordering
*** with respect to the given source graph
*** and the locations of output parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderInit (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to order                     +*/
SCOTCH_Ordering * const     ordeptr,              /*+ Ordering structure to initialize   +*/
SCOTCH_Num * const          permtab,              /*+ Direct permutation array           +*/
SCOTCH_Num * const          peritab,              /*+ Inverse permutation array          +*/
SCOTCH_Num * const          cblkptr,              /*+ Pointer to number of column blocks +*/
SCOTCH_Num * const          rangtab,              /*+ Column block range array           +*/
SCOTCH_Num * const          treetab)              /*+ Separator tree array               +*/
{
  Graph *             srcgrafptr;
  LibOrder *          libordeptr;

#ifdef SCOTCH_DEBUG_LIBRARY1
  if (sizeof (SCOTCH_Ordering) < sizeof (LibOrder)) {
    errorPrint ("SCOTCH_graphOrderInit: internal error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_LIBRARY1 */

  srcgrafptr = (Graph *) grafptr;                 /* Use structure as source graph */
  libordeptr = (LibOrder *) ordeptr;
  libordeptr->permtab = ((permtab == NULL) || ((void *) permtab == (void *) grafptr)) ? NULL : (Gnum *) permtab;
  libordeptr->peritab = ((peritab == NULL) || ((void *) peritab == (void *) grafptr)) ? NULL : (Gnum *) peritab;
  libordeptr->cblkptr = ((cblkptr == NULL) || ((void *) cblkptr == (void *) grafptr)) ? NULL : (Gnum *) cblkptr;
  libordeptr->rangtab = ((rangtab == NULL) || ((void *) rangtab == (void *) grafptr)) ? NULL : (Gnum *) rangtab;
  libordeptr->treetab = ((treetab == NULL) || ((void *) treetab == (void *) grafptr)) ? NULL : (Gnum *) treetab;

  return (orderInit (&libordeptr->o, srcgrafptr->baseval, srcgrafptr->vertnbr, libordeptr->peritab));
}

/*+ This routine frees an API ordering.
*** It returns:
*** - VOID  : in all cases.
+*/

void
SCOTCH_graphOrderExit (
const SCOTCH_Graph * const  grafptr,
SCOTCH_Ordering * const     ordeptr)
{
  orderExit (&((LibOrder *) ordeptr)->o);
}

/*+ This routine loads the contents of
*** the given ordering from the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderLoad (
const SCOTCH_Graph * const        grafptr,        /*+ Graph to order   +*/
SCOTCH_Ordering * restrict const  ordeptr,        /*+ Ordering to load +*/
FILE * restrict const             stream)         /*+ Output stream    +*/
{
  Graph *             srcgrafptr;
  LibOrder *          libordeptr;

  srcgrafptr = (Graph *) grafptr;
  libordeptr = (LibOrder *) ordeptr;

  if (orderLoad (&libordeptr->o, srcgrafptr->vlbltax, stream) != 0)
    return (1);

  if (libordeptr->permtab != NULL)                /* Build inverse permutation if wanted */
    orderPeri (libordeptr->o.peritab, srcgrafptr->baseval, libordeptr->o.vnodnbr, libordeptr->permtab, srcgrafptr->baseval);

  return (0);
}

/*+ This routine saves the contents of
*** the given ordering to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderSave (
const SCOTCH_Graph * const    grafptr,            /*+ Graph to order   +*/
const SCOTCH_Ordering * const ordeptr,            /*+ Ordering to save +*/
FILE * const                  stream)             /*+ Output stream    +*/
{
  return (orderSave (&((LibOrder *) ordeptr)->o, ((Graph *) grafptr)->vlbltax + ((Graph *) grafptr)->baseval, stream));
}

/*+ This routine saves to the given stream
*** the mapping data associated with the
*** given ordering.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderSaveMap (
const SCOTCH_Graph * const    grafptr,            /*+ Graph to order   +*/
const SCOTCH_Ordering * const ordeptr,            /*+ Ordering to save +*/
FILE * const                  stream)             /*+ Output stream    +*/
{
  return (orderSaveMap (&((LibOrder *) ordeptr)->o, ((Graph *) grafptr)->vlbltax, stream));
}

/*+ This routine saves to the given stream
*** the separator tree data associated with
*** the given ordering.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderSaveTree (
const SCOTCH_Graph * const    grafptr,            /*+ Graph to order   +*/
const SCOTCH_Ordering * const ordeptr,            /*+ Ordering to save +*/
FILE * const                  stream)             /*+ Output stream    +*/
{
  return (orderSaveTree (&((LibOrder *) ordeptr)->o, ((Graph *) grafptr)->vlbltax, stream));
}

/*+ This routine computes an ordering
*** of the API ordering structure with
*** respect to the given strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderCompute (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to order      +*/
SCOTCH_Ordering * const     ordeptr,              /*+ Ordering to compute +*/
const SCOTCH_Strat * const  stratptr)             /*+ Ordering strategy   +*/
{
  return (SCOTCH_graphOrderComputeList (grafptr, ordeptr, 0, NULL, stratptr));
}

/*+ This routine computes a partial ordering
*** of the listed vertices of the API ordering
*** structure graph with respect to the given
*** strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderComputeList (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to order                  +*/
SCOTCH_Ordering * const     ordeptr,              /*+ Ordering to compute             +*/
const SCOTCH_Num            listnbr,              /*+ Number of vertices in list      +*/
const SCOTCH_Num * const    listtab,              /*+ List of vertex indices to order +*/
const SCOTCH_Strat * const  stratptr)             /*+ Ordering strategy               +*/
{
  LibOrder *          libordeptr;                 /* Pointer to ordering             */
  Hgraph              srcgrafdat;                 /* Halo source graph structure     */
  VertList            srclistdat;                 /* Subgraph vertex list            */
  VertList *          srclistptr;                 /* Pointer to subgraph vertex list */
  const Strat *       ordstratptr;                /* Pointer to ordering strategy    */

  if (*((Strat **) stratptr) == NULL)             /* Set default ordering strategy if necessary */
    *((Strat **) stratptr) = stratInit (&hgraphorderststratab, "c{rat=0.7,cpr=n{sep=/(vert>120)?m{type=h,rat=0.7,vert=100,low=h{pass=10},asc=b{width=3,strat=f{bal=0.2}}}|m{type=h,rat=0.7,vert=100,low=h{pass=10},asc=b{width=3,strat=f{bal=0.2}}};,ole=f{cmin=0,cmax=100000,frat=0.0},ose=g},unc=n{sep=/(vert>120)?m{type=h,rat=0.7,vert=100,low=h{pass=10},asc=b{width=3,strat=f{bal=0.2}}}|m{type=h,rat=0.7,vert=100,low=h{pass=10},asc=b{width=3,strat=f{bal=0.2}}};,ole=f{cmin=15,cmax=100000,frat=0.0},ose=g}}");
  ordstratptr = *((Strat **) stratptr);
  if (ordstratptr->tabl != &hgraphorderststratab) {
    errorPrint ("SCOTCH_graphOrderComputeList: not an ordering strategy");
    return     (1);
  }

  memCpy (&srcgrafdat.s, grafptr, sizeof (Graph)); /* Copy non-halo graph data   */
  srcgrafdat.s.flagval &= ~GRAPHFREETABS;         /* Do not allow to free arrays */
  srcgrafdat.s.edlotax  = NULL;                   /* Never mind about edge loads */
  srcgrafdat.vnohnbr    = srcgrafdat.s.vertnbr;   /* All vertices are non-halo   */
  srcgrafdat.vnohnnd    = srcgrafdat.s.vertnnd;   /* No halo present             */
  srcgrafdat.vnhdtax    = srcgrafdat.s.vendtax;   /* End of non-halo vertices    */
  srcgrafdat.vnlosum    = srcgrafdat.s.velosum;   /* Sum of node vertex weights  */
  srcgrafdat.enohnbr    = srcgrafdat.s.edgenbr;   /* No halo present             */
  srcgrafdat.enohsum    = srcgrafdat.s.edlosum;
  srcgrafdat.levlnum    = 0;                      /* No nested dissection yet */

  libordeptr         = (LibOrder *) ordeptr;      /* Get ordering      */
  srclistdat.vnumnbr = (Gnum)   listnbr;          /* Build vertex list */
  srclistdat.vnumtab = (Gnum *) listtab;
  srclistptr = ((srclistdat.vnumnbr == 0) ||
                (srclistdat.vnumnbr == ((Graph *) grafptr)->vertnbr))
               ? NULL : &srclistdat;              /* Is the list really necessary */

/* TODO: Take list into account */
  hgraphOrderSt (&srcgrafdat, &libordeptr->o, 0, &libordeptr->o.cblktre, ordstratptr);

#ifdef SCOTCH_DEBUG_LIBRARY2
  if (orderCheck (&libordeptr->o) != 0)
    return (1);
#endif /* SCOTCH_DEBUG_LIBRARY2 */

  if (libordeptr->permtab != NULL)                /* Build direct permutation if wanted */
    orderPeri (libordeptr->o.peritab, srcgrafdat.s.baseval, libordeptr->o.vnodnbr, libordeptr->permtab, srcgrafdat.s.baseval);
  if (libordeptr->rangtab != NULL)                /* Build range array if column block data wanted */
    orderRang (&libordeptr->o, libordeptr->rangtab);
  if (libordeptr->treetab != NULL)                /* Build separator tree array if wanted */
      orderTree (&libordeptr->o, libordeptr->treetab);
  if (libordeptr->cblkptr != NULL)                /* Set number of column blocks if wanted */
    *(libordeptr->cblkptr) = libordeptr->o.cblknbr;

  return (0);
}

/*+ This routine computes an ordering
*** of the API ordering structure with
*** respect to the given strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrder (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to order                     +*/
const SCOTCH_Strat * const  stratptr,             /*+ Ordering strategy                  +*/
SCOTCH_Num * const          permtab,              /*+ Ordering permutation               +*/
SCOTCH_Num * const          peritab,              /*+ Inverse permutation array          +*/
SCOTCH_Num * const          cblkptr,              /*+ Pointer to number of column blocks +*/
SCOTCH_Num * const          rangtab,              /*+ Column block range array           +*/
SCOTCH_Num * const          treetab)              /*+ Separator tree array               +*/
{
  SCOTCH_Ordering     ordedat;
  int                 o;

  if (SCOTCH_graphOrderInit (grafptr, &ordedat, permtab, peritab, cblkptr, rangtab, treetab) != 0)
    return (1);

  o = SCOTCH_graphOrderCompute (grafptr, &ordedat, stratptr);
  SCOTCH_graphOrderExit (grafptr, &ordedat);

  return (o);
}

/*+ This routine computes an ordering
*** of the subgraph of the API ordering
*** structure graph induced by the given
*** vertex list, with respect to the given
*** strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderList (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to order                     +*/
const SCOTCH_Num            listnbr,              /*+ Number of vertices in list         +*/
const SCOTCH_Num * const    listtab,              /*+ List of vertex indices to order    +*/
const SCOTCH_Strat * const  stratptr,             /*+ Ordering strategy                  +*/
SCOTCH_Num * const          permtab,              /*+ Ordering permutation               +*/
SCOTCH_Num * const          peritab,              /*+ Inverse permutation array          +*/
SCOTCH_Num * const          cblkptr,              /*+ Pointer to number of column blocks +*/
SCOTCH_Num * const          rangtab,              /*+ Column block range array           +*/
SCOTCH_Num * const          treetab)              /*+ Column block range array           +*/
{
  SCOTCH_Ordering     ordedat;
  int                 o;

  SCOTCH_graphOrderInit (grafptr, &ordedat, permtab, peritab, cblkptr, rangtab, treetab);
  o = SCOTCH_graphOrderComputeList (grafptr, &ordedat, listnbr, listtab, stratptr);
  SCOTCH_graphOrderExit (grafptr, &ordedat);

  return (o);
}

/*+ This routine checks the consistency
*** of the given graph ordering.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphOrderCheck (
const SCOTCH_Graph * const    grafptr,
const SCOTCH_Ordering * const ordeptr)            /*+ Ordering to check +*/
{
  return (orderCheck (&((LibOrder *) ordeptr)->o));
}
