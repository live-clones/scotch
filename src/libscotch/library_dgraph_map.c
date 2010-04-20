/* Copyright 2008-2010 ENSEIRB, INRIA & CNRS
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
/**   NAME       : library_dgraph_map.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the distri-  **/
/**                buted graph mapping routines of the     **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 5.1  : from : 12 jun 2008     **/
/**                                 to     29 mar 2010     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "parser.h"
#include "dgraph.h"
#include "arch.h"
#include "dmapping.h"
#include "kdgraph.h"
#include "kdgraph_map_st.h"
#include "library_dmapping.h"
#include "scotch.h"

/************************************/
/*                                  */
/* These routines are the C API for */
/* the parallel mapping routines.   */
/*                                  */
/************************************/

/*+ This routine parses the given
*** mapping strategy.
*** It returns:
*** - 0   : if string successfully scanned.
*** - !0  : on error.
+*/

int
SCOTCH_stratDgraphMap (
SCOTCH_Strat * const        stratptr,
const char * const          string)
{
  if (*((Strat **) stratptr) != NULL)
    stratExit (*((Strat **) stratptr));

  if ((*((Strat **) stratptr) = stratInit (&kdgraphmapststratab, string)) == NULL) {
    errorPrint ("SCOTCH_stratDgraphMap: error in parallel mapping strategy");
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
SCOTCH_dgraphMapInit (
const SCOTCH_Dgraph * const grafptr,              /*+ Graph to map                    +*/
SCOTCH_Dmapping * const     mappptr,              /*+ Mapping structure to initialize +*/
const SCOTCH_Arch * const   archptr,              /*+ Target architecture used to map +*/
SCOTCH_Num * const          termloctab)           /*+ Mapping array                   +*/
{
  LibDmapping * restrict  srcmappptr;

#ifdef SCOTCH_DEBUG_LIBRARY1
  if (sizeof (SCOTCH_Dmapping) < sizeof (LibDmapping)) {
    errorPrint ("SCOTCH_dgraphMapInit: internal error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_LIBRARY1 */

  srcmappptr = (LibDmapping *) mappptr;
  srcmappptr->termloctab = ((termloctab == NULL) || ((void *) termloctab == (void *) grafptr)) ? NULL : termloctab;
  return (dmapInit (&srcmappptr->m, (Arch *) archptr));
}

/*+ This routine frees an API mapping.
*** It returns:
*** - VOID  : in all cases.
+*/

void
SCOTCH_dgraphMapExit (
const SCOTCH_Dgraph * const grafptr,
SCOTCH_Dmapping * const     mappptr)
{
  dmapExit (&((LibDmapping *) mappptr)->m);
}

/*+ This routine saves the contents of
*** the given mapping to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_dgraphMapSave (
const SCOTCH_Dgraph * const   grafptr,            /*+ Graph to map    +*/
const SCOTCH_Dmapping * const mappptr,            /*+ Mapping to save +*/
FILE * const                  stream)             /*+ Output stream   +*/
{
  return (dmapSave (&((LibDmapping *) mappptr)->m, (Dgraph *) grafptr, stream));
}

/*+ This routine computes a mapping
*** of the API mapping structure with
*** respect to the given strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_dgraphMapCompute (
const SCOTCH_Dgraph * const grafptr,              /*+ Graph to map       +*/
SCOTCH_Dmapping * const     mappptr,              /*+ Mapping to compute +*/
SCOTCH_Strat * const        stratptr)             /*+ Mapping strategy   +*/
{
  Kdgraph                 mapgrafdat;             /* Effective mapping graph     */
  Kdmapping               mapmappdat;             /* Initial mapping domain      */
  const Strat *           mapstratptr;            /* Pointer to mapping strategy */
  LibDmapping * restrict  srcmappptr;
  int                     o;

  if (*((Strat **) stratptr) == NULL)             /* Set default mapping strategy if necessary */
    *((Strat **) stratptr) = stratInit (&kdgraphmapststratab, "r{sep=m{asc=b{bnd=q{strat=f},org=q{strat=f}},low=q{strat=m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}|m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}},seq=q{strat=m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}|m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}}},seq=r{job=t,map=t,poli=S,sep=m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}|m{type=h,vert=80,low=h{pass=10}f{bal=0.0005,move=80},asc=b{bnd=d{dif=1,rem=1,pass=40}f{bal=0.005,move=80},org=f{bal=0.005,move=80}}}}}");
  mapstratptr = *((Strat **) stratptr);
  if (mapstratptr->tabl != &kdgraphmapststratab) {
    errorPrint ("SCOTCH_dgraphMapCompute: not a parallel graph mapping strategy");
    return     (1);
  }

  srcmappptr = (LibDmapping *) mappptr;
  if (kdgraphInit (&mapgrafdat, (Dgraph *) grafptr, &srcmappptr->m) != 0)
    return (1);
  mapmappdat.mappptr = &srcmappptr->m;

  if (((o = kdgraphMapSt (&mapgrafdat, &mapmappdat, mapstratptr)) == 0) && /* Perform mapping */
      (srcmappptr->termloctab != NULL))
    o = dmapTerm (&srcmappptr->m, &mapgrafdat.s, srcmappptr->termloctab); /* Use "&mapgrafdat.s" to take advantage of ghost arrays */
  kdgraphExit (&mapgrafdat);

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
SCOTCH_dgraphMap (
const SCOTCH_Dgraph * const grafptr,              /*+ Graph to map        +*/
const SCOTCH_Arch * const   archptr,              /*+ Target architecture +*/
SCOTCH_Strat * const        stratptr,             /*+ Mapping strategy    +*/
SCOTCH_Num * const          termloctab)           /*+ Mapping array       +*/
{
  SCOTCH_Dmapping     mappdat;
  int                 o;

  SCOTCH_dgraphMapInit (grafptr, &mappdat, archptr, termloctab);
  o = SCOTCH_dgraphMapCompute (grafptr, &mappdat, stratptr);
  SCOTCH_dgraphMapExit (grafptr, &mappdat);

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
SCOTCH_dgraphPart (
const SCOTCH_Dgraph * const grafptr,              /*+ Graph to map     +*/
const SCOTCH_Num            partnbr,              /*+ Number of parts  +*/
SCOTCH_Strat * const        stratptr,             /*+ Mapping strategy +*/
SCOTCH_Num * const          termloctab)           /*+ Mapping array    +*/
{
  SCOTCH_Arch         archdat;
  int                 o;

  SCOTCH_archInit  (&archdat);
  SCOTCH_archCmplt (&archdat, partnbr);
  o = SCOTCH_dgraphMap (grafptr, &archdat, stratptr, termloctab);
  SCOTCH_archExit  (&archdat);

  return (o);
}
