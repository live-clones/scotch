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
/**   NAME       : graph_io.c                              **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the source graph    **/
/**                input/output functions.                 **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 01 dec 1992     **/
/**                                 to     18 may 1993     **/
/**                # Version 1.3  : from : 30 apr 1994     **/
/**                                 to     18 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     31 oct 1994     **/
/**                # Version 3.0  : from : 07 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 28 nov 1995     **/
/**                                 to     08 jun 1996     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     15 mar 1999     **/
/**                # Version 4.0  : from : 25 nov 2001     **/
/**                                 to     21 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH_IO

#include "module.h"
#include "common.h"
#include "graph.h"
#include "graph_io.h"

/*******************************************/
/*                                         */
/* These routines handle source graph I/O. */
/*                                         */
/*******************************************/

/* This routine loads a source graph from
** the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
graphLoad (
Graph * restrict const      grafptr,              /* Graph structure to fill              */
FILE * const                stream,               /* Stream from which to read graph data */
const Gnum                  baseval,              /* Base value (-1 means keep file base) */
const GraphFlag             flagval)              /* Graph loading flags                  */
{
  Gnum                edgenum;                    /* Number of edges really allocated */
  Gnum                edgennd;
  Gnum                vlblnbr;                    /* = vertnbr if vertex labels       */
  Gnum                vlblmax;                    /* Maximum vertex label number      */
  Gnum                vlblval;                    /* Value where to read vertex label */
  Gnum                velonbr;                    /* = vertnbr if vertex loads wanted */
  Gnum                veloval;                    /* Value where to read vertex load  */
  Gnum                edlonbr;                    /* = edgenbr if edge loads wanted   */
  Gnum                edloval;                    /* Value where to read edge load    */
  Gnum                edgeval;                    /* Value where to read edge end     */
  Gnum                baseadj;
  Gnum                versval;
  Gnum                degrval;
  Gnum                propval;
  char                proptab[4];
  Gnum                vertnum;

  memSet (grafptr, 0, sizeof (Graph));

  if (intLoad (stream, &versval) != 1) {          /* Read version number */
    errorPrint ("graphLoad: bad input (1)");
    return     (1);
  }
  if (versval != 0) {                             /* If version not zero */
    errorPrint ("graphLoad: old-style graph format no longer supported");
    return     (1);
  }

  if ((intLoad (stream, &grafptr->vertnbr) +      /* Read rest of header */
       intLoad (stream, &grafptr->edgenbr) +
       intLoad (stream, &baseadj)          +
       intLoad (stream, &propval) != 4) ||
      (propval < 0)                     ||
      (propval > 111)) {
    errorPrint ("graphLoad: bad input (2)");
    return     (1);
  }
  sprintf (proptab, "%3.3d", (int) propval);      /* Compute file properties */
  proptab[0] -= '0';                              /* Vertex labels flag      */
  proptab[1] -= '0';                              /* Edge weights flag       */
  proptab[2] -= '0';                              /* Vertex loads flag       */

  grafptr->flagval = GRAPHFREETABS | GRAPHVERTGROUP | GRAPHEDGEGROUP;
  if (baseval == -1) {                            /* If keep file graph base     */
    grafptr->baseval = baseadj;                   /* Set graph base as file base */
    baseadj          = 0;                         /* No base adjustment needed   */
  }
  else {                                          /* If set graph base     */
    grafptr->baseval = baseval;                   /* Set wanted graph base */
    baseadj          = baseval - baseadj;         /* Update base adjust    */
  }

  velonbr = ((proptab[2] != 0) && ((flagval & GRAPHIONOLOADVERT) == 0)) ? grafptr->vertnbr : 0;
  vlblnbr = (proptab[0] != 0) ? grafptr->vertnbr : 0;
  edlonbr = ((proptab[1] != 0) && ((flagval & GRAPHIONOLOADEDGE) == 0)) ? grafptr->edgenbr : 0;

  if ((memAllocGroup ((void **)
                      &grafptr->verttax, (size_t) ((grafptr->vertnbr + 1) * sizeof (Gnum)),
                      &grafptr->velotax, (size_t) (velonbr                * sizeof (Gnum)),
                      &grafptr->vlbltax, (size_t) (vlblnbr                * sizeof (Gnum)), NULL) == NULL) ||
      (memAllocGroup ((void **)
                      &grafptr->edgetax, (size_t) (grafptr->edgenbr       * sizeof (Gnum)),
                      &grafptr->edlotax, (size_t) (edlonbr                * sizeof (Gnum)), NULL) == NULL)) {
    if (grafptr->verttax != NULL)
      memFree (grafptr->verttax);
    errorPrint ("graphLoad: out of memory");
    graphFree  (grafptr);
    return     (1);
  }
  grafptr->vertnnd  = grafptr->vertnbr + grafptr->baseval;
  grafptr->verttax -= grafptr->baseval;
  grafptr->vendtax  = grafptr->verttax + 1;       /* Use compact vertex array */
  grafptr->velotax  = (velonbr != 0) ? (grafptr->velotax - grafptr->baseval) : NULL;
  grafptr->vlbltax  = (vlblnbr != 0) ? (grafptr->vlbltax - grafptr->baseval) : NULL;
  grafptr->edgetax -= grafptr->baseval;
  grafptr->edlotax  = (edlonbr != 0) ? (grafptr->edlotax - grafptr->baseval) : NULL;

  edgennd          = grafptr->edgenbr + grafptr->baseval;
  edgenum          = grafptr->baseval;            /* No edges allocated yet  */
  vlblmax          = grafptr->vertnnd - 1;        /* No vertex labels known  */
  grafptr->velosum = 0;                           /* No load accumulated yet */
  grafptr->degrmax = 0;                           /* No maximum degree yet   */

  for (vertnum = grafptr->baseval; vertnum < grafptr->vertnnd; vertnum ++) {
    if (grafptr->vlbltax != NULL) {               /* If must read label */
      if (intLoad (stream, &vlblval) != 1) {      /* Read label data    */
        errorPrint ("graphLoad: bad input (3)");
        graphFree  (grafptr);
        return     (1);
      }
      grafptr->vlbltax[vertnum] = vlblval + baseadj; /* Adjust vertex label   */
      if (grafptr->vlbltax[vertnum] > vlblmax)    /* Get maximum vertex label */
        vlblmax = grafptr->vlbltax[vertnum];
    }
    if (proptab[2] != 0) {                        /* If must read vertex load */
      if (intLoad (stream, &veloval) != 1) {      /* Read vertex load data    */
        errorPrint ("graphLoad: bad input (4)");
        graphFree  (grafptr);
        return     (1);
      }
      if (grafptr->velotax != NULL) {
        grafptr->velotax[vertnum] = veloval;
        grafptr->velosum         += veloval;
      }
    }
    if (intLoad (stream, &degrval) != 1) {        /* Read vertex degree */
      errorPrint ("graphLoad: bad input (5)");
      graphFree  (grafptr);
      return     (1);
    }
    if (grafptr->degrmax < degrval)               /* Set maximum degree */
      grafptr->degrmax = degrval;

    grafptr->verttax[vertnum] = edgenum;          /* Set index in edge array */
    degrval += edgenum;
    if (degrval > edgennd) {                      /* Check if edge array overflows */
      errorPrint ("graphLoad: invalid arc count (1)");
      graphFree  (grafptr);
      return     (1);
    }

    for ( ; edgenum < degrval; edgenum ++) {
      if (proptab[1] != 0) {                      /* If must read edge load */
        if (intLoad (stream, &edloval) != 1) {    /* Read edge load data    */
          errorPrint ("graphLoad: bad input (6)");
          graphFree  (grafptr);
          return     (1);
        }
        if (grafptr->edlotax != NULL)
          grafptr->edlotax[edgenum] = (Gnum) edloval;
      }
      if (intLoad (stream, &edgeval) != 1) {      /* Read edge data */
        errorPrint ("graphLoad: bad input (7)");
        graphFree  (grafptr);
        return     (1);
      }
      grafptr->edgetax[edgenum] = edgeval + baseadj;
    }
  }
  grafptr->verttax[vertnum] = edgenum;            /* Set end of edge array */

  if (edgenum != edgennd) {                       /* Check if number of edges is valid */
    errorPrint ("graphLoad: invalid arc count (2)");
    graphFree  (grafptr);
    return     (1);
  }

  if (grafptr->velotax == NULL)                   /* If vertex loads not computed     */
    grafptr->velosum = grafptr->vertnbr;          /* Total load is number of vertices */

  if (grafptr->vlbltax != NULL) {                 /* If vertex label renaming necessary       */
    if (graphLoad2 (grafptr->baseval, grafptr->vertnnd, grafptr->verttax, /* Rename edge ends */
                    grafptr->vendtax, grafptr->edgetax, vlblmax, grafptr->vlbltax) != 0) {
      errorPrint ("graphLoad: cannot relabel vertices");
      graphFree  (grafptr);
      return     (1);
    }
  }

#ifdef SCOTCH_DEBUG_GRAPH2
  if (graphCheck (grafptr) != 0) {                /* Check graph consistency */
    errorPrint ("graphLoad: inconsistent graph data");
    graphFree  (grafptr);
    return     (1);
  }
#endif /* SCOTCH_DEBUG_GRAPH2 */

  return (0);
}

int
graphLoad2 (
const Gnum                  baseval,
const Gnum                  vertnnd,
const Gnum * const          verttax,
const Gnum * const          vendtax,
Gnum * restrict const       edgetax,
const Gnum                  vlblmax,
const Gnum * const          vlbltax)
{
  Gnum                vertnum;                    /* Number of current vertex        */
  Gnum * restrict     indxtab;                    /* Vertex label/number index table */

  if ((indxtab = (Gnum *) memAlloc ((vlblmax + 1) * sizeof (Gnum))) == NULL) {
    errorPrint  ("graphLoad2: out of memory");
    return      (1);
  }

  memSet (indxtab, ~0, (vlblmax + 1) * sizeof (Gnum)); /* Assume labels not used */
  for (vertnum = baseval; vertnum < vertnnd; vertnum ++) {
    if (indxtab[vlbltax[vertnum]] != ~0) {        /* If vertex label already used */
      errorPrint  ("graphLoad2: duplicate vertex label");
      memFree     (indxtab);
      return      (1);
    }
    indxtab[vlbltax[vertnum]] = vertnum;          /* Set vertex number index */
  }
  for (vertnum = baseval; vertnum < vertnnd; vertnum ++) {
    Gnum                edgenum;                  /* Number of current edge */

    for (edgenum = verttax[vertnum]; edgenum < vendtax[vertnum]; edgenum ++) {
      if (edgetax[edgenum] > vlblmax) {           /* If invalid edge end number */
        errorPrint ("graphLoad2: invalid arc end number (1)");
        memFree    (indxtab);
        return     (1);
      }
      if (indxtab[edgetax[edgenum]] == ~0) {      /* If unused edge end number */
        errorPrint ("graphLoad2: invalid arc end number (2)");
        memFree    (indxtab);
        return     (1);
      }
      edgetax[edgenum] = indxtab[edgetax[edgenum]]; /* Replace label by number */
    }
  }

  memFree (indxtab);                              /* Free index array */

  return (0);
}

/* This routine saves a source graph to
** the given stream, in the new-style
** graph format.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
graphSave (
const Graph * const         grafptr,
FILE * const                stream)
{
  Gnum                vertnum;
  Gnum                edgenum;
  char                propstr[4];                 /* Property string */
  int                 o;

  propstr[0] = (grafptr->vlbltax != NULL) ? '1' : '0'; /* Set property string */
  propstr[1] = (grafptr->edlotax != NULL) ? '1' : '0';
  propstr[2] = (grafptr->velotax != NULL) ? '1' : '0';
  propstr[3] = '\0';

  if (fprintf (stream, "0\n%ld\t%ld\n%ld\t%3s\n", /* Write file header */
               (long) grafptr->vertnbr,
               (long) grafptr->edgenbr,
               (long) grafptr->baseval,
               propstr) == EOF) {
    errorPrint ("graphSave: bad output (1)");
    return     (1);
  }

  for (vertnum = grafptr->baseval, o = 0;
       (vertnum < grafptr->vertnnd) && (o == 0); vertnum ++) {
    if (grafptr->vlbltax != NULL)                 /* Write vertex label if necessary */
      o  = (fprintf (stream, "%ld\t", (long) grafptr->vlbltax[vertnum]) == EOF);
    if (grafptr->velotax != NULL)                 /* Write vertex load if necessary */
      o |= (fprintf (stream, "%ld\t", (long) grafptr->velotax[vertnum]) == EOF);
    o |= (fprintf (stream, "%ld", (long) (grafptr->vendtax[vertnum] - grafptr->verttax[vertnum])) == EOF); /* Write vertex degree */

    for (edgenum = grafptr->verttax[vertnum];
         (edgenum < grafptr->vendtax[vertnum]) && (o == 0); edgenum ++) {
      Gnum                vertend;

      o |= (putc ('\t', stream) == EOF);
      if (grafptr->edlotax != NULL)               /* Write edge load if necessary */
        o |= (fprintf (stream, "%ld ", (long) grafptr->edlotax[edgenum]) == EOF);
      vertend = grafptr->edgetax[edgenum];
      o |= (intSave (stream,                      /* Write edge end */
                     (grafptr->vlbltax != NULL) ? grafptr->vlbltax[vertend] : vertend) != 1);
    }
    o |= (putc ('\n', stream) == EOF);
  }

  if (o != 0)
    errorPrint ("graphSave: bad output (2)");

  return (o);
}
