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
/**   NAME       : hmesh.h                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data           **/
/**                declarations for the halo mesh          **/
/**                structure.                              **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 31 dec 2001     **/
/**                                 to     29 apr 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Halo mesh structure. Only node vertices
    can be halo vertices, not element vertices.
    Halo node vertices are numbered with the
    highest available node numbers.
    Since un-haloing of a mesh should be costless,
    and since the vertex array must be continuous,
    when a halo mesh indeed bears halo nodes, it is
    preferrable that elements be numbered first, then
    non-halo node vertices, then halo vertices, so
    that the removal of the halo does not create
    holes in the vertex array. Else, the indices of
    the halo nodes must be reassigned to empty elements,
    which results in a larger structure until it is
    coarsened or induced. If no halo is present, the
    order of nodes and elements is not relevant.
    As for the halo graph structure, in the
    adjacency list of elements, halo node neighbors
    must all be put after non-halo node neighbors,
    so that the edge sub-array comprised between
    verttab[i] and vnhdtab[i] refer only to non-halo
    node neighbors, such that edgetab can be re-used
    by the un-halo-ed mesh.
    Since Hmesh halo meshes are used only for node
    ordering, the velotab and vnumtab arrays that are
    created by hmesh*() routines can be restricted to
    their node part. It must be guaranteed that this
    does not create problems at freeing time, for
    instance by grouping these arrays with verttab.      +*/

typedef struct Hmesh_ {
  Mesh                      m;                    /*+ Source mesh                                                              +*/
  Gnum * restrict           vehdtax;              /*+ End vertex array for elements [based] (non-halo nodes look at m.vendtax) +*/
  Gnum                      veihnbr;              /*+ Number of halo isolated element vertices, which have halo nodes only     +*/
  Gnum                      vnohnbr;              /*+ Number of non-halo node vertices                                         +*/
  Gnum                      vnohnnd;              /*+ Based number of first halo node vertex in mesh graph (m.vnodnnd if none) +*/
  Gnum                      vnhlsum;              /*+ Sum of non-halo node vertex weights                                      +*/
  Gnum                      enohnbr;              /*+ Number of non-halo edges                                                 +*/
  Gnum                      levlnum;              /*+ Nested dissection level                                                  +*/
} Hmesh;

/*
**  The function prototypes.
*/

#ifndef HMESH
#define static
#endif

void                        hmeshExit           (Hmesh * const);
Gnum                        hmeshBase           (Hmesh * const, const Gnum);
#ifdef HGRAPH_H
int                         hmeshHgraph         (const Hmesh * restrict const, Hgraph * restrict const);
#endif /* HGRAPH_H */
int                         hmeshInducePart     (const Hmesh * const, const GraphPart * const, const GraphPart, const Gnum, const Gnum, const Gnum, Hmesh * const);
int                         hmeshMesh           (const Hmesh * restrict const, Mesh * restrict const);
int                         hmeshCheck          (const Hmesh *);

#undef static
