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
/**   NAME       : mesh_graph.h                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the source mesh to source graph     **/
/**                building routine.                       **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 13 oct 2003     **/
/**                                 to     13 oct 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/** Prime number for cache-friendly perturbations. **/

#define MESHGRAPHHASHPRIME          37            /* Prime number */

/*
**  The type and structure definitions.
*/

/*+ A table made of such elements is used during
    graph building to build the edge array of the
    graph from the one of the mesh.               +*/

typedef struct MeshGraphHash_ {
  Gnum                      vertnum;              /*+ Origin vertex (i.e. pass) number in mesh +*/
  Gnum                      vertend;              /*+ End vertex number in mesh                +*/
} MeshGraphHash;
