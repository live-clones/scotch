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
/**   NAME       : bgraph_bipart_gg.c                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Luca SCARANO (v3.1)                     **/
/**                                                        **/
/**   FUNCTION   : This module computes a bipartition of   **/
/**                a bipartition graph by multiple runs of **/
/**                the greedy graph growing algorithm.     **/
/**                                                        **/
/**   DATES      : # Version 3.1  : from : 07 jan 1996     **/
/**                                 to     07 jun 1996     **/
/**                # Version 3.2  : from : 20 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 3.4  : from : 01 jun 2001     **/
/**                                 to     01 jun 2001     **/
/**                # Version 4.0  : from : 09 jan 2004     **/
/**                                 to     01 sep 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BGRAPH_BIPART_GG

#include "module.h"
#include "common.h"
#include "gain.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "bgraph.h"
#include "bgraph_bipart_gg.h"

/*
**  The static variables.
*/

static const Gnum           bgraphbipartggloadone  = 1;
static const Gnum           bgraphbipartggloadzero = 0;

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
bgraphBipartGg (
Bgraph * restrict const           grafptr,        /*+ Active graph      +*/
const BgraphBipartGgParam * const paraptr)        /*+ Method parameters +*/
{
  GainTabl * restrict             tablptr;        /* Pointer to gain table                      */
  BgraphBipartGgVertex * restrict vexxtax;        /* Extended vertex array                      */
  BgraphBipartGgVertex *          vexxptr;        /* Pointer to current vertex to swap          */
  const Gnum * restrict           veexptr;        /* Pointer to external gain of current vertex */
  Gnum * restrict                 permtab;        /* Permutation table for finding new roots    */
  Gnum                            permnum;        /* Current permutation index                  */
  const byte * restrict           velobax;        /* Data for handling of optional arrays       */
  int                             velosiz;
  const byte * restrict           veexbax;
  int                             veexsiz;
  const byte * restrict           edlobax;
  int                             edlosiz;
  byte * restrict                 flagtax;
  Gnum                            vertnum;
  Gnum                            fronnum;
  Gnum                            compsize1;
  Gnum                            commgainextn;
  unsigned int                    passnum;
  Anum                            domdist2;       /* Two times domdist */

  if (((tablptr = gainTablInit (GAIN_LINMAX, BGRAPHBIPARTGGGAINTABLSUBBITS)) == NULL) || /* Use logarithmic array only */
      ((vexxtax = (BgraphBipartGgVertex *) memAlloc (grafptr->s.vertnbr * sizeof (BgraphBipartGgVertex))) == NULL)) {
    errorPrint ("bgraphBipartGg: out of memory (1)");
    if (tablptr != NULL)
      gainTablExit (tablptr);
    return (1);
  }
  vexxtax -= grafptr->s.baseval;                  /* Base access to vexxtax                */
  permtab  = NULL;                                /* Do not allocate permutation array yet */

  domdist2 = 2 * grafptr->domdist;

  if (grafptr->s.edlotax == NULL) {               /* If graph has no edge weights */
    Gnum                vertnum;

    for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) {
      Gnum                commload;

      commload = (grafptr->s.vendtax[vertnum] - grafptr->s.verttax[vertnum]) * grafptr->domdist;
      vexxtax[vertnum].commgain0 = (grafptr->veextax == NULL) ? commload : commload + grafptr->veextax[vertnum];
    }

    edlobax = (byte *) &bgraphbipartggloadone;
    edlosiz = 0;
  }
  else {                                          /* Graph has edge weights */
    Gnum                vertnum;

    for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) {
      Gnum                commload;
      Gnum                edgenum;

      for (edgenum = grafptr->s.verttax[vertnum], commload = 0;
           edgenum < grafptr->s.vendtax[vertnum]; edgenum ++)
        commload += grafptr->s.edlotax[edgenum];
      commload *= grafptr->domdist;

      vexxtax[vertnum].commgain0 = (grafptr->veextax == NULL) ? commload : commload + grafptr->veextax[vertnum];
    }

    edlobax = (byte *) grafptr->s.edlotax;
    edlosiz = sizeof (Gnum);
  }
  if (grafptr->s.velotax == NULL) {               /* Set accesses to optional arrays             */
    velobax = (byte *) &bgraphbipartggloadone;    /* In case vertices not weighted (least often) */
    velosiz = 0;
  }
  else {
    velobax = (byte *) grafptr->s.velotax;
    velosiz = sizeof (Gnum);
  }
  if (grafptr->veextax == NULL) {
    veexbax = (byte *) &bgraphbipartggloadzero;
    veexsiz = 0;
  }
  else {
    veexbax = (byte *) grafptr->veextax;
    veexsiz = sizeof (Gnum);
  }

  for (passnum = 0; passnum < paraptr->passnbr; passnum ++) { /* For all passes */
    Gnum                vertnum;
    Gnum                commload;
    Gnum                compload0dlt;

    for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) { /* Reset extended vertex array */
      vexxtax[vertnum].gainlink.next = BGRAPHBIPARTGGSTATEFREE;
      vexxtax[vertnum].commgain      = vexxtax[vertnum].commgain0;
    }
    gainTablFree (tablptr);                       /* Reset gain table                          */
    permnum      = 0;                             /* No permutation built yet                  */
    compload0dlt = grafptr->s.velosum - grafptr->compload0avg; /* Reset bipartition parameters */
    commload     = grafptr->commloadextn0;

    vexxptr = vexxtax + (grafptr->s.baseval + intRandVal (grafptr->s.vertnbr)); /* Randomly select first root vertex */

    do {                                          /* For all root vertices, till balance  */
      vexxptr->gainlink.next =                    /* TRICK: Allow deletion of root vertex */
      vexxptr->gainlink.prev = (GainLink *) vexxptr;
#ifdef SCOTCH_DEBUG_GAIN2
      vexxptr->gainlink.tabl = NULL;
#endif /* SCOTCH_DEBUG_GAIN2 */

      do {                                        /* As long as vertices can be retrieved */
        const Gnum * restrict       edgeptr;      /* Pointer to current end vertex index  */
        const Gnum * restrict       edloptr;      /* Pointer to current edge load         */
        Gnum                        vertnum;      /* Number of current vertex             */
        Gnum                        veloval;      /* Load of selected vertex              */

        gainTablDel (tablptr, (GainLink *) vexxptr); /* Remove vertex from the table */
        vertnum = vexxptr - vexxtax;              /* Get number of selected vertex   */
        veloval = *((Gnum *) (velobax + (vertnum * velosiz)));

        if (abs (compload0dlt) < abs (compload0dlt - veloval)) { /* If swapping would cause imbalance */
          permnum = grafptr->s.vertnbr;           /* Terminate swapping process                       */
          vexxptr = NULL;
          break;
        }

        vexxptr->gainlink.next = BGRAPHBIPARTGGSTATEUSED; /* Mark it as swapped  */
        compload0dlt -= veloval;                  /* Update partition parameters */
        commload     += vexxptr->commgain;
        for (edgeptr = grafptr->s.edgetax + grafptr->s.verttax[vertnum], /* (Re-)link neighbors */
             edloptr = (Gnum *) (edlobax + (grafptr->s.verttax[vertnum] * edlosiz));
             edgeptr < grafptr->s.edgetax + grafptr->s.vendtax[vertnum];
             edgeptr ++, edloptr = (Gnum *) ((byte *) edloptr + edlosiz)) {
          BgraphBipartGgVertex *          vexxend; /* Pointer to end vertex of current edge */

          vexxend = vexxtax + *edgeptr;           /* Point to end vertex                                 */
          if (vexxend->gainlink.next != BGRAPHBIPARTGGSTATEUSED) { /* If vertex needs to be updated      */
            vexxend->commgain -= *edloptr * domdist2; /* Adjust gain value                               */
            if (vexxend->gainlink.next >= BGRAPHBIPARTGGSTATELINK) /* If vertex is linked                */
              gainTablDel (tablptr, (GainLink *) vexxend); /* Remove it from table                       */
            gainTablAdd (tablptr, (GainLink *) vexxend, vexxend->commgain); /* (Re-)link vertex in table */
          }
        }
      } while ((vexxptr = (BgraphBipartGgVertex *) gainTablFrst (tablptr)) != NULL);

      if (permnum == 0) {                         /* If permutation has not been built yet  */
        if (permtab == NULL) {                    /* If permutation array not allocated yet */
          if ((permtab = (Gnum *) memAlloc (grafptr->s.vertnbr * sizeof (Gnum))) == NULL) {
            errorPrint   ("bgraphBipartGg: out of memory (2)");
            memFree      (vexxtax + grafptr->s.baseval);
            gainTablExit (tablptr);
            return       (1);
          }
          intAscn (permtab, grafptr->s.vertnbr, grafptr->s.baseval); /* Initialize based permutation array */
        }
        intPerm (permtab, grafptr->s.vertnbr);    /* Build random permutation */
      }
      for ( ; permnum < grafptr->s.vertnbr; permnum ++) { /* Find next root vertex */
        if (vexxtax[permtab[permnum]].gainlink.next == BGRAPHBIPARTGGSTATEFREE) {
          vexxptr = vexxtax + permtab[permnum ++];
          break;
        }
      }
    } while (vexxptr != NULL);

    if ((passnum == 0) ||                         /* If first try                  */
        ( (grafptr->commload >  commload) ||      /* Or if better solution reached */
         ((grafptr->commload == commload) &&
          (abs (grafptr->compload0dlt) > abs (compload0dlt))))) {
      Gnum                vertnum;

      grafptr->compload0dlt = compload0dlt;       /* Set graph parameters */
      grafptr->commload     = commload;

      for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) /* Copy bipartition state with flag 2 for tabled vertices */
        grafptr->parttax[vertnum] = (vexxtax[vertnum].gainlink.next >= BGRAPHBIPARTGGSTATELINK) ? 2 : (GraphPart) ((int) vexxtax[vertnum].gainlink.next);
    }
  }

  flagtax = (byte *) (vexxtax + grafptr->s.baseval) - grafptr->s.baseval; /* Re-use extended vertex array for flag array */
  memSet (flagtax + grafptr->s.baseval, ~0, grafptr->s.vertnbr * sizeof (byte));
  for (vertnum = grafptr->s.baseval, veexptr = (Gnum *) veexbax, fronnum = 0, compsize1 = 0, commgainextn = grafptr->commgainextn0;
       vertnum < grafptr->s.vertnnd; vertnum ++, veexptr = (Gnum *) ((byte *) veexptr + veexsiz)) {
    int                 partval;

    partval = grafptr->parttax[vertnum];
    if (partval > 1) {                            /* If vertex belongs to frontier of part 0 */
      Gnum                edgenum;

      grafptr->frontab[fronnum ++] = vertnum;     /* Then it belongs to the frontier */
      grafptr->parttax[vertnum]    = 0;           /* And it belongs to part 0        */
      for (edgenum = grafptr->s.verttax[vertnum]; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
        Gnum                vertend;

        vertend = grafptr->s.edgetax[edgenum];
        if ((grafptr->parttax[vertend] == 1) &&   /* If vertex belongs to other part */
            (flagtax[vertend] != 0)) {            /* And it has not yet been flagged */
          grafptr->frontab[fronnum ++] = vertend; /* Then add it to the frontier     */
          flagtax[vertend] = 0;                   /* Flag it                         */
        }
      }
    }
    partval      &= 1;
    compsize1    += partval;
    commgainextn -= partval * 2 * *veexptr;
  }
  grafptr->compload0    = grafptr->compload0avg + grafptr->compload0dlt;
  grafptr->compsize0    = grafptr->s.vertnbr - compsize1;
  grafptr->commgainextn = commgainextn;
  grafptr->fronnbr      = fronnum;

  if (permtab != NULL)                            /* Free work arrays */
    memFree (permtab);
  memFree      (vexxtax + grafptr->s.baseval);
  gainTablExit (tablptr);

#ifdef SCOTCH_DEBUG_BGRAPH2
  if (bgraphCheck (grafptr) != 0) {
    errorPrint ("bgraphBipartGg: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  return (0);
}
