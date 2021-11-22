/* Copyright 2010,2014,2018,2021 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : wgraph_part_rb.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Jun-Ho HER (v6.0)                       **/
/**                                                        **/
/**   FUNCTION   : This module performs the vertex overla- **/
/**                pped graph partitioning based on recur- **/
/**                sive bipartitioning approach.           **/
/**                                                        **/
/**   DATES      : # Version 6.0  : from : 16 mar 2010     **/
/**                                 to   : 26 feb 2018     **/
/**                # Version 6.1  : from : 01 nov 2021     **/
/**                                 to   : 25 nov 2021     **/
/**                                                        **/
/**   NOTES      : # This code originally derived from     **/
/**                  the code of kgraph_map_rb_part.c,     **/
/**                  which was then adapted for vertex     **/
/**                  overlapped graph partitioning.        **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define WGRAPH_PART_RB

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "arch.h"
#include "vgraph.h"
#include "vgraph_separate_st.h"
#include "vgraph_separate_zr.h"
#include "wgraph.h"
#include "wgraph_part_rb.h"

/***********************************/
/*                                 */
/* Recursion management routines.  */
/*                                 */
/***********************************/

/* This routine propagates the local frontier
** array to the global frontier array.
** It returns:
** - void  : in all cases.
*/

static
void
wgraphPartRb3Fron (
WgraphPartRbData * restrict const dataptr,        /* Top-level graph and partition data */
const Graph * restrict const      orggrafptr,     /* Graph to induce and bipartition    */
const Gnum * restrict const       orgfrontab,     /* Frontier array of original graph   */
const Gnum                        orgfronnbr)     /* Part of graph to consider          */
{
  Gnum                fronnbr;
  Gnum                fronnum;

  const Gnum * restrict const       orgvnumtax = orggrafptr->vnumtax;
  Gnum * restrict const             frontab    = dataptr->frontab;

  fronnbr = dataptr->fronnbr;                     /* Get position where to insert frontier */
  dataptr->fronnbr = fronnbr + orgfronnbr;        /* Update current frontier end position  */

  if (orgvnumtax == NULL)                         /* If original graph is not itself a subgraph */
    memCpy (frontab + fronnbr, orgfrontab, orgfronnbr * sizeof (Gnum)); /* Directly copy array  */
  else {                                          /* Original graph is a subgraph               */
    for (fronnum = 0; fronnum < orgfronnbr; fronnum ++, fronnbr ++)
      frontab[fronnbr] = orgvnumtax[orgfrontab[fronnum]];
  }
}

/* This routine propagates the local frontier
** array to the global frontier array, and sets
** the separator part array.
** It returns:
** - void  : in all cases.
*/

static
void
wgraphPartRb3SepFron (
WgraphPartRbData * restrict const dataptr,        /* Top-level graph and partition data */
const Graph * restrict const      orggrafptr,     /* Graph to induce and bipartition    */
const Gnum * restrict const       orgfrontab,     /* Frontier array of original graph   */
const Gnum                        orgfronnbr)     /* Part of graph to consider          */
{
  Gnum                fronnbr;
  Gnum                fronnum;

  const Gnum * restrict const       orgvnumtax = orggrafptr->vnumtax;
  Anum * restrict const             parttax    = dataptr->parttax;
  Gnum * restrict const             frontab    = dataptr->frontab;

#ifdef SCOTCH_DEBUG_WGRAPH2
  if (orgfrontab == NULL) {                       /* Part array must exist */
    errorPrint ("wgraphPartRb3SepFron: invalid parameters");
    return;
  }
#endif /* SCOTCH_DEBUG_WGRAPH2 */

  fronnbr = dataptr->fronnbr;                     /* Get position where to insert frontier */
  dataptr->fronnbr = fronnbr + orgfronnbr;        /* Update current frontier end position  */

  if (orgvnumtax == NULL) {                       /* If original graph is not itself a subgraph */
    for (fronnum = 0; fronnum < orgfronnbr; fronnum ++, fronnbr ++) {
      Gnum                vertnum;

      vertnum = orgfrontab[fronnum];
      frontab[fronnbr] = vertnum;
      parttax[vertnum] = -1;
    }
  }
  else {                                          /* Original graph is a subgraph */
    for (fronnum = 0; fronnum < orgfronnbr; fronnum ++, fronnbr ++) {
      Gnum                vertnum;

      vertnum = orgvnumtax[orgfrontab[fronnum]];
      frontab[fronnbr] = vertnum;
      parttax[vertnum] = -1;
    }
  }
}

/* This routine fills the global part array
** with part data from the given part and
** its separator.
** It returns:
** - void  : in all cases.
*/

static
void
wgraphPartRb3One (
WgraphPartRbData * restrict const dataptr,        /* Top-level graph and partition data */
const Graph * restrict const      orggrafptr,     /* Graph to induce and bipartition    */
const GraphPart * restrict const  orgparttax,     /* Part array of original graph       */
const int                         indpartval,     /* Part value to consider             */
const Anum                        inddomnnum)     /* Domain onto which to map the part  */
{
  Anum                indparttmp;                 /* Part value to exclude */
  Gnum                vertnum;

  const Gnum * restrict const       orgvnumtax = orggrafptr->vnumtax;
  Anum * restrict const             parttax    = dataptr->parttax;

#ifdef SCOTCH_DEBUG_WGRAPH2
  if (orgparttax == NULL) {                       /* Graph can never be a full graph */
    errorPrint ("wgraphPartRb3One: invalid parameters");
    return;
  }
#endif /* SCOTCH_DEBUG_WGRAPH2 */

  indparttmp = 1 - indpartval;                    /* Part to exclude from update                */
  if (orgvnumtax == NULL) {                       /* If original graph is not itself a subgraph */
    for (vertnum = orggrafptr->baseval; vertnum < orggrafptr->vertnnd; vertnum ++) {
      GraphPart           orgpartval;

      orgpartval = orgparttax[vertnum];
      if (orgpartval != indparttmp)               /* If vertex belongs to the right part or the separator */
        parttax[vertnum] = (orgpartval == indpartval) ? inddomnnum : -1;
    }
  }
  else {
    for (vertnum = orggrafptr->baseval; vertnum < orggrafptr->vertnnd; vertnum ++) {
      GraphPart           orgpartval;

      orgpartval = orgparttax[vertnum];
      if (orgpartval != indparttmp)               /* If vertex belongs to the right part or the separator */
        parttax[orgvnumtax[vertnum]] = (orgpartval == indpartval) ? inddomnnum : -1;
    }
  }
}

/* This routine fills the global part array
** with part data from both parts and
** their separator.
** It returns:
** - void  : in all cases.
*/

static
void
wgraphPartRb3Both (
WgraphPartRbData * restrict const dataptr,        /* Top-level graph and partition data */
const Graph * restrict const      orggrafptr,     /* Graph to induce and bipartition    */
const GraphPart * restrict const  orgparttax,     /* Part array of original graph       */
const Anum                        inddomnnum)     /* Part of graph to consider          */
{
  Gnum                vertnum;

  const Gnum * restrict const       orgvnumtax = orggrafptr->vnumtax;
  Anum * restrict const             parttax    = dataptr->parttax;

#ifdef SCOTCH_DEBUG_WGRAPH2
  if (orgparttax == NULL) {                       /* Part array must exist */
    errorPrint ("wgraphPartRb3Both: invalid parameters");
    return;
  }
#endif /* SCOTCH_DEBUG_WGRAPH2 */

  if (orgvnumtax == NULL) {                       /* If original graph is not itself a subgraph */
    for (vertnum = orggrafptr->baseval; vertnum < orggrafptr->vertnnd; vertnum ++) {
      GraphPart           orgpartval;

      orgpartval = orgparttax[vertnum];
      parttax[vertnum] = (orgpartval < 2) ? (inddomnnum + (Anum) orgpartval) : -1;
    }
  }
  else {
    for (vertnum = orggrafptr->baseval; vertnum < orggrafptr->vertnnd; vertnum ++) {
      GraphPart           orgpartval;

      orgpartval = orgparttax[vertnum];
      parttax[orgvnumtax[vertnum]] = (orgpartval < 2) ? (inddomnnum + (Anum) orgpartval) : -1;
    }
  }
}

/* This routine is the recursive vertex
** bipartitioning core routine.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
int
wgraphPartRb2 (
WgraphPartRbData * restrict const dataptr,        /* Top-level graph and partition data       */
Graph * restrict const            orggrafptr,     /* Graph to induce and bipartition          */
const Gnum * restrict const       orgfrontab,     /* Graph frontier array                     */
const Gnum                        orgfronnbr,     /* Number of frontier vertices              */
const GraphPart * restrict const  orgparttax,     /* Part array of original graph to consider */
const GraphPart                   indpartval,     /* Part of graph to consider                */
const int                         indvertnbr,     /* Number of vertices in part or graph      */
const Anum                        inddomnnum,     /* Initial domain number to map             */
const Anum                        inddomnsiz)     /* Number of domains to map                 */
{
  Vgraph              actgrafdat;
  Anum                tmpdomnnum;
  Anum                tmpdomnsiz;
  int                 o;

  if (indpartval == 0) {                          /* If in small branch of the recursion; TRICK: never at first call      */
    if (inddomnsiz <= 1) {                        /* If target domain is terminal                                         */
      wgraphPartRb3Fron (dataptr, orggrafptr, orgfrontab, orgfronnbr); /* Copy previous frontier to global frontier array */
      wgraphPartRb3One  (dataptr, orggrafptr, orgparttax, indpartval, inddomnnum); /* Update mapping and return           */
      return (0);
    }
    wgraphPartRb3SepFron (dataptr, orggrafptr, orgfrontab, orgfronnbr); /* Copy previous frontier to global frontier array and update separator */
  }

  if (orgparttax == NULL) {                       /* If working graph is original graph */
    actgrafdat.s = *orggrafptr;                   /* Clone original graph data          */
    actgrafdat.s.flagval &= ~GRAPHFREETABS;       /* Nothing to be freed (yet)          */
    actgrafdat.s.vlbltax  = NULL;                 /* Vertex labels are no use           */
  }
  else {                                          /* If not the case, build induced subgraph */
    if (graphInducePart (orggrafptr, orgparttax, indvertnbr, indpartval, &actgrafdat.s) != 0) {
      errorPrint ("wgraphPartRb2: cannot induce graph");
      return (1);
    }
  }

  if (memAllocGroup ((void **) (void *)
                     &actgrafdat.parttax, (size_t) (actgrafdat.s.vertnbr * sizeof (GraphPart)),
                     &actgrafdat.frontab, (size_t) (actgrafdat.s.vertnbr * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("wgraphPartRb2: out of memory");
    graphExit  (&actgrafdat.s);
    return (1);
  }
  actgrafdat.parttax   -= actgrafdat.s.baseval;
  actgrafdat.s.flagval |= VGRAPHFREEPART;         /* Free group leader */
  actgrafdat.levlnum    = 0;                      /* Initial level     */

  vgraphZero (&actgrafdat);
  if (vgraphSeparateSt (&actgrafdat, dataptr->straptr) != 0) { /* Perform bipartitioning */
    errorPrint ("wgraphPartRb2: cannot bipartition graph");
    vgraphExit (&actgrafdat);
    return (1);
  }

  if (inddomnsiz <= 2) {                          /* If end of recursion, set both parts and separator */
    wgraphPartRb3Fron (dataptr, &actgrafdat.s, actgrafdat.frontab, actgrafdat.fronnbr);
    wgraphPartRb3Both (dataptr, &actgrafdat.s, actgrafdat.parttax, inddomnnum);
    vgraphExit        (&actgrafdat);
    return (0);
  }

  tmpdomnsiz = inddomnsiz / 2;                    /* Compute median values */
  tmpdomnnum = inddomnnum + tmpdomnsiz;

  if (actgrafdat.compsize[0] <= 0) {              /* If subpart is empty, run on other part (without considering separator vertices)                   */
    o = wgraphPartRb2 (dataptr, &actgrafdat.s, NULL, 0, NULL, 1, actgrafdat.s.vertnbr, tmpdomnnum, inddomnsiz - tmpdomnsiz); /* TRICK: use fake part 1 */
    vgraphExit        (&actgrafdat);
    return (o);
  }
  if (actgrafdat.compsize[1] <= 0) {              /* If subpart is empty, run on other part */
    o = wgraphPartRb2 (dataptr, &actgrafdat.s, NULL, 0, NULL, 1, actgrafdat.s.vertnbr, inddomnnum, tmpdomnsiz);
    vgraphExit        (&actgrafdat);
    return (o);
  }

  o = wgraphPartRb2 (dataptr, &actgrafdat.s, actgrafdat.frontab, actgrafdat.fronnbr, actgrafdat.parttax, 0,
                     actgrafdat.compsize[0], inddomnnum, tmpdomnsiz);
  if (o == 0)
    o = wgraphPartRb2 (dataptr, &actgrafdat.s, actgrafdat.frontab, actgrafdat.fronnbr, actgrafdat.parttax, 1,
                       actgrafdat.compsize[1], tmpdomnnum, inddomnsiz - tmpdomnsiz);

  vgraphExit (&actgrafdat);

  return (o);
}

/*********************************************/
/*                                           */
/* This is the entry point for vertex        */
/* overlapped graph partitioning based on    */
/* on the recursive bipartitioning approach. */
/*                                           */
/*********************************************/

int
wgraphPartRb (
Wgraph * restrict const                   grafptr,
const WgraphPartRbParam * restrict const  paraptr)
{
  WgraphPartRbData    datadat;

  if (grafptr->partnbr <= 1) {                    /* If only one part needed    */
    wgraphZero (grafptr);                         /* All vertices set to part 0 */
    return (0);
  }

  datadat.grafptr = &grafptr->s;                  /* Start with full graph    */
  datadat.parttax = grafptr->parttax;             /* Take part array          */
  datadat.frontab = grafptr->frontab;             /* Take frontier array      */
  datadat.fronnbr = 0;                            /* No frontier vertices yet */
  datadat.straptr = paraptr->straptr;

  if (wgraphPartRb2 (&datadat, &grafptr->s, NULL, 0, NULL, 1, grafptr->s.vertnbr, 0, grafptr->partnbr) != 0) { /* TRICK: initial fake part is 1 */
    errorPrint ("wgraphPartRb: cound not perform recursion");
    return (1);
  }
  grafptr->fronnbr = datadat.fronnbr;             /* Set overall number of frontier vertices */

  if (wgraphCost (grafptr) != 0) {
    errorPrint ("wgraphPartRb: could not compute partition cost");
    return (1);
  }

#ifdef SCOTCH_DEBUG_WGRAPH2
  if (wgraphCheck (grafptr) != 0) {
    errorPrint ("wgraphPartRb: inconsistent graph data");
    return (1);
  }
#endif /* SCOTCH_DEBUG_WGRAPH2 */

  return (0);
}
