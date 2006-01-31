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
/**   NAME       : vmesh_separate_gr.c                     **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module separates a node separation **/
/**                mesh by turning the mesh into a graph   **/
/**                and using a graph separation strategy.  **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 13 oct 2003     **/
/**                                 to     13 oct 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VMESH_SEPARATE_GR

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "vgraph.h"
#include "vgraph_separate_st.h"
#include "mesh.h"
#include "vmesh.h"
#include "vmesh_separate_gr.h"

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
vmeshSeparateGr (
Vmesh * restrict const                      meshptr, /*+ Node separation mesh +*/
const VmeshSeparateGrParam * restrict const paraptr) /*+ Method parameters    +*/
{
  Vgraph                            grafdat;
  Gnum                              fronnum;
  Gnum                              velmnum;
  Gnum                              ecmpsize1;

  graphInit (&grafdat.s);
  if (meshGraph (&meshptr->m, &grafdat.s) != 0) {
    errorPrint ("vmeshSeparateGr: cannot build graph");
    return     (1);
  }
  grafdat.parttax     = meshptr->parttax + (meshptr->m.vnodbas - grafdat.s.baseval); /* Get node area of part array */
  grafdat.compload[0] = meshptr->ncmpload[0];
  grafdat.compload[1] = meshptr->ncmpload[1];
  grafdat.compload[2] = meshptr->ncmpload[2];
  grafdat.comploaddlt = meshptr->ncmploaddlt;
  grafdat.compsize[0] = meshptr->ncmpsize[0];
  grafdat.compsize[1] = meshptr->ncmpsize[1];
  grafdat.fronnbr     = meshptr->fronnbr;
  grafdat.frontab     = meshptr->frontab;         /* Re-use frontier array */
  grafdat.levlnum     = meshptr->levlnum;

  for (fronnum = 0; fronnum < grafdat.fronnbr; fronnum ++)
    grafdat.frontab[fronnum] -= (meshptr->m.vnodbas - grafdat.s.baseval);

#ifdef SCOTCH_DEBUG_VMESH2
  if (vgraphCheck (&grafdat) != 0) {
    errorPrint ("vmeshSeparateGr: internal error (1)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VMESH2 */

  if (vgraphSeparateSt (&grafdat, paraptr->stratptr) != 0) {
    errorPrint ("vmeshSeparateGr: cannot separate graph");
    return     (1);
  }

  for (fronnum = 0; fronnum < grafdat.fronnbr; fronnum ++) /* Restore mesh-based frontier array */
    grafdat.frontab[fronnum] += (meshptr->m.vnodbas - grafdat.s.baseval);
  meshptr->ncmpload[0] = grafdat.compload[0];
  meshptr->ncmpload[1] = grafdat.compload[1];
  meshptr->ncmpload[2] = grafdat.compload[2];
  meshptr->ncmploaddlt = grafdat.comploaddlt;
  meshptr->ncmpsize[0] = grafdat.compsize[0];
  meshptr->ncmpsize[1] = grafdat.compsize[1];
  meshptr->fronnbr     = grafdat.fronnbr;

  for (velmnum = meshptr->m.velmbas, ecmpsize1 = 0;
       velmnum < meshptr->m.velmnnd; velmnum ++) { /* Compute part of all elements */
    Gnum                              eelmnum;
    GraphPart                         partval;

    partval = 0;                                  /* Empty elements move to part 0 */
    for (eelmnum = meshptr->m.verttax[velmnum];
         eelmnum < meshptr->m.vendtax[velmnum]; eelmnum ++) {
      Gnum                              vnodnum;

      vnodnum = meshptr->m.edgetax[eelmnum];
      partval = meshptr->parttax[vnodnum];
      if (partval != 2)
        break;
    }
    partval   &= 1;                               /* In case all nodes in separator */
    ecmpsize1 += (Gnum) partval;                  /* Count elements in part 1       */
    meshptr->parttax[velmnum] = partval;          /* Set part of element            */
  }
  meshptr->ecmpsize[0] = meshptr->m.velmnbr - ecmpsize1;
  meshptr->ecmpsize[1] = ecmpsize1;

#ifdef SCOTCH_DEBUG_VMESH2
  if (vmeshCheck (meshptr) != 0) {
    errorPrint ("vmeshSeparateGr: internal error (2)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VMESH2 */

  return (0);
}
