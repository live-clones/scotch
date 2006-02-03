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
/**   NAME       : library_order.h                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the library ordering          **/
/**                structure.                              **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 08 oct 1998     **/
/**                                 to   : 08 oct 1998     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to     25 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Ordering. +*/

typedef struct LibOrder_ {
  Order                     o;                    /*+ Ordering data                      +*/
  Gnum * restrict           permtab;              /*+ Direct permutation array           +*/
  Gnum * restrict           peritab;              /*+ Inverse permutation array          +*/
  Gnum * restrict           cblkptr;              /*+ Pointer to number of column blocks +*/
  Gnum * restrict           rangtab;              /*+ Column block range array           +*/
  Gnum * restrict           treetab;              /*+ Separator tree array               +*/
} LibOrder;
