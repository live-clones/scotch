/* Copyright 2007,2008 ENSEIRB, INRIA & CNRS
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
/**   NAME       : dgraph.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Francois CHATENET (P0.0)                **/
/**                Sebastien FOUCAULT (P0.0)               **/
/**                Nicolas GICQUEL (P0.1)                  **/
/**                Jerome LACOSTE (P0.1)                   **/
/**                Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the distributed source graph        **/
/**                structure.                              **/
/**                                                        **/
/**   DATES      : # Version P0.0 : from : 01 apr 1997     **/
/**                                 to   : 20 jun 1997     **/
/**                # Version P0.1 : from : 07 apr 1998     **/
/**                                 to   : 20 jun 1998     **/
/**                # Version P0.2 : from : 11 may 1999     **/
/**                                 to   : 02 feb 2000     **/
/**                # Version P0.3 : from : 16 jun 2005     **/
/**                                 to   : 16 jun 2005     **/
/**                # Version 5.0  : from : 22 jul 2005     **/
/**                                 to   : 03 aug 2007     **/
/**                # Version 5.1  : from : 11 nov 2007     **/
/**                                 to   : 03 apr 2008     **/
/**                                                        **/
/************************************************************/

#define DGRAPH_H

#define PTSCOTCH_FOLD_DUP                         /* Activate folding on coarsening */

/*
** The defines.
*/

/* Graph flags. */

#define DGRAPHNONE                  0x0000        /* No options set */

#define DGRAPHFREETABS              0x0001        /* Set if local arrays freed on exit */
#define DGRAPHVERTGROUP             0x0002        /* All vertex arrays grouped         */
#define DGRAPHEDGEGROUP             0x0004        /* All edge arrays grouped           */
#define DGRAPHFREEEDGEGST           0x0008        /* Set if edgegsttab freed on exit   */
#define DGRAPHFREECOMM              0x0010        /* MPI communicator has to be freed  */
#define DGRAPHFREEEXIT              0x0020        /* Do not preserve private data      */
#define DGRAPHHASEDGEGST            0x0040        /* Edge ghost array computed         */
#define DGRAPHFREEALL               (DGRAPHFREETABS | DGRAPHFREEEDGEGST | DGRAPHFREECOMM | DGRAPHFREEEXIT)

#define DGRAPHBITSUSED              0x007F        /* Significant bits for plain distributed graph routines               */
#define DGRAPHBITSNOTUSED           0x0080        /* Value above which bits not used by plain distributed graph routines */

/* Used in algorithms */

#define COARPERTPRIME               31            /* Prime number */
#define COARHASHPRIME               179           /* Prime number */

#define DGRAPHBUILDRANDOM           1             /* Distribute the grid randomly  */
#define DGRAPHBUILDSLICES           2             /* Distribute the grid by slices */


/*
** The type and structure definitions.
*/

/* The graph basic types, which must be signed. */

#ifndef GNUMMAX                                   /* If graph.h not included */
typedef INT                 Gnum;                 /* Vertex or edge number   */
#define GNUMMAX                     (INTVALMAX)   /* Maximum Gnum value      */
#endif /* GNUMMAX */

#define GNUM_MPI                    COMM_INT      /* MPI type for Gnum is MPI type for INT */

#define GRAPHPART_MPI               COMM_BYTE     /* Raw byte type for graph parts */

/* Tags used for point-to-point communications. */

typedef enum DgraphTag_ {
  TAGHALO = 0,                                    /*+ Halo message         +*/
  TAGPROCVRTTAB,                                  /*+ procvrttab message   +*/
  TAGVERTLOCTAB,                                  /*+ vertloctab message   +*/
  TAGVENDLOCTAB,                                  /*+ vendloctab message   +*/
  TAGVELOLOCTAB,                                  /*+ veloloctab message   +*/
  TAGVNUMLOCTAB,                                  /*+ vnumloctab message   +*/
  TAGVLBLLOCTAB,                                  /*+ vlblloctab message   +*/
  TAGEDGELOCTAB,                                  /*+ edgeloctab message   +*/
  TAGEDLOLOCTAB,                                  /*+ edloloctab message   +*/
  TAGVELOLOCSUM,                                  /*+ velolocsum message   +*/
  TAGDATALOCTAB,                                  /*+ Generic data message +*/
  TAGOK,                                          /*+ Positive answer      +*/
  TAGBAD,                                         /*+ Negative answer      +*/
  TAGNBR                                          /*+ Number of tags       +*/
} DgraphTag;

/*+ The graph flag type. +*/

typedef int DgraphFlag;                           /*+ Graph property flags +*/

/*+ The vertex part type, in compressed form. From graph.h +*/

#ifndef GRAPH_H
typedef byte GraphPart;
#endif /* GRAPH_H */

/* The distributed graph structure. */

typedef struct Dgraph_ {
  DgraphFlag                flagval;              /*+ Graph properties                                         +*/
  Gnum                      baseval;              /*+ Base index for edge/vertex arrays                        +*/
  Gnum                      vertglbnbr;           /*+ Global number of vertices                                +*/
  Gnum                      vertglbmax;           /*+ Maximum number of local vertices over all processes      +*/
  Gnum                      vertgstnbr;           /*+ Number of local + ghost vertices                         +*/
  Gnum                      vertgstnnd;           /*+ vertgstnbr + baseval                                     +*/
  Gnum                      vertlocnbr;           /*+ Local number of vertices                                 +*/
  Gnum                      vertlocnnd;           /*+ Local number of vertices + baseval                       +*/
  Gnum * restrict           vertloctax;           /*+ Local vertex beginning index array [based]               +*/
  Gnum * restrict           vendloctax;           /*+ Local vertex end index array [based]                     +*/
  Gnum * restrict           veloloctax;           /*+ Local vertex load array if present                       +*/
  Gnum                      velolocsum;           /*+ Local sum of all vertex loads                            +*/
  Gnum                      veloglbsum;           /*+ Global sum of all vertex loads                           +*/
  Gnum * restrict           vnumloctax;           /*+ Arrays of global vertex numbers in original graph        +*/
  Gnum * restrict           vlblloctax;           /*+ Arrays of vertex labels (when read from file)            +*/
  Gnum                      edgeglbnbr;           /*+ Global number of arcs                                    +*/
  Gnum                      edgeglbmax;           /*+ Maximum number of local edges over all processes         +*/
  Gnum                      edgelocnbr;           /*+ Number of local edges                                    +*/
  Gnum                      edgelocsiz;           /*+ Size of local edge array (= edgelocnbr when compact)     +*/
  Gnum                      edgeglbsmx;           /*+ Maximum size of local edge arrays over all processes     +*/
  Gnum * restrict           edgegsttax;           /*+ Edge array holding local indices of neighbors [based]    +*/
  Gnum * restrict           edgeloctax;           /*+ Edge array holding global neighbor numbers [based]       +*/
  Gnum * restrict           edloloctax;           /*+ Edge load array                                          +*/
  Gnum                      degrglbmax;           /*+ Maximum degree over all processes                        +*/
  MPI_Comm                  proccomm;             /*+ Graph communicator                                       +*/
  int                       procglbnbr;           /*+ Number of processes sharing graph data                   +*/
  int                       proclocnum;           /*+ Number of this process                                   +*/
  int * restrict            procvrttab;           /*+ Global array of vertex number ranges [+1,based]          +*/
  int * restrict            proccnttab;           /*+ Count array for local number of vertices                 +*/
  int * restrict            procdsptab;           /*+ Displacement array with respect to proccnttab [+1,based] +*/
  int                       procngbnbr;           /*+ Number of neighboring processes                          +*/
  int                       procngbmax;           /*+ Maximum number of neighboring processes                  +*/
  int * restrict            procngbtab;           /*+ Array of neighbor process numbers [sorted]               +*/
  int                       procgstmax;           /*+ Maximum number of ghost vertices per neighbor            +*/
  int * restrict            procrcvtab;           /*+ Indices to receive ghost vertex sub-arrays               +*/
  int                       procsndnbr;           /*+ Overall size of local send array                         +*/
  int * restrict            procsndtab;           /*+ Indices to send ghost vertex sub-arrays [+1]             +*/
  int * restrict            procsidtab;           /*+ Array of indices to build communication vectors (send)   +*/
  int                       procsidnbr;           /*+ Size of the send index array                             +*/
} Dgraph;

/*
** The function prototypes.
*/

int                         dgraphInit          (Dgraph * const, MPI_Comm);
void                        dgraphExit          (Dgraph * const);
void                        dgraphFree          (Dgraph * const);
int                         dgraphLoad          (Dgraph * const, FILE * const, const Gnum, const DgraphFlag);
int                         dgraphSave          (Dgraph * const, FILE * const);
int                         dgraphBuild         (Dgraph * const, const Gnum, const Gnum, const Gnum, Gnum * const, Gnum * const, Gnum * const, Gnum * const, Gnum * const, const Gnum, const Gnum, Gnum * const, Gnum * const, Gnum * const);
int                         dgraphBuild2        (Dgraph * const, const Gnum, const Gnum, const Gnum, Gnum * const, Gnum * const, Gnum * const, const Gnum, Gnum * const, Gnum * const, const Gnum, const Gnum, Gnum * const, Gnum * const, Gnum * const, const Gnum);
int                         dgraphBuild3        (Dgraph * const, const Gnum, const Gnum, Gnum * const, Gnum * const, Gnum * const, const Gnum, Gnum * const, Gnum * const, const Gnum, const Gnum, Gnum * const, Gnum * const, Gnum * const, const Gnum);
int                         dgraphBuild4        (Dgraph * const);
int                         dgraphBuildHcub     (Dgraph * const, const Gnum, const Gnum, const Gnum);
int                         dgraphBuildGrid3D   (Dgraph * const, const Gnum, const Gnum, const Gnum, const Gnum, const int);
int                         dgraphCheck         (const Dgraph * const);
int                         dgraphView          (const Dgraph * const, FILE * const);
int                         dgraphGhst          (Dgraph * const);
int                         dgraphBand          (Dgraph * restrict const, const Gnum, Gnum * restrict const, const GraphPart * restrict const, const Gnum, const Gnum, Gnum, Dgraph * restrict const, Gnum * restrict * const, GraphPart * restrict * const, Gnum * const, Gnum * const, Gnum * const);
int                         dgraphFold          (const Dgraph * restrict const, const int, Dgraph * restrict const, const void * restrict const, void ** restrict const, MPI_Datatype);
int                         dgraphFold2         (const Dgraph * restrict const, const int, Dgraph * restrict const, MPI_Comm, const void * restrict const, void ** restrict const, MPI_Datatype);
int                         dgraphFoldDup       (const Dgraph * restrict const, Dgraph * restrict const, void * restrict const, void ** restrict const, MPI_Datatype);
int                         dgraphInduceList    (Dgraph * const, const Gnum, const Gnum * const, Dgraph * const);
int                         dgraphInducePart    (Dgraph * const, GraphPart * restrict const, const Gnum, const GraphPart, Dgraph * const);
#ifdef GRAPH_H
int                         dgraphGather        (const Dgraph * restrict const, Graph * restrict);
int                         dgraphGather2       (const Dgraph * restrict const, Graph * restrict, const int, const Gnum);
int                         dgraphGatherAll     (const Dgraph * restrict const, Graph * restrict);
int                         dgraphGatherAll2    (const Dgraph * restrict const, Graph * restrict, const Gnum, const int);
int                         dgraphScatter       (Dgraph * const, const Graph * const);
#endif /* GRAPH_H */

int                         dgraphHaloSync      (Dgraph * const, byte *, MPI_Datatype);

/*
**  The macro definitions.
*/

#ifndef inline
#define inline static __inline__
#endif /* inline */

inline int
dgraphVertexProc (const Dgraph * restrict const g, Gnum v)
{
  int start, stop, median;

  start = 0;
  stop  = g->procglbnbr;

  if ((g->procvrttab[start] > (int) v) ||
      (g->procvrttab[stop] < (int) v))
    return (-1);

  while (stop > start + 1) {
    median = (start + stop) / 2;
    if (g->procvrttab[median] < (int) v)
      start = median;
    else if (g->procvrttab[median] > (int) v)
      stop = median;
    else
      stop = start = median;
  }
  return (start);
}

#define dgraphVertexLocal(g,v)      (((g)->procvrttab[(g)->proclocnum] <= (v)) && ((g)->procvrttab[(g)->proclocnum + 1] > (v)))
