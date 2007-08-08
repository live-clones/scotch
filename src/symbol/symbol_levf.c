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
/**   NAME       : symbol_levf.c                           **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module computes some parameters of **/
/**                the incomplete elimination tree of      **/
/**                symbolic matrices.                      **/
/**                                                        **/
/**   DATES      : # Version 1.0  : from : 24 jun 2002     **/
/**                                 to     26 jun 2002     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_LEVF

#include "common.h"
#include "symbol.h"

/******************************************/
/*                                        */
/* The symbolic matrix handling routines. */
/*                                        */
/******************************************/

/*+ This routine computes some parameters
*** of the incomplete elimination tree of
*** the given incomplete symbolic block matrix.
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolLevf (
const SymbolMatrix * const  symbptr,              /*+ Symbolic matrix to evaluate                +*/
INT * const                 levfmax,              /*+ Maximum level of fill                      +*/
INT ** const                levfptr)              /*+ Array of number of blocks at level of fill +*/
{
  INT * restrict      levftab;                    /* Array of level of fill counts */
  INT                 levfmac;                    /* Current maximum level of fill */
  INT                 bloknum;                    /* Number of current block       */

  if (levfptr != NULL) {                          /* If level of fill distribution wanted */
    if ((levftab = (INT *) memAlloc (symbptr->cblknbr * sizeof (INT))) == NULL) {
      errorPrint ("symbolLevf: out of memory");
      return     (1);
    }

    memSet (levftab, 0, symbptr->cblknbr * sizeof (INT));

    levfmac = 0;
    for (bloknum = 0; bloknum < symbptr->bloknbr; bloknum ++) {
      INT                 levfval;

      levfval = symbptr->bloktab[bloknum].levfval;

#ifdef SYMBOL_DEBUG
      if (levfval >= symbptr->cblknbr) {
        errorPrint ("symbolLevf: internal error");
        memFree    (levftab);
        return     (1);
      }
#endif /* SYMBOL_DEBUG */

      levftab[levfval] ++;
      if (levfval > levfmac)
        levfmac = levfval;
    }

    *levfptr = (INT *) memRealloc (levftab, (levfmac + 1) * sizeof (INT));
  }
  else {                                          /* Only maximum level of fill wanted */
    levfmac = 0;
    for (bloknum = 0; bloknum < symbptr->bloknbr; bloknum ++) {
      INT                 levfval;

      levfval = symbptr->bloktab[bloknum].levfval;
      if (levfval > levfmac)
        levfmac = levfval;
    }
  }

  *levfmax = levfmac;

  return (0);
}
