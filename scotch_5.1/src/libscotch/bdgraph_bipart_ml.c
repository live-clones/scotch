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
/**   NAME       : bdgraph_bipart_ml.c                     **/
/**                                                        **/
/**   AUTHOR     : Jun-Ho HER                              **/
/**                Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module bipartitions a distributed  **/
/**                graph using a multi-level scheme.       **/
/**                                                        **/
/**   DATES      : # Version 5.1  : from : 30 oct 2007     **/
/**                                 to   : 22 jul 2008     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BDGRAPH_BIPART_ML

#include "module.h"
#include "common.h"
#include "parser.h"
#include "arch.h"
#include "dgraph.h"
#include "dgraph_coarsen.h"
#include "bdgraph.h"
#include "bdgraph_bipart_ml.h"
#include "bdgraph_bipart_st.h"

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
** - 1  : if threshold reached or on error.
*/

static
int
bdgraphBipartMlCoarsen (
Bdgraph * restrict const              finegrafptr, /*+ Finer graph                         +*/
Bdgraph * restrict const              coargrafptr, /*+ Coarser graph to build              +*/
DgraphCoarsenMulti * restrict * const coarmultptr, /*+ Pointer to multinode table to build +*/
const BdgraphBipartMlParam * const    paraptr)    /*+ Method parameters                    +*/
{
  int dofolddup;

  dofolddup = 1;
  if ((paraptr->duplvlmax > -1) &&                  /* duplvlmax can allow fold dup */
      (paraptr->duplvlmax < finegrafptr->levlnum + 1)) 
      dofolddup = 0;
  
  else if (paraptr->duplvlmax < -1) {               /* duplvlmax can allow only fold */
    if (-(paraptr->duplvlmax + 1) < finegrafptr->levlnum + 1)
      dofolddup = 0;
    else
      dofolddup = -1;
  }

  if (dgraphCoarsen (&finegrafptr->s, &coargrafptr->s, coarmultptr, paraptr->coarnbr,
                     dofolddup, paraptr->dupmax * finegrafptr->s.procglbnbr, paraptr->coarrat, paraptr->reqsize) != 0)
    return (1);                                   /* Return if coarsening failed */

  if (coargrafptr->s.procglbnbr == 0) {           /* Not owner of graph */
    coargrafptr->fronloctab = NULL;
    coargrafptr->partgsttax = NULL;
    coargrafptr->fronglbnbr = -1;                 /* Mark frontab as invalid */
    return (0);
  }
  else
    coargrafptr->fronglbnbr = 0;                  /* Mark frontab as valid */

#ifdef SCOTCH_DEBUG_BDGRAPH2
  if (dgraphCheck (&coargrafptr->s) != 0) {
    errorPrint ("bdgraphBipartMlCoarsen: internal error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
  if (finegrafptr->veexloctax != NULL) {          /* Merge external gains for coarsened vertices */
    DgraphCoarsenMulti * restrict coarmulttax;
    Gnum * restrict               coarveexloctax;
    Gnum                          coarvertnum;

    if ((coarveexloctax = (Gnum *) memAlloc (coargrafptr->s.vertlocnbr * sizeof (Gnum))) == NULL) {
      errorPrint ("bdgraphBipartMlCoarsen: out of memory");
      dgraphExit (&coargrafptr->s);               /* Only free Dgraph since veexloctax not allocated */
      return     (1);
    }
    coarveexloctax -= coargrafptr->s.baseval;
    coargrafptr->veexloctax = coarveexloctax;
    coarmulttax = *coarmultptr;

    for (coarvertnum = coargrafptr->s.baseval; coarvertnum < coargrafptr->s.vertlocnnd; coarvertnum++) {
      Gnum		finevertnum0;             /* First multinode vertex  */
      Gnum              finevertnum1;             /* Second multinode vertex */

      finevertnum0 = coarmulttax[coarvertnum].vertnum[0];
      finevertnum1 = coarmulttax[coarvertnum].vertnum[1];
      coarveexloctax[coarvertnum] = (finevertnum0 != finevertnum1)
                                    ? finegrafptr->veexloctax[finevertnum0] + finegrafptr->veexloctax[finevertnum1] 
                                    : finegrafptr->veexloctax[finevertnum0];
    }
  }
  else                                            /* If fine graph does not have external gains */
    coargrafptr->veexloctax = NULL;               /* Coarse graph does not have external gains  */

  coargrafptr->partgsttax       = NULL;           /* Do not allocate partition data yet */
  coargrafptr->fronloctab       = NULL;
  coargrafptr->compglbload0avg  = finegrafptr->compglbload0avg; /* Only set constant partition parameters as others will be set on uncoarsening */
  coargrafptr->commglbloadextn0 = finegrafptr->commglbloadextn0;
  coargrafptr->commglbgainextn0 = finegrafptr->commglbgainextn0;
  coargrafptr->domdist          = finegrafptr->domdist;
  coargrafptr->domwght[0]       = finegrafptr->domwght[0];
  coargrafptr->domwght[1]       = finegrafptr->domwght[1];
  coargrafptr->levlnum          = finegrafptr->levlnum + 1;

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
bdgraphBipartMlOpBest (
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
bdgraphBipartMlBest (
const Bdgraph * restrict const  coargrafptr,
const MPI_Comm                  parentcomm,
const int                       proclocnum)
{
  Gnum              reduloctab[4];                /* Local array for best bipartition data (7 for Bcast) */
  Gnum              reduglbtab[4];                /* Global array for best bipartition data              */
  MPI_Datatype      besttypedat;                  /* Data type for finding best bipartition              */
  MPI_Op            bestoperdat;                  /* Handle of MPI operator for finding best bipartition */
  int               myassoc;                      /* To know how graphs are folded                       */
  int               bestassoc;


  if (coargrafptr->fronglbnbr < 0) {              /* Processors own coargrafptr      */
    MPI_Comm_size (parentcomm, &myassoc);         /* Mark higher than other vertices */
    ++myassoc;
    reduloctab[0] = 0;
    reduloctab[1] = 0;                            /* To avoid warning in valgrind    */
    reduloctab[2] = myassoc;
    reduloctab[3] = 1;
  }
  else {
    bestassoc = proclocnum;
    if (MPI_Allreduce (&bestassoc, &myassoc, 1, MPI_INT, MPI_MIN, coargrafptr->s.proccomm) != MPI_SUCCESS)  {
      errorPrint ("bdgraphBipartMlBest: communication error (1)");
      return     (-1);
    }
    reduloctab[0] = coargrafptr->commglbload;     /* Communication load */
    reduloctab[1] = coargrafptr->compglbload0dlt; /* Load imbalance              */
    reduloctab[2] = myassoc;
    reduloctab[3] = (coargrafptr->fronglbnbr <= 0) ? 1 : 0;
  }

  if ((MPI_Type_contiguous (4, GNUM_MPI, &besttypedat)                              != MPI_SUCCESS) ||
      (MPI_Type_commit (&besttypedat)                                               != MPI_SUCCESS) ||
      (MPI_Op_create ((MPI_User_function *) bdgraphBipartMlOpBest, 1, &bestoperdat) != MPI_SUCCESS)) {
    errorPrint ("bdgraphBipartMlBest: communication error (2)");
    return     (-1);
  }

  if (MPI_Allreduce (reduloctab, reduglbtab, 1, besttypedat, bestoperdat, parentcomm) != MPI_SUCCESS) {
    errorPrint ("bdgraphBipartMlBest: communication error (3)");
    return     (-1);
  }

  bestassoc = (int) reduglbtab[2];
  if ((MPI_Op_free   (&bestoperdat) != MPI_SUCCESS) ||
      (MPI_Type_free (&besttypedat) != MPI_SUCCESS)) {
    errorPrint ("bdgraphBipartMlBest: communication error (4)");
    return     (-1);
  }

  return ((myassoc == bestassoc) ? 1 : 0);
}

/* This routine propagates the bipartitioning of the
** coarser graph back to the finer graph, according
** to the multinode table of collapsed vertices.
** After the bipartitioning is propagated, it finishes
** to compute the parameters of the finer graph that
** were not computed at the coarsening stage.
** It returns:
** - 0   : if coarse graph data has been propagated to fine graph.
** - !0  : on error.
*/

static
int
bdgraphBipartMlUncoarsen (
Bdgraph * restrict                        finegrafptr, /*+ Finer graph     +*/
const Bdgraph * restrict const            coargrafptr, /*+ Coarser graph   +*/
const DgraphCoarsenMulti * restrict const coarmulttax) /*+ Multinode array +*/
{
  int                     i;                      /* Index for loop                            */
  int                     low;                    /* For binary search                         */
  int                     high;                   /* For binary search                         */
  int                     procnum;                /* Index to browse processor neighbors       */
  int                     fronidxtmp;             /* Temporary index for frontiers in buffers  */
  int                     partidxtmp;             /* Temporary index for vertices in buffers   */
  int * restrict          partnbr1;               /* Array to store current sizes of Part 1    */
  int * restrict          senddsptab;
  int * restrict          sendcnttab;
  int * restrict          recvdsptab;
  int * restrict          recvcnttab;
  int * restrict          partsndtab;
  int * restrict          partrcvtab;
  Gnum                    coarvertnum;            /* Number of current coarse vertex           */
  Gnum                    finevertnum;            /* Number of first multinode  vertex         */
  Gnum                    finevertnum1;           /* Number of second multinode vertex         */
  Gnum                    glbfronnbr;             /* Number of frontiers to be sent            */ 
  Gnum                    partglbnbr;             /* Number of vertices to be sent             */ 
  Gnum                    finevertadj;            /* Global vertex adjustment                  */
  Gnum                    coarfronnum;            /* Index for frontier array of coarse graph  */
  Gnum                    finefronnum;            /* Index for frontier array of fine graph    */
  Gnum                    fronlocnum;             /* Index for frontier array fronloctab       */
  Gnum                    partval;                /* Part of current vertex                    */
  Gnum                    vertnum;                /* Current vertex                            */
  Gnum                    veloval;                /* External load of current vertex           */
  Gnum                    veexval;                /* External gain of current vertex           */ 
  Gnum                    edlolocval;
  Gnum                    commlocloadintn;
  Gnum                    commlocloadextn;
  Gnum                    commlocgainextn;
  Gnum                    reduloctab[8];          /* Temporary array for reduction */
  Gnum                    reduglbtab[8];          /* Temporary array for reduction */
  Gnum                    cheklocval;
  Gnum                    sendsize;               /* Size of partsndtab */
  Gnum                    recvsize;               /* Size of partrcvtab */
  Gnum                  * fronlocptr;             /* Pointer to current frontier vertex        */
  Gnum                  * fronlocend;             /* End of array */
  Gnum * restrict         fronloctab;             /* Temporary array to store additional frontiers  */
  Gnum * restrict         frontmptab;             /* Temporary array to update frontiers  */
  GraphPart               coarpartval;            /* Value of current multinode part */
  BdgraphBipartMlPart   * partglbptr;             /* Pointer to current vertex data to send    */
  BdgraphBipartMlPart   * partglbend;             /* End of array */
  BdgraphBipartMlPart * restrict partglbtab;      /* Array of vertices to send to their owner  */
#ifdef SCOTCH_DEBUG_BDGRAPH2
  int                     index;
#endif /* SCOTCH_DEBUG_BDGRAPH2 */

  if (finegrafptr->partgsttax == NULL) {          /* If partition array not yet allocated           */
    if (dgraphGhst (&finegrafptr->s) != 0) {      /* Create ghost edge array and compute vertgstnbr */
      errorPrint ("bdgraphBipartMlUncoarsen: cannot compute ghost edge array");
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
    if ((finegrafptr->partgsttax = (GraphPart *) memAlloc (finegrafptr->s.vertgstnbr * sizeof (GraphPart))) == NULL) {
      errorPrint ("bdgraphBipartMlUncoarsen: out of memory (1)");
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
    finegrafptr->partgsttax -= finegrafptr->s.baseval;
    if ((finegrafptr->fronloctab = (Gnum *) memAlloc (finegrafptr->s.vertlocnbr * sizeof (Gnum))) == NULL) {
      errorPrint ("bdgraphBipartMlUncoarsen: out of memory (2)");
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
  }
#ifdef SCOTCH_DEBUG_BDGRAPH2
  memSet (finegrafptr->partgsttax + finegrafptr->s.baseval, 3, finegrafptr->s.vertgstnbr * sizeof (GraphPart));
#endif /* SCOTCH_DEBUG_BDGRAPH2 */

  cheklocval = 0;
  if (coargrafptr != NULL) {                      /* If coarser graph provided                */
    Gnum                finesize1;                /* Number of vertices in fine part 1        */
    Gnum                fineload1;                /* Temporary data for complocload of part 1 */
    int                 sendmode;                 /* To know how make this exchange           */

#ifdef SCOTCH_DEBUG_BDGRAPH2
    if ((coargrafptr->s.procglbnbr != 0) && (bdgraphCheck (coargrafptr) != 0)) {
      errorPrint ("bdgraphBipartMlUncoarsen: inconsistent input graph data");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */

#ifdef PTSCOTCH_FOLD_DUP
    if ((coargrafptr->s.procglbnbr == 0) ||
        (coargrafptr->s.proccomm != finegrafptr->s.proccomm)) /* Folding occurs, select best partition */
      sendmode = bdgraphBipartMlBest (coargrafptr, finegrafptr->s.proccomm, finegrafptr->s.proclocnum);
    else
      sendmode = 1;                               /* We can exploit our data */
#else /* PTSCOTCH_FOLD_DUP */
    sendmode = 1;
#endif /* PTSCOTCH_FOLD_DUP */

#ifdef SCOTCH_DEBUG_BDGRAPH2
    if (MPI_Allreduce (&sendmode, &index, 1, MPI_INT, MPI_MAX, finegrafptr->s.proccomm) != MPI_SUCCESS)  {
      errorPrint ("bdgraphBipartMlUncoarsen: communication error (1)");
      cheklocval = 1;
    }
    if (index == 0) {
      errorPrint ("bdgraphBipartMlUncoarsen: common error (1)");
      cheklocval = 1;
    }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */

    if (memAllocGroup ((void **) (void *)
		       &partglbtab, (size_t) (2 * coargrafptr->s.vertlocnbr * sizeof (BdgraphBipartMlPart)),
		       &fronloctab, (size_t) (coargrafptr->fronlocnbr   * sizeof (Gnum)),
	               &senddsptab, (size_t) (finegrafptr->s.procglbnbr * sizeof (int)),
                       &sendcnttab, (size_t) (finegrafptr->s.procglbnbr * sizeof (int)),
                       &recvdsptab, (size_t) (finegrafptr->s.procglbnbr * sizeof (int)),
                       &recvcnttab, (size_t) (finegrafptr->s.procglbnbr * sizeof (int)),
                       &partnbr1,   (size_t) (finegrafptr->s.procglbnbr * sizeof (int)),
                       NULL) == NULL) {
      errorPrint ("bdgraphBipartMlUncoarsen: out of memory (3)");
      cheklocval = 1;
      memFree (finegrafptr->partgsttax + finegrafptr->s.baseval);
      finegrafptr->partgsttax = NULL;
      return     (1);                             /* Allocated data will be freed along with graph structure */
    }
    partglbnbr  = 0;
    partglbptr  = partglbtab;
    finevertadj = finegrafptr->s.procvrttab[finegrafptr->s.proclocnum] - finegrafptr->s.baseval;
    
    /* Initialize variables for finer Bdgraph */
    glbfronnbr                = 0; 
    fronlocnum                = 0; 
    finefronnum               = 0;
    finegrafptr->fronlocnbr   = 0;
    finesize1                 = 0;
    fineload1                 = 0;
    finegrafptr->complocload0 = 0;
    finegrafptr->complocsize0 = 0;
    commlocloadextn           = 0;
    commlocgainextn           = 0;

    /* Mark all vertices as unvisited */
    memSet (finegrafptr->partgsttax + finegrafptr->s.baseval, 2, finegrafptr->s.vertgstnbr * sizeof (GraphPart));

    if (sendmode) {                               /* We must browse local data */
      for (coarfronnum = 0; coarfronnum < coargrafptr->fronlocnbr; coarfronnum ++) {
        coarvertnum  = coargrafptr->fronloctab[coarfronnum];
	finevertnum  = coarmulttax[coarvertnum].vertnum[0];
	finevertnum1 = coarmulttax[coarvertnum].vertnum[1];
        coarpartval  = coargrafptr->partgsttax[coarvertnum];
	
	if (finevertnum != finevertnum1) {        /* If multinode is made of two distinct vertices */
	  if (dgraphVertexLocal (&finegrafptr->s, finevertnum)) {
	    finegrafptr->fronloctab[finefronnum ++] = finevertnum - finevertadj; /* Record it in lieu of the coarse frontier vertex */ 
#ifdef SCOTCH_DEBUG_BDGRAPH2
            if (finegrafptr->partgsttax[finevertnum - finevertadj] != 2) {
              errorPrint ("bdgraphBipartMlUncoarsen: internal error (1)");
              return (1);
            }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
            finesize1 += (Gnum) (coarpartval & 1); /* One extra vertex created in part 1 if (coarpartval == 1) */
            if (finegrafptr->s.veloloctax != NULL) {
              veloval    = finegrafptr->s.veloloctax[finevertnum - finevertadj]; 
	      fineload1 += veloval & (-coarpartval);
	    }
	    if (finegrafptr->veexloctax != NULL) {
	      veexval          = finegrafptr->veexloctax[finevertnum - finevertadj];
	      commlocloadextn += veexval * coarpartval;
	      commlocgainextn += veexval * (1 - 2 * coarpartval);
	    }
            finegrafptr->partgsttax[finevertnum - finevertadj] = coarpartval;
	  }
          else {                                  /* Non local vertex */
            partglbptr->vertnum = finevertnum;
            partglbptr->partval = (Gnum) coarpartval;
            partglbptr ++;
          }
	  if (dgraphVertexLocal (&finegrafptr->s, finevertnum1)) {
	    fronloctab[fronlocnum ++] = finevertnum1 - finevertadj; /* Record it in the temporary local frontier array  */
#ifdef SCOTCH_DEBUG_BDGRAPH2
            if (finegrafptr->partgsttax[finevertnum1 - finevertadj] != 2) {
              errorPrint ("bdgraphBipartMlUncoarsen: internal error (2)");
              return (1);
            }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
            finesize1 += (Gnum) (coarpartval & 1); /* One extra vertex created in part 1 if (coarpartval == 1) */
            if (finegrafptr->s.veloloctax != NULL) {
              veloval    = finegrafptr->s.veloloctax[finevertnum1 - finevertadj]; 
	      fineload1 += veloval & (-coarpartval);
	    }
	    if (finegrafptr->veexloctax   != NULL) {
	      veexval          = finegrafptr->veexloctax[finevertnum1 - finevertadj];
	      commlocloadextn += veexval * coarpartval;
	      commlocgainextn += veexval * (1 - 2 * coarpartval);
	    }
            finegrafptr->partgsttax[finevertnum1 - finevertadj] = coarpartval;
	  }
          else {                                  /* Non local vertex */
            partglbptr->vertnum = finevertnum1;
            partglbptr->partval = (Gnum) coarpartval;
            partglbptr ++;
          }
	}
	else {                                    /* If coarse vertex is single node */
	  if (dgraphVertexLocal (&finegrafptr->s, finevertnum)) {
	    finegrafptr->fronloctab[finefronnum ++] = finevertnum - finevertadj; /* Record it in lieu of the coarse frontier vertex */
#ifdef SCOTCH_DEBUG_BDGRAPH2
            if (finegrafptr->partgsttax[finevertnum - finevertadj] != 2) {
              errorPrint ("bdgraphBipartMlUncoarsen: internal error (3)");
              return     (1);
            }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
            finesize1 += (Gnum) (coarpartval & 1); /* One extra vertex created in part 1 if (coarpartval == 1) */
            if (finegrafptr->s.veloloctax != NULL) {
              veloval    = finegrafptr->s.veloloctax[finevertnum - finevertadj]; 
              fineload1 += veloval & (-coarpartval);
	    }
	    if (finegrafptr->veexloctax   != NULL) {
	      veexval          = finegrafptr->veexloctax[finevertnum - finevertadj];
	      commlocloadextn += veexval * coarpartval;
	      commlocgainextn += veexval * (1 - 2 * coarpartval);
	    }
            finegrafptr->partgsttax[finevertnum - finevertadj] = coarpartval;
	  }
          else {                                  /* Non local vertex */
            partglbptr->vertnum = finevertnum;
            partglbptr->partval = (Gnum) coarpartval;
            partglbptr ++;
          }
	}
	coargrafptr->partgsttax[coarvertnum] = 2; /* Mark the coarse vertex as already treated */
      }
      
      if (fronlocnum > 0) {                       /* Update fronloctab of fine graph, if any */
	memCpy (finegrafptr->fronloctab + finefronnum, fronloctab, fronlocnum * sizeof (Gnum));
	finefronnum += fronlocnum;
      }
      
      glbfronnbr = partglbptr - partglbtab;       /* Record the number of frontiers to be sent */

      for (coarvertnum = coargrafptr->s.baseval; coarvertnum < coargrafptr->s.vertlocnnd; coarvertnum ++) {
        coarpartval = coargrafptr->partgsttax[coarvertnum];
#ifdef SCOTCH_DEBUG_BDGRAPH2
        if (coarpartval > 2) {
          errorPrint ("bdgraphBipartMlUncoarsen: internal error (4)");
          cheklocval = 1;
        }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
	if (coarpartval == 2) /* Already dealt during frontier processing phase */
	  continue;

        for (i = 0; i < 2; i ++) {
          finevertnum = coarmulttax[coarvertnum].vertnum[i];
          if (dgraphVertexLocal (&finegrafptr->s, finevertnum)) { /* Vertex is a local one */
#ifdef SCOTCH_DEBUG_BDGRAPH2
            if (finegrafptr->partgsttax[finevertnum - finevertadj] != 2) {
              errorPrint ("bdgraphBipartMlUncoarsen: internal error (5)");
              return (1);
            }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
            finesize1 += (Gnum) (coarpartval & 1); /* One extra vertex created in part 1 if (coarpartval == 1) */
            if (finegrafptr->s.veloloctax != NULL) {
              veloval    = finegrafptr->s.veloloctax[finevertnum - finevertadj]; 
	      fineload1 += veloval & (-coarpartval);
	    }
	    if (finegrafptr->veexloctax   != NULL) {
	      veexval          = finegrafptr->veexloctax[finevertnum - finevertadj];
	      commlocloadextn += veexval * coarpartval;
	      commlocgainextn += veexval * (1 - 2 * coarpartval);
	    }
            finegrafptr->partgsttax[finevertnum - finevertadj] = coarpartval;
          }
          else {                                  /* Non local vertex */
            partglbptr->vertnum = finevertnum;
            partglbptr->partval = (Gnum) coarpartval;
            partglbptr ++;
          }
          if (coarmulttax[coarvertnum].vertnum[0] == coarmulttax[coarvertnum].vertnum[1]) /* In fact no contraction */
            break;
        }
      }
      
      partglbend = partglbptr;
    }
    else
      partglbend = partglbtab;

    memSet (sendcnttab, 0, finegrafptr->s.procglbnbr * sizeof (int));
    for (partglbptr = partglbtab; partglbptr < partglbend; ++ partglbptr) {
      low = 0;
      high = finegrafptr->s.procglbnbr - 1;
      while (low <= high) {
	procnum = (low + high) / 2;
	if ((partglbptr->vertnum < finegrafptr->s.procvrttab[procnum + 1]) &&
	    (partglbptr->vertnum >= finegrafptr->s.procvrttab[procnum]))
	  break;                                                           /* Owner process number found */
	else if (partglbptr->vertnum >= finegrafptr->s.procvrttab[procnum + 1])
	  low  = procnum + 1;
	else
	  high = procnum - 1;
      }
      if (partglbptr == partglbend)
	partglbptr --;
      sendcnttab[procnum] ++;
    }

    for (procnum = 0; procnum < finegrafptr->s.procglbnbr; procnum ++)
      sendcnttab[procnum] += (sendcnttab[procnum] == 0) ? 0 : 3;         /* Three additional information */

    if (MPI_Alltoall (sendcnttab, 1, MPI_INT, recvcnttab, 1, MPI_INT, 
	              finegrafptr->s.proccomm) != MPI_SUCCESS) {
      errorPrint ("bdgraphBipartMlUncoarsen: communication error (2)");
      cheklocval = 1;
    }

    for (procnum = 0, sendsize = recvsize = 0; 
	 procnum < finegrafptr->s.procglbnbr; procnum ++) {           /* Compute sizes and displacements */
      recvdsptab[procnum] = recvsize;    
      recvsize += recvcnttab[procnum];
      senddsptab[procnum] = sendsize;    
      sendsize += sendcnttab[procnum];
    }

    if (memAllocGroup ((void **) (void *)
	               &partsndtab, (size_t) (sendsize * sizeof (int)),
                       &partrcvtab, (size_t) (recvsize * sizeof (int)),
                       NULL) == NULL) {
      errorPrint ("bdgraphBipartMlUncoarsen: out of memory (4)");
      cheklocval = 1;
    }
    
    /* Fill the send buffer */
    memSet (partsndtab, 0, sendsize * sizeof (int));
    memSet (partrcvtab, 0, recvsize * sizeof (int));
    memSet (partnbr1, 0, finegrafptr->s.procglbnbr * sizeof (int));
    for (partglbptr = partglbtab; partglbptr < (partglbtab + glbfronnbr); 
	 ++ partglbptr) {                                                       /* For frontiers         */
      low = 0;
      high = finegrafptr->s.procglbnbr - 1;
      while (low <= high) {
	procnum = (low + high) / 2;
	if ((partglbptr->vertnum < finegrafptr->s.procvrttab[procnum + 1]) &&
	    (partglbptr->vertnum >= finegrafptr->s.procvrttab[procnum]))
	  break;                                                                /* owner process found   */
	else if (partglbptr->vertnum >= finegrafptr->s.procvrttab[procnum + 1])
	  low  = procnum + 1;
	else
	  high = procnum - 1;
      }
      if (partglbptr->partval == 0) {
	fronidxtmp = partsndtab[senddsptab[procnum]] + 3;        /* The number of header elements is '3' */
	partsndtab[senddsptab[procnum] + fronidxtmp] = partglbptr->vertnum;        
	partsndtab[senddsptab[procnum]] ++;                              /* Update first header element  */
      }
      else if (partglbptr->partval == 1) {
	fronidxtmp = partsndtab[senddsptab[procnum] + 1]; 
	partsndtab[(senddsptab[procnum] + sendcnttab[procnum]) - fronidxtmp - 1] = /* From back of array */
	partglbptr->vertnum;                                                     
	partsndtab[senddsptab[procnum] + 1] ++;                         /* Update second header element  */
      }

    }
    
    for (partglbptr = partglbtab + glbfronnbr; partglbptr < partglbend;
	 ++ partglbptr) {                                                       /* For normal vertices   */
      low = 0;
      high = finegrafptr->s.procglbnbr - 1;
      while (low <= high) {
	procnum = (low + high) / 2;
	if ((partglbptr->vertnum < finegrafptr->s.procvrttab[procnum + 1]) &&
	    (partglbptr->vertnum >= finegrafptr->s.procvrttab[procnum]))
	  break;                                                                /* Owner process found   */
	else if (partglbptr->vertnum >= finegrafptr->s.procvrttab[procnum + 1])
	  low  = procnum + 1;
	else
	  high = procnum - 1;
      }
      if (partglbptr->partval == 0) {
	partidxtmp = partsndtab[senddsptab[procnum]] + 
		     partsndtab[senddsptab[procnum] + 2] + 
		     3;                  /* Summation of first, currnet third header values, and the '3' */       
	partsndtab[senddsptab[procnum] + partidxtmp] = partglbptr->vertnum;
	partsndtab[senddsptab[procnum] + 2] ++;                          /* Update third header element  */
      }
      else if (partglbptr->partval == 1) {
	partidxtmp = partsndtab[senddsptab[procnum] + 1] + 
		     partnbr1[procnum];  /* Summation of second header value and current size of Part 1  */       
	partsndtab[(senddsptab[procnum] + sendcnttab[procnum]) - partidxtmp - 1] = /* From back of array */
	partglbptr->vertnum;                                                     
	partnbr1[procnum] ++; 
      }
      
    }

    if (MPI_Alltoallv (partsndtab, sendcnttab, senddsptab, MPI_INT, 
	partrcvtab, recvcnttab, recvdsptab, MPI_INT, finegrafptr->s.proccomm) != MPI_SUCCESS) {
      errorPrint ("bdgraphBipartMlUncoarsen: communication error (3)");
      cheklocval = 1;
    }
    
    /* Update local ones from the buffer for receiving data */
    for (procnum = 0 ; procnum < finegrafptr->s.procglbnbr ; ++ procnum) {
      if (recvcnttab[procnum] > 0) {
	partidxtmp = partrcvtab[recvdsptab[procnum]] + partrcvtab[recvdsptab[procnum] + 2] + 3;
	/* For part 0 */
	for (i = 3; i < partidxtmp; i ++) {
	  vertnum = partrcvtab[recvdsptab[procnum] + i];
	  finegrafptr->partgsttax[vertnum - finevertadj] = 0;
	  if (finegrafptr->veexloctax != NULL) {
	    veexval = finegrafptr->veexloctax[vertnum -finevertadj];
	    commlocgainextn += veexval;
	  }
	}

	/* For part 1 */
	for (i = partidxtmp; i < recvcnttab[procnum]; i ++) {
	  vertnum = partrcvtab[recvdsptab[procnum] + i];
	  finegrafptr->partgsttax[vertnum - finevertadj] = 1;
	  finesize1 ++;
	  if (finegrafptr->s.veloloctax != NULL) {
	    veloval    = finegrafptr->s.veloloctax[vertnum - finevertadj];
	    fineload1 += veloval;
	  }
	  if (finegrafptr->veexloctax != NULL) {
	    veexval = finegrafptr->veexloctax[vertnum - finevertadj];
	    commlocloadextn   += veexval; 
	    commlocgainextn   -= veexval;
	  }
	}
        
        /* For frontiers (part 0) */
	for (i = 3; i < partidxtmp; i ++) {
	  vertnum = partrcvtab[recvdsptab[procnum] + i];
	  finegrafptr->fronloctab[finefronnum ++] = vertnum - finevertadj;
	}
        
	/* For frontiers (part 1) */
	for (i = 1; i <= partrcvtab[recvdsptab[procnum] + 1]; i ++) {
	  vertnum = partrcvtab[(recvdsptab[procnum] + recvcnttab[procnum]) - i];
	  finegrafptr->fronloctab[finefronnum ++] = vertnum - finevertadj;
	}
      }
      else if (recvcnttab[procnum] == 0)
	continue;
#ifdef SCOTCH_DEBUG_BDGRAPH2
      else {
	errorPrint ("bdgraphBipartMlUncoarsen: internal error (8)");
	cheklocval = 1;
      }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */
    }
#ifdef SCOTCH_DEBUG_BDGRAPH2
    /* Verify that the partgsttax is fully completed for local vertices */
    for (finevertnum = finegrafptr->s.baseval ; finevertnum < finegrafptr->s.vertlocnnd ; ++ finevertnum) {
      if (finegrafptr->partgsttax[finevertnum] > 1) {
        errorPrint ("bdgraphBipartMlUncoarsen: internal error (9)");
        cheklocval = 1;
        break;
      }
    }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */

    /* Do synchronized halo */
    if (dgraphHaloSync (&finegrafptr->s, (byte *) (finegrafptr->partgsttax + finegrafptr->s.baseval), GRAPHPART_MPI) != 0) {
      errorPrint ("bdgraphBipartMlUncoarsen: cannot perform halo exchange");
      cheklocval = 1;
    }
    
    /* Extract real frontier vertices and calculate communication local load values */
    commlocloadintn  = 0;
    edlolocval       = 1;                            /* Assume edges are not weighted */
    if ((frontmptab = (Gnum *) memAlloc (finefronnum * sizeof (Gnum))) == NULL) {
      errorPrint ("bdgraphBipartMlUncoarsen: out of memory (5)");
      cheklocval = 1;
    }
    for (fronlocptr = finegrafptr->fronloctab, fronlocend = finegrafptr->fronloctab + finefronnum, i = 0; 
	 fronlocptr < fronlocend; fronlocptr ++) {
      Gnum                edgelocnum;                /* Current edge number                */ 
      Gnum                commcut;                   /* Number of communication cuts for the current vertex */

      partval = (Gnum) finegrafptr->partgsttax[* fronlocptr];
      for (edgelocnum = finegrafptr->s.vertloctax[* fronlocptr], commcut = 0; 
	   edgelocnum < finegrafptr->s.vendloctax[* fronlocptr]; edgelocnum ++) {
	int                 partend;
	int                 partdlt;
	partend  = finegrafptr->partgsttax[finegrafptr->s.edgegsttax[edgelocnum]];
	partdlt  = partval ^ partend;
        commcut += partdlt;
        if (finegrafptr->s.edloloctax != NULL)
	    edlolocval = finegrafptr->s.edloloctax[edgelocnum];
	commlocloadintn += partdlt * edlolocval; /* Counted in both part, should be divided by 2 in summing up phase */
      }
      if (commcut > 0) 
	frontmptab[i ++] = *fronlocptr; 
    }
    if ((finefronnum >= 1) && (finefronnum > i)) { /* if any non-frontier was found from frontier array */
      finefronnum = i;
      memCpy (finegrafptr->fronloctab, frontmptab, finefronnum * sizeof (Gnum));
    }
    memFree (frontmptab);
    memFree (partsndtab);

    /* Now compute Bdgraph fields */
    finegrafptr->complocsize0   = finegrafptr->s.vertlocnbr - finesize1; 
    if (finegrafptr->s.veloloctax == NULL)
      finegrafptr->complocload0 = finegrafptr->complocsize0;
    else
      finegrafptr->complocload0 = finegrafptr->s.velolocsum - fineload1;
    finegrafptr->fronlocnbr     = finefronnum;

    reduloctab[0] = cheklocval;
    reduloctab[1] = finegrafptr->complocload0;
    reduloctab[2] = fineload1;
    reduloctab[3] = finegrafptr->complocsize0;
    reduloctab[4] = finegrafptr->fronlocnbr;
    reduloctab[5] = commlocloadintn;
    reduloctab[6] = commlocloadextn;
    reduloctab[7] = commlocgainextn;
    MPI_Allreduce (reduloctab, reduglbtab, 8, GNUM_MPI, MPI_SUM, finegrafptr->s.proccomm);
    if (reduglbtab[0] > 0) {
      errorPrint ("bdgraphBipartMlUncoarsen: common error (2)");
      return (1);
    }
    finegrafptr->compglbload0    = reduglbtab[1];
    finegrafptr->compglbload0dlt = finegrafptr->compglbload0 - finegrafptr->compglbload0avg;
    finegrafptr->compglbsize0    = reduglbtab[3];
    finegrafptr->fronglbnbr      = reduglbtab[4];
    finegrafptr->commglbload     = ((reduglbtab[5] / 2) * finegrafptr->domdist +
	                            reduglbtab[6] + finegrafptr->commglbloadextn0);
    finegrafptr->commglbgainextn = reduglbtab[7];

    memFree (partglbtab);
    if (coargrafptr->s.procglbnbr != 0)
      memFree ((void *) (coarmulttax + coargrafptr->s.baseval));
  }
  else                                            /* No coarse graph provided      */
    bdgraphZero (finegrafptr);                    /* Assign all vertices to part 0 */

#ifdef SCOTCH_DEBUG_BDGRAPH2
  if (bdgraphCheck (finegrafptr) != 0) {
    errorPrint ("bdgraphBipartMlUncoarsen: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BDGRAPH2 */

  return (0);
}

/* This routine performs the
** bipartitioning recursion.
** It returns:
** - 0   : if bipartitioning could be computed.
** - !0  : on error.
*/

static
int
bdgraphBipartMl2 (
Bdgraph * restrict const             grafptr,     /* Active graph      */
const BdgraphBipartMlParam * const paraptr)       /* Method parameters */
{
  Bdgraph                       coargrafdat;
  DgraphCoarsenMulti * restrict coarmulttax;
  int                           o;

  if (grafptr->s.procglbnbr <= 1) {               /* Enter into sequential mode          */
    if (((o = bdgraphBipartMlUncoarsen (grafptr, NULL, NULL)) == 0) && /* Finalize graph */
        ((o = bdgraphBipartSt (grafptr, paraptr->stratseq)) != 0))
      errorPrint ("bdgraphBipartMl2: cannot apply seq strategy");
    return (o);
  }

  if (bdgraphBipartMlCoarsen (grafptr, &coargrafdat, &coarmulttax, paraptr) == 0) {
    if (coargrafdat.fronglbnbr == -1)             /* If frontab is invalid */
      o = 0;
    else
      o = bdgraphBipartMl2 (&coargrafdat, paraptr);
    if ((o == 0) &&
        ((o = bdgraphBipartMlUncoarsen (grafptr, &coargrafdat, coarmulttax)) == 0) &&
        ((o = bdgraphBipartSt          (grafptr, paraptr->stratasc))         != 0)) /* Apply ascending strategy */
      errorPrint ("bdgraphBipartMl2: cannot apply ascending strategy");
    bdgraphExit (&coargrafdat);
  }
  else {                                          /* Cannot coarsen due to lack of memory or error */
    if (((o = bdgraphBipartMlUncoarsen (grafptr, NULL, NULL)) == 0) && /* Finalize graph           */
        ((o = bdgraphBipartSt          (grafptr, paraptr->stratlow)) != 0)) /* Apply low strategy  */
      errorPrint ("bdgraphBipartMl2: cannot apply low strategy");
  }

  return (o);
}

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the muti-level bipartitioning.
** It returns:
** - 0 : if bipartitioning could be computed.
** - 1 : on error.
*/

int
bdgraphBipartMl (
Bdgraph * const                       grafptr,    /*+ Active graph      +*/
const BdgraphBipartMlParam * const  paraptr)      /*+ Method parameters +*/
{
  Gnum                levlnum;                    /* Save value for graph level */
  int                 o;

  levlnum = grafptr->levlnum;                     /* Save graph level                   */
  grafptr->levlnum = 0;                           /* Initialize coarsening level        */
  o = bdgraphBipartMl2 (grafptr, paraptr);        /* Perform multi-level bipartitioning */
  grafptr->levlnum = levlnum;                     /* Restore graph level                */

  return (o);
}
