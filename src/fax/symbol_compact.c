/* Copyright 2004,2007 INRIA
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
/**   NAME       : symbol_compact.c                        **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This routine compacts all blocks which  **/
/**                belong to the same facing column block, **/
/**                irrespective of their level of fill.    **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 17 oct 2003     **/
/**                                 to     17 oct 2003     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "common.h"
#include "symbol.h"
#include "order.h"
#include "fax.h"

/********************/
/*                  */
/* Symbolic matrix  */
/* merging routine. */
/*                  */
/********************/

/*+ This routine merges the given
*** symbol matrix.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolCompact (
SymbolMatrix * const        symbptr)
{
  SymbolBlok * restrict     bloktax;
  INT                       cblknum;
  INT                       bloknew;

  bloktax = symbptr->bloktab - symbptr->baseval;

  for (cblknum = 0, bloknew = symbptr->baseval;
       cblknum < symbptr->cblknbr; cblknum ++) {
    INT                       bloknum;
    INT                       bloklst;

    bloknum = symbptr->cblktab[cblknum].bloknum;  /* Update block index in column block array */
    symbptr->cblktab[cblknum].bloknum = bloknew;

    bloktax[bloknew] = bloktax[bloknum];          /* Copy diagonal block to its new position */
    bloklst = bloknew;                            /* Set it as last block                    */

    for (bloknum ++, bloknew ++;
         bloknum < symbptr->cblktab[cblknum + 1].bloknum; bloknum ++) {
      if ((bloktax[bloknum].cblknum == bloktax[bloklst].cblknum) &&
          (bloktax[bloknum].frownum == (bloktax[bloklst].lrownum + 1))) {
        bloktax[bloklst].lrownum = bloktax[bloknum].lrownum;
        if (bloktax[bloknum].levfval < bloktax[bloklst].levfval)
          bloktax[bloklst].levfval = bloktax[bloknum].levfval;
      }
      else {
        bloktax[bloknew] = bloktax[bloknum];      /* Copy block to its new position  */
        bloklst = bloknew ++;                     /* One more block in symbol matrix */
      }
    }
  }
  symbptr->cblktab[cblknum].bloknum = bloknew;    /* Set end of column block array */

  symbptr->bloknbr = bloknew - symbptr->baseval;  /* Update number of blocks */

#ifdef FAX_DEBUG
  if (symbolCheck (symbptr) != 0) {               /* Should not happen */
    errorPrint ("symbolCompact: internal error");
    return (1);
  }
#endif /* FAX_DEBUG */

  return (0);
}
