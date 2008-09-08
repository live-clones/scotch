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
/**   NAME       : dgraph_coarsen.h                        **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This file implements the coarsening     **/
/**                phase of the multi-level method.        **/
/**                The implementation uses several         **/
/**                processes, which could have several     **/
/**                threads each (2 at this moment).        **/
/**                                                        **/
/**   DATES      : # Version 1.0  : from : 27 Jul 2005     **/
/**                                 to   : 24 feb 2007     **/
/**                                                        **/
/************************************************************/

/*
 * The defines
 */

#define TAGCOARVERT 1
#define TAGCOARSIZE 2
#define TAGCOAREDGE 3
#define TAGCOAREDLO 4

/*+ The multinode table element, which contains
    pairs of based indices of collapsed vertices.
    Both values are equal for uncollapsed vertices. +*/

typedef struct DgraphCoarsenMulti_  {
  Gnum                      vertnum[2];           /*+ Global indices of the collapsed vertices of a multinode +*/
} DgraphCoarsenMulti;


/*+ The following structure is used to exchange vertices which
    have been selected to migrate during the matching phase.   +*/

typedef struct DgraphCoarsenVert_ {
  Gnum                      vertnum;              /*+ Number of the vertex (global) +*/
  Gnum                      edgenbr;              /*+ Number of edges associated with this vertex +*/
  Gnum                      edgebegin;
  Gnum                      weight;               /*+ Weight of the vertex, 1 if unweighted graph +*/
} DgraphCoarsenVert;


/*+ A table made of such elements is used during
    coarsening to build the edge array of the new
    graph, after the labeling of the vertices.    +*/

typedef struct DgraphCoarsenHash_ {
  Gnum                      vertorgnum;           /*+ Origin vertex (i.e. pass) number +*/
  Gnum                      vertendnum;           /*+ Other end vertex number          +*/
  Gnum                      edgenum;              /*+ Number of corresponding edge     +*/
} DgraphCoarsenHash;


/*+ The following structure is used to exchange
    vertices which are needed as extremity of edges
    from a local vertex. The structure is used for
    sending and receiving such vertices.
    In fact we use two tables of this structure,
    indexed by proc number.                       +*/

typedef struct DgraphCoarsenCom_ {
  Gnum vertnbr;                                   /*+ Nbr of vertices which are exchanged +*/
  Gnum vertnum;                                   /*+ Current nbr of vertices ready to go +*/
/*   union { */
    Gnum                 last;                    /*+ Indice of last vertex found to be sent to current proc +*/
    DgraphCoarsenMulti * begin;                   /*+ Beginning of vertices array to be sent to the current proc +*/
/*   }; */
} DgraphCoarsenCom;


/*+ The coarsening routine is multi-threaded, and
    the following structure is designed for
    inter-thread communications.                  +*/

typedef struct DgraphCoarsenShared_ {
  Dgraph * restrict               finegrafptr;    /*+ Pointer to fine graph                                     +*/
  Dgraph * restrict               coargrafptr;    /*+ Pointer to coarse graph which is built                    +*/
  DgraphCoarsenCom * restrict     vertnbrrcvtab;  /*+ Tab of vertices received, protect by previous mutexes     +*/
  DgraphCoarsenMulti * restrict * coarproctab;
  DgraphCoarsenVert * restrict *  vertproctab;    /*+ Using Dichotomie into to search vertex                    +*/
  Gnum * restrict *               edgeproctab;    /*+ Edges associated to immigrant vertices                    +*/
  Gnum * restrict *               edloproctab;    /*+ Edge loads associated to immigrant vertices               +*/
  DgraphCoarsenMulti * restrict   coarmulttax;    /*+ Structure that contains result of the matching            +*/
  int  * restrict                 finefoldtax;    /*+ Tab which contains proc target for vertex                 +*/
  Gnum * restrict                 finecoartax;    /*+ Tab which contains local number for big vertex            +*/
  int  * restrict                 vertsnd;        /*+ Tab of number of vertices to send (indexed by proc)       +*/
  int  * restrict                 vertrcv;        /*+ Tab of number of vertices to receive (indexed by proc)    +*/
  int  * restrict                 edgesnd;        /*+ Tab of number of edges to send (indexed by proc)          +*/
  int  * restrict                 edgercv;        /*+ Tab of number of edges to receive (indexed by proc)       +*/
  Gnum *                          verttosndtab;
  Gnum * restrict *               verttosnd;
  MPI_Request *                   requesttab;     /*+ Used to complete vertices migration +*/
} DgraphCoarsenShared;

/*
** The functions
*/

#ifndef DGRAPH_COARSEN_C
# define static
#endif /* DGRAPH_COARSEN_C */


/**
   @brief
   This routine coarsens the given "finegraph" into
   "coargraph", as long as the coarsening ratio remains
   below some threshold value and the coarsened graph
   is not too small.

   @param finegrafptr pointer to finer graph
   @param coargrafptr pointer to coarser graph
   @param coarmultptr pointer to multinode table
   @param coarnbr     minimum number of coarse vertices
   @param coarrat     maximum contraction ratio

   @return
   - 0  : if the graph has been coarsened.
   - 1  : if the graph could not be coarsened.
   - 2  : on error.
*/

int
dgraphCoarsen (
	       Dgraph * restrict const               finegrafptr, /*+ Graph to coarsen                     +*/
	       Dgraph * restrict const               coargrafptr, /*+ Coarse graph to build                +*/
	       DgraphCoarsenMulti * restrict * const coarmultptr, /*+ Pointer to multinode table to build  +*/
	       const Gnum                            coarnbr,     /*+ Minimum number of coarse vertices    +*/
         const int                             dofolddup,   /*+ Allow fold/dup or fold or nofold     +*/
	       const Gnum                            dupmax,      /*+ Maximum number of vertices to do dup +*/
	       const double                          coarrat,     /*+ Maximum contraction ratio            +*/
	       const unsigned int                    reqsize);    /*+ Sizeof MPI buffer in matching        +*/

#ifndef DGRAPH_COARSEN_C
# undef static
#endif /* DGRAPH_COARSEN_C */


/*
 * The following functions are private
 */

#ifdef DGRAPH_COARSEN_C

/**
   @brief

   This   function  is   used  to   build  the   new   edgeloctax  and
   edloloctax.  All   datas  must  be  available   when  running  (all
   receptions    done).      This    function    is     inspired    by
   libscotch/src/graph_coarsen_edge.c.
*/

static void
dgraphCoarsenEdge (
const DgraphCoarsenShared * restrict const shared,      /*+ shared data           +*/
const DgraphCoarsenMulti * restrict const  coarmulttax,  /*+ Multinode array       +*/
DgraphCoarsenHash * restrict const         coarhashtab,  /*+ End vertex hash table +*/
const Gnum                                coarhashmsk); /*+ Hash table mask       +*/

static Gnum                 dgraphCoarsenSharedInit (DgraphCoarsenShared * restrict, const Dgraph * const, Dgraph *  const);
static void                 dgraphCoarsenSharedExit (DgraphCoarsenShared *);
static Gnum                 dgraphCoarsenSearchMulti (const DgraphCoarsenShared * restrict const, Gnum);
static Gnum                 dgraphCoarsenSearchVert (const DgraphCoarsenShared * restrict const, Gnum, Gnum);

static __inline__ Gnum
dgraphCoarsenSearch (const void * restrict const tab,
                     size_t offset, size_t elementsize,
                     int nbr, Gnum target)
{
  Gnum lowerbound;
  Gnum upperbound;
  Gnum median = -1;                               /* To avoid warning */

#define ACCESS_TAB(num) *(Gnum*)(tab + elementsize * (num) + offset)

  if (ACCESS_TAB(0) == target)
    return (0);

  if (ACCESS_TAB(nbr-1) == target)
    return (nbr-1);


#ifdef SCOTCH_DEBUG_DGRAPH2
  if ((ACCESS_TAB(0) > target)
      || (ACCESS_TAB(nbr - 1) < target)) {
    errorPrint ("dgraphCoarsenSearch : Internal Error (1)");
    return (-1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  lowerbound = 0; upperbound = nbr-1;

  while (lowerbound != upperbound) {
    median = (lowerbound + upperbound) /2;
    if (ACCESS_TAB(median) == target)
      return (median);

    if (ACCESS_TAB(median) < target) {
      if (median != lowerbound)
        lowerbound = median;
      else
        break;
    }
    else
      upperbound = median;
  }

#ifdef SCOTCH_DEBUG_DGRAPH2
  if (ACCESS_TAB(median) != target) {
    errorPrint ("dgraphCoarsenSearch : Internal Error (2)");
    return (-1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  return (median);

#undef ACCESS_TAB
}


static __inline__ Gnum
dgraphCoarsenSearchMulti (const DgraphCoarsenShared * restrict const shared,
                          Gnum finevertnum)
{
  Gnum procnum;
  Gnum element;

  procnum = dgraphVertexProc (shared->finegrafptr, finevertnum);

  element = dgraphCoarsenSearch (shared->vertnbrrcvtab[procnum].begin,
                                 (size_t)&shared->vertnbrrcvtab[procnum].begin[0].vertnum[0] - (size_t)&shared->vertnbrrcvtab[procnum].begin[0],
                                 sizeof (DgraphCoarsenMulti),
                                 shared->vertnbrrcvtab[procnum].vertnbr,
                                 finevertnum);

#ifdef SCOTCH_DEBUG_DGRAPH2
  if (element == -1) {
    errorPrint ("dgraphCoarsenSearchMulti : vertex not found");
    return (-1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  return (shared->vertnbrrcvtab[procnum].begin[element].vertnum[1]);
}


static __inline__ Gnum
dgraphCoarsenSearchVert (const DgraphCoarsenShared * restrict const shared,
                         Gnum procnum, Gnum vertnum)
{
  return (dgraphCoarsenSearch (shared->vertproctab[procnum],
                               (size_t)&shared->vertproctab[procnum][0].vertnum - (size_t)&shared->vertproctab[procnum][0],
                               sizeof(DgraphCoarsenVert), shared->vertrcv[procnum],
                               vertnum));
}



/**
   @brief

   This  function is  used  in  a thread  which  computes the  coarser
   graph. This  thread does not make  any MPI call and  wait data from
   other threads using mutex.

   @return
   - ! 0 : on error.
   - 0 : else.
*/

static Gnum
dgraphCoarsenThreadConstruct (DgraphCoarsenShared * restrict shared);


/**
   @brief

   This function  is used in a  thread.  It computes  vertices that we
   shall send  to other  processor, this calculation  can done  at the
   time  we  do  the   matching.   Then,  the  thread  waits  matching
   termination  to send and  receive new  vertices number  from tother
   procs and unlock mutexes to allow other threads to use these datas.

   @return
   - ! 0 : on error.
   - 0 : else.
*/

static Gnum
dgraphCoarsenThreadMigrate (DgraphCoarsenShared * restrict shared);

#endif /* DGRAPH_COARSEN_C */
