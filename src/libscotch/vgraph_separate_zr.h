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
/**   NAME       : vgraph_separate_zr.h                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the move-all-to-first-subdomain     **/
/**                separation method.                      **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 31 may 1999     **/
/**                                 to     31 may 1999     **/
/**                # Version 4.0  : from : 19 dec 2001     **/
/**                                 to     01 jan 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The function prototypes.
*/

#ifndef VGRAPH_SEPARATE_ZR
#define static
#endif

int                         vgraphSeparateZr    (Vgraph * const);

#undef static
