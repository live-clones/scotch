/* Copyright 2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : vdgraph_separate_ml.h                   **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the sequential vertex separation    **/
/**                routine for distributed graphs.         **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 07 feb 2006     **/
/**                                 to   : 03 aug 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines
*/

/* Mpi tags */

#define TAGMLSIZE    1
#define TAGMLVERT    2
#define TAGMLPARTTAX 3

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct VdgraphSeparateMlParam_ {
  INT                       coarnbr;              /*+ Minimum number of vertices                  +*/
  INT                       dupmax;               /*+ Maximum number of vertices to do fold-dup   +*/
  INT                       duplvlmax;            /*+ Maximum level for allowing fold-dup         +*/
  double                    coarrat;              /*+ Coarsening ratio                            +*/
  INT                       reqsize;
  Strat *                   stratlow;             /*+ Strategy at lowest level                    +*/
  Strat *                   stratasc;             /*+ Strategy at ascending levels                +*/
  Strat *                   stratseq;             /*+ Strategy when running on a single processor +*/
  INT                       seqnbr;               /*+ Threshold when entering into seq mode       +*/
} VdgraphSeparateMlParam;

typedef struct VdgraphSeparateMlPart_ {
  Gnum                      vertnum;              /*+ Global vertex index for uncoarsening   +*/
  Gnum                      partval;              /*+ Gnum to have same type and for sorting +*/
} VdgraphSeparateMlPart;

/*
**  The function prototypes.
*/

#ifndef VDGRAPH_SEPARATE_ML
#define static
#endif

static int                  vdgraphSeparateMlCoarsen (Vdgraph * const, Vdgraph * const, DgraphCoarsenMulti * restrict * const, const VdgraphSeparateMlParam * const);
static int                  vdgraphSeparateMlUncoarsen (Vdgraph *, const Vdgraph * const, const DgraphCoarsenMulti * restrict const);

int                         vdgraphSeparateMl    (Vdgraph * const, const VdgraphSeparateMlParam * const);
static int                  vdgraphSeparateMl2   (Vdgraph * const, const VdgraphSeparateMlParam * const);

#undef static
