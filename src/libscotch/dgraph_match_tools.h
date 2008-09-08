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
/**   NAME       : dgraph_match_tools.h                    **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This file contains declarations of      **/
/**                auxiliary functions used by the two     **/
/**                matching algorithms.                    **/
/**                                                        **/
/**   DATES      : # Version 0.5  : from : 08 dec 2006     **/
/**                                 to   : 03 aug 2007     **/
/**                                                        **/
/************************************************************/

#ifndef SCOTCH_DETERMINIST
# define MATCH_ANTICIP
#endif /* SCOTCH_DETERMINIST */

/*
** The types
*/

typedef struct DgraphMatchEdgeQueue_ {
  Gnum *                    head;                 /*+ Head of edge queue      +*/
  Gnum *                    tail;                 /*+ Tail of edge queue      +*/
  Gnum *                    cursor;               /*+ Cursor of edge queue    +*/
} DgraphMatchEdgeQueue;

typedef struct DgraphMatchSort_ {
  Gnum load;
  Gnum vertnum;
} DgraphMatchSort;

/* This structure is used to make conversion between gst and glb indices (both directions) */
typedef struct DgraphMatchConvert_ {
  Gnum *                    gst2glbtab;           /* Tab for converting gst to glb indices */
} DgraphMatchConvert;


#define inline static __inline__

/* inline int */
/* dgraphMatchSyncSortEdgesNotLoad ( */
/* const Gnum * restrict const  sort0, */
/* const Gnum * restrict const  sort1) */
/* { */
/*   return ((*sort0 < *sort1)?1:-1); */
/* } */


inline int
dgraphMatchEdgeInit(DgraphMatchEdgeQueue * queue, Gnum * tab, Gnum size,
                    Gnum * load, DgraphMatchSort * sorttmp, Gnum vertlocnnd)
{
  Gnum * pload, * ptab;
  DgraphMatchSort * psort;
#ifdef MATCH_ANTICIP
  Gnum neighborload = 0;
  Gnum totalload = 0;
#endif /* MATCH_ANTICIP */

  queue->head = queue->cursor = tab;
  queue->tail = tab + size;

  if (size == 0)
    return (0);

  if (load == NULL) {
    /*     intPerm(tab, size); */
#ifdef MATCH_ANTICIP
    for (ptab = queue->head; ptab < queue->tail ; ++ ptab)
      if (*ptab >= vertlocnnd)
        neighborload ++;
    return ((neighborload*100)/size);             /* return chance to be matched by neighbor */
#else /* MATCH_ANTICIP */
    return (0);
#endif /* MATCH_ANTICIP */
  }

  for (ptab = tab, psort = sorttmp, pload = load ; ptab < tab + size ;
       ptab ++, pload ++, psort ++) {
    psort->load = *pload;
    psort->vertnum = *ptab;
  }

  intSort2asc1 ((void *) sorttmp, size);

  for (ptab = tab, psort = sorttmp, pload = load ; ptab < tab + size ;
       ptab ++, pload ++, psort ++) {
    *ptab = psort->vertnum;
#ifdef MATCH_ANTICIP
    totalload += psort->load;
    if (*ptab >= vertlocnnd)
      neighborload += psort->load;
#endif /* MATCH_ANTICIP */
  }
#ifdef MATCH_ANTICIP
  return ((neighborload*100)/totalload);          /* return chance to be matched by neighbor */
#else /* MATCH_ANTICIP */
  return (0);
#endif /* MATCH_ANTICIP */
}

inline Gnum
dgraphMatchEdgeSize(DgraphMatchEdgeQueue * queue)
{
  return (queue->tail - queue->head);
}

inline Gnum
dgraphMatchEdgeGet(DgraphMatchEdgeQueue * queue)
{
#ifdef SCOTCH_DEBUG_DGRAPH2
  if (!dgraphMatchEdgeSize(queue))
    return (-1);
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  return (*(queue->head ++));
}

inline Gnum
dgraphMatchEdgeFlush (DgraphMatchEdgeQueue * queue)
{
  queue->head = queue->cursor;
  return (dgraphMatchEdgeSize(queue));
}

inline void
dgraphMatchEdgeMark (DgraphMatchEdgeQueue * queue)
{
  Gnum vnum;
#ifdef SCOTCH_DEBUG_DGRAPH2
  if (queue->head == queue->cursor)
    return ;
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  vnum = *(queue->cursor);
  *(queue->cursor ++) = *(queue->head - 1);
  *(queue->head - 1) = vnum;
}


int
dgraphMatchConvertInit (const Dgraph * restrict const graph,
                        DgraphMatchConvert * restrict convert);

void
dgraphMatchConvertExit (const Dgraph * restrict const graph,
                        DgraphMatchConvert * restrict convert);

Gnum
dgraphMatchVertGst2Glb (const DgraphMatchConvert * restrict const convert,
                        Gnum                                      gstnum);

#define dgraphMatchVertGst2Glb(convert,gstnum) (convert)->gst2glbtab[(gstnum)]

inline Gnum
dgraphMatchVertGlb2Gst(const Dgraph * restrict const finegraph,
                       Gnum finevertgstnum, Gnum finevertlocnum,
                       const DgraphMatchConvert * restrict const convert)
{
  Gnum edgenum;

  for (edgenum = finegraph->vertloctax[finevertlocnum] ; edgenum < finegraph->vendloctax[finevertlocnum] ;
       ++edgenum) {
    if (finegraph->edgeloctax[edgenum] == finevertgstnum) {
#ifdef SCOTCH_DEBUG_DGRAPH2
      if (finegraph->edgegsttax[edgenum] < finegraph->vertlocnnd) {
        errorPrint ("dgraphMatchVertGlb2Gst : not gst vertex");
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
      return (finegraph->edgegsttax[edgenum]);
    }
  }
#ifdef SCOTCH_DEBUG_DGRAPH2
  errorPrint ("dgraphMatchVertGlb2Gst : vertex not found");
  *(int*)NULL = 0;
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  return (-1);
}

#undef inline
