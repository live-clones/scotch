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
/**   NAME       : order_grid.h                            **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the data declarations   **/
/**                for the grid ordering routine.          **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 04 sep 1998     **/
/**                                 to     02 may 1999     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     28 feb 2004     **/
/**                                                        **/
/************************************************************/

#ifdef CXREF_DOC
#include "common.h"
#include "graph.h"
#include "scotch.h"
#endif /* CXREF_DOC */

/*
**  The function prototypes.
*/

#ifndef ORDER_GRID
#define static
#endif

static void                 graphOrderGrid22    (INT * const cblkptr, INT ** const rangptr, INT * const permtab, INT * const ordeptr, const INT xnbr, const INT xlim, const INT ylim, const INT xmin, const INT ymin, const INT xext, const INT yext, int flag);

static void                 graphOrderGrid32    (INT * const cblkptr, INT ** const rangptr, INT * const permtab, INT * const ordeptr, const INT xnbr, const INT ynbr, const INT xlim, const INT ylim, const INT zlim, const INT xmin, const INT ymin, const INT zmin, const INT xext, const INT yext, const INT zext, int flag);

#undef static
