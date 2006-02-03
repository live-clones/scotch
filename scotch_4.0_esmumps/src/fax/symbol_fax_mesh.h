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
/**   NAME       : symbol_fax_mesh.h                       **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the data declarations   **/
/**                for the mesh complete symbolic          **/
/**                factorization routine.                  **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 23 oct 2003     **/
/**                                 to     23 oct 2003     **/
/**                # Version 3.0  : from : 03 mar 2004     **/
/**                                 to     03 mar 2004     **/
/**                                                        **/
/************************************************************/

#define SYMBOL_FAX_MESH_H

/*
**  The defines.
*/

/* Prime number for hashing vertex numbers. */

#define SYMBOL_FAX_MESH_HASHPRIME   11            /*+ Prime number for hashing +*/

/*
**  The type and structure definitions.
*/

/*+ A table made of such elements is used during
    coarsening to build the edge array of the new
    mesh, after the labeling of the vertices.     +*/

typedef struct SymbolFaxMeshHash_ {
  INT                       vnodnum;              /*+ Origin node vertex (i.e. pass) number +*/
  INT                       vnodend;              /*+ End node vertex number in mesh        +*/
} SymbolFaxMeshHash;

/*+ The adjacency structure. +*/

typedef struct SymbolFaxMeshNghb_ {
  INT                           hashmsk;          /*+ Mask for hash structure    +*/
  SymbolFaxMeshHash * restrict  hashtab;          /*+ Hash table for neighbors   +*/
  INT                           baseval;          /*+ Base value                 +*/
  INT                           degrmax;          /*+ Maximum degree             +*/
  const INT * restrict          verttax;          /*+ Vertex array               +*/
  const INT * restrict          edgetax;          /*+ Edge array                 +*/
  INT                           vnodadj;          /*+ Node index adjustment      +*/
  INT                           vnodnum;          /*+ Current node               +*/
  INT                           enodnum;          /*+ Current node edge          +*/
  INT                           enodend;          /*+ End node edge              +*/
  INT                           eelmnum;          /*+ Current element edge       +*/
  INT                           eelmend;          /*+ End element edge           +*/
} SymbolFaxMeshNghb;

/*
**  The function prototypes.
*/

#ifndef SYMBOL_FAX_MESH
#define static
#endif

static INT                  symbolFaxMeshDegr   (SymbolFaxMeshNghb * const nghbptr, const INT vertnum);
static INT                  symbolFaxMeshFrst   (SymbolFaxMeshNghb * const nghbptr, const INT vertnum);
static INT                  symbolFaxMeshNext   (SymbolFaxMeshNghb * const nghbptr);

#undef static
