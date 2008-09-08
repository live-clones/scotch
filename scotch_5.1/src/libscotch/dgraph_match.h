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
/**   NAME       : dgraph_match.h                          **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This file provides the coarsening phase **/
/**                of the multi-level method.              **/
/**                The implementation uses several         **/
/**                processes, which could have several     **/
/**                threads each (2 at this moment).        **/
/**                                                        **/
/**   DATES      : # Version 0.5  : from : 04 may 2006     **/
/**                                 to   : 21 jun 2007     **/
/**                                                        **/
/************************************************************/

/*
** Notes :
** This module, mixing MPI and threads, needs a thread safe implementation of
** MPI, and works better if the implementation supports MPI_THREAD_MULTIPLE
** (such as in mpich2 and announced in OpenMPI).
*/


/*
** The defines
*/

#define VERT_LOCKED               -2
#define VERT_MATCHED              -1              /* Negative to be different with a coarmultnum */
#define FREE                       1
#define NOTFREE                    0

#define LOOPNUMBER                 -1             /* Number of matching attempts. Set to -1      */
#define PROGRESS_VAL               2              /* Number of loops with no progress allowed */
#define MINSPEED                   0.1            /* if speed of matching < MINSPEED, loop is considered as stalled */
#define LOOPLIMIT                  15
#define LOOPRAND                   4
#define LOCKLIMIT                  1000


/*
** The types
*/

typedef enum DgraphMatchTag_ {
  TAGCOARMATCH = 42,                              /* used for matching request           */
  TAGCOAREND,                                     /* used to end the receiving thread    */
  TAGCOARKILL,                                    /* used to kill the receiving thread   */
  TAGCOARCOM,                                     /* used to allow communications        */
  TAGCOARINIT,                                    /* used every time else                */
  TAGCOARQUEUE,
  TAGCOARLOCAL,                                   /* used for local communications       */
  TAGCOARINFO,                                    /* used to communicate data requests   */
  TAGCOARANSWER,                                  /* used to answer at matching requests */
  TAGCOARNEG,                                     /* negative answer                     */
  TAGCOARPOS,                                     /* positive answer                     */
  TAGCOARASK,                                     /* matching request                    */
  TAGCOARDEBUG                                    /* used to check matching              */
} DgraphMatchTag;

/* This structure contains all the tables describing the matching state */
typedef struct DgraphMatchParam_ {
  Gnum                    * finecoartax;   /* State table for local and ghost vertices */
  int                     * finefoldtax;   /* Table containing the owner process for each ghost vertice */
  DgraphCoarsenMulti      * coarmulttax;   /* Table containing the matched vertices */
  Gnum                      coarmultnum;   /* Number of coar-vertices, useful for coarsening routine */
  unsigned int              reqsize;       /* Maximum size of MPI requests */
  int                     * vertsndgsttab; /* Number of ghost vertices to be sent          */
  int                     * edgesndgsttab; /* Number of ghost vertices to be received      */
  int                     * vertrcvgsttab; /* Number of ghost vertices' edges to be sent   */
  int                     * edgercvgsttab; /* Number of ghost vertices' edges to be sent   */
  Gnum                    * verttosndtab;  /* Indices of vertices to exchange */
} DgraphMatchParam;


/**********************************************************
 *                                                        *
 * Main functions.                                        *
 *                                                        *
 *********************************************************/


/*
**   This routine is an asynchronous matching routine
**
**   graph       pointer to the current dgraph.
**   shared      pointer to datas which are shared between threads.
**   coarvertmax maximum number of matching pairs (must be lower than vertnbr/2)
**
**   It returns :
**     - 0  if no error
**     - !0 else
*/
int dgraphMatchAsync (const Dgraph * restrict const, DgraphMatchParam *, int);

/*
**   This routine is a synchronous matching routine
**
**   graph       pointer to the current dgraph.
**   shared      pointer to datas which are shared between threads.
**   coarvertmax maximum number of matching pairs (must be lower than vertnbr/2)
**
**   It returns :
**     - 0  if no error
**     - !0 else
*/
int dgraphMatchSync (const Dgraph * restrict const, DgraphMatchParam *, int);

/*
** Verify if matching seems to be correct
** It returns :
**   - 0 in case of success
**   - !0 if an error occurs
*/
int dgraphMatchCheck (const Dgraph * restrict const, const DgraphMatchParam * restrict const);
