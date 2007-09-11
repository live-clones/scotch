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
/**   NAME       : symbol_tree.c                           **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module computes some parameters of **/
/**                the elimination tree of symbolic        **/
/**                matrices.                               **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 27 oct 1998     **/
/**                                 to     27 oct 1998     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_TREE

#include "common.h"
#include "dof.h"
#include "symbol.h"

/******************************************/
/*                                        */
/* The symbolic matrix handling routines. */
/*                                        */
/******************************************/

/*+ This routine computes some parameters
*** of the elimination tree of the given
*** symbolic block matrix, whose nodes hold
*** the number of DOFs given by the proper
*** DOF structure.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolTree (
const SymbolMatrix * const  symbptr,              /*+ Symbolic matrix to evaluate              +*/
const Dof * const           deofptr,              /*+ DOF structure associated with the matrix +*/
INT * const                 leafnbrptr,           /*+ Number of leaves in elimination tree     +*/
INT * const                 heigminptr,           /*+ Minimum leaf height in elimination tree  +*/
INT * const                 heigmaxptr,           /*+ Maximum leaf height in elimination tree  +*/
double * const              heigavgptr,           /*+ Average leaf height in elimination tree  +*/
double * const              heigdltptr)           /*+ Deviation of heights                     +*/
{
  const SymbolCblk *  cblktnd;                    /* End of column block array */
  const SymbolCblk *  cblkptr;                    /* Pointer to current        */
  const SymbolBlok *  bloktax;                    /* Based access to bloktab   */
  INT                 nodenum;                    /* Number of current node    */
  byte *              leaftab;                    /* Flag array                */
  byte *              leaftax;                    /* Based access to leaftab   */
  byte *              leafptr;                    /* Pointer to current        */
  INT                 leafnbr;                    /* Number of leaves          */
  INT *               heigtab;                    /* Array of node heights     */
  INT *               heigtax;                    /* Based access to heigtab   */
  INT *               heigtnd;                    /* End of height array       */
  INT *               heigptr;                    /* Pointer to current        */
  INT                 heigval;                    /* Current height value      */
  INT                 heigmin;
  INT                 heigmax;
  double              heigsum;
  double              heigavg;
  double              heigdlt;

  if (((heigtab = (INT *) memAlloc (symbptr->nodenbr * sizeof (INT) + symbptr->cblknbr * sizeof (byte))) == NULL)) {
    errorPrint ("symbolTree: out of memory");
    return     (1);
  }
  leaftab = (byte *) (heigtab + symbptr->nodenbr);

  memSet (leaftab, 0, symbptr->cblknbr * sizeof (byte));
#ifdef SYMBOL_DEBUG
  memSet (heigtab, ~0, symbptr->nodenbr * sizeof (INT));
#endif /* SYMBOL_DEBUG */

  leaftax = leaftab          - symbptr->baseval;
  heigtax = heigtab          - symbptr->baseval;
  bloktax = symbptr->bloktab - symbptr->baseval;

  for (cblkptr = symbptr->cblktab + symbptr->cblknbr - 1,
       heigptr = heigtab + symbptr->nodenbr - 1,
       nodenum = symbptr->nodenbr - 1 + symbptr->baseval;
       cblkptr >= symbptr->cblktab;
       cblkptr --) {
    if (cblkptr[1].bloknum - cblkptr[0].bloknum > 1) { /* If extra-diagonal block present         */
      heigval = heigtax[bloktax[cblkptr->bloknum + 1].frownum] + 1; /* Get height of father, + 1  */
      leaftax[bloktax[cblkptr->bloknum + 1].cblknum] = 1; /* Flag father column block as not leaf */
    }
    else                                          /* Block is root or isolated */
      heigval = 1;

    for (heigtnd = heigtax + cblkptr->fcolnum;
         heigptr >= heigtnd; heigptr --, nodenum --) {
      *heigptr = heigval;                         /* Set node height                   */
      heigval += noddDlt (deofptr, nodenum);      /* Increase height by number of DOFs */
    }
  }

  leafnbr = 0;
  heigmin = INT_MAX;
  heigmax = 0;
  heigsum = 0.0L;

  for (cblkptr = symbptr->cblktab, leafptr = leaftab, heigptr = heigtab,
       cblktnd = cblkptr + symbptr->cblknbr;
       cblkptr < cblktnd;
       cblkptr ++, leafptr ++) {
    if (*leafptr == 0) {                          /* If column block is leaf  */
      heigval = heigtax[cblkptr->fcolnum];        /* Get height of first node */
      *heigptr ++ = heigval;                      /* Compress height array    */

      leafnbr ++;                                 /* Update leaf data */
      heigsum += (double) heigval;
      if (heigval < heigmin)
        heigmin = heigval;
      if (heigval > heigmax)
        heigmax = heigval;
    }
  }

  heigavg = (leafnbr == 0) ? 0.0L : heigsum / (double) leafnbr;
  heigdlt = 0.0L;
  for (heigptr = heigtab, heigtnd = heigptr + leafnbr;
       heigptr < heigtnd; heigptr ++)
    heigdlt += fabs ((double) *heigptr - heigavg);
  if (leafnbr > 0)
    heigdlt /= (heigavg * (double) leafnbr);

  *leafnbrptr = leafnbr;
  *heigminptr = heigmin;
  *heigmaxptr = heigmax;
  *heigavgptr = heigavg;
  *heigdltptr = heigdlt;

  memFree (heigtab);

  return (0);
}
