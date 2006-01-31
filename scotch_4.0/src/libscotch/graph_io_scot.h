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
/**   NAME       : graph_io_scot.h                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the Scotch graph format       **/
/**                module.                                 **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 06 nov 1997     **/
/**                                 to     06 nov 1997     **/
/**                # Version 3.3  : from : 13 dec 1998     **/
/**                                 to     21 dec 1998     **/
/**                # Version 4.0  : from : 18 dec 2001     **/
/**                                 to     19 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ The sort structure, used to sort graph vertices by label. +*/

typedef struct GraphGeomScotSort_ {
  Gnum                      labl;                 /*+ Vertex label  +*/
  Gnum                      num;                  /*+ Vertex number +*/
} GraphGeomScotSort;

/*
**  The function prototypes.
*/

#ifndef GRAPH_IO_SCOT
#define static
#endif

static int                  graphGeomLoadScot2  (const GraphGeomScotSort * const, const GraphGeomScotSort * const);

#undef static
