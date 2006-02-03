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
/**   NAME       : library_graph_check.c                   **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the source   **/
/**                graph handling routines of the          **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 18 aug 1998     **/
/**                                 to     18 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     01 nov 2001     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to     22 apr 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "graph.h"
#include "scotch.h"

/************************************/
/*                                  */
/* These routines are the C API for */
/* the graph handling routines.     */
/*                                  */
/************************************/

/*+ This routine checks the consistency
*** of the given graph.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
SCOTCH_graphCheck (
const SCOTCH_Graph * const  grafptr)
{
  return (graphCheck ((const Graph * const) grafptr));
}
