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
/**   NAME       : separate_th.c                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module thins a vertex separator.   **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 17 oct 1998     **/
/**                                 to     17 oct 1998     **/
/**                # Version 4.0  : from : 12 dec 2001     **/
/**                                 to     06 jan 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VGRAPH_SEPARATE_TH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "vgraph.h"
#include "vgraph_separate_th.h"

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
vgraphSeparateTh (
Vgraph * const              grafptr)
{
  Gnum                fronnbr;                    /* Current number of frontier vertices */
  Gnum                fronnum;                    /* Number of current frontier vertex   */
  Gnum                commcut[3];                 /* Cut count array ([3] for halo)      */

  fronnbr = grafptr->fronnbr;                     /* Get current number of frontier vertices */
  for (fronnum = 0; fronnum < fronnbr; ) {
    Gnum                vertnum;
    Gnum                edgenum;

    vertnum    = grafptr->frontab[fronnum];       /* Get vertex number */
    commcut[0] =
    commcut[1] =
    commcut[2] = 0;
    for (edgenum = grafptr->s.verttax[vertnum]; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++)
      commcut[grafptr->parttax[grafptr->s.edgetax[edgenum]]] ++;

    if (commcut[0] == 0) {
      grafptr->parttax[vertnum] = 1;
      grafptr->compload[1] += (grafptr->s.velotax == NULL) ? 1 : grafptr->s.velotax[vertnum];
      grafptr->compsize[1] ++;
      grafptr->frontab[fronnum] = grafptr->frontab[-- fronnbr]; /* Replace frontier vertex by another */
    }
    else if (commcut[1] == 0) {
      grafptr->parttax[vertnum] = 0;
      grafptr->compload[0] += (grafptr->s.velotax == NULL) ? 1 : grafptr->s.velotax[vertnum];
      grafptr->compsize[0] ++;
      grafptr->frontab[fronnum] = grafptr->frontab[-- fronnbr]; /* Replace frontier vertex by another */
    }
    else
      fronnum ++;                                 /* Keep vertex in separator */
  }
  grafptr->fronnbr     = fronnbr;                 /* Set new frontier parameters */
  grafptr->compload[2] = grafptr->s.velosum - (grafptr->compload[0] + grafptr->compload[1]);
  grafptr->comploaddlt = grafptr->compload[0] - grafptr->compload[1];

  return (0);
}
