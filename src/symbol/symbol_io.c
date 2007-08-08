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
/**   NAME       : symbol_io.c                             **/
/**                                                        **/
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the input/output        **/
/**                routines for symbolic matrices.         **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 23 aug 1998     **/
/**                                 to     07 oct 1998     **/
/**                # Version 0.1  : from : 21 mar 2002     **/
/**                                 to     21 mar 2002     **/
/**                # Version 1.0  : from : 03 jun 2002     **/
/**                                 to     08 sep 2003     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_IO

#include "common.h"
#include "symbol.h"

/******************************************/
/*                                        */
/* The symbolic matrix handling routines. */
/*                                        */
/******************************************/

/*+ This routine loads the given
*** block matrix structure
*** from the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolLoad (
SymbolMatrix * const        symbptr,
FILE * const                stream)
{
  INT                 versval;
  INT                 baseval;
  INT                 nodenbr;
  INT                 cblknbr;
  INT                 cblknum;
  INT                 bloknbr;
  INT                 bloknum;

  if ((intLoad (stream, &versval) +               /* Read header */
       intLoad (stream, &cblknbr) +
       intLoad (stream, &bloknbr) +
       intLoad (stream, &nodenbr) +
       intLoad (stream, &baseval) != 5) ||
      (versval < 0)                     ||        /* Version should be 0 or 1 */
      (versval > 1)                     ||
      (bloknbr < cblknbr)               ||
      (nodenbr < cblknbr)) {
    errorPrint ("symbolLoad: bad input (1)");
    return     (1);
  }

  if (((symbptr->cblktab = (SymbolCblk *) memAlloc ((cblknbr + 1) * sizeof (SymbolCblk))) == NULL) ||
      ((symbptr->bloktab = (SymbolBlok *) memAlloc ( bloknbr      * sizeof (SymbolBlok))) == NULL)) {
    errorPrint ("symbolLoad: out of memory");
    symbolExit (symbptr);
    symbolInit (symbptr);
    return     (1);
  }
  symbptr->baseval = baseval;
  symbptr->cblknbr = cblknbr;
  symbptr->bloknbr = bloknbr;
  symbptr->nodenbr = nodenbr;

  for (cblknum = 0; cblknum < cblknbr; cblknum ++) {
    if ((intLoad (stream, &symbptr->cblktab[cblknum].fcolnum) + /* Read column blocks */
         intLoad (stream, &symbptr->cblktab[cblknum].lcolnum) +
         intLoad (stream, &symbptr->cblktab[cblknum].bloknum) != 3) ||
        (symbptr->cblktab[cblknum].fcolnum > symbptr->cblktab[cblknum].lcolnum)) {
      errorPrint ("symbolLoad: bad input (2)");
      symbolExit (symbptr);
      symbolInit (symbptr);
      return     (1);
    }
  }
  symbptr->cblktab[cblknbr].fcolnum =             /* Set last column block */
  symbptr->cblktab[cblknbr].lcolnum = nodenbr + baseval;
  symbptr->cblktab[cblknbr].bloknum = bloknbr + baseval;

  for (bloknum = 0; bloknum < bloknbr; bloknum ++) {
    if ((intLoad (stream, &symbptr->bloktab[bloknum].frownum) + /* Read column blocks */
         intLoad (stream, &symbptr->bloktab[bloknum].lrownum) +
         intLoad (stream, &symbptr->bloktab[bloknum].cblknum) != 3) ||
        (symbptr->bloktab[bloknum].frownum > symbptr->bloktab[bloknum].lrownum)) {
      errorPrint ("symbolLoad: bad input (3)");
      symbolExit (symbptr);
      symbolInit (symbptr);
      return     (1);
    }

    symbptr->bloktab[bloknum].levfval = 0;        /* Assume version 0 */
    if ((versval > 0) &&
        ((intLoad (stream, &symbptr->bloktab[bloknum].levfval) != 1) ||
         (symbptr->bloktab[bloknum].levfval < 0))) {
      errorPrint ("symbolLoad: bad input (4)");
      symbolExit (symbptr);
      symbolInit (symbptr);
      return     (1);
    }
  }

  return (0);
}

/*+ This routine saves the given
*** block matrix structure
*** to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolSave (
const SymbolMatrix * const  symbptr,
FILE * const                stream)
{
  const SymbolCblk *  cblktnd;
  const SymbolCblk *  cblkptr;
  const SymbolBlok *  bloktnd;
  const SymbolBlok *  blokptr;
  int                 o;

  o = (fprintf (stream, "1\n%ld\t%ld\t%ld\t%ld\n", /* Write file header */
                (long) symbptr->cblknbr,
                (long) symbptr->bloknbr,
                (long) symbptr->nodenbr,
                (long) symbptr->baseval) == EOF);
  for (cblkptr = symbptr->cblktab, cblktnd = cblkptr + symbptr->cblknbr;
       (cblkptr < cblktnd) && (o == 0); cblkptr ++) {
    o = (fprintf (stream, "%ld\t%ld\t%ld\n",
                  (long) cblkptr->fcolnum,
                  (long) cblkptr->lcolnum,
                  (long) cblkptr->bloknum) == EOF);
  }
  for (blokptr = symbptr->bloktab, bloktnd = blokptr + symbptr->bloknbr;
       (blokptr < bloktnd) && (o == 0); blokptr ++) {
    o = (fprintf (stream, "%ld\t%ld\t%ld\t%ld\n",
                  (long) blokptr->frownum,
                  (long) blokptr->lrownum,
                  (long) blokptr->cblknum,
                  (long) blokptr->levfval) == EOF);
  }

  return (o);
}
