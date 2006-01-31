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
/**   NAME       : hall_order_hf.h                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the block-oriented Halo       **/
/**                Approximate (Multiple) Minimum Fill     **/
/**                ordering routine.                       **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 15 may 2001     **/
/**                                 to   : 15 may 2001     **/
/**                # Version 4.0  : from : 10 jan 2003     **/
/**                                 to   : 08 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The function prototypes.
*/

#ifndef HALL_ORDER_HF
#define static
#endif

void                        hallOrderHfR2hamdf4 (const Gnum n, const Gnum nbelts, const Gnum nbbuck, const Gnum iwlen, Gnum pe[], Gnum pfree, Gnum len[], Gnum iw[], Gnum nv[], Gnum elen[], Gnum last[], Gnum * ncmpa, Gnum degree[], Gnum wf[], Gnum next[], Gnum w[], Gnum head[]);

#undef static
