/* Copyright 2012,2018,2019,2023 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : library_dgraph_grow.c                   **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the distri-  **/
/**                buted graph growing routine of          **/
/**                the libScotch library.                  **/
/**                                                        **/
/**   DATES      : # Version 6.0  : from : 26 sep 2012     **/
/**                                 to   : 21 may 2018     **/
/**                # Version 7.0  : from : 27 aug 2019     **/
/**                                 to   : 21 jan 2023     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "module.h"
#include "common.h"
#include "context.h"
#include "dgraph.h"
#include "dgraph_halo.h"
#include "ptscotch.h"

/**********************************/
/*                                */
/* Distance computation routines. */
/*                                */
/**********************************/

#define DGRAPHBANDGROWNAME          dgraphGrow2
#define DGRAPHBANDGROWEDGE(n)                     /* No need to count edges         */
#define DGRAPHBANDGROWENQ1                        /* Color array already set        */
#define DGRAPHBANDGROWENQ2          vnumgsttax[vertlocnum] /* Set vertex color      */
#define DGRAPHBANDGROWENQ3          vsnddattab[nsndidxnum ++] = vnumgsttax[vertlocnum] /* Send color value */
#define DGRAPHBANDGROWENQ4          vrcvdatptr[vertrcvnum + 1] /* Get and set color */
#define DGRAPHBANDGROWSMUL(n)       ((n) * 2)     /* Add space for color value      */
#include "dgraph_band_grow.h"
#include "dgraph_band_grow.c"
#undef DGRAPHBANDGROWNAME
#undef DGRAPHBANDGROWEDGE
#undef DGRAPHBANDGROWENQ1
#undef DGRAPHBANDGROWENQ2
#undef DGRAPHBANDGROWENQ3
#undef DGRAPHBANDGROWENQ4
#undef DGRAPHBANDGROWSMUL

/*********************************/
/*                               */
/* This routine is the C API for */
/* the graph growing routine.    */
/*                               */
/*********************************/

/*+ This routine grows areas from a given
*** set of seeds of some color. Several
*** seeds can have the same color.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_dgraphGrow (
SCOTCH_Dgraph * const       orggrafptr,
const SCOTCH_Num            seedlocnbr,
SCOTCH_Num * const          seedloctab,
const SCOTCH_Num            distmax,
SCOTCH_Num * const          partgsttab)
{
  Gnum *              bandpartgsttax;
  Gnum                bandvertlocnbr;             /* Not used */
  Gnum                bandvertlvlnum;             /* Not used */
  Gnum                bandedgelocsiz;             /* Not used */
  CONTEXTDECL        (orggrafptr);
  Dgraph              grafdat;
  int                 o;

  o = 1;                                          /* Assume an error */

  if (CONTEXTINIT (orggrafptr)) {
    errorPrint (STRINGIFY (SCOTCH_dgraphBand) ": cannot initialize context");
    goto abort;
  }

  grafdat = *((Dgraph *) CONTEXTGETOBJECT (orggrafptr)); /* Clone original graph */
  grafdat.flagval &= ~DGRAPHFREEALL;              /* Never free existing fields  */

  if (dgraphGhst (&grafdat) != 0) {               /* Compute ghost edge array if not already present */
    errorPrint (STRINGIFY (SCOTCH_dgraphGrow) ": cannot compute ghost edge array");
    return (1);
  }

  bandpartgsttax = (partgsttab != NULL) ? (Gnum *) partgsttab - grafdat.baseval : NULL;

  o = (((grafdat.flagval & DGRAPHCOMMPTOP) != 0) ? dgraphGrow2Ptop : dgraphGrow2Coll)
        (&grafdat, seedlocnbr, seedloctab, distmax, bandpartgsttax, &bandvertlvlnum, &bandvertlocnbr, &bandedgelocsiz, CONTEXTGETDATA (orggrafptr));

  dgraphExit (&grafdat);                          /* Free ghost edge arrays if any */
abort:
  CONTEXTEXIT (orggrafptr);
  return (o);
}
