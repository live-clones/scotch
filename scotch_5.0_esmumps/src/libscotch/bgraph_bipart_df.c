/* Copyright 2004,2007 INRIA
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
/**   NAME       : bgraph_bipart_df.c                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module computes a bipartition of   **/
/**                a bipartition graph by using a          **/
/**                diffusion scheme.                       **/
/**                                                        **/
/**   NOTES      : # This algorithm has been designed to   **/
/**                  work on band graphs only, for which   **/
/**                  the two anchor vertices are the two   **/
/**                  last vertices, the before-last as     **/
/**                  anchor of part 0, and the last as     **/
/**                  anchor of part 1.                     **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 09 jan 2007     **/
/**                                 to     08 jun 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BGRAPH_BIPART_DF

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "bgraph.h"
#include "bgraph_bipart_df.h"

/*
**  The static variables.
*/

static const Gnum           bgraphbipartdfloadzero = 0;
static const Gnum           bgraphbipartdfloadone  = 1;

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
bgraphBipartDf (
Bgraph * restrict const           grafptr,        /*+ Active graph      +*/
const BgraphBipartDfParam * const paraptr)        /*+ Method parameters +*/
{
  BgraphBipartDfVertex * restrict vexxtax;        /* Extended vertex array     */
  float                           cdifval;        /* Diffusion coefficient     */
  float                           cremval;        /* Remains coefficient       */
  float * restrict                difotax;        /* Old diffusion value array */
  float * restrict                difntax;        /* New diffusion value array */
  Gnum                            fronnum;
  Gnum                            compload1;
  Gnum                            compsize1;
  Gnum                            commloadintn;
  Gnum                            commloadextn;
  Gnum                            commgainextn;
  const Gnum * restrict           veexbax;
  Gnum                            veexmsk;
  const Gnum * restrict           velobax;
  Gnum                            velomsk;
  Gnum                            vancval0;
  Gnum                            vancval1;
  Gnum                            vertnum;
  unsigned int                    passnum;

#ifdef SCOTCH_DEBUG_BGRAPH2
  if ((BGRAPHBIPARTDFGNUMSGNMSK ( 4242) !=  0) ||
      (BGRAPHBIPARTDFGNUMSGNMSK (-4242) != ~0)) {
    errorPrint ("bgraphBipartDf: internal error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  if (memAllocGroup ((void **)
                     &vexxtax, (size_t) (grafptr->s.vertnbr * sizeof (BgraphBipartDfVertex)),
                     &difotax, (size_t) (grafptr->s.vertnbr * sizeof (float)),
                     &difntax, (size_t) (grafptr->s.vertnbr * sizeof (float)), NULL) == NULL) {
    errorPrint ("bgraphBipartDf: out of memory (1)");
    return     (1);
  }
  vexxtax -= grafptr->s.baseval;                  /* Base access to vexxtax and diffusion arrays */
  difotax -= grafptr->s.baseval;
  difntax -= grafptr->s.baseval;

  vancval0 = (float) - grafptr->compload0avg;     /* Values to be injected to anchor vertices at every iteration */
  vancval1 = (float) (grafptr->s.velosum - grafptr->compload0avg);
  if (grafptr->s.edlotax == NULL) {               /* If graph has no edge weights */
    Gnum                vertnum;

    for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) {
      difotax[vertnum]         = 0.0;
      vexxtax[vertnum].veexval = 0.0;
      vexxtax[vertnum].ielsval = 1.0F / (float) (grafptr->s.vendtax[vertnum] - grafptr->s.verttax[vertnum]);
    }
  }
  else {                                          /* If graph has edge weights */
    Gnum                vertnum;

    for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) {
      Gnum                edgenum;
      Gnum                edlosum;

      for (edgenum = grafptr->s.verttax[vertnum], edlosum = 0;
           edgenum < grafptr->s.vendtax[vertnum]; edgenum ++)
        edlosum += grafptr->s.edlotax[edgenum];

      difotax[vertnum]         = 0.0;
      vexxtax[vertnum].veexval = 0.0;
      vexxtax[vertnum].ielsval = 1.0 / (float) edlosum;
    }
  }

  if (grafptr->veextax != NULL) {
    Gnum                vertnum;
    Gnum                veexsum;
    Gnum                veexsum0;
    float               idodist;                  /* Inverse of domdist */

    idodist = 1.0 / (float) grafptr->domdist;
    for (vertnum = grafptr->s.baseval, veexsum = veexsum0 = 0;
         vertnum < grafptr->s.vertnnd; vertnum ++) {
      Gnum                veexval;

      veexval = grafptr->veextax[vertnum];
      veexsum  += veexval;                        /* Sum all external gains, positive and negative           */
      veexsum0 += BGRAPHBIPARTDFGNUMSGNMSK (veexval); /* Sum all negative external gains; superscalar update */
      vexxtax[vertnum].veexval = (float) veexval / idodist;

    }
    vancval0 += (float) veexsum0;
    vancval1 += (float) (veexsum - veexsum0);
  }
  vancval1 -= BGRAPHBIPARTDFEPSILON;              /* Slightly tilt value to add to part 1 */

  vexxtax[grafptr->s.vertnnd - 2].veexval += vancval0; /* Negative value to inject to part 0 at every step */
  vexxtax[grafptr->s.vertnnd - 1].veexval += vancval1; /* Positive value to add to part 1, tilted          */

  difotax[grafptr->s.vertnnd - 2] = 10 * vexxtax[grafptr->s.vertnnd - 2].veexval * vexxtax[grafptr->s.vertnnd - 2].ielsval; /* Load anchor vertices for first pass */
  difotax[grafptr->s.vertnnd - 1] = 10 * vexxtax[grafptr->s.vertnnd - 1].veexval * vexxtax[grafptr->s.vertnnd - 1].ielsval;

  cdifval = (float) paraptr->cdifval;
  cremval = (float) paraptr->cremval;
  for (passnum = 0; passnum < paraptr->passnbr; passnum ++) { /* For all passes */
    Gnum                vertnum;
    float * restrict    difttax;                  /* Temporary swap value */
    float               veloval;

    veloval = 1.0F;                               /* Assume no vertex loads */
    for (vertnum = grafptr->s.baseval ; vertnum < grafptr->s.vertnnd; vertnum ++) {
      Gnum                edgenum;
      float               diffval;

      diffval = 0.0F;
      edgenum = grafptr->s.verttax[vertnum];
      if (grafptr->s.edlotax != NULL)
        for ( ; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++)
          diffval += difotax[grafptr->s.edgetax[edgenum]] * (float) grafptr->s.edlotax[edgenum];
      else
        for ( ; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++)
          diffval += difotax[grafptr->s.edgetax[edgenum]];

      diffval *= cdifval;
      diffval += vexxtax[vertnum].veexval + (difotax[vertnum] * cremval) / vexxtax[vertnum].ielsval;
      if (grafptr->s.velotax != NULL)
        veloval = (float) grafptr->s.velotax[vertnum];
      if (diffval >= 0.0F) {
        diffval = (diffval - veloval) * vexxtax[vertnum].ielsval;
        if (diffval <= 0.0F)
          diffval = +BGRAPHBIPARTDFEPSILON;
      }
      else {
        diffval = (diffval + veloval) * vexxtax[vertnum].ielsval;
        if (diffval >= 0.0F)
          diffval = -BGRAPHBIPARTDFEPSILON;
      }
      difntax[vertnum] = diffval;
    }

    difttax = difntax;                            /* Swap old and new diffusion arrays */
    difntax = difotax;
    difotax = difttax;
  }

  for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) { /* Set vertices as not flagged */
    grafptr->parttax[vertnum] = (difotax[vertnum] <= 0.0F) ? 0 : 1;
    vexxtax[vertnum].veexval  = 0.0F;
  }
  if (grafptr->veextax != NULL) {
    veexbax = grafptr->veextax;
    veexmsk = ~((Gnum) 0);
  }
  else {
    veexbax = &bgraphbipartdfloadzero;
    veexmsk = 0;
  }
  if (grafptr->s.velotax != NULL) {
    velobax = grafptr->s.velotax;
    velomsk = ~((Gnum) 0);
  }
  else {
    velobax = &bgraphbipartdfloadone;
    velomsk = 0;
  }

  for (vertnum = grafptr->s.baseval, fronnum = 0, commloadextn = grafptr->commloadextn0, commgainextn = commloadintn = compload1 = compsize1 = 0;
       vertnum < grafptr->s.vertnnd; vertnum ++) {
    Gnum                edgenum;
    Gnum                partval;
    Gnum                commload;                 /* Vertex internal communication load */

    partval = (Gnum) grafptr->parttax[vertnum];
    compsize1    += partval;
    compload1    += partval * velobax[vertnum & velomsk];
    commloadextn += partval * veexbax[vertnum & veexmsk];
    commgainextn += (1 - 2 * partval) * veexbax[vertnum & veexmsk];
    commload      = 0;
    if (grafptr->s.edlotax != NULL) {
      for (edgenum = grafptr->s.verttax[vertnum]; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
        Gnum                partend;

        partend   = (Gnum) grafptr->parttax[grafptr->s.edgetax[edgenum]];
        commload += (partval ^ partend) * grafptr->s.edlotax[edgenum];
      }
    }
    else {
      for (edgenum = grafptr->s.verttax[vertnum]; edgenum < grafptr->s.vendtax[vertnum]; edgenum ++)
        commload += partval ^ (Gnum) grafptr->parttax[grafptr->s.edgetax[edgenum]];
    }
    commloadintn += commload;                     /* Internal loads will be added twice */
    if (commload != 0)                            /* If end vertex is in the other part */
      grafptr->frontab[fronnum ++] = vertnum;     /* Then it belongs to the frontier    */
  }
  grafptr->fronnbr      = fronnum;
  grafptr->compload0    = grafptr->s.velosum - compload1;
  grafptr->compload0dlt = grafptr->compload0 - grafptr->compload0avg;
  grafptr->compsize0    = grafptr->s.vertnbr - compsize1;
  grafptr->commload     = commloadextn + (commloadintn / 2) * grafptr->domdist;
  grafptr->commgainextn = commgainextn;

  memFree (vexxtax + grafptr->s.baseval);

#ifdef SCOTCH_DEBUG_BGRAPH2
  if (bgraphCheck (grafptr) != 0) {
    errorPrint ("bgraphBipartDf: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  return (0);
}
