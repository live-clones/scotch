/* Copyright 2007-2010,2012,2014,2018,2019,2021,2023-2026 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : library_dgraph_order.c                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the distri-  **/
/**                buted graph ordering routines of the    **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 25 apr 2006     **/
/**                                 to   : 11 nov 2008     **/
/**                # Version 5.1  : from : 29 mar 2010     **/
/**                                 to   : 14 aug 2010     **/
/**                # Version 6.0  : from : 08 jan 2012     **/
/**                                 to   : 25 apr 2018     **/
/**                # Version 6.1  : from : 24 sep 2021     **/
/**                                 to   : 25 sep 2021     **/
/**                # Version 7.0  : from : 27 aug 2019     **/
/**                                 to   : 08 feb 2026     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "module.h"
#include "common.h"
#include "context.h"
#include "parser.h"
#include "dgraph.h"
#include "order.h"
#include "dorder.h"
#include "hdgraph.h"
#include "hdgraph_order_si.h"
#include "hdgraph_order_st.h"
#include "ptscotch.h"

/************************************/
/*                                  */
/* These routines are the C API for */
/* the distributed graph ordering   */
/* routines.                        */
/*                                  */
/************************************/

/*+ This routine initializes an API ordering
*** with respect to the given source graph
*** and the locations of output parameters.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_dgraphOrderInit (
const SCOTCH_Dgraph * const libgrafptr,           /*+ Distributed graph to order         +*/
SCOTCH_Dordering * const    libordeptr)           /*+ Ordering structure to initialize   +*/
{
  Dgraph *            srcgrafptr;
  Dorder *            srcordeptr;

  if (sizeof (SCOTCH_Dordering) < sizeof (Dorder)) {
    errorPrint (STRINGIFY (SCOTCH_graphDorderInit) ": internal error");
    return (1);
  }

  srcgrafptr = (Dgraph *) CONTEXTOBJECT (libgrafptr); /* Use structure as source graph */
  srcordeptr = (Dorder *) libordeptr;
  return (dorderInit (srcordeptr, srcgrafptr->baseval, srcgrafptr->vertglbnbr, srcgrafptr->proccomm));
}

/*+ This routine frees an API ordering.
*** It returns:
*** - VOID  : in all cases.
+*/

void
SCOTCH_dgraphOrderExit (
const SCOTCH_Dgraph * const libgrafptr,
SCOTCH_Dordering * const    libordeptr)
{
  dorderExit ((Dorder *) libordeptr);
}

/*+ This routine saves the contents of
*** the given ordering to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_dgraphOrderSave (
const SCOTCH_Dgraph * const     libgrafptr,       /*+ Graph to order   +*/
const SCOTCH_Dordering * const  libordeptr,       /*+ Ordering to save +*/
FILE * const                    stream)           /*+ Output stream    +*/
{
  return (dorderSave ((Dorder *) libordeptr, (Dgraph *) CONTEXTOBJECT (libgrafptr), stream));
}

/*+ This routine computes an ordering
*** of the API ordering structure with
*** respect to the given strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_dgraphOrderCompute (
SCOTCH_Dgraph * const       grafptr,              /*+ Graph to order      +*/
SCOTCH_Dordering * const    ordeptr,              /*+ Ordering to compute +*/
SCOTCH_Strat * const        straptr)              /*+ Ordering strategy   +*/
{
  return (SCOTCH_dgraphOrderComputeList (grafptr, ordeptr, ((Dgraph *) CONTEXTOBJECT (grafptr))->vertlocnbr, NULL, straptr));
}

/*+ This routine computes a partial ordering
*** of the listed vertices of the API ordering
*** structure graph with respect to the given
*** strategy.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_dgraphOrderComputeList (
SCOTCH_Dgraph * const       libgrafptr,           /*+ Graph to order                   +*/
SCOTCH_Dordering * const    libordeptr,           /*+ Ordering to compute              +*/
const SCOTCH_Num            listlocnbr,           /*+ Number of vertices in local list +*/
const SCOTCH_Num * const    listloctab,           /*+ List of vertex indices to order  +*/
SCOTCH_Strat * const        straptr)              /*+ Ordering strategy                +*/
{
  Gnum                listglbnbr;                 /* Global number of vertices in list */
  Dorder *            srcordeptr;                 /* Pointer to ordering               */
  Dgraph * restrict   srcgrafptr;                 /* Pointer to scotch graph           */
  const Strat *       ordstraptr;                 /* Pointer to ordering strategy      */
  Hdgraph             halgrafdat;                 /* Halo source graph structure       */
  Hdgraph *           halgrafptr;
  Hdgraph             halgraftmp;
  DorderCblk *        cbl0ptr;                    /* Root column block                 */
  DorderCblk *        cbl1ptr;                    /* Column block induced from list    */
  DorderCblk *        cbl2ptr;                    /* Complement of cbl1                */
  DorderCblk *        cblkptr;                    /* Column block to be ordered        */
  CONTEXTDECL        (libgrafptr);
  int                 o;

  halgrafptr = NULL;
  cbl0ptr    = NULL;
  cbl1ptr    = NULL;
  cbl2ptr    = NULL;
  cblkptr    = NULL;

  o = 1;                                          /* Assume an error */

  if (CONTEXTINIT (libgrafptr)) {
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": cannot initialize context");
    goto abort;
  }

  srcgrafptr = (Dgraph *) CONTEXTGETOBJECT (libgrafptr);

#ifdef SCOTCH_DEBUG_DGRAPH1
  if ((listlocnbr < 0) || (listlocnbr > srcgrafptr->vertlocnbr)) {
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": invalid parameters (1)");
    goto abort;
  }
#endif /* SCOTCH_DEBUG_DGRAPH1 */
#ifdef SCOTCH_DEBUG_DGRAPH2
  if (dgraphCheck (srcgrafptr) != 0) {
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": invalid input graph");
    goto abort;
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  if (*((Strat **) straptr) == NULL) {            /* Set default ordering strategy if necessary */
    if (SCOTCH_stratDgraphOrderBuild (straptr, SCOTCH_STRATQUALITY, srcgrafptr->procglbnbr, 0, 0.2))
      goto abort;
  }

  ordstraptr = *((Strat **) straptr);
  if (ordstraptr->tablptr != &hdgraphorderststratab) {
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": not a distributed ordering strategy");
    goto abort;
  }

  srcordeptr = (Dorder *) libordeptr;             /* Get ordering */

  dorderFree (srcordeptr);                        /* Clean all existing ordering data */
  if ((cbl0ptr = dorderFrst (srcordeptr)) == NULL) {
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": cannot create root column block");
    goto abort;
  }

  if (dgraphGhst (srcgrafptr) != 0) {             /* Compute ghost edge array if not already present */
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": cannot compute ghost edge array");
    goto abort;
  }

  halgrafdat.s            = *srcgrafptr;          /* Copy non-halo graph data       */
  halgrafdat.s.flagval   &= ~DGRAPHFREEALL;       /* Do not free anything from it   */
  halgrafdat.s.edloloctax = NULL;                 /* Never mind about edge loads    */
  halgrafdat.s.vlblloctax = NULL;                 /* Do not propagate vertex labels */
  halgrafdat.vhallocnbr   = 0;                    /* No halo on graph               */
  halgrafdat.vhndloctax   = halgrafdat.s.vendloctax;
  halgrafdat.ehallocnbr   = 0;
  halgrafdat.levlnum      = 0;
  halgrafdat.contptr      = CONTEXTGETDATA (libgrafptr);

  halgrafptr = &halgrafdat;                       /* Assume we work on the whole graph */

  if (MPI_Allreduce (&listlocnbr, &listglbnbr, 1, GNUM_MPI, MPI_SUM, srcgrafptr->proccomm) != MPI_SUCCESS) {
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": communication error (1)");
    goto abort;
  }
  if (listglbnbr == 0) {                          /* If empty list, return identity permutation */
    o = hdgraphOrderSi (halgrafptr, cbl0ptr);
    goto abort;
  }

  if (listglbnbr == srcgrafptr->vertglbnbr)       /* If all vertices in list, order whole graph */
    cblkptr = cbl0ptr;
  else {                                          /* If not, order the subset of vertices */
    if (hdgraphInduceList (&halgrafdat, listlocnbr, listloctab, &halgraftmp) != 0) {
      errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": cannot create induced subgraph");
      goto abort;
    }
    halgrafptr = &halgraftmp;                     /* Ordering will be computed on induced graph */

    cbl0ptr->typeval = DORDERCBLKDICO;            /* Node is a set of two disconnected components */

    if ((cbl1ptr = dorderNew (cbl0ptr, srcordeptr->proccomm)) == NULL) {
      errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": cannot create induced column block (1)");
      goto abort;
    }
    cbl1ptr->ordeglbval = 0;                      /* Un-based inverse permutation index */
    cbl1ptr->vnodglbnbr = halgrafptr->s.vertglbnbr;
    cbl1ptr->cblkfthnum = 0;

    cblkptr = cbl1ptr;                            /* Order induced graph */
  }

  if (hdgraphOrderSt (halgrafptr, cblkptr, ordstraptr) != 0) {
    errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": cannot compute ordering");
    goto abort;
  }

  if (listglbnbr < srcgrafptr->vertglbnbr) {      /* Order vertices excluded from list, if any    */
    Gnum                vnumlocnbr;               /* Local number of vertices excluded from list  */
    Gnum                procvrtval;               /* Vertex offset of the process                 */
    Gnum                vertlocadj;               /* Adjustment from local to global vertex index */
    Gnum *              vnumloctab;
    Gnum                vertlocnum;
    Gnum                vnumlocnum;
    Gnum                listlocnum;

    vertlocadj = halgrafdat.s.procvrttab[halgrafdat.s.proclocnum];
    vnumlocnbr = halgrafdat.s.vertlocnbr - listlocnbr;

    if ((vnumloctab = memAlloc (halgrafdat.s.vertlocnbr * sizeof (Gnum))) == NULL) {
      errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": out of memory");
      goto abort;
    }

    memSet (vnumloctab, 0, halgrafdat.s.vertlocnbr * sizeof (Gnum));
    for (listlocnum = 0; listlocnum < listlocnbr; listlocnum ++) /* Flag vertices in list */
      vnumloctab[listloctab[listlocnum] - halgrafdat.s.baseval] = ~0;

    for (vertlocnum = 0, vnumlocnum = 0; vertlocnum < halgrafdat.s.vertlocnbr; vertlocnum ++) {
      if (vnumloctab[vertlocnum] != ~0)           /* Vertex was excluded from list */
        vnumloctab[vnumlocnum ++] = vertlocnum + vertlocadj;
    }
#ifdef SCOTCH_DEBUG_DGRAPH1
    if (vnumlocnum != vnumlocnbr) {
      errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": internal error");
      memFree    (vnumloctab);
      goto abort;
    }
#endif /* SCOTCH_DEBUG_DGRAPH1 */

    if ((cbl2ptr = dorderNew (cbl0ptr, srcordeptr->proccomm)) == NULL) { /* TRICK: create column block after first ordering */
      errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": cannot create induced column block (2)");
      memFree    (vnumloctab);
      goto abort;
    }
    cbl2ptr->ordeglbval = halgrafptr->s.vertglbnbr;
    cbl2ptr->vnodglbnbr = srcgrafptr->vertglbnbr - halgrafptr->s.vertglbnbr;
    cbl2ptr->cblkfthnum = 1;

    if (MPI_Scan (&vnumlocnbr, &procvrtval,       /* Compute vertex offset by partial sum reduction */
                  1, GNUM_MPI, MPI_SUM, srcgrafptr->proccomm) != MPI_SUCCESS) {
      errorPrint (STRINGIFY (SCOTCH_dgraphOrderComputeList) ": communication error (2)");
      memFree    (vnumloctab);
      goto abort;
    }
    procvrtval -= vnumlocnbr - halgrafdat.s.baseval;

    o = hdgraphOrderSi2 (cbl2ptr,                 /* Simple ordering of vertices not in list */
                         halgrafdat.s.baseval, vnumlocnbr, vnumloctab - halgrafdat.s.baseval, procvrtval, halgrafdat.s.proccomm);

    memFree (vnumloctab);
  }
  else
    o = 0;

abort:
  if (halgrafptr != NULL) {                       /* If at least one halo graph structure has been created */
    hdgraphExit (&halgrafdat);                    /* Free the contents of the full halo graph structure    */
    if (halgrafptr == &halgraftmp)                /* If the induced subgraph structure has been created    */
      hdgraphExit (&halgraftmp);                  /* Free it as well                                       */
  }

  CONTEXTEXIT (libgrafptr);

  return (o);
}

/*+ This routine parses the given
*** distributed graph ordering strategy.
*** It returns:
*** - 0   : if string successfully scanned.
*** - !0  : on error.
+*/

int
SCOTCH_stratDgraphOrder (
SCOTCH_Strat * const        straptr,
const char * const          string)
{
  if (*((Strat **) straptr) != NULL)
    stratExit (*((Strat **) straptr));

  if ((*((Strat **) straptr) = stratInit (&hdgraphorderststratab, string)) == NULL) {
    errorPrint (STRINGIFY (SCOTCH_stratDgraphOrder) ": error in ordering strategy");
    return (1);
  }

  return (0);
}

/*+ This routine provides predefined
*** ordering strategies.
*** It returns:
*** - 0   : if string successfully initialized.
*** - !0  : on error.
+*/

int
SCOTCH_stratDgraphOrderBuild (
SCOTCH_Strat * const        straptr,              /*+ Strategy to create                 +*/
const SCOTCH_Num            flagval,              /*+ Desired characteristics            +*/
const SCOTCH_Num            procnbr,              /*+ Number of processes for running    +*/
const SCOTCH_Num            levlnbr,              /*+ Number of nested dissection levels +*/
const double                balrat)               /*+ Desired imbalance ratio            +*/
{
  char                bufftab[8192];              /* Should be enough */
  char                bbaltab[32];
  char                levltab[32];
  char                verttab[32];
  Gnum                vertnbr;
  char *              tstpptr;
  char *              tstsptr;
  char *              oleaptr;
  char *              osepptr;

  vertnbr = MAX (2000 * procnbr, 10000);
  vertnbr = MIN (vertnbr, 1000000);

  sprintf (bbaltab, "%lf", balrat);
  sprintf (levltab, GNUMSTRING, levlnbr);
  sprintf (verttab, GNUMSTRING, vertnbr);

  strcpy (bufftab, "n{sep=/(<TSTP>)?m{vert=<VERT>,asc=b{width=3,strat=q{strat=f}},low=q{strat=h},seq=q{strat=m{vert=120,low=h{pass=10},asc=b{width=3,bnd=f{bal=<BBAL>},org=h{pass=10}f{bal=<BBAL>}}}}};,ole=q{strat=n{sep=/(<TSTS>)?m{vert=120,low=h{pass=10},asc=b{width=3,bnd=f{bal=<BBAL>},org=h{pass=10}f{bal=<BBAL>}}};,ole=<OLEA>,ose=<OSEP>}},ose=s,osq=n{sep=/(<TSTS>)?m{vert=120,low=h{pass=10},asc=b{width=3,bnd=f{bal=<BBAL>},org=h{pass=10}f{bal=<BBAL>}}};,ole=<OLEA>,ose=<OSEP>}}");

  switch (flagval & (SCOTCH_STRATLEVELMIN | SCOTCH_STRATLEVELMAX)) {
    case SCOTCH_STRATLEVELMIN :
      tstpptr = "0=0";
      tstsptr = "(levl<<LEVL>)|(vert>240)";
      break;
    case SCOTCH_STRATLEVELMAX :
      tstpptr = "(levl<<LEVL>)";
      tstsptr = "(levl<<LEVL>)&(vert>240)";
      break;
    case (SCOTCH_STRATLEVELMIN | SCOTCH_STRATLEVELMAX) :
      tstpptr =
      tstsptr = "levl<<LEVL>";
      oleaptr = "s";                              /* Simple ordering for leaves */
      break;
    default :
      tstpptr = "0=0";
      tstsptr = "vert>240";
      break;
  }

  oleaptr = ((flagval & SCOTCH_STRATLEAFSIMPLE) != 0)
            ? "s"
            : "f{cmin=15,cmax=100000,frat=0.0}";

  osepptr = ((flagval & SCOTCH_STRATSEPASIMPLE) != 0)
            ? "s"
            : "g";

  stringSubst (bufftab, "<TSTP>", tstpptr);
  stringSubst (bufftab, "<TSTS>", tstsptr);
  stringSubst (bufftab, "<LEVL>", levltab);
  stringSubst (bufftab, "<OLEA>", oleaptr);
  stringSubst (bufftab, "<OSEP>", osepptr);
  stringSubst (bufftab, "<BBAL>", bbaltab);
  stringSubst (bufftab, "<VERT>", verttab);

  return (SCOTCH_stratDgraphOrder (straptr, bufftab));
}
