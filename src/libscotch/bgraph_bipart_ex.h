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
/**   NAME       : bgraph_bipart_ex.h                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the exact-balance post-processing   **/
/**                module.                                 **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 25 oct 1994     **/
/**                                 to     26 oct 1994     **/
/**                # Version 3.0  : from : 18 nov 1995     **/
/**                                 to     20 nov 1995     **/
/**                # Version 3.1  : from : 20 nov 1995     **/
/**                                 to     20 nov 1995     **/
/**                # Version 3.2  : from : 15 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 11 dec 2003     **/
/**                                 to     11 dec 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ System-defined constants. +*/

#define BGRAPHBIPARTEXGAINTABLSUBBITS 1

#define BGRAPHBIPARTEXSTATEFREE     ((GainLink *) 0) /*+ Vertex in initial state (TRICK: must be 0) +*/
#define BGRAPHBIPARTEXSTATEUSED     ((GainLink *) 1) /*+ Swapped vertex                             +*/
#define BGRAPHBIPARTEXSTATELINK     ((GainLink *) 2) /*+ Currently in gain table if higher          +*/

/*
**  The function prototypes.
*/

#ifndef BGRAPH_BIPART_EX
#define static
#endif

int                         bgraphBipartEx      (Bgraph * restrict const);

#undef static
