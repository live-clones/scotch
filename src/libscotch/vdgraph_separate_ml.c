/* Copyright 2007-2009 ENSEIRB, INRIA & CNRS
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
/**   NAME       : vdgraph_separate_ml.c                   **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This module contains the multi-level    **/
/**                separation strategy.                    **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 07 mar 2006     **/
/**                                 to   : 01 mar 2008     **/
/**                # Version 5.1  : from : 14 dec 2008     **/
/**                                 to   : 04 feb 2009     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VDGRAPH_SEPARATE_ML

#include "module.h"
#include "common.h"
#include "parser.h"
#include "dgraph.h"
#include "dgraph_coarsen.h"
#include "vdgraph.h"
#include "vdgraph_separate_ml.h"
#include "vdgraph_separate_st.h"

/*********************************************/
/*                                           */
/* The coarsening and uncoarsening routines. */
/*                                           */
/*********************************************/

/* This routine builds a coarser graph from the
** Dgraph that is given on input. The coarser
** Dgraphs differ at this stage from classical
** active Dgraphs as their internal gains are not
** yet computed.
** It returns:
** - 0  : if the coarse Dgraph has been built.
** - 1  : if threshold achieved or on error.
*/

static
int
vdgraphSeparateMlCoarsen (
Vdgraph * restrict const              finegrafptr,/*+ Finer graph                         +*/
Vdgraph * restrict const              coargrafptr,/*+ Coarser graph to build              +*/
DgraphCoarsenMulti * restrict * const coarmultptr,/*+ Pointer to multinode table to build +*/
const VdgraphSeparateMlParam * const  paraptr)    /*+ Method parameters                   +*/
{
  int dofolddup;

  dofolddup = 1;
  if ((paraptr->duplvlmax > -1) &&                  /* duplvlmax can allow fold dup */
      (paraptr->duplvlmax < finegrafptr->levlnum + 1)) {
      dofolddup = 0;
  }
  else if (paraptr->duplvlmax < -1) {              /* duplvlmax can allow only fold */
    if (-(paraptr->duplvlmax + 1) < finegrafptr->levlnum + 1)
      dofolddup = 0;
    else
      dofolddup = -1;
  }

  if (dgraphCoarsen (&finegrafptr->s, &coargrafptr->s, coarmultptr, paraptr->coarnbr,
                     dofolddup, paraptr->dupmax, paraptr->coarrat) != 0)
    return (1);                                   /* Return if coarsening failed */

  if (coargrafptr->s.procglbnbr == 0) {           /* Not a owner graph */
    coargrafptr->fronloctab = NULL;
    coargrafptr->partgsttax = NULL;
    coargrafptr->compglbsize[2] = -1;             /* Mark frontab as invalid */
    return (0);
  }
  else
    coargrafptr->compglbsize[2] = 0;              /* Mark as non-invalid */

#ifdef SCOTCH_DEBUG_VDGRAPH2
  if (dgraphCheck (&coargrafptr->s) != 0) {
    errorPrint ("vdgraphSeparateMlCoarsen: internal error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

  coargrafptr->partgsttax = NULL;                 /* Do not allocate partition data yet               */
  coargrafptr->levlnum    = finegrafptr->levlnum + 1; /* Graph level is coarsening level              */
  if (coargrafptr->s.vertlocnbr <= finegrafptr->s.vertlocnbr) /* If (folded) coarser graph is smaller */
    coargrafptr->fronloctab = finegrafptr->fronloctab; /* Re-use frontier array for coarser graph     */
  else {                                          /* Else allocate new private frontier array         */
    if ((coargrafptr->fronloctab = memAlloc (coargrafptr->s.vertlocnbr * sizeof (Gnum))) == NULL) {
      errorPrint ("vdgraphSeparateMlCoarsen: out of memory");
      return     (1);
    }
  }

  return (0);
}

/* This routine is the reduction-loc operator which
** returns in inout[2] the rank of the process which
** holds the best partition.
** It returns:
** - void  : in all cases.
*/

static
void
vdgraphSeparateMlOpBest (
const Gnum * const          in,                   /* First operand                               */
Gnum * const                inout,                /* Second and output operand                   */
const int * const           len,                  /* Number of instances ; should be 1, not used */
const MPI_Datatype * const  typedat)              /* MPI datatype ; not used                     */
{
  if (inout[3] == 1) {                            /* Handle cases when at least one of them is erroneous */
    if (in[3] == 1) {
      if (inout[2] > in[2])                       /* To enforce commutativity, always keep smallest process number */
        inout[2] = in[2];
      return;
    }

    inout[0] = in[0];
    inout[1] = in[1];
    inout[2] = in[2];
    inout[3] = in[3];
    return;
  }
  else if (in[3] == 1)
    return;

  if ((in[0] < inout[0]) ||                       /* Select best partition */
      ((in[0] == inout[0]) && ((in[1] < inout[1]) ||
			       ((in[1] == inout[1]) && (in[2] < inout[2]))))) {
    inout[0] = in[0];
    inout[1] = in[1];
    inout[2] = in[2];
  }
}

/* This routine selects the best coarsen partition to
** project to finer graph.
*/

static
int
vdgraphHaveBestPart (
const Vdgraph * restrict const  coargrafptr,
const MPI_Comm                  parentcomm,
const int                       proclocnum)
{
  Gnum              reduloctab[4];                /* Local array for best separator data (7 for Bcast) */
  Gnum              reduglbtab[4];                /* Global array for best separator data              */
  MPI_Datatype      besttypedat;                  /* Data type for finding best separator              */
  MPI_Op            bestoperdat;                  /* Handle of MPI operator for finding best separator */
  int               myassoc;                      /* To know how graphs are folded                     */
  int               bestassoc;


  if (coargrafptr->compglbsize[2] < 0) {          /* Processors own coargrafptr      */
    MPI_Comm_size (parentcomm, &myassoc);         /* Mark higher than other vertices */
    ++myassoc;
    reduloctab[0] = 0;
    reduloctab[1] = 0;                            /* To avoid warning in valgrind */
    reduloctab[2] = myassoc;
    reduloctab[3] = 1;
  }
  else {
    bestassoc = proclocnum;
    if (MPI_Allreduce (&bestassoc, &myassoc, 1, MPI_INT, MPI_MIN, coargrafptr->s.proccomm) != MPI_SUCCESS)  {
      errorPrint ("vdgraphHaveBestPart: communication error (1)");
      return     (1);
    }
    reduloctab[0] = coargrafptr->compglbsize[2];  /* Frontier size  */
    reduloctab[1] = coargrafptr->compglbloaddlt;  /* Load imbalance */
    reduloctab[2] = myassoc;
    reduloctab[3] = (coargrafptr->compglbsize[2] <= 0) ? 1 : 0;
  }

  if ((MPI_Type_contiguous (4, GNUM_MPI, &besttypedat)                                != MPI_SUCCESS) ||
      (MPI_Type_commit (&besttypedat)                                                 != MPI_SUCCESS) ||
      (MPI_Op_create ((MPI_User_function *) vdgraphSeparateMlOpBest, 1, &bestoperdat) != MPI_SUCCESS)) {
    errorPrint ("vdgraphHaveBestPart: communication error (2)");
    return     (1);
  }

  if (MPI_Allreduce (reduloctab, reduglbtab, 1, besttypedat, bestoperdat, parentcomm) != MPI_SUCCESS) {
    errorPrint ("vdgraphHaveBestPart: communication error (3)");
    return     (1);
  }

  bestassoc = (int) reduglbtab[2];
  if ((MPI_Op_free   (&bestoperdat) != MPI_SUCCESS) ||
      (MPI_Type_free (&besttypedat) != MPI_SUCCESS)) {
    errorPrint ("vdgraphHaveBestPart: communication error (4)");
    return     (1);
  }

  return ((myassoc == bestassoc) ? 1 : 0);
}

/* This routine propagates the separation of the
** coarser graph back to the finer graph, according
** to the multinode table of collapsed vertices.
** After the separation is propagated, it finishes
** to compute the parameters of the finer graph that
** were not computed at the coarsening stage.
** It returns:
** - 0   : if coarse graph data has been propagated to fine graph.
** - !0  : on error.
*/

static
int
vdgraphSeparateMlUncoarsen (
Vdgraph * restrict                        finegrafptr, /*+ Finer graph     +*/
const Vdgraph * restrict const            coargrafptr, /*+ Coarser graph   +*/
const DgraphCoarsenMulti * restrict const coarmulttax) /*+ Multinode array +*/
{
  Gnum                    coarvertnum;            /* Number of current coarse vertex           */
  Gnum                    finevertnum;
  Gnum                    finevertadj;            /* Global vertex adjustment                  */
  VdgraphSeparateMlPart * partglbtab;             /* Array of vertices to send to their owner  */
  VdgraphSeparateMlPart * partglbptr;             /* Pointer to current vertex data to send    */
  VdgraphSeparateMlPart * partglbend;             /* End of array */
  MPI_Request           * requests;               /* Array of requests */
  MPI_Request           * requestrcv;             /* Shortcut on receiving requests */
  MPI_Request           * requestsnd;             /* Shortcut on sending requests   */
  MPI_Request           * requestsndsize;         /* Shortcut on sending requests   */
  int                     flag;                   /* Result of MPI_Test */
  int                     i;                      /* Indice for loop */
  int                     procnum;                /* Indice to browse processor neighbors */
  Gnum                    reduloctab[7];          /* Temporary array for reduction */
  Gnum                    reduglbtab[7];          /* Temporary array for reduction */
  Gnum                    cheklocval;
  Gnum * restrict         sendsize;
  Gnum *                  recvsize;               /* [norestrict:async] */
  VdgraphSeparateMlPart * restrict * sendoffset;
#ifdef SCOTCH_DEBUG_VDGRAPH2
  int                     index;
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
  Gnum                    vertsize;
  VdgraphSeparateMlPart * restrict rcvparttab;
  VdgraphSeparateMlPart * currentrcvptr;

  if (finegrafptr->partgsttax == NULL) {          /* If partition array not yet allocated           */
    if (dgraphGhst (&finegrafptr->s) != 0) {      /* Create ghost edge array and compute vertgstnbr */
      errorPrint ("vdgraphSeparateMlUncoarsen: cannot compute ghost edge array");
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
    if ((finegrafptr->partgsttax = (GraphPart *) memAlloc (finegrafptr->s.vertgstnbr * sizeof (GraphPart))) == NULL) {
      errorPrint ("vdgraphSeparateMlUncoarsen: out of memory (1)");
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
    finegrafptr->partgsttax -= finegrafptr->s.baseval;
  }
#ifdef SCOTCH_DEBUG_VDGRAPH2
  memSet (finegrafptr->partgsttax + finegrafptr->s.baseval, 4, finegrafptr->s.vertgstnbr * sizeof (GraphPart));
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

  cheklocval = 0;
  if (coargrafptr != NULL) {                      /* If coarser graph provided         */
    Gnum                finesize1;                /* Number of vertices in fine part 1 */
    int                 sendmode;                 /* To know how make this exchange */

#ifdef SCOTCH_DEBUG_VDGRAPH2
    if ((coargrafptr->s.procglbnbr != 0) && (vdgraphCheck (coargrafptr) != 0)) {
      errorPrint ("vdgraphSeparateMlUncoarsen: inconsistent input graph data");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

#ifdef PTSCOTCH_FOLD_DUP
    if ((coargrafptr->s.procglbnbr == 0) ||
        (coargrafptr->s.proccomm != finegrafptr->s.proccomm)) /* Folding occurs, select best partition */
      sendmode = vdgraphHaveBestPart (coargrafptr, finegrafptr->s.proccomm, finegrafptr->s.proclocnum);
    else
      sendmode = 1;                               /* We can exploit our data */
#else /* PTSCOTCH_FOLD_DUP */
    sendmode = 1;
#endif /* PTSCOTCH_FOLD_DUP */

#ifdef SCOTCH_DEBUG_VDGRAPH2
    if (MPI_Allreduce (&sendmode, &index, 1, MPI_INT, MPI_MAX, finegrafptr->s.proccomm) != MPI_SUCCESS)  {
      errorPrint ("vdgraphSeparateMlUncoarsen: communication error (1)");
      cheklocval = 1;
    }
    if (index == 0) {
      errorPrint ("vdgraphSeparateMlUncoarsen: common error (1)");
      cheklocval = 1;
    }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

    if (memAllocGroup ((void **) (void *)
                       &partglbtab, (size_t) (2 * coargrafptr->s.vertlocnbr * sizeof (VdgraphSeparateMlPart)),
                       &requests,   (size_t) (3 * finegrafptr->s.procglbnbr * sizeof (MPI_Request)), /* Max nbr communications */
                       &sendsize,   (size_t) (finegrafptr->s.procglbnbr * sizeof (Gnum)),
                       &sendoffset, (size_t) ((finegrafptr->s.procglbnbr + 1) * sizeof (VdgraphSeparateMlPart*)),
                       &recvsize,   (size_t) (finegrafptr->s.procglbnbr * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("vdgraphSeparateMlUncoarsen: out of memory (2)");
      cheklocval = 1;
      memFree (finegrafptr->partgsttax + finegrafptr->s.baseval);
      finegrafptr->partgsttax = NULL;
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
    requestrcv     = requests;
    requestsndsize = requestrcv + finegrafptr->s.procglbnbr;
    requestsnd     = requestsndsize + finegrafptr->s.procglbnbr;
    partglbptr     = partglbtab;

    for (procnum = 0; procnum < finegrafptr->s.procglbnbr; procnum ++) { /* Post receive before computing */
      if (procnum == finegrafptr->s.proclocnum) { /* Local communication is not useful                    */
        requestrcv[procnum] = MPI_REQUEST_NULL;
        continue;
      }
      MPI_Irecv (&recvsize[procnum], 1, GNUM_MPI,
                 procnum, TAGMLSIZE, finegrafptr->s.proccomm,
                 &requestrcv[procnum]);
      requestsnd[procnum] = MPI_REQUEST_NULL;     /* Initialize sending */
      requestsndsize[procnum] = MPI_REQUEST_NULL; /* Initialize sending size */
    }

    finevertadj = finegrafptr->s.procvrttab[finegrafptr->s.proclocnum] - finegrafptr->s.baseval;

    /* Initialize fields of finer Vgraph */
    finegrafptr->complocsize[2] = 0;
    finesize1 = 0;
    memSet (finegrafptr->complocsize, 0, 3 * sizeof (Gnum));
    memSet (finegrafptr->complocload, 0, 3 * sizeof (Gnum));

    /* Mark all vertices as unvisited */
    memSet (finegrafptr->partgsttax + finegrafptr->s.baseval, 3, finegrafptr->s.vertgstnbr * sizeof (GraphPart));

    if (sendmode) {                               /* We must browse local data */
      for (coarvertnum = coargrafptr->s.baseval ;
           coarvertnum < coargrafptr->s.vertlocnnd; coarvertnum ++) {
        GraphPart           coarpartval;          /* Value of current multinode part */

        coarpartval = coargrafptr->partgsttax[coarvertnum];
#ifdef SCOTCH_DEBUG_VDGRAPH2
        if (coarpartval > 2) {
          errorPrint ("vdgraphSeparateMlUncoarsen: internal error (1)");
          cheklocval = 1;
        }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

        for (i = 0; i < 2; i ++) {
          finevertnum = coarmulttax[coarvertnum].vertglbnum[i];
          if (dgraphVertexLocal (&finegrafptr->s, finevertnum)) { /* Vertex is a local one */
#ifdef SCOTCH_DEBUG_VDGRAPH2
            if (finegrafptr->partgsttax[finevertnum - finevertadj] != 3) {
              errorPrint ("vdgraphSeparateMlUncoarsen: internal error (2)");
              return (1);
            }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
            finesize1 += (Gnum) (coarpartval & 1); /* One extra vertex created in part 1 if (coarpartval == 1) */
            if (finegrafptr->s.veloloctax != NULL)
              finegrafptr->complocload[coarpartval] += finegrafptr->s.veloloctax[finevertnum - finevertadj];
            finegrafptr->partgsttax[finevertnum - finevertadj] = coarpartval;
            if (coarpartval == 2)
              finegrafptr->fronloctab[finegrafptr->complocsize[2] ++] = finevertnum - finevertadj;
          }
          else {                                  /* Non local vertex */
            partglbptr->vertnum = finevertnum;
            partglbptr->partval = (Gnum) coarpartval;
            partglbptr ++;
          }
          if (coarmulttax[coarvertnum].vertglbnum[0] == coarmulttax[coarvertnum].vertglbnum[1]) /* In fact no contraction */
            break;
        }
      }
    }
    /* Local step done, now we do the same with other vertices */
    partglbend = partglbptr;
    if (partglbend != partglbtab)
      intSort2asc1 ((void *) partglbtab, partglbend - partglbtab);

    /* Send and receive vertices */
    for (partglbptr = partglbtab, procnum = 0, sendoffset[0] =  partglbtab ;
         (partglbptr <= partglbend) && (procnum < finegrafptr->s.procglbnbr) ; ++ partglbptr) {

      while ((procnum < finegrafptr->s.procglbnbr)
             && ((partglbptr == partglbend)       /* Last neighbors */
                 || (partglbptr->vertnum >= finegrafptr->s.procvrttab[procnum + 1]))) {
        /* We must change neighbor, thus the current is ready */
        sendoffset[procnum + 1] = partglbptr;
        sendsize[procnum]       = partglbptr - sendoffset[procnum];

        if (procnum == finegrafptr->s.proclocnum) {               /* local communication is not useful */
#ifdef SCOTCH_DEBUG_VDGRAPH2
          if (sendsize[procnum] != 0) {
            errorPrint ("vdgraphSeparateMlUncoarsen: internal error (3)");
            cheklocval = 1;
          }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
          requestsndsize[procnum] = MPI_REQUEST_NULL;
          recvsize[procnum] = 0;
        }
        else {
          MPI_Isend (&sendsize[procnum], 1, COMM_INT, procnum, TAGMLSIZE,
                     finegrafptr->s.proccomm, &requestsndsize[procnum]);
        }

        if (sendsize[procnum] > 0) {
          MPI_Isend (sendoffset[procnum], 2 * sendsize[procnum], GNUM_MPI, procnum,
                     TAGMLVERT, finegrafptr->s.proccomm, &requestsnd[procnum]);
        }
        else {
#ifdef SCOTCH_DEBUG_VDGRAPH2
          if (sendsize[procnum] < 0) {
            errorPrint ("vdgraphSeparateMlUncoarsen: internal error (4)");
            cheklocval = 1;
          }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
          requestsnd[procnum] = MPI_REQUEST_NULL;
	}

        procnum ++;
      }
      if (partglbptr == partglbend)               /* We are already on the last vertex */
        partglbptr --;
    }

    if (MPI_Waitall (finegrafptr->s.procglbnbr * 2, requests, MPI_STATUSES_IGNORE) != MPI_SUCCESS) { /* Wait for needed datas */
      cheklocval = 1;
      errorPrint ("vdgraphSeparateMlUncoarsen: communication error (1)");
    }

    for (procnum = 0, vertsize = 0; procnum < finegrafptr->s.procglbnbr; procnum ++) /* Compute size of data to receive */
      vertsize += recvsize[procnum];

    if ((rcvparttab = memAlloc (vertsize * sizeof (VdgraphSeparateMlPart))) == NULL) {
      cheklocval = 1;
      errorPrint ("vdgraphSeparateMlUncoarsen: out of memory (2)");
    }

    currentrcvptr = rcvparttab;
    for (procnum = 0 ; procnum < finegrafptr->s.procglbnbr ; ++ procnum) { /* Post that reception is ready */
      if (recvsize[procnum] > 0)
        MPI_Irecv (currentrcvptr, 2 * recvsize[procnum], GNUM_MPI,
                   procnum, TAGMLVERT, finegrafptr->s.proccomm,
                   &requestrcv[procnum]);
      else {
#ifdef SCOTCH_DEBUG_VDGRAPH2
        if (recvsize[procnum] < 0) {
          errorPrint ("vdgraphSeparateMlUncoarsen: internal error (5)");
          cheklocval = 1;
        }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
        requestrcv[procnum] = MPI_REQUEST_NULL;
      }
      currentrcvptr += recvsize[procnum];
    }

    if (MPI_Waitall (finegrafptr->s.procglbnbr, requestrcv, MPI_STATUSES_IGNORE) != MPI_SUCCESS) { /* Wait for vertices */
      cheklocval = 1;
      errorPrint ("vdgraphSeparateMlUncoarsen: communication error (2)");
    }

    for (partglbptr = rcvparttab, partglbend = rcvparttab + vertsize; /* Browse new vertices */
         partglbptr < partglbend; partglbptr ++) {
#ifdef SCOTCH_DEBUG_VDGRAPH2
      if ((partglbptr->partval > 2) || (! dgraphVertexLocal (&finegrafptr->s, partglbptr->vertnum))) {
        fprintf (stderr, "partval %ld vertnum %ld\n", (long) partglbptr->partval, (long) partglbptr->vertnum);
        errorPrint ("vdgraphSeparateMlUncoarsen: internal error (6)");
        cheklocval = 1;
      }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
      if (finegrafptr->partgsttax[partglbptr->vertnum - finevertadj] != 3) { /* TODO : verify why useful */
#ifdef SCOTCH_DEBUG_VDGRAPH2
        if (sendmode) {
          for (procnum = coargrafptr->s.baseval ; procnum < coargrafptr->s.vertlocnnd ; ++ procnum) {
            if ((coarmulttax[procnum].vertglbnum[0] == partglbptr->vertnum) ||
                (coarmulttax[procnum].vertglbnum[1] == partglbptr->vertnum))
              break;
          }
        }
        *(int*)NULL = 0;
/*         if (finegrafptr->partgsttax[partglbptr->vertnum - finevertadj] != partglbptr->partval) { */
        errorPrint ("vdgraphSeparateMlUncoarsen: internal error (7)");
        cheklocval = 1;
        return (1);
/*         } */
#endif /* SCOTCH_DEBUG_VDGRAPH2 */
        continue;
      }
      finegrafptr->partgsttax[partglbptr->vertnum - finevertadj] = (GraphPart) partglbptr->partval;
      finesize1 += (partglbptr->partval & 1);     /* One extra vertex created in part 1 if (coarpartval == 1) */
      if (finegrafptr->s.veloloctax != NULL)
        finegrafptr->complocload[partglbptr->partval] +=
          finegrafptr->s.veloloctax[partglbptr->vertnum - finevertadj];
      if (partglbptr->partval == 2)
        finegrafptr->fronloctab[finegrafptr->complocsize[2] ++] = partglbptr->vertnum - finevertadj;
    }

#ifdef SCOTCH_DEBUG_VDGRAPH2
    /* Verify that the partgsttax is fully completed for local vertices */
    for (finevertnum = finegrafptr->s.baseval ; finevertnum < finegrafptr->s.vertlocnnd ; ++ finevertnum) {
      if (finegrafptr->partgsttax[finevertnum] > 2) {
        errorPrint ("vdgraphSeparateMlUncoarsen: internal error (8)");
        cheklocval = 1;
        break;
      }
    }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

    memFree (rcvparttab);

    /* All local finegraph vertices have been marked in one part */

    if (MPI_Waitall (finegrafptr->s.procglbnbr, requestsnd, MPI_STATUSES_IGNORE) != MPI_SUCCESS) { /* Wait for needed datas */
      cheklocval = 1;
      errorPrint ("vdgraphSeparateMlUncoarsen: communication error (3)");
    }

    /* Now compute Vdgraph fields */
    finegrafptr->complocsize[0] = finegrafptr->s.vertlocnbr - finegrafptr->complocsize[2] - finesize1;
    finegrafptr->complocsize[1] = finesize1;

    if (finegrafptr->s.veloloctax == NULL)
      memCpy (finegrafptr->complocload, finegrafptr->complocsize, 3 * sizeof(Gnum));

    reduloctab[0] = cheklocval;
    memCpy (reduloctab + 1, finegrafptr->complocload, 3 * sizeof (Gnum));
    memCpy (reduloctab + 4, finegrafptr->complocsize, 3 * sizeof (Gnum));
    MPI_Allreduce (reduloctab, reduglbtab, 7, GNUM_MPI, MPI_SUM, finegrafptr->s.proccomm);
    if (reduglbtab[0] > 0) {
      errorPrint ("vdgraphSeparateMlUncoarsen: common error (1)");
      return (1);
    }
    memCpy (finegrafptr->compglbload, reduglbtab + 1, 3 * sizeof (Gnum));
    memCpy (finegrafptr->compglbsize, reduglbtab + 4, 3 * sizeof (Gnum));
    finegrafptr->compglbloaddlt = finegrafptr->compglbload[0] - finegrafptr->compglbload[1];

    memFree (partglbtab);                         /* Free memory group leader */
    if (coargrafptr->s.procglbnbr != 0)
      memFree ((void *) (coarmulttax + coargrafptr->s.baseval));
  }
  else                                            /* No coarse graph provided      */
    vdgraphZero (finegrafptr);                    /* Assign all vertices to part 0 */

#ifdef SCOTCH_DEBUG_VDGRAPH2
  if (vdgraphCheck (finegrafptr) != 0) {
    errorPrint ("vdgraphSeparateMlUncoarsen: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_VDGRAPH2 */

  return (0);
}

/* This routine recursively performs the
** separation recursion.
** It returns:
** - 0   : if separator could be computed.
** - !0  : on error.
*/

static
int
vdgraphSeparateMl2 (
Vdgraph * restrict const             grafptr,     /* Vertex-separation graph */
const VdgraphSeparateMlParam * const paraptr)     /* Method parameters       */
{
  Vdgraph                       coargrafdat;
  DgraphCoarsenMulti * restrict coarmulttax;
  int                           o;

  if (grafptr->s.procglbnbr <= paraptr->seqnbr) { /* We must enter in another mode         */
    if (((o = vdgraphSeparateMlUncoarsen (grafptr, NULL, NULL)) == 0) && /* Finalize graph */
        ((o = vdgraphSeparateSt (grafptr, paraptr->stratseq)) != 0))
      errorPrint ("vdgraphSeparateMl2: cannot apply seq strategy");
    return (o);
  }

  if (vdgraphSeparateMlCoarsen (grafptr, &coargrafdat, &coarmulttax, paraptr) == 0) {
    if (coargrafdat.compglbsize[2] == -1)         /* Mark frontab as invalid */
      o = 0;
    else
      o = vdgraphSeparateMl2 (&coargrafdat, paraptr);
    if ((o == 0) &&
        ((o = vdgraphSeparateMlUncoarsen (grafptr, &coargrafdat, coarmulttax)) == 0) &&
        ((o = vdgraphSeparateSt          (grafptr, paraptr->stratasc))         != 0)) /* Apply ascending strategy */
      errorPrint ("vdgraphSeparateMl2: cannot apply ascending strategy");
    if (coargrafdat.fronloctab == grafptr->fronloctab) /* If coarse graph shares fronloctab with fine graph */
      coargrafdat.fronloctab = NULL;              /* Prevent fronloctab of fine graph from being freed      */
    vdgraphExit (&coargrafdat);
  }
  else {                                          /* Cannot coarsen due to lack of memory or error  */
    if (((o = vdgraphSeparateMlUncoarsen (grafptr, NULL, NULL)) == 0) && /* Finalize graph          */
        ((o = vdgraphSeparateSt          (grafptr, paraptr->stratlow)) != 0)) /* Apply low strategy */
      errorPrint ("vdgraphSeparateMl2: cannot apply low strategy");
  }

  return (o);
}

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the muti-level separation.
** It returns:
** - 0 : if separator could be computed.
** - 1 : on error.
*/

int
vdgraphSeparateMl (
Vdgraph * const                       grafptr,    /*+ Vertex-separation graph +*/
const VdgraphSeparateMlParam * const  paraptr)    /*+ Method parameters       +*/
{
  Gnum                levlnum;                    /* Save value for graph level */
  int                 o;

  levlnum = grafptr->levlnum;                     /* Save graph level               */
  grafptr->levlnum = 0;                           /* Initialize coarsening level    */
  o = vdgraphSeparateMl2 (grafptr, paraptr);      /* Perform multi-level separation */
  grafptr->levlnum = levlnum;                     /* Restore graph level            */

  return (o);
}
