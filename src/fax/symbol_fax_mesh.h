/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
** 
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
** 
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
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
