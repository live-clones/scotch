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
/**   NAME       : kgraph_map_st.c                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the strategy and   **/
/**                method tables for the multi-way static  **/
/**                mapping routines.                       **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 15 oct 1996     **/
/**                                 to     26 may 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     17 may 1999     **/
/**                # Version 3.4  : from : 12 sep 2001     **/
/**                                 to     12 sep 2001     **/
/**                # Version 4.0  : from : 12 jan 2004     **/
/**                                 to     05 jan 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define KGRAPH_MAP_ST

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "graph_coarsen.h"
#include "arch.h"
#include "mapping.h"
#include "bgraph.h"
#include "bgraph_bipart_st.h"
#include "kgraph.h"
#include "kgraph_map_rb.h"
#include "kgraph_map_st.h"

/*
**  The static and global variables.
*/

static union {
  KgraphMapRbParam          param;
  StratNodeMethodData       padding;
} kgraphmapstdefaultrb = { { 1, 1, KGRAPHMAPRBPOLINGSIZE, &stratdummy } };

static StratMethodTab       kgraphmapstmethtab[] = { /* Mapping methods array */
                              { KGRAPHMAPSTMETHRB, "b",  kgraphMapRb, &kgraphmapstdefaultrb },
                              { -1,                NULL, NULL,        NULL } };

static StratParamTab        kgraphmapstparatab[] = { /* Method parameter list */
                              { KGRAPHMAPSTMETHRB,  STRATPARAMCASE,   "job",
                                (byte *) &kgraphmapstdefaultrb.param,
                                (byte *) &kgraphmapstdefaultrb.param.flagjobtie,
                                (void *) "ut" },
                              { KGRAPHMAPSTMETHRB,  STRATPARAMCASE,   "map",
                                (byte *) &kgraphmapstdefaultrb.param,
                                (byte *) &kgraphmapstdefaultrb.param.flagmaptie,
                                (void *) "ut" },
                              { KGRAPHMAPSTMETHRB,  STRATPARAMCASE,   "poli",
                                (byte *) &kgraphmapstdefaultrb.param,
                                (byte *) &kgraphmapstdefaultrb.param.poli,
                                (void *) "rls LSo" },
                              { KGRAPHMAPSTMETHRB,  STRATPARAMSTRAT,  "strat",
                                (byte *) &kgraphmapstdefaultrb.param,
                                (byte *) &kgraphmapstdefaultrb.param.strat,
                                (void *) &bgraphbipartststratab },
                              { KGRAPHMAPSTMETHNBR, STRATPARAMINT,    NULL,
                                NULL, NULL, NULL } };

static StratParamTab        kgraphmapstcondtab[] = { /* Graph condition parameter table */
                              { STRATNODENBR,        STRATPARAMINT,    NULL,
                                NULL, NULL, NULL } };

StratTab                    kgraphmapststratab = { /* Strategy tables for graph mapping methods */
                              kgraphmapstmethtab,
                              kgraphmapstparatab,
                              kgraphmapstcondtab };

/****************************************/
/*                                      */
/* This is the generic mapping routine. */
/*                                      */
/****************************************/

/* This routine computes the given
** mapping according to the given
** strategy.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
kgraphMapSt (
Kgraph * restrict const       grafptr,            /*+ Mapping graph    +*/
const Strat * restrict const  strat)              /*+ Mapping strategy +*/
{
  StratTest           val;
  int                 o;

#ifdef SCOTCH_DEBUG_KGRAPH2
  if (sizeof (Gnum) != sizeof (INT)) {
    errorPrint ("kgraphMapSt: invalid type specification for parser variables");
    return     (1);
  }
  if ((sizeof (KgraphMapRbParam) > sizeof (StratNodeMethodData))) {
    errorPrint ("kgraphMapSt: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_KGRAPH2 */
#ifdef SCOTCH_DEBUG_KGRAPH1
  if ((strat->tabl != &kgraphmapststratab) &&
      (strat       != &stratdummy)) {
    errorPrint ("kgraphMapSt: invalid parameter (1)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_KGRAPH1 */

  o = 0;
  switch (strat->type) {
    case STRATNODECONCAT :
      o = kgraphMapSt (grafptr, strat->data.concat.strat[0]); /* Apply first strategy          */
      if (o == 0)                                 /* If it worked all right                    */
        o |= kgraphMapSt (grafptr, strat->data.concat.strat[1]); /* Then apply second strategy */
      break;
    case STRATNODECOND :
      o = stratTestEval (strat->data.cond.test, &val, (void *) grafptr); /* Evaluate expression */
      if (o == 0) {                               /* If evaluation was correct                  */
#ifdef SCOTCH_DEBUG_KGRAPH2
        if ((val.typetest != STRATTESTVAL) ||
            (val.typenode != STRATPARAMLOG)) {
          errorPrint ("kgraphMapSt: invalid test result");
          o = 1;
          break;
        }
#endif /* SCOTCH_DEBUG_KGRAPH2 */
        if (val.data.val.vallog == 1)             /* If expression is true                 */
          o = kgraphMapSt (grafptr, strat->data.cond.strat[0]); /* Apply first strategy    */
        else {                                    /* Else if expression is false           */
          if (strat->data.cond.strat[1] != NULL)  /* And if there is an else statement     */
            o = kgraphMapSt (grafptr, strat->data.cond.strat[1]); /* Apply second strategy */
        }
      }
      break;
    case STRATNODEEMPTY :
      break;
    case STRATNODESELECT :
      errorPrint ("kgraphMapSt: selection operator not implemented for k-way strategies");
      return      (1);
#ifdef SCOTCH_DEBUG_KGRAPH1
    case STRATNODEMETHOD :
#else /* SCOTCH_DEBUG_KGRAPH1 */
    default :
#endif /* SCOTCH_DEBUG_KGRAPH1 */
      return (strat->tabl->methtab[strat->data.method.meth].func (grafptr, (void *) &strat->data.method.data));
#ifdef SCOTCH_DEBUG_KGRAPH1
    default :
      errorPrint ("kgraphMapSt: invalid parameter (2)");
      return     (1);
#endif /* SCOTCH_DEBUG_KGRAPH1 */
  }
  return (o);
}
