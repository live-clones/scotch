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
/**   NAME       : library_graph_base_f.c                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API for  **/
/**                the source graph handling routines of   **/
/**                the libSCOTCH library.                  **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 22 apr 2004     **/
/**                                 to     23 apr 2004     **/
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

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the graph handling routines.   */
/*                                    */
/**************************************/

/*
**
*/

FORTRAN (                                       \
SCOTCHFGRAPHBASE, scotchfgraphbase, (           \
SCOTCH_Graph * const        grafptr,            \
const SCOTCH_Num * const    basenew,            \
SCOTCH_Num * const          baseold),           \
(grafptr, basenew, baseold))
{
  *baseold = SCOTCH_graphBase (grafptr, *basenew);
}
