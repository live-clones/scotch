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
/**   NAME       : hgraph_order_st.c                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the generic call to the  **/
/**                graph ordering module, using a given    **/
/**                strategy.                               **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 19 oct 1996     **/
/**                                 to     09 sep 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     07 sep 2001     **/
/**                # Version 4.0  : from : 27 dec 2001     **/
/**                                 to     05 jan 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HGRAPH_ORDER_ST

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "order.h"
#include "hgraph.h"
#include "hgraph_order_bl.h"
#include "hgraph_order_cp.h"
#include "hgraph_order_gp.h"
#include "hgraph_order_hd.h"
#include "hgraph_order_hf.h"
#include "hgraph_order_nd.h"
#include "hgraph_order_si.h"
#include "hgraph_order_st.h"
#include "vgraph.h"
#include "vgraph_separate_st.h"

/*
**  The static and global variables.
*/

static Hgraph               hgraphorderstgraphdummy; /* Dummy graph for offset computations */

static union {                                    /* Default parameters for block splitting method */
  HgraphOrderBlParam        param;                /* Parameter zone                                */
  StratNodeMethodData       padding;              /* To avoid reading out of structure             */
} hgraphorderstdefaultbl = { { &stratdummy, 8 } };

static union {
  HgraphOrderCpParam        param;
  StratNodeMethodData       padding;
} hgraphorderstdefaultcp = { { 0.70L, &stratdummy, &stratdummy } };

static union {
  HgraphOrderGpParam        param;
  StratNodeMethodData       padding;
} hgraphorderstdefaultgp = { { 3 } };

static union {
  HgraphOrderHdParam        param;
  StratNodeMethodData       padding;
} hgraphorderstdefaulthd = { { 1, 10000, 0.08L } };

static union {
  HgraphOrderHfParam        param;
  StratNodeMethodData       padding;
} hgraphorderstdefaulthf = { { 1, 1000000, 0.08L } };

static union {                                    /* Default parameters for nested dissection method */
  HgraphOrderNdParam        param;
  StratNodeMethodData       padding;
} hgraphorderstdefaultnd = { { &stratdummy, &stratdummy, &stratdummy } };

static StratMethodTab       hgraphorderstmethtab[] = { /* Graph ordering methods array */
                              { HGRAPHORDERSTMETHBL, "b",  hgraphOrderBl, &hgraphorderstdefaultbl },
                              { HGRAPHORDERSTMETHCP, "c",  hgraphOrderCp, &hgraphorderstdefaultcp },
                              { HGRAPHORDERSTMETHGP, "g",  hgraphOrderGp, &hgraphorderstdefaultgp },
                              { HGRAPHORDERSTMETHHD, "d",  hgraphOrderHd, &hgraphorderstdefaulthd },
                              { HGRAPHORDERSTMETHHF, "f",  hgraphOrderHf, &hgraphorderstdefaulthf },
                              { HGRAPHORDERSTMETHND, "n",  hgraphOrderNd, &hgraphorderstdefaultnd },
                              { HGRAPHORDERSTMETHSI, "s",  hgraphOrderSi, NULL },
                              { -1,                  NULL, NULL,          NULL } };

static StratParamTab        hgraphorderstparatab[] = { /* The method parameter list */
                              { HGRAPHORDERSTMETHBL,  STRATPARAMSTRAT,  "strat",
                                (byte *) &hgraphorderstdefaultbl.param,
                                (byte *) &hgraphorderstdefaultbl.param.strat,
                                (void *) &hgraphorderststratab },
                              { HGRAPHORDERSTMETHBL,  STRATPARAMINT,    "cmin",
                                (byte *) &hgraphorderstdefaultbl.param,
                                (byte *) &hgraphorderstdefaultbl.param.cblkmin,
                                NULL },
                              { HGRAPHORDERSTMETHCP,  STRATPARAMDOUBLE, "rat",
                                (byte *) &hgraphorderstdefaultcp.param,
                                (byte *) &hgraphorderstdefaultcp.param.comprat,
                                NULL },
                              { HGRAPHORDERSTMETHCP,  STRATPARAMSTRAT,  "cpr",
                                (byte *) &hgraphorderstdefaultcp.param,
                                (byte *) &hgraphorderstdefaultcp.param.stratcpr,
                                (void *) &hgraphorderststratab },
                              { HGRAPHORDERSTMETHCP,  STRATPARAMSTRAT,  "unc",
                                (byte *) &hgraphorderstdefaultcp.param,
                                (byte *) &hgraphorderstdefaultcp.param.stratunc,
                                (void *) &hgraphorderststratab },
                              { HGRAPHORDERSTMETHGP,  STRATPARAMINT,    "pass",
                                (byte *) &hgraphorderstdefaultgp.param,
                                (byte *) &hgraphorderstdefaultgp.param.passnbr,
                                NULL },
                              { HGRAPHORDERSTMETHHD,  STRATPARAMINT,    "cmin",
                                (byte *) &hgraphorderstdefaulthd.param,
                                (byte *) &hgraphorderstdefaulthd.param.colmin,
                                NULL },
                              { HGRAPHORDERSTMETHHD,  STRATPARAMINT,    "cmax",
                                (byte *) &hgraphorderstdefaulthd.param,
                                (byte *) &hgraphorderstdefaulthd.param.colmax,
                                NULL },
                              { HGRAPHORDERSTMETHHD,  STRATPARAMDOUBLE, "frat",
                                (byte *) &hgraphorderstdefaulthd.param,
                                (byte *) &hgraphorderstdefaulthd.param.fillrat,
                                NULL },
                              { HGRAPHORDERSTMETHHF,  STRATPARAMINT,    "cmin",
                                (byte *) &hgraphorderstdefaulthf.param,
                                (byte *) &hgraphorderstdefaulthf.param.colmin,
                                NULL },
                              { HGRAPHORDERSTMETHHF,  STRATPARAMINT,    "cmax",
                                (byte *) &hgraphorderstdefaulthf.param,
                                (byte *) &hgraphorderstdefaulthf.param.colmax,
                                NULL },
                              { HGRAPHORDERSTMETHHF,  STRATPARAMDOUBLE, "frat",
                                (byte *) &hgraphorderstdefaulthf.param,
                                (byte *) &hgraphorderstdefaulthf.param.fillrat,
                                NULL },
                              { HGRAPHORDERSTMETHND,  STRATPARAMSTRAT,  "sep",
                                (byte *) &hgraphorderstdefaultnd.param,
                                (byte *) &hgraphorderstdefaultnd.param.sepstrat,
                                (void *) &vgraphseparateststratab },
                              { HGRAPHORDERSTMETHND,  STRATPARAMSTRAT,  "ole",
                                (byte *) &hgraphorderstdefaultnd.param,
                                (byte *) &hgraphorderstdefaultnd.param.ordstratlea,
                                (void *) &hgraphorderststratab },
                              { HGRAPHORDERSTMETHND,  STRATPARAMSTRAT,  "ose",
                                (byte *) &hgraphorderstdefaultnd.param,
                                (byte *) &hgraphorderstdefaultnd.param.ordstratsep,
                                (void *) &hgraphorderststratab },
                              { HGRAPHORDERSTMETHNBR, STRATPARAMINT,    NULL,
                                NULL, NULL, NULL } };

static StratParamTab        hgraphorderstcondtab[] = { /* Graph condition parameter table */
                              { STRATNODECOND,        STRATPARAMINT,    "edge",
                                (byte *) &hgraphorderstgraphdummy,
                                (byte *) &hgraphorderstgraphdummy.s.edgenbr,
                                NULL },
                              { STRATNODECOND,       STRATPARAMINT,     "levl",
                                (byte *) &hgraphorderstgraphdummy,
                                (byte *) &hgraphorderstgraphdummy.levlnum,
                                NULL },
                              { STRATNODECOND,        STRATPARAMINT,    "load",
                                (byte *) &hgraphorderstgraphdummy,
                                (byte *) &hgraphorderstgraphdummy.vnlosum,
                                NULL },
                              { STRATNODECOND,        STRATPARAMDOUBLE, "mdeg",
                                (byte *) &hgraphorderstgraphdummy,
                                (byte *) &hgraphorderstgraphdummy.s.degrmax,
                                NULL },
                              { STRATNODECOND,        STRATPARAMINT,    "vert",
                                (byte *) &hgraphorderstgraphdummy,
                                (byte *) &hgraphorderstgraphdummy.s.vertnbr,
                                NULL },
                              { STRATNODENBR,         STRATPARAMINT,    NULL,
                                NULL, NULL, NULL } };

StratTab                    hgraphorderststratab = { /* Strategy tables for graph ordering methods */
                              hgraphorderstmethtab,
                              hgraphorderstparatab,
                              hgraphorderstcondtab };

/************************************/
/*                                  */
/* This routine is the entry point  */
/* for the graph ordering routines. */
/*                                  */
/************************************/

/* This routine computes an ordering
** with respect to a given strategy.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
hgraphOrderSt (
const Hgraph * restrict const   grafptr,              /*+ Subgraph to order          +*/
Order * restrict const          ordeptr,              /*+ Ordering to complete       +*/
const Gnum                      ordenum,              /*+ Index to start ordering at +*/
OrderCblk * restrict const      cblkptr,              /*+ Current column block       +*/
const Strat * restrict const    strat)                /*+ Graph ordering strategy    +*/
{
  StratTest           val;
  int                 o;

  if (grafptr->vnohnnd == grafptr->s.baseval)     /* Return immediately if nothing to do */
    return (0);

  o = 0;
  switch (strat->type) {
    case STRATNODECONCAT :
      errorPrint ("hgraphOrderSt: concatenation operator not available for graph ordering strategies");
      return     (1);
    case STRATNODECOND :
      o = stratTestEval (strat->data.cond.test, &val, (void *) grafptr); /* Evaluate expression */
      if (o == 0) {                               /* If evaluation was correct                  */
#ifdef SCOTCH_DEBUG_HGRAPH2
        if ((val.typetest != STRATTESTVAL) &&
            (val.typenode != STRATPARAMLOG)) {
          errorPrint ("hgraphOrderSt: invalid test result");
          o = 1;
          break;
        }
#endif /* SCOTCH_DEBUG_HGRAPH2 */
        if (val.data.val.vallog == 1)             /* If expression is true                                              */
          o = hgraphOrderSt (grafptr, ordeptr, ordenum, cblkptr, strat->data.cond.strat[0]); /* Apply first strategy    */
        else {                                    /* Else if expression is false                                        */
          if (strat->data.cond.strat[1] != NULL)  /* And if there is an else statement                                  */
            o = hgraphOrderSt (grafptr, ordeptr, ordenum, cblkptr, strat->data.cond.strat[1]); /* Apply second strategy */
        }
      }
      break;
    case STRATNODEEMPTY :
      hgraphOrderSi (grafptr, ordeptr, ordenum, cblkptr); /* Always maintain a coherent ordering */
      break;
    case STRATNODESELECT :
      errorPrint ("hgraphOrderSt: selection operator not available for graph ordering strategies");
      return     (1);
#ifdef SCOTCH_DEBUG_HGRAPH2
    case STRATNODEMETHOD :
#else /* SCOTCH_DEBUG_HGRAPH2 */
    default :
#endif /* SCOTCH_DEBUG_HGRAPH2 */
      return (strat->tabl->methtab[strat->data.method.meth].func (grafptr, ordeptr, ordenum, cblkptr, (void *) &strat->data.method.data));
#ifdef SCOTCH_DEBUG_HGRAPH2
    default :
      errorPrint ("hgraphOrderSt: invalid parameter");
      return     (1);
#endif /* SCOTCH_DEBUG_HGRAPH2 */
  }
  return (o);
}
