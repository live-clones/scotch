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
/**   NAME       : dgraph_match_sync.h                     **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This file contains declarations of      **/
/**                matching algorithm for multi-level.     **/
/**                The implementation uses an synchrone    **/
/**                way with one thread.                    **/
/**                                                        **/
/**   DATES      : # Version 0.5  : from : 14 sep 2006     **/
/**                                 to   : 08 sep 2008     **/
/**                                                        **/
/************************************************************/

/*
** The types
*/

typedef struct DgraphMatchSyncQueue_ {
  Gnum *                    head;                 /*+ Head of distance queue  +*/
  Gnum *                    tail;                 /*+ Tail of distance queue  +*/
  Gnum *                    qtab;                 /*+ Array of queue elements +*/
  Gnum *                    qend;                 /*+ Mark end of buffer      +*/
  Gnum *                    oldtail;              /*+ Oldtail for restarting  +*/
  Gnum                      size;                 /*+ Number of elements      +*/
} DgraphMatchSyncQueue;

/* This structure contains infos for a distant matching */
typedef struct DgraphMatchSyncMessage_ {
  Gnum                      finevertdstnum;       /*+ Global indice of destination vertex                         +*/
  Gnum                      finevertsrcnum;       /*+ Global indice of source vertex                              +*/
  union {
    Gnum                    coarvertnum;          /*+ Global indice of matched vertex                             +*/
    Gnum                    degree;               /*+ Number of edges to exchange in case of successfull matching +*/
  } data;
} DgraphMatchSyncMessage;

/* This structure contains datas to allow exchange of ghost vertices infos */
typedef struct DgraphMatchSyncCommProc_ {
  DgraphMatchSyncMessage *      begin;            /*+ Pointer on first data to exchange +*/
  DgraphMatchSyncMessage *      end;              /*+ Pointer on last data to exchange  +*/
  int                           maxsize;
} DgraphMatchSyncCommProc;


typedef enum {LUBY_SYNC=314,                      /*+ Exchange using Luby coloration of processors +*/
              SIMPLE_SYNC=42,                     /*+ Exchande using a ParMetis like All2All comm  +*/
              PREPAR_SYNC,                        /*+ Used to initialize luby, in order to mix strat +*/
              LUBY_SIMPLE_SYNC,                   /*+ Used to initialize luby, in order to mix strat +*/
} DgraphMatchSyncExchangeMethod;

/* This structure is used to call various synchronisation method */
typedef struct DgraphMatchSyncComm_ {
  DgraphMatchSyncExchangeMethod      method;
  int                     * restrict colortab;    /*+ Used in Luby Processor Coloration +*/
  int                                mycolor;     /*+ Used in Luby Processor Coloration +*/
  int                                nbrcolor;
  int                                begcolor;
  MPI_Request             * restrict requesttab;  /*+ Array of requests +*/
  MPI_Datatype                       datatype;    /*+ Datatype for exchanges +*/
  DgraphMatchSyncCommProc * restrict comsndtab;
  DgraphMatchSyncCommProc * restrict comrcvtab;
  DgraphMatchConvert                 convertstruct;/*+ To convert between ghst et local nums +*/
  DgraphCoarsenMulti      * restrict coarmultgsttab;/*+ Tab used for inter-proc prematching +*/
  Gnum                               coarmultgstnum;/*+ Indice in previous table +*/
  Gnum                  * restrict * verttosndtab;/*+ Indirections table to keep migrant vertices +*/
  DgraphMatchSyncMessage          ** proccoarmulttab;
} DgraphMatchSyncComm;

/**********************************************************
 *                                                        *
 * Auxiliary functions.                                   *
 *                                                        *
 *********************************************************/

int
dgraphMatchSyncDoMatching (
const Dgraph * restrict const finegraph,     /* Graph to match                               */
DgraphMatchParam   * restrict shared,     /* Shared tables concerning the vertices states */
int                           coarvertmax);  /* Maximum number of matching pairs             */


int
dgraphMatchSyncCommInit (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchParam    * restrict shared,
DgraphMatchSyncComm * restrict commptr);

void
dgraphMatchSyncCommExit (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr);

int
dgraphMatchSyncCommDo (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
Gnum                         * nonmatenbr,
DgraphMatchSyncQueue * restrict nonmatequeue);

/* A ParMetis Like exchange function */
int
dgraphMatchSyncSimpleExchange (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
Gnum                         *  nonmatenbr,
DgraphMatchSyncQueue * restrict nonmatequeue);

int
dgraphMatchSyncLubyExchange (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
Gnum                         *  nonmatenbr,
DgraphMatchSyncQueue * restrict nonmatequeue);

int
dgraphMatchSyncBalance (
const Dgraph * restrict const  finegraph,     /* Graph to match                               */
DgraphMatchSyncComm * restrict commptr,
DgraphMatchParam    * restrict shared,
int                            nonmatenbr);


int dgraphMatchLubyColor (
  const Dgraph * restrict const dgrafptr,
  int *                         mycolor,
  int * restrict                colors,
  int *                         nbrcolor);


#define inline static __inline__

inline void
dgraphMatchSyncQueueFlush (DgraphMatchSyncQueue * queue)
{
  queue->head    = queue->tail = queue->qtab;
  queue->oldtail = queue->tail;
  queue->size    = 0;
}

inline void
dgraphMatchSyncQueueInit (DgraphMatchSyncQueue * queue, Gnum size)
{
  queue->qend = queue->qtab + size;
  dgraphMatchSyncQueueFlush (queue);
}

inline int
dgraphMatchSyncQueueEmpty (DgraphMatchSyncQueue * queue)
{
  return (queue->size == 0);
}

inline int
dgraphMatchSyncQueueReady (DgraphMatchSyncQueue * queue)
{
  return ((!dgraphMatchSyncQueueEmpty (queue)) &&(queue->head != queue->oldtail));
}

inline void
dgraphMatchSyncQueueSetReady (DgraphMatchSyncQueue * queue)
{
  queue->oldtail = queue->tail;
}

inline void
dgraphMatchSyncQueuePut (DgraphMatchSyncQueue * queue , Gnum vnum)
{
#ifdef SCOTCH_DEBUG_DGRAPH2
  if (queue->qtab + queue->size >= queue->qend) {
    errorPrint ("dgraphMatchSyncQueuePut : queue too small\n");
    return;
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  *(queue->tail) = vnum;
  if (queue->tail == queue->qend - 1)
    queue->tail = queue->qtab;
  else
    queue->tail ++;
  queue->size ++;
}

inline Gnum
dgraphMatchSyncQueueGet (DgraphMatchSyncQueue * queue)
{
  Gnum vnum;

#ifdef SCOTCH_DEBUG_DGRAPH2
  if (queue->size == 0) {
    errorPrint ("dgraphMatchSyncQueueGet : queue has no element small\n");
    return (-1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  vnum = *(queue->head);
  if (queue->head == queue->qend - 1)
    queue->head = queue->qtab;
  else
    queue->head ++;
  queue->size --;

  return (vnum);
}

inline Gnum
dgraphMatchSyncQueueSize (DgraphMatchSyncQueue * queue)
{
  return (queue->size);
}

#undef inline
