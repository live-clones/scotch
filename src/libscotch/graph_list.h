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
/**   NAME       : graph_list.h                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the source graph functions.         **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 02 dec 1992     **/
/**                                 to     18 may 1993     **/
/**                # Version 1.3  : from : 30 apr 1994     **/
/**                                 to     18 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     18 aug 1994     **/
/**                # Version 3.0  : from : 07 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 28 nov 1995     **/
/**                                 to     28 nov 1995     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     15 sep 1998     **/
/**                # Version 3.3  : from : 28 sep 1998     **/
/**                                 to     23 mar 1999     **/
/**                # Version 3.4  : from : 20 mar 2000     **/
/**                                 to     20 mar 2000     **/
/**                # Version 4.0  : from : 24 nov 2001     **/
/**                                 to     27 sep 2002     **/
/**                                                        **/
/************************************************************/

#define GRAPH_LIST_H

/*
**  The type and structure definitions.
*/

/*+ The vertex list structure. Since a vertex list
    always refers to a given graph, vertex indices
    contained in the vertex list array are based with
    respect to the base value of the associated graph.
    However, the array itself is not based.            +*/

typedef struct VertList_ {
  Gnum                      vnumnbr;              /*+ Number of vertices in list +*/
  Gnum * restrict           vnumtab;              /*+ Pointer to vertex array    +*/
} VertList;

/*
**  The function prototypes.
*/

#ifndef GRAPH_LIST
#define static
#endif

int                         listInit            (VertList *);
void                        listExit            (VertList *);
int                         listAlloc           (VertList *, Gnum);
int                         listFree            (VertList *);
int                         listLoad            (VertList *, FILE *);
int                         listSave            (VertList *, FILE *);
void                        listSort            (VertList *);
int                         listCopy            (VertList *, VertList *);

#undef static
