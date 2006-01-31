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
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the input/output        **/
/**                routines for the graph structure.       **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 19 oct 1998     **/
/**                                 to     17 apr 2002     **/
/**                                                        **/
/**   NOTES      : # This code has been taken verbatim     **/
/**                  from the graph I/O routines of        **/
/**                  Scotch. Consequently, graph formats   **/
/**                  are identical in both projects,       **/
/**                  although edge weights are not used    **/
/**                  by Emilio graphs.                     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH_IO

#include "common.h"
#include "graph.h"

/********************************/
/*                              */
/* The graph handling routines. */
/*                              */
/********************************/

/*+ This routine reads the given graph
*** structure from the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
graphLoad (
Graph * const               grafptr,
FILE * const                stream)
{
  INT *               indxtab;                    /* Vertex label/number index table   */
  INT                 edgenbr;                    /* Number of edges really allocated  */
  INT *               edgetnd;                    /* End of edge array                 */
  INT *               edgeptr;                    /* Pointer to current edge           */
  INT                 edloval;                    /* Value where to read edge load     */
  INT                 vertnum;                    /* Number of current vertex          */
  INT *               verttnd;                    /* End of vertex array               */
  INT *               vertptr;                    /* Pointer to current vertex         */
  INT *               veloptr;                    /* Pointer to current vertex load    */
  INT *               vlbltnd;                    /* End of vertex label array         */
  INT *               vlblptr;                    /* Pointer to current vertex label   */
  INT                 vlblmax;                    /* Maximum vertex label number       */
  INT                 versval;                    /* File version value                */
  INT                 degrval;                    /* Degree of current vertex          */
  INT                 propval;                    /* File flag value                   */
  char                proptab[4];                 /* Decomposed file flag value        */

  memset (grafptr, 0, sizeof (Graph));

  if ((intLoad (stream, &versval)          +      /* Read header */
       intLoad (stream, &grafptr->vertnbr) +
       intLoad (stream, &grafptr->edgenbr) +
       intLoad (stream, &grafptr->baseval) +
       intLoad (stream, &propval) != 5) ||
      (versval != 0)                    ||        /* Version should be zero */
      (propval < 0)                     ||
      (propval > 111)) {
    errorPrint ("graphLoad: bad input (1)");
    return     (1);
  }
  sprintf (proptab, "%03d", (int) propval);       /* Compute file properties */
  proptab[0] -= '0';
  proptab[1] -= '0';
  proptab[2] -= '0';

  if (((grafptr->verttab = (INT *) memAlloc ((grafptr->vertnbr + 1) * sizeof (INT))) == NULL) ||
      ((proptab[2] != 0) &&
       ((grafptr->vlbltab = (INT *) memAlloc (grafptr->vertnbr * sizeof (INT))) == NULL)) ||
      ((proptab[0] != 0) &&
       ((grafptr->velotab = (INT *) memAlloc (grafptr->vertnbr * sizeof (INT))) == NULL)) ||
      ((grafptr->edgetab = (INT *) memAlloc (grafptr->edgenbr * sizeof (INT))) == NULL)) {
    errorPrint ("graphLoad: out of memory (1)");
    graphExit  (grafptr);
    graphInit  (grafptr);
    return     (1);
  }

  edgenbr          = 0;                           /* No edges allocated yet  */
  vlblmax          = grafptr->vertnbr - 1;        /* No vertex labels known  */
  grafptr->degrmax = 0;                           /* No maximum degree yet   */
  for (vertptr = grafptr->verttab, verttnd = vertptr + grafptr->vertnbr,
       vlblptr = grafptr->vlbltab, veloptr = grafptr->velotab,
       edgeptr = grafptr->edgetab;
       vertptr < verttnd; vertptr ++) {
    if (vlblptr != NULL) {                        /* If must read label */
      if (intLoad (stream, vlblptr) != 1) {       /* Read label data    */
        errorPrint ("graphLoad: bad input (2)");
        graphExit  (grafptr);
        graphInit  (grafptr);
        return     (1);
      }
      if (*vlblptr > vlblmax)                     /* Get maximum vertex label */
        vlblmax = *vlblptr;
      vlblptr ++;
    }
    if (veloptr != NULL) {                        /* If must read vertex load */
      if (intLoad (stream, veloptr) != 1) {       /* Read vertex load data    */
        errorPrint ("graphLoad: bad input (3)");
        graphExit  (grafptr);
        graphInit  (grafptr);
        return     (1);
      }
      veloptr ++;
    }
    if (intLoad (stream, &degrval) != 1) {        /* Read vertex degree */
      errorPrint ("graphLoad: bad input (4)");
      graphExit  (grafptr);
      graphInit  (grafptr);
      return     (1);
    }
    if (grafptr->degrmax < degrval)               /* Set maximum degree */
      grafptr->degrmax = degrval;

    *vertptr = edgenbr + grafptr->baseval;        /* Set index in edge array */
    degrval += edgenbr;
    if (degrval > grafptr->edgenbr) {             /* Check if edge array overflows */
      errorPrint ("graphLoad: invalid arc count (1)");
      graphExit  (grafptr);
      graphInit  (grafptr);
      return     (1);
    }

    for ( ; edgenbr < degrval; edgenbr ++, edgeptr ++) {
      if (proptab[1] != 0) {                      /* If must read edge load */
        if (intLoad (stream, &edloval) != 1) {    /* Read edge load data    */
          errorPrint ("graphLoad: bad input (5)");
          graphExit  (grafptr);
          graphInit  (grafptr);
          return     (1);
        }
      }                                           /* Do not use edge load */
      if (intLoad (stream, edgeptr) != 1) {       /* Read edge data       */
        errorPrint ("graphLoad: bad input (6)");
        graphExit  (grafptr);
        graphInit  (grafptr);
        return     (1);
      }
    }
  }
  *vertptr = edgenbr + grafptr->baseval;          /* Set end of edge array */

  if (edgenbr != grafptr->edgenbr) {              /* Check if number of edges is correct */
    errorPrint ("graphLoad: invalid arc count (2)");
    graphExit  (grafptr);
    graphInit  (grafptr);
    return     (1);
  }

  if (grafptr->vlbltab != NULL) {                 /* If vertex label renaming necessary */
    if ((indxtab = (INT *) memAlloc ((vlblmax + 1) * sizeof (INT))) == NULL) {
      errorPrint ("graphLoad: out of memory (2)");
      graphExit  (grafptr);
      graphInit  (grafptr);
      return     (1);
    }

    memset (indxtab, ~0, (vlblmax + 1) * sizeof (INT)); /* Assume labels not used */
    for (vlblptr = grafptr->vlbltab, vlbltnd = vlblptr + grafptr->vertnbr, vertnum = grafptr->baseval;
         vlblptr < vlbltnd; vlblptr ++, vertnum ++) {
      if (indxtab[*vlblptr] != ~0) {              /* If vertex label already used */
        errorPrint ("graphLoad: duplicate vertex label");
        graphExit  (grafptr);
        graphInit  (grafptr);
        return     (1);
      }
      indxtab[*vlblptr] = vertnum;                /* Set vertex number index */
    }
    for (edgeptr = grafptr->edgetab, edgetnd = edgeptr + grafptr->edgenbr;
         edgeptr < edgetnd; edgeptr ++) {
      if (*edgeptr > vlblmax) {                   /* If invalid edge end number */
        errorPrint ("graphLoad: invalid arc end number (1)");
        graphExit  (grafptr);
        graphInit  (grafptr);
        return     (1);
      }
      if (indxtab[*edgeptr] == ~0) {              /* If unused edge end number */
        errorPrint ("graphLoad: invalid arc end number (2)");
        graphExit  (grafptr);
        graphInit  (grafptr);
        return     (1);
      }
      *edgeptr = indxtab[*edgeptr];               /* Replace label by number */
    }
    memFree (indxtab);                            /* Free index array */
  }

#ifdef GRAPH_DEBUG
  if (graphCheck (grafptr) != 0) {                /* Check graph consistency */
    errorPrint ("graphLoad: inconsistent graph data");
    graphExit  (grafptr);
    graphInit  (grafptr);
    return     (1);
  }
#endif /* GRAPH_DEBUG */

  return (0);
}

/*+ This routine saves the given graph
*** structure to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
graphSave (
const Graph * const         grafptr,
FILE * const                stream)
{
  INT                 vertnum;
  INT                 edgenum;
  char                propstr[4];                 /* Property string */
  int                 o;

  propstr[0] = (grafptr->vlbltab != NULL) ? '1' : '0'; /* Set property string */
  propstr[1] = '0';
  propstr[2] = (grafptr->velotab != NULL) ? '1' : '0';
  propstr[3] = '\0';

  if (fprintf (stream, "0\n%ld\t%ld\n%ld\t%3s\n", /* Write file header */
               (long) grafptr->vertnbr,
               (long) grafptr->edgenbr,
               (long) grafptr->baseval,
               propstr) == EOF) {
    errorPrint ("graphSave: bad output (1)");
    return     (1);
  }

  for (vertnum = 0, o = 0; vertnum < grafptr->vertnbr; vertnum ++) {
    if (grafptr->vlbltab != NULL)                 /* Write vertex label if necessary */
      o  = (fprintf (stream, "%ld\t", (long) grafptr->vlbltab[vertnum]) == EOF);
    if (grafptr->velotab != NULL)                 /* Write vertex load if necessary */
      o |= (fprintf (stream, "%ld\t", (long) grafptr->velotab[vertnum]) == EOF);
    o |= (fprintf (stream, "%ld", (long) (grafptr->verttab[vertnum + 1] - grafptr->verttab[vertnum])) == EOF); /* Write vertex degree */

    for (edgenum = grafptr->verttab[vertnum]; (edgenum < grafptr->verttab[vertnum + 1]) && (o == 0); edgenum ++) {
      o |= (putc ('\t', stream) == EOF);
      o |= (intSave (stream, (grafptr->vlbltab != NULL) /* Write edge end */
                               ? grafptr->vlbltab[grafptr->edgetab[edgenum] - grafptr->baseval]
                               : grafptr->edgetab[edgenum]) != 1);
    }
    o |= (putc ('\n', stream) == EOF);
  }

  if (o != 0)
    errorPrint ("graphSave: bad output (2)");

  return (o);
}
