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
/**   NAME       : bgraph_bipart_ex.c                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module tries to balance the        **/
/**                subgraphs of the partition as best as   **/
/**                it can.                                 **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 25 oct 1994     **/
/**                                 to     03 nov 1994     **/
/**                # Version 3.0  : from : 18 nov 1995     **/
/**                                 to     18 nov 1995     **/
/**                # Version 3.1  : from : 20 nov 1995     **/
/**                                 to     29 nov 1995     **/
/**                # Version 3.2  : from : 15 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 3.4  : from : 01 jun 2001     **/
/**                                 to     01 jun 2001     **/
/**                # Version 4.0  : from : 11 dec 2003     **/
/**                                 to     11 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes
*/

#define BGRAPH_BIPART_EX

#include "module.h"
#include "common.h"
#include "gain.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "bgraph.h"
#include "bgraph_bipart_ex.h"
#include "bgraph_bipart_fm.h"

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the bipartitioning.
** It returns:
** - 0 : if bipartitioning could be computed.
** - 1 : on error.
*/

int
bgraphBipartEx (
Bgraph * restrict const     grafptr)
{
  BgraphBipartFmParam paradat;                    /* Parameter area for Fiduccia-Mattheyses algorithm */

  if (grafptr->compload0dlt == 0)                 /* Return if nothing to do */
    return (0);

  paradat.movenbr = grafptr->s.vertnbr;
  paradat.passnbr = ~0;
  paradat.deltrat = 0.0L;                         /* Exact balance required */
  if (bgraphBipartFm (grafptr, &paradat) != 0)    /* Return if error        */
    return (1);

#ifdef SCOTCH_DEBUG_BGRAPH2
  if (bgraphCheck (grafptr) != 0) {
    errorPrint ("bgraphBipartEx: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  return (0);
}
