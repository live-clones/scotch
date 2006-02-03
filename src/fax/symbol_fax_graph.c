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
/**   NAME       : symbol_fax_graph.c                      **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This is the block symbolic factoriza-   **/
/**                tion routine for graphs.                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 22 jul 1998     **/
/**                                 to     29 sep 1998     **/
/**                # Version 0.2  : from : 08 may 2000     **/
/**                                 to     09 may 2000     **/
/**                # Version 1.0  : from : 01 jun 2002     **/
/**                                 to     03 jun 2002     **/
/**                # Version 1.1  : from : 26 jun 2002     **/
/**                                 to     26 jun 2002     **/
/**                # Version 2.0  : from : 21 mar 2003     **/
/**                                 to     21 mar 2003     **/
/**                # Version 3.0  : from : 02 mar 2004     **/
/**                                 to     02 mar 2004     **/
/**                                                        **/
/**   NOTES      : # symbolFaxGraph() could have called    **/
/**                  symbolFax() in the regular way, as    **/
/**                  do all of the grid-like factorization **/
/**                  routines. However, for efficiency     **/
/**                  reasons, we have decided to inline    **/
/**                  symbolFax(), to avoid a function call **/
/**                  for every arc.                        **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_FAX
#define SYMBOL_FAX_GRAPH

#include "common.h"
#include "scotch.h"
#include "graph.h"
#include "symbol.h"
#include "order.h"
#include "fax.h"
#include "symbol_fax.h"

/***********************************/
/*                                 */
/* Symbolic factorization routine. */
/*                                 */
/***********************************/

/*+ This routine computes the block symbolic
*** factorization of the given matrix graph
*** according to the given vertex ordering.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolFaxGraph (
SymbolMatrix * const        symbptr,              /*+ Symbolic block matrix [based]        +*/
const Graph * const         grafptr,              /*+ Matrix adjacency structure [based]   +*/
const Order * const         ordeptr)              /*+ Matrix ordering                      +*/
{
  INT                   baseval;
  INT                   vertnbr;
  INT *                 verttab;
  const INT * restrict  verttax;
  INT                   edgenbr;
  INT                   edgenum;
  INT *                 edgetab;
  const INT * restrict  edgetax;

  SCOTCH_graphData (grafptr, &baseval, &vertnbr, &verttab, NULL, NULL, NULL, &edgenbr, &edgetab, NULL);
  verttax = verttab - baseval;
  edgetax = edgetab - baseval;

#define SYMBOL_FAX_ITERATOR(ngbdptr, vertnum, vertend) \
                                    for (edgenum = verttax[vertnum];     \
                                         edgenum < verttax[vertnum + 1]; \
                                         edgenum ++) {                   \
                                      vertend = edgetax[edgenum];

#define SYMBOL_FAX_VERTEX_DEGREE(ngbdptr, vertnum) \
                                    (verttax[(vertnum) + 1] - verttax[(vertnum)])

  {
#define SYMBOL_FAX_INCLUDED
#include "symbol_fax.c"
  }
}
