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
/**   NAME       : bgraph_check.c                          **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the bipartition    **/
/**                graph consistency checking routine.     **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 08 jan 2004     **/
/**                                 to     07 dec 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BGRAPH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "bgraph.h"

/*************************/
/*                       */
/* These routines handle */
/* bipartition graphs.   */
/*                       */
/*************************/

/* This routine checks the consistency
** of the given bipartition graph.
** It returns:
** - 0   : if graph data are consistent.
** - !0  : on error.
*/

int
bgraphCheck (
const Bgraph * restrict const grafptr)
{
  int * restrict      flagtax;                    /* Frontier flag array       */
  Gnum                vertnum;                    /* Number of current vertex  */
  Gnum                fronnum;                    /* Number of frontier vertex */
  Gnum                compload[2];
  Gnum                compsize[2];
  Gnum                commcut[2];
  Gnum                commloadintn;
  Gnum                commloadextn;
  Gnum                commgainextn;
  Gnum                edloval;
  Gnum                veexval;

  if ((flagtax = memAlloc (grafptr->s.vertnbr * sizeof (Gnum))) == NULL) {
    errorPrint ("bgraphCheck: out of memory");
    return     (1);
  }
  memSet (flagtax, ~0, grafptr->s.vertnbr * sizeof (Gnum));
  flagtax -= grafptr->s.baseval;

  if (grafptr->compload0 != (grafptr->compload0avg + grafptr->compload0dlt)) {
    errorPrint ("bgraphCheck: invalid balance");
    return     (1);
  }

  for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) {
    if (grafptr->parttax[vertnum] > 1) {
      errorPrint ("bgraphCheck: invalid part array");
      return     (1);
    }
  }

  if ((grafptr->fronnbr < 0) ||
      (grafptr->fronnbr > grafptr->s.vertnbr)) {
    errorPrint ("bgraphCheck: invalid number of frontier vertices");
    return     (1);
  }
  for (fronnum = 0; fronnum < grafptr->fronnbr; fronnum ++) {
    Gnum                vertnum;
    Gnum                edgenum;
    Gnum                commcut;
    int                 partval;

    vertnum = grafptr->frontab[fronnum];
    if ((vertnum < grafptr->s.baseval) || (vertnum >= grafptr->s.vertnnd)) {
      errorPrint ("bgraphCheck: invalid vertex index in frontier array");
      return     (1);
    }
    if (flagtax[vertnum] != ~0) {
      errorPrint ("bgraphCheck: duplicate vertex in frontier array");
      return     (1);
    }
    flagtax[vertnum] = 0;
    partval = grafptr->parttax[vertnum];

    for (edgenum = grafptr->s.verttax[vertnum], commcut = 0;
         edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
      int                 partdlt;

      partdlt  = grafptr->parttax[grafptr->s.edgetax[edgenum]] ^ partval;
      commcut += partdlt;
    }
    if (commcut == 0) {
      errorPrint ("bgraphCheck: invalid vertex in frontier array");
      return     (1);
    }
  }

  compload[0]  =
  compload[1]  = 0;
  compsize[0]  =
  compsize[1]  = 0;
  commloadintn = 0;
  commloadextn = grafptr->commloadextn0;
  commgainextn = 0;
  edloval      = 1;                               /* Assume edges are not weighted */
  veexval      = 0;
  for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) {
    Gnum                partval;                  /* Part of current vertex */
    Gnum                edgenum;                  /* Number of current edge */

    if (grafptr->veextax != NULL)
      veexval = grafptr->veextax[vertnum];
    partval = grafptr->parttax[vertnum];
    commloadextn += veexval * partval;
    commgainextn += veexval * (1 - 2 * partval);

    compload[partval] += (grafptr->s.velotax == NULL) ? 1 : grafptr->s.velotax[vertnum];
    compsize[partval] ++;

    commcut[0] =
    commcut[1] = 0;
    for (edgenum = grafptr->s.verttax[vertnum]; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
      int                 partend;
      int                 partdlt;

      if (grafptr->s.edlotax != NULL)
        edloval = grafptr->s.edlotax[edgenum];
      partend = grafptr->parttax[grafptr->s.edgetax[edgenum]];
      partdlt = partval ^ partend;
      commcut[partend] ++;
      commloadintn += partdlt * edloval * partend; /* Only count loads once, when (partend == 1) */
    }

    if ((commcut[0] != 0) && (commcut[1] != 0) && /* If vertex should be in separator */
        (flagtax[vertnum] != 0)) {
      errorPrint ("bgraphCheck: vertex should be in separator");
      return     (1);
    }
  }
  if (compsize[0] != grafptr->compsize0) {
    errorPrint ("bgraphCheck: invalid part size");
    return     (1);
  }
  if ((commloadintn * grafptr->domdist + commloadextn) != grafptr->commload) {
    errorPrint ("bgraphCheck: invalid communication loads");
    return     (1);
  }
  if (commgainextn != grafptr->commgainextn) {
    errorPrint ("bgraphCheck: invalid communication gains");
    return     (1);
  }

  memFree (flagtax + grafptr->s.baseval);

  return (0);
}
