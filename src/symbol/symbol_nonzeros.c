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
/**   NAME       : symbol_nonzeros.c                       **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This test module writes the coordinates **/
/**                of all non-zeroes so as to check that   **/
/**                several factorization algorithms yield  **/
/**                the same results.                       **/
/**                                                        **/
/**   DATES      : # Version 1.2  : from : 28 aug 2002     **/
/**                                 to     28 aug 2002     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_NONZEROS

#include "common.h"
#include "symbol.h"

/******************************************/
/*                                        */
/* The symbolic matrix handling routines. */
/*                                        */
/******************************************/

/*+ This routine computes the factorization
*** and solving cost of the given symbolic
*** block matrix, whose nodes hold the number
*** of DOFs given by the proper DOF structure.
*** To ensure maximum accuracy and minimum loss
*** of precision, costs are summed-up recursively.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolNonzeros (
const SymbolMatrix * const  symbptr,              /*+ Symbolic matrix to evaluate +*/
FILE * const                stream)               /*+ Output file                 +*/
{
  INT                   cblknum;                  /* Number of current column block */
  SymbolBlok * restrict bloktax;                  /* Based pointer to block array   */
  INT                   bloknum;                  /* Number of current block        */
  INT                   colnum;
  INT                   rownum;

  bloktax = symbptr->bloktab - symbptr->baseval;

  for (cblknum = 0; cblknum < symbptr->cblknbr; cblknum ++) {
    for (colnum  = symbptr->cblktab[cblknum].fcolnum;
         colnum <= symbptr->cblktab[cblknum].lcolnum; colnum ++) {
      for (bloknum = symbptr->cblktab[cblknum].bloknum;
           bloknum < symbptr->cblktab[cblknum + 1].bloknum; bloknum ++) {
        for (rownum  = bloktax[bloknum].frownum;
             rownum <= bloktax[bloknum].lrownum; rownum ++) {
          if (fprintf (stream, "%ld\t%ld\t%c\n",
                       (long) rownum,
                       (long) colnum,
                       (bloktax[bloknum].levfval > 0) ? 'n' : 'z') == EOF)
            return (1);
        }
      }
    }
  }

  return (0);
}
