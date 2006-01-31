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
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the data declarations   **/
/**                for the mesh structure.                 **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 21 mar 2003     **/
/**                                 to     24 oct 2003     **/
/**                                                        **/
/************************************************************/

#define MESH_H

/*
**  The type and structure definitions.
*/

/** The mesh. **/

typedef struct Mesh_ {
  INT                       baseval;              /*+ Base value for numberings                    +*/
  INT                       vertnbr;              /*+ Total number of vertices (nodes + elements)  +*/
  INT                       velmnbr;              /*+ Number of element vertices                   +*/
  INT                       velmbas;              /*+ Based number of first element                +*/
  INT                       velmnnd;              /*+ Based number of first non-element vertex     +*/
  INT                       vnodnbr;              /*+ Number of node vertices in mesh              +*/
  INT                       vnodbas;              /*+ Based number of first node                   +*/
  INT                       vnodnnd;              /*+ Based number of first non-node vertex        +*/
  INT                       vnlosum;              /*+ Sum of node vertex weights                   +*/
  INT                       velosum;              /*+ Sum of node and element vertex weights       +*/
  INT                       edgenbr;              /*+ Number of edges (arcs in fact) in edge array +*/
  INT                       edgesiz;              /*+ Size of edge array                           +*/
  INT * restrict            verttab;              /*+ Array of vertex neighobr indices [based]     +*/
  INT * restrict            velotab;              /*+ Array of vertex loads [based]                +*/
  INT * restrict            vlbltab;              /*+ Array of vertex labels [based]               +*/
  INT * restrict            edgetab;              /*+ Array of neighbors [based]                   +*/
  INT                       degrmax;              /*+ Maximum degree of mesh vertices              +*/
} Mesh;

/*
**  The function prototypes.
*/

#ifndef MESH
#define static
#endif

int                         meshInit            (Mesh * const meshptr);
void                        meshFree            (Mesh * const meshptr);
void                        meshExit            (Mesh * const meshptr);
int                         meshLoad            (Mesh * const meshptr, FILE * const stream);
int                         meshSave            (const Mesh * const meshptr, FILE * const stream);
void                        meshBase            (Mesh * const meshptr, const INT baseval);
int                         meshBuildMesh       (Mesh * const meshptr, const INT vertnbr, const INT edgenbr, const INT degrmax, const INT baseval, const INT velmbas, const INT vnodbas, const INT * restrict verttab, const INT * restrict velotab, const INT veloval, const INT * restrict edgetab);
int                         meshCheck           (const Mesh * const meshptr);

#undef static
