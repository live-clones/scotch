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
/**   NAME       : symbol_level.c                          **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module contains the incomplete     **/
/**                symbol matrix pruning routine.          **/
/**                                                        **/
/**   DATES      : # Version 1.0  : from : 03 jun 2002     **/
/**                                 to     03 jun 2002     **/
/**                # Version 1.1  : from : 26 jun 2002     **/
/**                                 to     03 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_LEVEL

#include "common.h"
#include "symbol.h"
#include "order.h"
#include "fax.h"

/*************************/
/*                       */
/* Level making routine. */
/*                       */
/*************************/

/*+ This routine computes an incomplete
*** block symbolic factorization structure
*** of given level by removing all blocks
*** the fill level of which is strictly
*** greater than the given cut-off value.
*** In order for the elimination tree to
*** have the same tree structure as the
*** complete factored matrix, first
*** extra-diagonal blocks of every column
*** are never removed.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolLevel (
SymbolMatrix * const        dstsymbptr,           /*+ New symbolic block matrix [based] +*/
const SymbolMatrix * const  srcsymbptr,           /*+ Old symbolic block matrix [based] +*/
const INT                   levfval)              /*+ Cut-off level of fill             +*/
{
  INT                         baseval;            /* Base value                             */
  INT                         cblknum;            /* Based number of current column block   */
  const SymbolCblk * restrict srccblktax;         /* Based access to old column block array */
  SymbolCblk * restrict       dstcblktax;         /* Based access to new column block array */
  const SymbolBlok * restrict srcbloktax;         /* Based access to old block array        */
  SymbolBlok * restrict       dstbloktax;         /* Based access to new block array        */
  INT                         srcbloknum;         /* Based number of current old block      */
  INT                         dstbloknum;         /* Based number of current new block      */

  if (((dstsymbptr->cblktab = (SymbolCblk *) memAlloc ((srcsymbptr->cblknbr + 1) * sizeof (SymbolCblk))) == NULL) ||
      ((dstsymbptr->bloktab = (SymbolBlok *) memAlloc ( srcsymbptr->bloknbr      * sizeof (SymbolBlok))) == NULL)) {
    errorPrint ("symbolLevel: out of memory");
    if (dstsymbptr->cblktab != NULL) {
      memFree (dstsymbptr->cblktab);
      dstsymbptr->cblktab = NULL;
    }
    return (1);
  }
  baseval    = srcsymbptr->baseval;
  srccblktax = srcsymbptr->cblktab - baseval;     /* Set based accesses */
  dstcblktax = dstsymbptr->cblktab - baseval;
  srcbloktax = srcsymbptr->bloktab - baseval;
  dstbloktax = dstsymbptr->bloktab - baseval;

  for (cblknum = srcbloknum = dstbloknum = baseval; /* For all column blocks */
       cblknum < srcsymbptr->cblknbr + baseval; cblknum ++) {
    dstcblktax[cblknum].fcolnum = srccblktax[cblknum].fcolnum; /* Build new column block data */
    dstcblktax[cblknum].lcolnum = srccblktax[cblknum].lcolnum;
    dstcblktax[cblknum].bloknum = dstbloknum;

    dstbloktax[dstbloknum].frownum = dstcblktax[cblknum].fcolnum; /* Build diagonal block */
    dstbloktax[dstbloknum].lrownum = dstcblktax[cblknum].lcolnum;
    dstbloktax[dstbloknum].cblknum = cblknum;
    dstbloktax[dstbloknum].levfval = 0;

    if (srcbloknum >= srccblktax[cblknum + 1].bloknum) /* If no extra-diagonal block */
      continue;                                   /* Proceed with next column block  */

    dstbloktax[dstbloknum].frownum = srcbloktax[srcbloknum].frownum; /* Copy data of first column block as is */
    dstbloktax[dstbloknum].lrownum = srcbloktax[srcbloknum].lrownum;
    dstbloktax[dstbloknum].cblknum = cblknum;
    dstbloktax[dstbloknum].levfval = srcbloktax[srcbloknum].levfval;
    dstbloknum ++;
    srcbloknum ++;

    for ( ; srcbloknum < srccblktax[cblknum + 1].bloknum; srcbloknum ++) {
      if (srcbloktax[srcbloknum].levfval <= levfval) { /* If block should be kept */
        dstbloktax[dstbloknum].frownum = srcbloktax[srcbloknum].frownum;
        dstbloktax[dstbloknum].lrownum = srcbloktax[srcbloknum].lrownum;
        dstbloktax[dstbloknum].cblknum = cblknum;
        dstbloktax[dstbloknum].levfval = srcbloktax[srcbloknum].levfval;
        dstbloknum ++;
      }
    }
  }
  dstcblktax[cblknum].fcolnum = srccblktax[cblknum].fcolnum; /* Build terminating column block */
  dstcblktax[cblknum].lcolnum = srccblktax[cblknum].lcolnum;
  dstcblktax[cblknum].bloknum = dstbloknum;

  dstsymbptr->baseval = baseval;                  /* Fill in matrix fields */
  dstsymbptr->cblknbr = srcsymbptr->cblknbr;
  dstsymbptr->bloknbr = dstbloknum - baseval;
  dstsymbptr->nodenbr = srcsymbptr->nodenbr;

#ifdef SYMBOL_DEBUG
  if (symbolCheck (dstsymbptr) != 0) {
    errorPrint ("symbolLevel: internal error");
    symbolExit (dstsymbptr);
    return     (1);
  }
#endif /* SYMBOL_DEBUG */

  return (0);
}
