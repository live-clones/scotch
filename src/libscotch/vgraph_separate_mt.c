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
/**   NAME       : separate_mt.c                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module separates an active         **/
/**                graph using some routine of libMeTiS    **/
/**                by Karypis. It is intended as a testbed **/
/**                for the comparison of algorithms.       **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 16 oct 1998     **/
/**                                 to     16 oct 1998     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SEPARATE_MT

#include "module.h"
#include "common.h"
#include "graph.h"
#include "separate.h"
#ifndef SCOTCH_COMMON_EXTERNAL                    /* Test module only for debugging */
#include "metis/defs.h"
#include "metis/struct.h"
#include "metis/macros.h"
#include "metis/rename.h"
#include "metis/proto.h"
#endif /* SCOTCH_COMMON_EXTERNAL */

/*
**  The static variables.
*/

static const S_Num          V_MT_loadone = 1;

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the bipartitioning.
** It returns:
** - 0   : if the bipartitioning could be computed.
** - !0  : on error.
*/

int
V_MT_graphSeparate (
V_Graph * const             grafptr)              /*+ Active graph +*/
{
#ifndef SCOTCH_COMMON_EXTERNAL                    /* Test module only for debugging */
  CtrlType            ctrl;
  GraphType           graph;
  int *               wherptr;
  V_GraphPart *       partptr;
  S_Num *             fronptr;
  S_Num               vertnum;                    /* Number of current vertex                */
  const byte *        velobas;                    /* Data for handling of optional arrays */
  int                 velosiz;
  const S_Num *       veloptr;

  if (grafptr->s.velotab == NULL) {               /* Set accesses to optional arrays */
    velobas = (byte *) &V_MT_loadone;
    velosiz = 0;
  }
  else {
    velobas = (byte *) grafptr->s.velotab;
    velosiz = sizeof (S_Num);
  }

  ctrl.CType = ONMETIS_CTYPE;                     /* From Lib/ometis.c (METIS_NodeWND) */
  ctrl.IType = ONMETIS_ITYPE;
  ctrl.RType = ONMETIS_RTYPE;
  ctrl.dbglvl = ONMETIS_DBGLVL;
  ctrl.oflags  = OFLAG_COMPRESS;
  ctrl.pfactor = 0;
  ctrl.nseps = 2;
  ctrl.optype = OP_ONMETIS;
  ctrl.CoarsenTo = 100;
  ctrl.maxvwgt = 1.5*(grafptr->s.velosum/ctrl.CoarsenTo);

  SetUpGraph(&graph, OP_ONMETIS, (int) grafptr->s.vertnbr, 1,
             (int *) grafptr->s.verttab, (int *) grafptr->s.edgetab,
             (int *) grafptr->s.velotab, NULL, 2);

  InitRandom(-1);
  AllocateWorkSpace(&ctrl, &graph, 2);

  Allocate2WayNodePartitionMemory (&ctrl, &graph);
  memSet (graph.where, 0, graph.nvtxs * sizeof (int));
  Compute2WayNodePartitionParams  (&ctrl, &graph);
  GrowBisectionNode               (&ctrl, &graph, ORDER_UNBALANCE_FRACTION);

  grafptr->v.compload[0]  =
  grafptr->v.compload[1]  =
  grafptr->v.compload[2]  = 0;
  grafptr->v.compsize[0]  =
  grafptr->v.compsize[1]  =
  grafptr->v.compsize[2]  = 0;
  for (vertnum = 0, partptr = grafptr->v.parttab, wherptr = graph.where, fronptr = grafptr->v.frontab, veloptr = (S_Num *) velobas;
       vertnum < grafptr->s.vertnbr;
       vertnum ++, wherptr ++, partptr ++, veloptr = (S_Num *) ((byte *) veloptr + velosiz)) {
    *partptr = *wherptr;
    grafptr->v.compload[*partptr] += *veloptr;
    grafptr->v.compsize[*partptr] ++;
    if (*partptr == 2)
      *fronptr ++ = vertnum;                      /* Vertex belongs to frontier */
  }
  grafptr->v.comploaddlt = grafptr->v.compload[0] - grafptr->v.compload[1];
#ifdef SCOTCH_DEBUG_V2
  if (V_graphCheck (grafptr) != 0) {
    errorPrint ("V_MT_graphSeparate: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_V2 */

  GKfree(&graph.gdata, &graph.nvwgt, &graph.rdata, LTERM); /* From Lib/memory.c (FreeGraph) */
  FreeWorkSpace (&ctrl, &graph);

#endif /* SCOTCH_COMMON_EXTERNAL */
  return (0);
}
