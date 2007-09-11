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
/**   NAME       : order_base.c                            **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines hold the base changing      **/
/**                routine for the ordering structure.     **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 06 oct 2003     **/
/**                                 to     06 oct 2003     **/
/**                # Version 2.0  : from : 21 apr 2004     **/
/**                                 to     21 apr 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ORDER

#include "common.h"
#include "order.h"

/***********************************/
/*                                 */
/* The ordering handling routines. */
/*                                 */
/***********************************/

/* This routine sets the base of the
** given ordering structure to the given
** base value.
** It returns:
** - VOID  : in all cases.
*/

void
orderBase (
Order * restrict const      ordeptr,              /*+ Ordering structure +*/
const INT                   baseval)              /*+ New base value     +*/
{
  INT               baseadj;                      /* Base adjust */
  INT               cblknum;
  INT               vertnbr;
  INT               vertnum;

  if (ordeptr->rangtab == NULL)                   /* Cannot know old base if range array not provided */
    return;

  baseadj = baseval - ordeptr->rangtab[0];        /* Set base adjust     */
  if (baseadj == 0)                               /* If base already set */
    return;

  for (cblknum = 0; cblknum <= ordeptr->cblknbr; cblknum ++)
    ordeptr->rangtab[cblknum] += baseadj;

  vertnbr = ordeptr->rangtab[ordeptr->cblknbr] - ordeptr->rangtab[0];
  if (ordeptr->permtab != NULL) {
    for (vertnum = 0; vertnum < vertnbr; vertnum ++)
      ordeptr->permtab[vertnum] += baseadj;
  }
  if (ordeptr->peritab != NULL) {
    for (vertnum = 0; vertnum < vertnbr; vertnum ++)
      ordeptr->peritab[vertnum] += baseadj;
  }
}
