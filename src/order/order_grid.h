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
