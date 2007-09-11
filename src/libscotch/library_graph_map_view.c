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
/**   NAME       : library_graph_map-view.c                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the mapping  **/
/**                routines of the libSCOTCH library.      **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 19 aug 1998     **/
/**                                 to     20 aug 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     30 mar 1999     **/
/**                # Version 3.4  : from : 01 nov 2001     **/
/**                                 to     01 nov 2001     **/
/**                # Version 4.0  : from : 13 jan 2004     **/
/**                                 to     30 nov 2006     **/
/**                # Version 5.0  : from : 04 feb 2007     **/
/**                                 to     12 sep 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY
#define LIBRARY_GRAPH_MAP_VIEW

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "kgraph.h"
#include "library_mapping.h"
#include "library_graph_map_view.h"
#include "scotch.h"

/************************************/
/*                                  */
/* These routines are the C API for */
/* the mapping routines.            */
/*                                  */
/************************************/

/*+ This routine writes mapping statistics
*** to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphMapView (
const SCOTCH_Graph * const    libgrafptr,
const SCOTCH_Mapping * const  libmappptr,
FILE * const                  stream)
{
  const Graph * restrict    grafptr;
  const Mapping * restrict  mappptr;
  Anum * restrict           parttax;              /* Part array                                   */
  MappingSort * restrict    domtab;               /* Pointer to domain sort array                 */
  ArchDom                   domfrst;              /* Largest domain in architecture               */
  Anum                      tgtnbr;               /* Number of processors in target topology      */
  Anum                      mapnbr;               /* Number of processors effectively used        */
  Anum                      mapnum;
  double                    mapavg;               /* Average mapping weight                       */
  Gnum                      mapmin;
  Gnum                      mapmax;
  Gnum                      mapsum;               /* (Partial) sum of vertex loads                */
  double                    mapdlt;
  double                    mapmmy;               /* Maximum / average ratio                      */
  Anum *                    labltab;
  Anum *                    labltax;
  Anum * restrict           ngbtab;               /* Table storing neighbors of current subdomain */
  Anum                      ngbnbr;
  Anum                      ngbsum;
  Anum                      ngbnum;
  Anum                      ngbmin;
  Anum                      ngbmax;
  Gnum                      vertnum;
  Gnum                      vertload;
  Gnum                      edgeload;
  Gnum                      commdist[256];        /* Array of load distribution */
  Gnum                      commload;             /* Total edge load (edge sum) */
  Gnum                      commdilat;            /* Total edge dilation        */
  Gnum                      commexpan;            /* Total edge expansion       */
  Anum                      distmax;
  Anum                      distval;
  Gnum                      diammin;
  Gnum                      diammax;
  Gnum                      diamsum;
  Anum                      i;

  grafptr = (Graph *) libgrafptr;
  mappptr = &((LibMapping *) libmappptr)->m;

  if ((grafptr->vertnbr == 0) ||                  /* Return if nothing to do */
      (grafptr->edgenbr == 0))
    return (0);

  if (memAllocGroup ((void **) (void *)
                     &domtab,  (size_t) ((grafptr->vertnbr + 1) * sizeof (MappingSort)),
                     &ngbtab,  (size_t) ((grafptr->vertnbr + 2) * sizeof (Anum)),
                     &parttax, (size_t) (grafptr->vertnbr       * sizeof (Anum)),
                     &labltab, (size_t) (grafptr->vertnbr       * sizeof (Anum)), NULL) == NULL) {
    errorPrint ("mapView: out of memory");
    return     (1);
  }

  memSet (parttax, ~0, grafptr->vertnbr * sizeof (Anum));
  parttax -= grafptr->baseval;

  for (vertnum = 0; vertnum < grafptr->vertnbr; vertnum ++) {
    labltab[vertnum]     =
    domtab[vertnum].labl = archDomNum (&mappptr->archdat, mapDomain (mappptr, vertnum + grafptr->baseval));
    domtab[vertnum].peri = vertnum + grafptr->baseval; /* Build inverse permutation */
  }
  domtab[grafptr->vertnbr].labl = ARCHDOMNOTTERM; /* TRICK: avoid testing (i+1)    */
  domtab[grafptr->vertnbr].peri = ~0;             /* Prevent Valgrind from yelling */

  intSort2asc2 (domtab, grafptr->vertnbr);        /* Sort domain label array by increasing target labels */

  archDomFrst (&mappptr->archdat, &domfrst);      /* Get architecture domain   */
  tgtnbr = archDomSize (&mappptr->archdat, &domfrst); /* Get architecture size */

  mapsum   = 0;
  mapnbr   = 0;
  vertload = 1;                                   /* Assume unweighted vertices */
  for (vertnum = 0; domtab[vertnum].labl != ARCHDOMNOTTERM; vertnum ++) {
    parttax[domtab[vertnum].peri] = mapnbr;       /* Build map of partition parts starting from 0 */
    if (domtab[vertnum].labl != domtab[vertnum + 1].labl) /* TRICK: if new (or end) domain label  */
      mapnbr ++;
    if (grafptr->velotax != NULL)
      vertload = grafptr->velotax[domtab[vertnum].peri];
    mapsum += vertload;
  }
  mapavg = (mapnbr == 0) ? 0.0L : (double) mapsum / (double) mapnbr;

  mapsum = 0;
  mapmin = INT_MAX;
  mapmax = 0;
  mapdlt = 0.0L;
  for (vertnum = 0; domtab[vertnum].labl != ARCHDOMNOTTERM; vertnum ++) {
    if (grafptr->velotax != NULL)
      vertload = grafptr->velotax[domtab[vertnum].peri];
    mapsum += vertload;

    if (domtab[vertnum].labl != domtab[vertnum + 1].labl) { /* TRICK: if new (or end) domain label */
      if (mapsum < mapmin)
        mapmin = mapsum;
      if (mapsum > mapmax)
        mapmax = mapsum;
      mapdlt += fabs ((double) mapsum - mapavg);
      mapsum = 0;                                 /* Reset domain load sum */
    }
  }
  mapdlt = (mapnbr != 0) ? mapdlt / ((double) mapnbr * mapavg) : 0.0L;
  mapmmy = (mapnbr != 0) ? (double) mapmax / (double) mapavg : 0.0L;

  if (mapnbr > tgtnbr) {                          /* If more subdomains than architecture size */
#ifdef SCOTCH_DEBUG_MAP2
    if (strncmp (archName (&mappptr->archdat), "var", 3) != 0) { /* If not a variable-sized architecture */
      errorPrint ("mapView: invalid mapping");
      memFree    (domtab);                        /* Free group leader */
      return     (1);
    }
#endif /* SCOTCH_DEBUG_MAP2 */
    tgtnbr = mapnbr;                              /* Assume it is a variable-sized architecture */
  }

  fprintf (stream, "M\tProcessors %ld/%ld (%g)\n",
           (long) mapnbr, (long) tgtnbr,
           (double) mapnbr / (double) tgtnbr);
  fprintf (stream, "M\tTarget min=%ld\tmax=%ld\tavg=%g\tdlt=%g\tmaxavg=%g\n",
           (long) mapmin,
           (long) mapmax,
           mapavg,
           mapdlt,
           mapmmy);

  labltax = labltab - grafptr->baseval;

  ngbnbr = 0;
  ngbmin = INT_MAX;
  ngbmax = 0;
  ngbsum = 0;
  ngbnum = 0;
  ngbtab[0] = INT_MAX;
  for (vertnum = 0; domtab[vertnum].labl != ARCHDOMNOTTERM; vertnum ++) {
    Gnum                edgenum;

    for (edgenum = grafptr->verttax[domtab[vertnum].peri];
         edgenum < grafptr->vendtax[domtab[vertnum].peri];
         edgenum ++) {
      if ((labltax[grafptr->edgetax[edgenum]] != labltax[domtab[vertnum].peri]) && /* If edge is not internal */
          (labltax[grafptr->edgetax[edgenum]] != ngbtab[ngbnum])) /* And neighbor has not just been found     */
        ngbtab[++ ngbnum] = labltax[grafptr->edgetax[edgenum]];
    }
    if (domtab[vertnum].labl != domtab[vertnum + 1].labl) { /* TRICK: if new (or end) domain label  */
      intSort1asc1 (ngbtab, ngbnum + 1);          /* Sort neighbor label array by increasing labels */
      for (i = 0, ngbnbr = 0; ngbtab[i] != INT_MAX; i ++) {
        if (ngbtab[i] != ngbtab[i + 1])
          ngbnbr ++;
      }
      if (ngbnbr < ngbmin)
        ngbmin = ngbnbr;
      if (ngbnbr > ngbmax)
        ngbmax = ngbnbr;
      ngbsum += ngbnbr;

      ngbnum = 0;
      ngbtab[0] = INT_MAX;
    }
  }

  fprintf (stream, "M\tNeighbors min=%ld\tmax=%ld\tsum=%ld\n",
           (long) ngbmin,
           (long) ngbmax,
           (long) ngbsum);

  memset (commdist, 0, 256 * sizeof (Gnum));      /* Initialize the data */
  commload  =
  commdilat =
  commexpan = 0;

  edgeload = 1;
  for (vertnum = 0; vertnum < grafptr->vertnbr; vertnum ++) {
    Gnum                edgenum;

    if (labltab[vertnum] == ARCHDOMNOTTERM)       /* Skip unmapped vertices */
      continue;
    for (edgenum = grafptr->verttax[vertnum + grafptr->baseval];
         edgenum < grafptr->vendtax[vertnum + grafptr->baseval]; edgenum ++) {
      if (labltax[grafptr->edgetax[edgenum]] == ARCHDOMNOTTERM)
        continue;
      distval = archDomDist (&mappptr->archdat, mapDomain (mappptr, vertnum + grafptr->baseval), mapDomain (mappptr, grafptr->edgetax[edgenum]));
      if (grafptr->edlotax != NULL)              /* Get edge weight if any */
        edgeload = grafptr->edlotax[edgenum];
      commdist[(distval > 255) ? 255 : distval] += edgeload;
      commload  += edgeload;
      commdilat += distval;
      commexpan += distval * edgeload;
    }
  }

  fprintf (stream, "M\tCommDilat=%f\t(%u)\n",     /* Print expansion parameters */
           (double) commdilat / grafptr->edgenbr,
           commdilat / 2);
  fprintf (stream, "M\tCommExpan=%f\t(%u)\n",
           ((commload == 0) ? (double) 0.0L
                            : (double) commexpan / (double) commload),
           commexpan / 2);
  fprintf (stream, "M\tCommCutSz=%f\t(%u)\n",
           ((commload == 0) ? (double) 0.0L
                            : (double) (commload - commdist[0]) / (double) commload),
           (commload - commdist[0]) / 2);
  fprintf (stream, "M\tCommDelta=%f\n",
           (((double) commload  * (double) commdilat) == 0.0L)
           ? (double) 0.0L
           : ((double) commexpan * (double) grafptr->edgenbr) /
             ((double) commload  * (double) commdilat));

  for (distmax = 255; distmax != -1; distmax --)  /* Find longest distance */
    if (commdist[distmax] != 0)
      break;
  for (distval = 0; distval <= distmax; distval ++) /* Print distance histogram */
    fprintf (stream, "M\tCommLoad[%ld]=%f\n",
             (long) distval, (double) commdist[distval] / (double) commload);

  diammin = INT_MAX;
  diammax = 0;
  diamsum = 0;
  for (mapnum = 0; mapnum < mapnbr; mapnum ++) {
    Gnum                diamval;

    diamval  = graphMapView2 (grafptr, parttax, mapnum);
    diamsum += diamval;
    if (diamval < diammin)
      diammin = diamval;
    if (diamval > diammax)
      diammax = diamval;
  }
  fprintf (stream, "M\tPartDiam\tmin=%ld\tmax=%ld\tavg=%lf\n",
           (long) diammin, (long) diammax, (double) diamsum / (double) mapnbr);

  memFree (domtab);                               /* Free group leader */

  return (0);
}

/*+ This routine computes the pseudo-diameter of
*** tthe given part.    
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

static
Gnum
graphMapView2 (
const Graph * const         grafptr,              /*+ Graph      +*/
const Anum * const          parttax,              /*+ Part array +*/
const Anum                  partval)              /*+ Part value +*/
{
  GraphMapViewQueue             queudat;          /* Neighbor queue                         */
  GraphMapViewVertex * restrict vexxtax;          /* Based access to vexxtab                */
  Gnum                          rootnum;          /* Number of current root vertex          */
  Gnum                          vertdist;         /* Vertex distance                        */
  int                           diamflag;         /* Flag set if diameter changed           */
  Gnum                          diambase;         /* Base distance for connected components */
  Gnum                          diamdist;         /* Current diameter distance              */
  Gnum                          diamnum;          /* Vertex which achieves diameter         */
  Gnum                          passnum;          /* Pass number                            */
  const Gnum * restrict         verttax;          /* Based access to vertex array           */
  const Gnum * restrict         vendtax;          /* Based access to vertex end array       */
  const Gnum * restrict         edgetax;

  if (memAllocGroup ((void **) (void *)
                     &queudat.qtab, (size_t) (grafptr->vertnbr * sizeof (Gnum)),
                     &vexxtax,      (size_t) (grafptr->vertnbr * sizeof (GraphMapViewVertex)), NULL) == NULL) {
    errorPrint ("graphMapView2: out of memory");
    return     (-1);
  }

  memSet (vexxtax, 0, grafptr->vertnbr * sizeof (GraphMapViewVertex)); /* Initialize pass numbers */
  edgetax  = grafptr->edgetax;
  verttax  = grafptr->verttax;
  vendtax  = grafptr->vendtax;
  vexxtax -= grafptr->baseval;

  diamnum  = 0;                                   /* Start distances from zero */
  diamdist = 0;
  for (passnum = 1, rootnum = grafptr->baseval; ; passnum ++) { /* For all connected components */
    while ((rootnum < grafptr->vertnbr) &&
           ((vexxtax[rootnum].passnum != 0) ||    /* Find first unallocated vertex */
            (parttax[rootnum] != partval)))
      rootnum ++;
    if (rootnum >= grafptr->vertnbr)              /* Exit if all of graph processed */
      break;

    diambase = ++ diamdist;                       /* Start from previous distance */
    diamnum  = rootnum;                           /* Start from found root        */

    for (diamflag = 1; diamflag -- != 0; passnum ++) { /* Loop if modifications */
      graphMapViewQueueFlush (&queudat);          /* Flush vertex queue         */
      graphMapViewQueuePut   (&queudat, diamnum); /* Start from diameter vertex */
      vexxtax[diamnum].passnum  = passnum;        /* It has been enqueued       */
      vexxtax[diamnum].vertdist = diambase;       /* It is at base distance     */

      do {                                        /* Loop on vertices in queue */
        Gnum                vertnum;
        Gnum                edgenum;

        vertnum  = graphMapViewQueueGet (&queudat); /* Get vertex from queue */
        vertdist = vexxtax[vertnum].vertdist;     /* Get vertex distance     */

        if ((vertdist > diamdist) ||              /* If vertex increases diameter */
            ((vertdist == diamdist) &&            /* Or is at diameter distance   */
             ((vendtax[vertnum] - verttax[vertnum]) < /* With smaller degree  */
              (vendtax[diamnum] - verttax[diamnum])))) {
          diamnum  = vertnum;                     /* Set it as new diameter vertex */
          diamdist = vertdist;
          diamflag = 1;
        }

        vertdist ++;                              /* Set neighbor distance */
        for (edgenum = verttax[vertnum]; edgenum < vendtax[vertnum]; edgenum ++) {
          Gnum                vertend;

          vertend = edgetax[edgenum];
          if ((vexxtax[vertend].passnum < passnum) && /* If vertex not queued yet */
              (parttax[vertend] == partval)) {    /* And of proper part           */
            graphMapViewQueuePut (&queudat, vertend); /* Enqueue neighbor vertex  */
            vexxtax[vertend].passnum  = passnum;
            vexxtax[vertend].vertdist = vertdist;
          }
        }
      } while (! graphMapViewQueueEmpty (&queudat)); /* As long as queue is not empty */
    }
  }

  return (diamdist);
}
