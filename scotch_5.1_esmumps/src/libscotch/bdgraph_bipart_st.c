/* Copyright 2007-2009 ENSEIRB, INRIA & CNRS
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
/**   NAME       : bdgraph_bipart_st.c                     **/
/**                                                        **/
/**   AUTHOR     : Jun-Ho HER                              **/
/**                Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the strategy and   **/
/**                method tables for distributed graph     **/
/**                bipartitioning methods.                 **/
/**                                                        **/
/**   DATES      : # Version 5.1  : from : 10 sep 2007     **/
/**                                 to   : 25 may 2009     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BDGRAPH_BIPART_ST

#include "module.h"
#include "common.h"
#include "gain.h"
#include "parser.h"
#include "graph.h"
#include "arch.h"
#include "bgraph.h"
#include "bgraph_bipart_st.h"
#include "dgraph.h"
#include "dgraph_coarsen.h"
#include "bdgraph.h"
#include "bdgraph_bipart_bd.h"
#include "bdgraph_bipart_df.h"
#include "bdgraph_bipart_ml.h"
#include "bdgraph_bipart_sq.h"
#include "bdgraph_bipart_st.h"
#include "bdgraph_bipart_zr.h"

/*
**  The static and global variables.
*/

static Bdgraph              bdgraphdummy;     /* Dummy distributed bipartitioned graph for offset computations */

static union {
  BdgraphBipartBdParam      param;
  StratNodeMethodData       padding;
} bdgraphbipartdefaultbd = { { 3, &stratdummy } };

static union {
  BdgraphBipartDfParam      param;
  StratNodeMethodData       padding;
} bdgraphbipartdefaultdf = { { 500, 1.0F, 0.0F } };

static union {
  BdgraphBipartMlParam      param;
  StratNodeMethodData       padding;
} bdgraphbipartdefaultml = { { 5, 1000, 100, 0, 0.8L, &stratdummy, &stratdummy, &stratdummy} };

static union {
  BdgraphBipartSqParam      param;
  StratNodeMethodData       padding;
} bdgraphbipartdefaultsq = { { &stratdummy } };

static StratMethodTab       bdgraphbipartstmethtab[] = { /* Bipartitioning methods array */
                              { BDGRAPHBIPARTMETHBD, "b",  bdgraphBipartBd, &bdgraphbipartdefaultbd },
                              { BDGRAPHBIPARTMETHDF, "d",  bdgraphBipartDf, &bdgraphbipartdefaultdf },
                              { BDGRAPHBIPARTMETHML, "m",  bdgraphBipartMl, &bdgraphbipartdefaultml },
                              { BDGRAPHBIPARTMETHSQ, "q",  bdgraphBipartSq, &bdgraphbipartdefaultsq },
                              { BDGRAPHBIPARTMETHZR, "z",  bdgraphBipartZr, NULL },
                              { -1,                  NULL, NULL,            NULL } };

static StratParamTab        bdgraphbipartstparatab[] = { /* Method parameter list */
                              { BDGRAPHBIPARTMETHBD,  STRATPARAMINT,    "width",
                                (byte *) &bdgraphbipartdefaultbd.param,
                                (byte *) &bdgraphbipartdefaultbd.param.distmax,
                                NULL },
                              { BDGRAPHBIPARTMETHBD,  STRATPARAMSTRAT,  "bnd",
                                (byte *) &bdgraphbipartdefaultbd.param,
                                (byte *) &bdgraphbipartdefaultbd.param.stratbnd,
                                (void *) &bdgraphbipartststratab },
                              { BDGRAPHBIPARTMETHBD,  STRATPARAMSTRAT,  "org",
                                (byte *) &bdgraphbipartdefaultbd.param,
                                (byte *) &bdgraphbipartdefaultbd.param.stratorg,
                                (void *) &bdgraphbipartststratab },
                              { BDGRAPHBIPARTMETHDF,  STRATPARAMINT,    "pass",
                                (byte *) &bdgraphbipartdefaultdf.param,
                                (byte *) &bdgraphbipartdefaultdf.param.passnbr,
                                NULL },
                              { BDGRAPHBIPARTMETHDF,  STRATPARAMDOUBLE, "dif",
                                (byte *) &bdgraphbipartdefaultdf.param,
                                (byte *) &bdgraphbipartdefaultdf.param.cdifval,
                                NULL },
                              { BDGRAPHBIPARTMETHDF,  STRATPARAMDOUBLE, "rem",
                                (byte *) &bdgraphbipartdefaultdf.param,
                                (byte *) &bdgraphbipartdefaultdf.param.cremval,
                                NULL },
                              { BDGRAPHBIPARTMETHML,  STRATPARAMSTRAT,  "asc",
                                (byte *) &bdgraphbipartdefaultml.param,
                                (byte *) &bdgraphbipartdefaultml.param.stratasc,
                                (void *) &bdgraphbipartststratab },
                              { BDGRAPHBIPARTMETHML,  STRATPARAMSTRAT,  "low",
                                (byte *) &bdgraphbipartdefaultml.param,
                                (byte *) &bdgraphbipartdefaultml.param.stratlow,
                                (void *) &bdgraphbipartststratab },
                              { BDGRAPHBIPARTMETHML,  STRATPARAMSTRAT,  "seq",
                                (byte *) &bdgraphbipartdefaultml.param,
                                (byte *) &bdgraphbipartdefaultml.param.stratseq,
                                (void *) &bdgraphbipartststratab },
                              { BDGRAPHBIPARTMETHML,  STRATPARAMINT,    "pass",
                                (byte *) &bdgraphbipartdefaultml.param,
                                (byte *) &bdgraphbipartdefaultml.param.passnbr,
                                NULL },
                              { BDGRAPHBIPARTMETHML,  STRATPARAMINT,    "vert",
                                (byte *) &bdgraphbipartdefaultml.param,
                                (byte *) &bdgraphbipartdefaultml.param.coarnbr,
                                NULL },
                              { BDGRAPHBIPARTMETHML,  STRATPARAMDOUBLE, "rat",
                                (byte *) &bdgraphbipartdefaultml.param,
                                (byte *) &bdgraphbipartdefaultml.param.coarrat,
                                NULL },
                              { BDGRAPHBIPARTMETHSQ,  STRATPARAMSTRAT,  "strat",
                                (byte *) &bdgraphbipartdefaultsq.param,
                                (byte *) &bdgraphbipartdefaultsq.param.strat,
                                (void *) &bgraphbipartststratab },
                              { BDGRAPHBIPARTMETHNBR, STRATPARAMINT,    NULL,
                                NULL, NULL, NULL } };

static StratParamTab        bdgraphbipartstcondtab[] = { /* Active graph condition parameter table */
                              { STRATNODECOND,       STRATPARAMINT,    "load",
                                (byte *) &bdgraphdummy,
                                (byte *) &bdgraphdummy.s.veloglbsum,
                                NULL },
                              { STRATNODECOND,       STRATPARAMINT,    "load0",
                                (byte *) &bdgraphdummy,
                                (byte *) &bdgraphdummy.compglbload0,
                                NULL },
                              { STRATNODECOND,       STRATPARAMINT,    "edge",
                                (byte *) &bdgraphdummy,
                                (byte *) &bdgraphdummy.s.edgeglbnbr,
                                NULL },
                              { STRATNODECOND,       STRATPARAMINT,    "vert",
                                (byte *) &bdgraphdummy,
                                (byte *) &bdgraphdummy.s.vertglbnbr,
                                NULL },
                              { STRATNODECOND,       STRATPARAMDOUBLE, "levl",
                                (byte *) &bdgraphdummy,
                                (byte *) &bdgraphdummy.levlnum,
                                NULL },
                              { STRATNODECOND,       STRATPARAMINT,    "proc",
                                (byte *) &bdgraphdummy,
                                (byte *) &bdgraphdummy.s.procglbnbr,
                                NULL },
                              { STRATNODECOND,       STRATPARAMINT,    "rank",
                                (byte *) &bdgraphdummy,
                                (byte *) &bdgraphdummy.s.proclocnum,
                                NULL },
                              { STRATNODENBR,        STRATPARAMINT,    NULL,
                                NULL, NULL, NULL } };

StratTab                    bdgraphbipartststratab = { /* Strategy tables for graph bipartitioning methods */
                              bdgraphbipartstmethtab,
                              bdgraphbipartstparatab,
                              bdgraphbipartstcondtab };

/***********************************************/
/*                                             */
/* This is the generic bipartitioning routine. */
/*                                             */
/***********************************************/

/* This routine performs the bipartitioning of
** the given active graph according to the
** given strategy.
** It returns:
** - 0 : if bipartitioning could be computed.
** - 1 : on error.
*/

int
bdgraphBipartSt (
Bdgraph * restrict const      grafptr,            /*+ Active graph to bipartition +*/
const Strat * restrict const  strat)              /*+ Bipartitioning strategy     +*/
{
  StratTest           val;                        /* Result of condition evaluation */
  BdgraphStore        savetab[2];                 /* Results of the two strategies  */
  int                 o;
  int                 o2;
#ifdef SCOTCH_DEBUG_BDGRAPH2
  MPI_Comm            proccommold;                /*Save area for old communicator */
#endif /* SCOTCH_DEBUG_BDGRAPH2 */

#ifdef SCOTCH_DEBUG_BDGRAPH2
  if (sizeof (Gnum) != sizeof (INT)) {
    errorPrint ("bdgraphBipartSt: invalid type specification for parser variables");
    return     (1);
  }
  if (/*(sizeof (BdgraphBipartFmParam) > sizeof (StratNodeMethodData)) ||
      (sizeof (BdgraphBipartGgParam) > sizeof (StratNodeMethodData)) ||*/
      (sizeof (BdgraphBipartMlParam) > sizeof (StratNodeMethodData))) {
    errorPrint ("bdgraphBipartSt: invalid type specification");
    return     (1);
  } /* TODO REMOVE */
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
#ifdef SCOTCH_DEBUG_BDGRAPH1
  if ((strat->tabl != &bdgraphbipartststratab) &&
      (strat       != &stratdummy)) {
    errorPrint ("bdgraphBipartSt: invalid parameter (1)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BDGRAPH1 */

  o = 0;
  switch (strat->type) {
    case STRATNODECONCAT :
      o = bdgraphBipartSt (grafptr, strat->data.concat.strat[0]); /* Apply the first strategy      */
      if (o == 0)                                 /* If it worked all right                        */
        o |= bdgraphBipartSt (grafptr, strat->data.concat.strat[1]); /* Then apply second strategy */
      break;
    case STRATNODECOND :
      o = stratTestEval (strat->data.cond.test, &val, (void *) grafptr); /* Evaluate expression */
      if (o == 0) {                               /* If evaluation was correct                  */
#ifdef SCOTCH_DEBUG_BDGRAPH2
        if ((val.typetest != STRATTESTVAL) ||
            (val.typenode != STRATPARAMLOG)) {
          errorPrint ("bdgraphBipartSt: invalid test result");
          o = 1;
          break;
        }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
        if (val.data.val.vallog == 1)             /* If expression is true                     */
          o = bdgraphBipartSt (grafptr, strat->data.cond.strat[0]); /* Apply first strategy    */
        else {                                    /* Else if expression is false               */
          if (strat->data.cond.strat[1] != NULL)  /* And if there is an else statement         */
            o = bdgraphBipartSt (grafptr, strat->data.cond.strat[1]); /* Apply second strategy */
        }
      }
      break;
    case STRATNODEEMPTY :
      break;
    case STRATNODESELECT :
      if (((bdgraphStoreInit (grafptr, &savetab[0])) != 0) || /* Allocate save areas */
          ((bdgraphStoreInit (grafptr, &savetab[1])) != 0)) {
        errorPrint ("bdgraphBipartSt: out of memory");
        bdgraphStoreExit (&savetab[0]);
        return          (1);
      }

      bdgraphStoreSave     (grafptr, &savetab[1]); /* Save initial bipartition              */
      o = bdgraphBipartSt  (grafptr, strat->data.select.strat[0]); /* Apply first strategy  */
      bdgraphStoreSave     (grafptr, &savetab[0]); /* Save its result                       */
      bdgraphStoreUpdt     (grafptr, &savetab[1]); /* Restore initial bipartition           */
      o2 = bdgraphBipartSt (grafptr, strat->data.select.strat[1]); /* Apply second strategy */

      if ((o == 0) || (o2 == 0)) {                /* If at least one method did bipartition     */
        if ( (savetab[0].commglbload <  grafptr->commglbload) || /* If first strategy is better */
            ((savetab[0].commglbload == grafptr->commglbload) &&
             (abs (savetab[0].compglbload0dlt) < abs (grafptr->compglbload0dlt))))
          bdgraphStoreUpdt (grafptr, &savetab[0]); /* Restore its result */
      }
      if (o2 < o)                                 /* o = min(o,o2): if one biparts, then bipart */
        o = o2;                                   /* Else if one stops, then stop, else error   */

      bdgraphStoreExit (&savetab[0]);             /* Free both save areas */
      bdgraphStoreExit (&savetab[1]);
      break;
#ifdef SCOTCH_DEBUG_BDGRAPH1
    case STRATNODEMETHOD :
#else /* SCOTCH_DEBUG_BDGRAPH1 */
    default :
#endif /* SCOTCH_DEBUG_BDGRAPH1 */
#ifdef SCOTCH_DEBUG_BDGRAPH2
      proccommold = grafptr->s.proccomm;          /* Create new communicator to isolate method communications */
      MPI_Comm_dup (proccommold, &grafptr->s.proccomm); 
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
      o = (strat->tabl->methtab[strat->data.method.meth].func (grafptr, (void *) &strat->data.method.data));
#ifdef SCOTCH_DEBUG_BDGRAPH2
      MPI_Comm_free (&grafptr->s.proccomm);       /* Restore old communicator */
      grafptr->s.proccomm = proccommold;
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
#ifdef SCOTCH_DEBUG_BDGRAPH1
      break;
    default :
      errorPrint ("bdgraphBipartSt: invalid parameter (2)");
      return     (1);
#endif /* SCOTCH_DEBUG_BDGRAPH1 */
  }
  return (o);
}
