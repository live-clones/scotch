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
/**                                 to     28 nov 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "kgraph.h"
#include "library_mapping.h"
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
  MappingSort * restrict    domtab;               /* Pointer to domain sort array            */
  ArchDom                   domfrst;              /* Largest domain in architecture          */
  Anum                      tgtnbr;               /* Number of processors in target topology */
  Anum                      mapnbr;               /* Number of processors effectively used   */
  double                    mapavg;               /* Average mapping weight                  */
  Gnum                      mapmin;
  Gnum                      mapmax;
  Gnum                      mapsum;               /* (Partial) sum of vertex loads  */
  double                    mapdlt;
  double                    mapmmy;               /* Maximum / average ratio */
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
  Anum                      i;

  grafptr = (Graph *) libgrafptr;
  mappptr = &((LibMapping *) libmappptr)->m;

  if ((grafptr->vertnbr == 0) ||                  /* Return if nothing to do */
      (grafptr->edgenbr == 0))
    return (0);

  if (((domtab  = (MappingSort *) memAlloc ((grafptr->vertnbr + 1) * sizeof (MappingSort))) == NULL) ||
      ((ngbtab  = (Anum *)        memAlloc ((grafptr->vertnbr + 2) * sizeof (Anum)))        == NULL) ||
      ((labltab = (Anum *)        memAlloc (grafptr->vertnbr       * sizeof (Anum)))        == NULL)) {
    errorPrint ("mapView: out of memory");
    if (domtab != NULL) {
      memFree (domtab);
      if (ngbtab != NULL)
        memFree (ngbtab);
    }
    return (1);
  }

  for (vertnum = 0; vertnum < grafptr->vertnbr; vertnum ++) {
    labltab[vertnum]     =
    domtab[vertnum].labl = archDomNum (&mappptr->archdat, mapDomain (mappptr, vertnum));
    domtab[vertnum].peri = vertnum + grafptr->baseval; /* Build inverse permutation */
  }
  domtab[grafptr->vertnbr].labl = ARCHDOMNOTTERM; /* TRICK: avoid testing (i+1)  */
  domtab[grafptr->vertnbr].peri = ~0;             /* Prevent Purify from yelling */

  intSort2asc2 (domtab, grafptr->vertnbr);        /* Sort domain label array by increasing target labels */

  archDomFrst (&mappptr->archdat, &domfrst);      /* Get architecture domain   */
  tgtnbr = archDomSize (&mappptr->archdat, &domfrst); /* Get architecture size */

  mapsum   = 0;
  mapnbr   = 0;
  vertload = 1;                                   /* Assume unweighted vertices */
  for (vertnum = 0; domtab[vertnum].labl != ARCHDOMNOTTERM; vertnum ++) {
    if (domtab[vertnum].labl != domtab[vertnum + 1].labl) /* TRICK: If new (or end) domain label */
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

    if (domtab[vertnum].labl != domtab[vertnum + 1].labl) { /* TRICK: If new (or end) domain label */
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
      memFree    (labltab);
      memFree    (ngbtab);
      memFree    (domtab);
      return     (1);
    }
#endif /* SCOTCH_DEBUG_MAP2 */
    tgtnbr = mapnbr;                              /* Assume it is a variable-sized architecture */
  }

  fprintf (stream, "M\tProcessors %ld/%ld (%g)\n",
           (long) mapnbr, (long) tgtnbr,
           (double) mapnbr / (double) tgtnbr);
  fprintf (stream, "M\tTarget min=%ld\tmax=%ld\tavg=%g\tdlt=%g\tmaxmoy=%g\n",
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
    if (domtab[vertnum].labl != domtab[vertnum + 1].labl) { /* TRICK: If new (or end) domain label  */
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
    for (edgenum = grafptr->verttax[vertnum]; edgenum < grafptr->vendtax[vertnum]; edgenum ++) {
      if (labltax[grafptr->edgetax[edgenum]] == ARCHDOMNOTTERM)
        continue;
      distval = archDomDist (&mappptr->archdat, mapDomain (mappptr, vertnum), mapDomain (mappptr, grafptr->edgetax[edgenum]));
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

  memFree (labltab);
  memFree (ngbtab);
  memFree (domtab);

  return (0);
}
