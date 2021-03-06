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
/**   NAME       : bipart_gp.c                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module bipartitions an active      **/
/**                graph using the Gibbs, Poole, and       **/
/**                Stockmeyer algorithm.                   **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 02 jun 1994     **/
/**                                 to     05 oct 1994     **/
/**                # Version 3.1  : from : 02 may 1996     **/
/**                                 to     02 may 1996     **/
/**                # Version 3.2  : from : 21 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 3.4  : from : 01 jun 2001     **/
/**                                 to     01 jun 2001     **/
/**                # Version 4.0  : from : 04 nov 2003     **/
/**                                 to     20 aug 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BGRAPH_BIPART_GP

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "bgraph.h"
#include "bgraph_bipart_gp.h"

/*
**  The static variables.
*/

static const Gnum           B_GP_loadzero = 0;
static const Gnum           B_GP_loadone  = 1;

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the bipartitioning.
** It returns:
** - 0 : if bipartitioning could be computed.
** - 1 : on error.
*/

int
bgraphBipartGp (
Bgraph * restrict const           grafptr,
const BgraphBipartGpParam * const paraptr)        /*+ Method parameters +*/
{
  BgraphBipartGpQueue             queudat;        /* Neighbor queue               */
  BgraphBipartGpVertex * restrict vexxtax;        /* Complementary vertex array   */
  const Gnum * restrict           verttax;        /* Based access to graph arrays */
  const Gnum * restrict           vendtax;
  const Gnum * restrict           velotax;
  const Gnum * restrict           edgetax;
  const Gnum * restrict           edlotax;
  const Gnum * restrict           veextax;
  Gnum                            compload0dlt;
  Gnum                            compsize0;
  Gnum                            commloadintn;
  Gnum                            commloadextn;
  Gnum                            commgainextn;
  Gnum                            rootnum;        /* Index of potential next root */

  if (grafptr->compload0 != grafptr->s.velosum)   /* If not all vertices already in part 0 */
    bgraphZero (grafptr);                         /* Move all graph vertices to part 0     */

  if (memAllocGroup ((void **)
                     &queudat.queutab, (size_t) (grafptr->s.vertnbr * sizeof (Gnum)),
                     &vexxtax,         (size_t) (grafptr->s.vertnbr * sizeof (BgraphBipartGpVertex)), NULL) == NULL) {
    errorPrint ("bgraphBipartGp: out of memory");
    return     (1);
  }

  memSet (vexxtax, 0, grafptr->s.vertnbr * sizeof (BgraphBipartGpVertex)); /* Initialize pass numbers */
  vexxtax -= grafptr->s.baseval;
  verttax  = grafptr->s.verttax;
  vendtax  = grafptr->s.vendtax;
  velotax  = grafptr->s.velotax;
  edgetax  = grafptr->s.edgetax;
  edlotax  = grafptr->s.edlotax;
  veextax  = grafptr->veextax;

  compsize0    = grafptr->s.vertnbr;              /* All vertices in part zero */
  compload0dlt = grafptr->s.velosum - grafptr->compload0avg;
  commloadintn = 0;
  commloadextn = grafptr->commloadextn0;
  commgainextn = grafptr->commgainextn0;
  for (rootnum = grafptr->s.baseval;              /* Loop on connected components */
       (rootnum < grafptr->s.vertnnd) && (compload0dlt > 0); rootnum ++) {
    Gnum                passnum;                  /* Pass number                                        */
    Gnum                diamnum;                  /* Number of current diameter vertex                  */
    Gnum                diamval;                  /* Current diameter value                             */
    Gnum                diamdeg;                  /* Degree of current diameter vertex                  */
    int                 diamflag;                 /* Flag set if improvement in diameter between passes */

    while (vexxtax[rootnum].passnum != 0)         /* Find first unallocated vertex */
      rootnum ++;

    for (diamnum = rootnum, diamval = diamdeg = 0, diamflag = 1, passnum = 1; /* Start from root   */
         (passnum < paraptr->passnbr) && (diamflag -- != 0); passnum ++) { /* Loop if improvements */
      bgraphBipartGpQueueFlush (&queudat);        /* Flush vertex queue                            */
      bgraphBipartGpQueuePut   (&queudat, diamnum); /* Start from diameter vertex                  */
      vexxtax[diamnum].passnum = passnum;         /* It has been enqueued                          */
      vexxtax[diamnum].distval = 0;

      do {                                        /* Loop on vertices in queue */
        Gnum                vertnum;
        Gnum                distval;
        Gnum                edgenum;

        vertnum = bgraphBipartGpQueueGet (&queudat); /* Get vertex from queue */
        distval = vexxtax[vertnum].distval;       /* Get vertex distance      */

        if ((distval > diamval) ||                /* If vertex increases diameter         */
            ((distval == diamval) &&              /* Or is at diameter distance           */
             ((vendtax[vertnum] - verttax[vertnum]) < diamdeg))) { /* With smaller degree */
          diamnum  = vertnum;                     /* Set it as new diameter vertex        */
          diamval  = distval;
          diamdeg  = vendtax[vertnum] - verttax[vertnum];
          diamflag = 1;
        }

        distval ++;                               /* Set neighbor distance */
        for (edgenum = verttax[vertnum]; edgenum < vendtax[vertnum]; edgenum ++) {
          Gnum                vertend;            /* End vertex number */

          vertend = edgetax[edgenum];
          if (vexxtax[vertend].passnum < passnum) { /* If vertex not yet queued      */
            bgraphBipartGpQueuePut (&queudat, vertend); /* Enqueue neighbor vertex */
            vexxtax[vertend].passnum = passnum;
            vexxtax[vertend].distval = distval;
          }
        }
      } while (! bgraphBipartGpQueueEmpty (&queudat)); /* As long as queue is not empty */
    }

    bgraphBipartGpQueueFlush (&queudat);          /* Flush vertex queue         */
    bgraphBipartGpQueuePut   (&queudat, diamnum); /* Start from diameter vertex */
    vexxtax[diamnum].passnum = passnum;           /* It has been enqueued       */
    vexxtax[diamnum].distval = 0;

    do {                                          /* Loop on vertices in queue */
      Gnum                vertnum;
      Gnum                veloval;
      Gnum                veexval;
      Gnum                distval;
      Gnum                edgenum;

      vertnum = bgraphBipartGpQueueGet (&queudat); /* Get vertex from queue */
      veloval = (velotax != NULL) ? velotax[vertnum] : 1;
      veexval = (veextax != NULL) ? veextax[vertnum] : 0;
      grafptr->parttax[vertnum] = 1;              /* Move selected vertex to part 1 */
      compsize0    --;
      compload0dlt -= veloval;
      commloadextn += veexval;
      commgainextn -= veexval;

      distval = vexxtax[vertnum].distval + 1;
      for (edgenum = verttax[vertnum]; edgenum < vendtax[vertnum]; edgenum ++) {
        Gnum                vertend;              /* End vertex number */

        vertend = edgetax[edgenum];
        if (vexxtax[vertend].passnum < passnum) { /* If vertex not yet queued    */
          bgraphBipartGpQueuePut (&queudat, vertend); /* Enqueue neighbor vertex */
          vexxtax[vertend].passnum = passnum;
          vexxtax[vertend].distval = distval;
        }
      }
    } while ((compload0dlt > 0) && (! bgraphBipartGpQueueEmpty (&queudat))); /* As long as balance not achieved and queue is not empty */

    if (! bgraphBipartGpQueueEmpty (&queudat)) {  /* If frontier non empty */
      Gnum                edloval;
      Gnum                fronnbr;

      fronnbr = 0;                                /* No frontier yet      */
      edloval = 1;                                /* Assume no edge loads */
      do {
        Gnum                vertnum;
        Gnum                edgenum;

        vertnum = bgraphBipartGpQueueGet (&queudat); /* Get vertex from queue */
        grafptr->frontab[fronnbr ++] = vertnum;
#ifdef SCOTCH_DEBUG_BGRAPH2
        if (grafptr->parttax[vertnum] != 0) {
          errorPrint ("bgraphBipartGp: internal error");
          return     (1);
        }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

        for (edgenum = verttax[vertnum]; edgenum < vendtax[vertnum]; edgenum ++) {
          Gnum                vertend;            /* End vertex number */

          vertend = edgetax[edgenum];
          if (grafptr->parttax[vertend] == 1) {   /* If vertex belongs to other part */
            if (edlotax != NULL)
              edloval = edlotax[edgenum];
            commloadintn += edloval;
            if (vexxtax[vertend].distval != ~0) { /* If neighbor vertex not already put in frontier */
              grafptr->frontab[fronnbr ++] = vertend; /* Record it in frontier                      */
              vexxtax[vertend].distval = ~0;      /* Set it as recorded                             */
            }
          }
        }
      } while (! bgraphBipartGpQueueEmpty (&queudat));
      grafptr->fronnbr = fronnbr;
      break;                                      /* No need to process rest of graph */
    }                                             /* Else grafptr->fronnbr = 0 anyway */
  }

  grafptr->compload0    = grafptr->compload0avg + compload0dlt;
  grafptr->compload0dlt = compload0dlt;
  grafptr->compsize0    = compsize0;
  grafptr->commload     = commloadintn * grafptr->domdist + commloadextn;
  grafptr->commgainextn = commgainextn;

  memFree (queudat.queutab);                      /* Free group leader */

#ifdef SCOTCH_DEBUG_BGRAPH2
  if (bgraphCheck (grafptr) != 0) {
    errorPrint ("bgraphBipartGp: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  return (0);
}
