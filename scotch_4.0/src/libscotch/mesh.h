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
/**   NAME       : mesh.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the source mesh functions.          **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 29 dec 2001     **/
/**                                 to     11 may 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ Mesh option flags. +*/

#define MESHNONE                    0x0000        /* No options set */

#define MESHFREEEDGE                0x0001        /* Free edgetab array        */
#define MESHFREEVERT                0x0002        /* Free verttab array        */
#define MESHFREEVEND                0x0004        /* Free verttab array        */
#define MESHFREEVNUM                0x0008        /* Free vnumtab array        */
#define MESHFREEOTHR                0x0010        /* Free all other arrays     */
#define MESHFREETABS                0x001F        /* Free all mesh arrays      */
#define MESHVERTGROUP               0x0010        /* All vertex arrays grouped */

/*+ The Mesh flag type. +*/

typedef int MeshFlag;                             /*+ Mesh property flags +*/

/*+ Mesh structure. It is basically a graph
    structure. It is a bipartite graph in the
    sense that node vertices are adjacent
    to element vertices only, and that element
    vertices are adjacent to node vertices only.
    Node vertices can all be put before or after
    element vertices, but node and element
    vertices cannot be mixed. In most algorithms,
    elements are put at the beginning because
    critical algorithms, such as the mesh
    induction and mesh coarsening routines,
    start by scanning element edges, such that
    elements can then be built on the fly before
    nodes are processed. Furthermore, as halo
    meshes comprise halo nodes but not halo
    elements, all halo nodes will be put at the
    end of the node array, making un-haloing much
    easier and inexpensive.
    Vertex global indices are also different,
    as vnumtab is only valid for (non-halo)
    node vertices. The base of the vnumtax array
    is thus vnodbas, and not s.baseval . Moreover,
    the contents of vnumtab is based with respect
    to baseval, and not to vnodbas, so that
    building the inverse permutation does not
    require to know vnodbas to trim node indices.
    When vertex loads are available, node loads
    represent the number of degrees of freedom
    per node, and element loads should be set as
    the sum of the vertex loads of all of their
    adjacent nodes (used by routines such as
    vmeshSeparateGg).                              +*/

typedef struct Mesh_ {
  MeshFlag                  flagval;              /*+ Graph properties                         +*/
  Gnum                      baseval;              /*+ Base index for edge/vertex arrays        +*/
  Gnum                      velmnbr;              /*+ Number of element vertices               +*/
  Gnum                      velmbas;              /*+ Based number of first element            +*/
  Gnum                      velmnnd;              /*+ Based number of first non-element vertex +*/
  Gnum                      veisnbr;              /*+ Number of isolated element vertices      +*/
  Gnum                      vnodnbr;              /*+ Number of node vertices in mesh          +*/
  Gnum                      vnodbas;              /*+ Based number of first node               +*/
  Gnum                      vnodnnd;              /*+ Based number of first non-node vertex    +*/
  Gnum * restrict           verttax;              /*+ Vertex array [based]                     +*/
  Gnum * restrict           vendtax;              /*+ End vertex array [based]                 +*/
  Gnum * restrict           velotax;              /*+ Element vertex load array (if present)   +*/
  Gnum * restrict           vnlotax;              /*+ Node vertex load array (if present)      +*/
  Gnum                      velosum;              /*+ Sum of element vertex weights            +*/
  Gnum                      vnlosum;              /*+ Sum of node vertex weights               +*/
  Gnum * restrict           vnumtax;              /*+ Vertex number in ancestor graph          +*/
  Gnum * restrict           vlbltax;              /*+ Vertex label (from file)                 +*/
  Gnum                      edgenbr;              /*+ Number of edges (arcs) in graph          +*/
  Gnum * restrict           edgetax;              /*+ Edge array [based]                       +*/
  Gnum                      degrmax;              /*+ Maximum degree                           +*/
} Mesh;

/*
**  The function prototypes.
*/

#ifndef MESH
#define static
#endif

int                         meshInit            (Mesh * const);
void                        meshExit            (Mesh * const);
void                        meshFree            (Mesh * const);
int                         meshLoad            (Mesh * restrict const, FILE * restrict const, const Gnum);
int                         meshSave            (const Mesh * restrict const, FILE * restrict const);
Gnum                        meshBase            (Mesh * const, const Gnum);
int                         meshGraph           (const Mesh * restrict const, Graph * restrict const);
int                         meshInduceList      (const Mesh *, Mesh *, const VertList *);
int                         meshInducePart      (const Mesh *, Mesh *, const Gnum, const GraphPart *, const GraphPart);
int                         meshInduceSepa      (const Mesh * restrict const, const GraphPart * restrict const, const Gnum, const Gnum * restrict const, Mesh * restrict const);
int                         meshCheck           (const Mesh * const);
int                         meshReorder         (const Mesh * restrict const, Mesh * restrict const);

#ifdef GEOM_H
int                         meshGeomLoadHabo    (Mesh * restrict const, Geom * restrict const, FILE * const, FILE * const, const char * const);
int                         meshGeomLoadScot    (Mesh * restrict const, Geom * restrict const, FILE * const, FILE * const, const char * const);
int                         meshGeomSaveScot    (const Mesh * restrict const, const Geom * restrict const, FILE * const, FILE * const, const char * const);
#endif /* GEOM_H */

#undef static
