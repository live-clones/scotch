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
/**   NAME       : library_graph_map.c                     **/
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
/**                                 to     13 nov 2005     **/
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
#include "kgraph_map_st.h"
#include "library_mapping.h"
#include "scotch.h"

/************************************/
/*                                  */
/* These routines are the C API for */
/* the mapping routines.            */
/*                                  */
/************************************/

/*+ This routine parses the given
*** mapping strategy.
*** It returns:
*** - 0   : if string successfully scanned.
*** - !0  : on error.
+*/

int
SCOTCH_stratGraphMap (
SCOTCH_Strat * const        stratptr,
const char * const          string)
{
  if (*((Strat **) stratptr) != NULL)
    stratExit (*((Strat **) stratptr));

  if ((*((Strat **) stratptr) = stratInit (&kgraphmapststratab, string)) == NULL) {
    errorPrint ("SCOTCH_stratGraphMap: error in mapping strategy");
    return     (1);
  }

  return (0);
}

/*+ This routine initializes an API opaque
*** mapping with respect to the given source
*** graph and the locations of output parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphMapInit (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to map                    +*/
SCOTCH_Mapping * const      mappptr,              /*+ Mapping structure to initialize +*/
const SCOTCH_Arch * const   archptr,              /*+ Target architecture used to map +*/
SCOTCH_Num * const          parttab)              /*+ Mapping array                   +*/
{
  LibMapping * restrict srcmappptr;

#ifdef SCOTCH_DEBUG_LIBRARY1
  if (sizeof (SCOTCH_Mapping) < sizeof (LibMapping)) {
    errorPrint ("SCOTCH_graphMapInit: internal error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_LIBRARY1 */

  srcmappptr = (LibMapping *) mappptr;
  return (mapInit (&srcmappptr->m, ((Graph *) grafptr)->baseval, ((Graph *) grafptr)->vertnbr, (Arch *) archptr, (Gnum *) parttab));
  srcmappptr->parttab = parttab;
}

/*+ This routine frees an API mapping.
*** It returns:
*** - VOID  : in all cases.
+*/

void
SCOTCH_graphMapExit (
const SCOTCH_Graph * const  grafptr,
SCOTCH_Mapping * const      mappptr)
{
  mapExit (&((LibMapping *) mappptr)->m);
}

/*+ This routine saves the contents of
*** the given mapping to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphMapLoad (
const SCOTCH_Graph * const    grafptr,            /*+ Graph to order  +*/
const SCOTCH_Mapping * const  mappptr,            /*+ Mapping to save +*/
FILE * const                  stream)             /*+ Output stream   +*/
{
  return (mapLoad (&((LibMapping *) mappptr)->m, ((Graph *) grafptr)->vlbltax, stream));
}

/*+ This routine saves the contents of
*** the given mapping to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphMapSave (
const SCOTCH_Graph * const    grafptr,            /*+ Graph to order  +*/
const SCOTCH_Mapping * const  mappptr,            /*+ Mapping to save +*/
FILE * const                  stream)             /*+ Output stream   +*/
{
  return (mapSave (&((LibMapping *) mappptr)->m, ((Graph *) grafptr)->vlbltax, stream));
}

/*+ This routine computes a mapping
*** of the API mapping structure with
*** respect to the given strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphMapCompute (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to order     +*/
SCOTCH_Mapping * const      mappptr,              /*+ Mapping to compute +*/
const SCOTCH_Strat * const  stratptr)             /*+ Mapping strategy   +*/
{
  Kgraph              mapgrafdat;                 /* Effective mapping graph     */
  const Strat *       mapstratptr;                /* Pointer to mapping strategy */
  int                 o;

  if (*((Strat **) stratptr) == NULL)             /* Set default mapping strategy if necessary */
    *((Strat **) stratptr) = stratInit (&kgraphmapststratab, "b{job=t,map=t,poli=S,strat=m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=f{bal=0.005,move=80}}|m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=f{bal=0.005,move=80}}}");
  mapstratptr = *((Strat **) stratptr);
  if (mapstratptr->tabl != &kgraphmapststratab) {
    errorPrint ("SCOTCH_graphMapCompute: not a graph mapping strategy");
    return     (1);
  }

  if (kgraphInit (&mapgrafdat, (Graph *) grafptr, &((LibMapping *) mappptr)->m) != 0)
    return (1);
  o = kgraphMapSt (&mapgrafdat, mapstratptr);     /* Perform mapping */
  kgraphExit (&mapgrafdat, &((LibMapping *) mappptr)->m);

  return (o);
}

/*+ This routine computes a mapping of the
*** given graph structure onto the given
*** target architecture with respect to the
*** given strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphMap (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to map        +*/
const SCOTCH_Arch * const   archptr,              /*+ Target architecture +*/
const SCOTCH_Strat * const  stratptr,             /*+ Mapping strategy    +*/
SCOTCH_Num * const          maptab)               /*+ Mapping array       +*/
{
  SCOTCH_Mapping      mapdat;
  int                 o;

  SCOTCH_graphMapInit (grafptr, &mapdat, archptr, maptab);
  o = SCOTCH_graphMapCompute (grafptr, &mapdat, stratptr);
  SCOTCH_graphMapExit (grafptr, &mapdat);

  return (o);
}

/*+ This routine computes a partition of
*** the given graph structure with respect
*** to the given strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphPart (
const SCOTCH_Graph * const  grafptr,              /*+ Graph to map     +*/
const SCOTCH_Num            partnbr,              /*+ Number of parts  +*/
const SCOTCH_Strat * const  stratptr,             /*+ Mapping strategy +*/
SCOTCH_Num * const          maptab)               /*+ Mapping array    +*/
{
  SCOTCH_Arch         archdat;
  int                 o;

  SCOTCH_archInit  (&archdat);
  SCOTCH_archCmplt (&archdat, partnbr);
  o = SCOTCH_graphMap (grafptr, &archdat, stratptr, maptab);
  SCOTCH_archExit  (&archdat);

  return (o);
}
