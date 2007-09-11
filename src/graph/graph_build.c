/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : graph_build.c                           **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These module holds the generic graph    **/
/**                building routine.                       **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 15 oct 2000     **/
/**                                 to     15 oct 2000     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     28 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GRAPH

#include "common.h"
#include "scotch.h"
#include "graph.h"

/********************************/
/*                              */
/* The graph handling routines. */
/*                              */
/********************************/

/* This routine builds a graph
** structure from the given
** algorithmic graph definition.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
*/

int
graphBuild (
Graph * const               grafptr,              /*+ Graph to build                           +*/
const INT                   baseval,              /*+ Base value                               +*/
const INT                   vertnbr,              /*+ Number of vertices                       +*/
const INT                   edgenbr,              /*+ Number of arcs                           +*/
void * const                nghbptr,              /*+ Pointer to adjacency structure           +*/
INT                         nghbfrstfunc (void * const, const INT), /*+ Returns first neighbor +*/
INT                         nghbnextfunc (void * const)) /*+ Returns next neighbor             +*/
{
  INT * restrict      verttab;
  INT * restrict      verttax;
  INT * restrict      edgetab;
  INT * restrict      edgetax;
  INT                 vertnum;
  INT                 edgenum;

  if (sizeof (INT) != sizeof (SCOTCH_Num)) {      /* Check integer consistency */
    errorPrint ("graphBuild: inconsistent integer types");
    return     (1);
  }

  if (((verttab = (INT *) memAlloc ((vertnbr + 1) * sizeof (INT))) == NULL) ||
      ((edgetab = (INT *) memAlloc (edgenbr       * sizeof (INT))) == NULL)) {
    errorPrint ("graphBuild: out of memory");
    if (verttab != NULL)
      memFree (verttab);
    return (1);
  }
  verttax = verttab - baseval;
  edgetax = edgetab - baseval;

  for (vertnum = edgenum = baseval; vertnum < (vertnbr + baseval); vertnum ++) {
    INT                 vertend;

    verttax[vertnum] = edgenum;

    if ((vertend = nghbfrstfunc (nghbptr, vertnum)) >= baseval) {
#ifdef GRAPH_DEBUG
      if (edgenum >= (edgenbr + baseval)) {
        errorPrint ("graphBuild: too many edges in graph (1)");
        memFree    (edgetab);
        memFree    (verttab);
        return     (1);
      }
#endif /* GRAPH_DEBUG */
      edgetax[edgenum ++] = vertend;
      while ((vertend = nghbnextfunc (nghbptr)) >= baseval) {
#ifdef GRAPH_DEBUG
        if (edgenum >= (edgenbr + baseval)) {
          errorPrint ("graphBuild: too many edges in graph (2)");
          memFree    (edgetab);
          memFree    (verttab);
          return     (1);
        }
#endif /* GRAPH_DEBUG */
        edgetax[edgenum ++] = vertend;
      }
    }
  }
  verttax[vertnum] = edgenum;                     /* Set end of array */

  SCOTCH_graphBuild (grafptr, baseval, vertnbr, verttab, NULL, NULL, NULL,
                     edgenum - baseval, edgetab, NULL); /* Use exact number of arcs */

#ifdef GRAPH_DEBUG
  if (graphCheck (grafptr) != 0) {                /* Check graph consistency */
    errorPrint ("graphBuild: inconsistent graph data");
    memFree    (edgetab);
    memFree    (verttab);
    return     (1);
  }
#endif /* GRAPH_DEBUG */

  return (0);
}
