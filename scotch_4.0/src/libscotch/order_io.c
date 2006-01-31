/* Copyright INRIA 2004
**
** This file is part of the Scotch distribution.
**
** The Scotch distribution is libre/free software; you can
** redistribute it and/or modify it under the terms of the
** GNU Lesser General Public License as published by the
** Free Software Foundation; either version 2.1 of the
** License, or (at your option) any later version.
**
** The Scotch distribution is distributed in the hope that
** it will be useful, but WITHOUT ANY WARRANTY; without even
** the implied warranty of MERCHANTABILITY or FITNESS FOR A
** PARTICULAR PURPOSE. See the GNU Lesser General Public
** License for more details.
**
** You should have received a copy of the GNU Lesser General
** Public License along with the Scotch distribution; if not,
** write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
**
** $Id$
*/
/************************************************************/
/**                                                        **/
/**   NAME       : order_io.c                              **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles generic orderings.  **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 19 oct 1996     **/
/**                                 to     27 aug 1998     **/
/**                # Version 4.0  : from : 19 dec 2001     **/
/**                                 to     28 jun 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ORDER_IO

#include "module.h"
#include "common.h"
#include "graph.h"
#include "order.h"

/************************************/
/*                                  */
/* These routines handle orderings. */
/*                                  */
/************************************/

/* This routine loads an ordering.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
orderLoad (
Order * restrict const        ordeptr,
const Gnum * restrict const   vlbltab,
FILE * restrict const         stream)
{
  Gnum * restrict     permtab;
  Gnum                vertnum;

  if (vlbltab != NULL) {
    errorPrint ("orderLoad: vertex labels not yet supported");
    return     (1);
  }

  if ((permtab = memAlloc (ordeptr->vnodnbr * sizeof (Gnum))) == NULL) {
    errorPrint ("orderLoad: out of memory");
    return     (1);
  }

  if (intLoad (stream, &ordeptr->vnodnbr) != 1) {
    errorPrint ("orderLoad: bad input (1)");
    memFree    (permtab);
    return     (1);
  }

  if (vlbltab == NULL) {                          /* If ordering does not have label array */
    for (vertnum = 0; vertnum < ordeptr->vnodnbr; vertnum ++) {
      Gnum                vertval;

      if (intLoad (stream, &vertval) +            /* Read item data */
          intLoad (stream, &permtab[vertnum]) != 2) {
        errorPrint ("orderLoad: bad input (2)");
        memFree    (permtab);
        return     (1);
      }
      if (vertval != (vertnum + ordeptr->baseval)) { /* Read item data */
        errorPrint ("orderLoad: bad input (3)");
        memFree    (permtab);
        return     (1);
      }
    }
  }

  orderPeri (permtab, ordeptr->baseval, ordeptr->vnodnbr, ordeptr->peritab, ordeptr->baseval); /* Compute inverse permutation */

  memFree (permtab);
  return  (0);
}

/* This routine saves an ordering.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
orderSave (
const Order * restrict const  ordeptr,
const Gnum * restrict const   vlbltab,
FILE * restrict const         stream)
{
  const Gnum * restrict vlbltax;
  Gnum * restrict       permtab;
  Gnum                  vertnum;

  vlbltax = (vlbltab != NULL) ? (vlbltab - ordeptr->baseval) : NULL;

  if ((permtab = memAlloc (ordeptr->vnodnbr * sizeof (Gnum))) == NULL) {
    errorPrint ("orderSave: out of memory");
    return     (1);
  }

  if (fprintf (stream, "%ld\n", (long) ordeptr->vnodnbr) == EOF) {
    errorPrint ("orderSave: bad output (1)");
    memFree    (permtab);
    return     (1);
  }

  orderPeri (ordeptr->peritab, ordeptr->baseval, ordeptr->vnodnbr, permtab, ordeptr->baseval); /* Compute direct permutation */

  if (vlbltax != NULL) {                          /* If ordering has label array */
    for (vertnum = 0; vertnum < ordeptr->vnodnbr; vertnum ++) {
      if (fprintf (stream, "%ld\t%ld\n",
                   (long) vlbltax[vertnum + ordeptr->baseval],
                   (long) vlbltax[permtab[vertnum]]) == EOF) {
        errorPrint ("orderSave: bad output (2)");
        memFree    (permtab);
        return     (1);
      }
    }
  }
  else {
    for (vertnum = 0; vertnum < ordeptr->vnodnbr; vertnum ++) {
      if (fprintf (stream, "%ld\t%ld\n",
                   (long) (vertnum + ordeptr->baseval),
                   (long) permtab[vertnum]) == EOF) {
        errorPrint ("orderSave: bad output (3)");
        memFree    (permtab);
        return     (1);
      }
    }
  }

  memFree (permtab);
  return  (0);
}

/* This routine saves the column
** block mapping data of the given
** ordering.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
orderSaveMap (
const Order * restrict const  ordeptr,
const Gnum * restrict const   vlbltab,
FILE * const                  stream)
{
  const Gnum * restrict vlbltax;
  Gnum * restrict       rangtab;
  Gnum * restrict       cblktab;
  Gnum                  vertnum;
  Gnum                  cblknum;

  vlbltax = (vlbltab != NULL) ? (vlbltab - ordeptr->baseval) : NULL;

  if (memAllocGroup ((void **) 
        &rangtab, (size_t) ((ordeptr->vnodnbr + 1) * sizeof (Gnum)),
        &cblktab, (size_t) ( ordeptr->vnodnbr      * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("orderSaveMap: out of memory");
    return     (1);
  }

  orderRang (ordeptr, rangtab);
  for (vertnum = 0, cblknum = 0; vertnum < ordeptr->vnodnbr; vertnum ++) {
    if (vertnum >= rangtab[cblknum + 1])
      cblknum ++;
    cblktab[ordeptr->peritab[vertnum] - ordeptr->baseval] = cblknum;
  }

  if (fprintf (stream, "%ld\n", (long) ordeptr->vnodnbr) == EOF) {
    errorPrint ("orderSaveMap: bad output (1)");
    memFree    (rangtab);                         /* Free memory group leader */
    return     (1);
  }
  for (vertnum = 0; vertnum < ordeptr->vnodnbr; vertnum ++) {
    if (fprintf (stream, "%ld\t%ld\n",
                 (long) ((vlbltax != NULL) ? vlbltax[vertnum + ordeptr->baseval] : (vertnum + ordeptr->baseval)),
                 (long) cblktab[vertnum]) == EOF) {
      errorPrint ("orderSaveMap: bad output (2)");
      memFree    (rangtab);                       /* Free memory group leader */
      return     (1);
    }
  }

  memFree (rangtab);                              /* Free memory group leader */
  return  (0);
}

/* This routine saves the separator
** tree data of the given ordering.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
orderSaveTree (
const Order * restrict const  ordeptr,
const Gnum * restrict const   vlbltab,
FILE * const                  stream)
{
  const Gnum * restrict vlbltax;
  Gnum * restrict       rangtab;
  Gnum * restrict       treetab;
  Gnum * restrict       cblktab;
  Gnum                  vertnum;
  Gnum                  cblknum;

  vlbltax = (vlbltab != NULL) ? (vlbltab - ordeptr->baseval) : NULL;

  if (memAllocGroup ((void **) 
        &rangtab, (size_t) ((ordeptr->vnodnbr + 1) * sizeof (Gnum)),
        &treetab, (size_t) ( ordeptr->vnodnbr      * sizeof (Gnum)),
        &cblktab, (size_t) ( ordeptr->vnodnbr      * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("orderSaveTree: out of memory");
    return     (1);
  }

  orderRang (ordeptr, rangtab);
  orderTree (ordeptr, treetab);
  for (vertnum = 0, cblknum = 0; vertnum < ordeptr->vnodnbr; vertnum ++) {
    if (vertnum >= rangtab[cblknum + 1])
      cblknum ++;
    cblktab[ordeptr->peritab[vertnum] - ordeptr->baseval] = treetab[cblknum];
  }

  if (fprintf (stream, "%ld\n", (long) ordeptr->vnodnbr) == EOF) {
    errorPrint ("orderSaveTree: bad output (1)");
    memFree    (rangtab);                         /* Free memory group leader */
    return     (1);
  }
  for (vertnum = 0; vertnum < ordeptr->vnodnbr; vertnum ++) {
    if (fprintf (stream, "%ld\t%ld\n",
                 (long) ((vlbltax != NULL) ? vlbltax[vertnum + ordeptr->baseval] : (vertnum + ordeptr->baseval)),
                 (long) cblktab[vertnum]) == EOF) {
      errorPrint ("orderSaveTree: bad output (2)");
      memFree    (rangtab);                       /* Free memory group leader */
      return     (1);
    }
  }

  memFree (rangtab);                              /* Free memory group leader */
  return  (0);
}
