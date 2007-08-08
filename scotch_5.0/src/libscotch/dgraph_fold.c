/* Copyright 2007 INRIA
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
/**   NAME       : dgraph_fold.c                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the distributed     **/
/**                source graph folding function.          **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 10 aug 2006     **/
/**                                 to   : 03 aug 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define DGRAPH

#include "module.h"
#include "common.h"
#include "dgraph.h"
#include "dgraph_fold.h"
#include "dgraph_fold_comm.h"

/******************************/
/*                            */
/* This routine handles       */
/* distributed source graphs. */
/*                            */
/******************************/

/* This routine builds a folded graph by
** merging graph data to the processes of
** the first half or to the second half
** of the communicator.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
dgraphFold (
const Dgraph * restrict const orggrafptr,
const int                     partval,            /* 0 for first half, 1 for second half */
Dgraph * restrict const       fldgrafptr,
const void * restrict const   vertinfoptrin,      /* Pointer on array of informations which must be kept, like coarmulttax */
void ** restrict const        vertinfoptrout,     /* Pointer on array of informations which must be kept, like coarmulttax */
MPI_Datatype                  vertinfotype)
{
  int               fldprocnbr;
  int               fldprocnum;                   /* Index of local process in folded communicator   */
  int               fldproccol;                   /* Color of receiver or not wanted in communicator */
  MPI_Comm          fldproccomm;                  /* Communicator of folded part                     */

  fldprocnbr = (orggrafptr->procglbnbr + 1) / 2;
  fldprocnum = orggrafptr->proclocnum;
  if (partval == 1) {
    fldprocnum = fldprocnum - fldprocnbr;
    fldprocnbr = orggrafptr->procglbnbr - fldprocnbr;
  }
  fldproccol = ((fldprocnum >= 0) && (fldprocnum < fldprocnbr)) ? 0 : MPI_UNDEFINED;

  if (MPI_Comm_split (orggrafptr->proccomm, fldproccol, fldprocnum, &fldproccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphFold: communication error");
    return     (1);
  }

  return (dgraphFold2 (orggrafptr, partval, fldgrafptr, fldproccomm, vertinfoptrin, vertinfoptrout, vertinfotype));
}

int
dgraphFold2 (
const Dgraph * restrict const orggrafptr,
const int                     partval,            /* 0 for first half, 1 for second half */
Dgraph * restrict const       fldgrafptr,
MPI_Comm                      fldproccomm,
const void * restrict const   vertinfoptrin,      /* Pointer on array of informations which must be kept, like coarmulttax */
void ** restrict const        vertinfoptrout,     /* Pointer on array of informations which must be kept, like coarmulttax */
MPI_Datatype                  vertinfotype)
{
  int                   fldcommtypval;            /* Type of communication for this process                 */
  DgraphFoldCommData    fldcommdattab[DGRAPHFOLDCOMMNBR]; /* Array of two communication data                */
  Gnum                  fldcommvrttab[DGRAPHFOLDCOMMNBR]; /* Starting global send indices of communications */
  Gnum                  fldvertidxtab[DGRAPHFOLDCOMMNBR]; /* Start indices of remote vertex arrays          */
  Gnum                  fldedgenbrtab[DGRAPHFOLDCOMMNBR]; /* Number of edges sent during each communication */
  Gnum                  fldvertlocnbr;            /* Number of vertices in local folded part                */
  Gnum                  fldedgelocsiz;            /* (Upper bound of) number of edges in folded graph       */
  Gnum                  fldedlolocsiz;            /* (Upper bound of) number of edge loads in folded graph  */
  int                   fldprocglbnbr;
  int                   fldproclocnum;            /* Index of local process in folded communicator          */
  int                   fldvertadjnbr;
  Gnum * restrict       fldvertadjtab;            /* Array of global start indices for adjustment slots     */
  Gnum * restrict       fldvertdlttab;            /* Array of index adjustments for original global indices */
  int                   cheklocval;
  int                   chekglbval;
  int                   requnbr;
  MPI_Request           requtab[DGRAPHFOLDTAGNBR * DGRAPHFOLDCOMMNBR];
  MPI_Status            stattab[DGRAPHFOLDTAGNBR * DGRAPHFOLDCOMMNBR];
  int                   vertinfosize;             /* Size of one information                                */

#ifdef SCOTCH_DEBUG_DGRAPH1
  if (orggrafptr->vendloctax != (orggrafptr->vertloctax + 1)) {
    errorPrint ("dgraphFold: graph must be compact");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH1 */


  fldprocglbnbr = (orggrafptr->procglbnbr + 1) / 2;
  if (partval == 1) {
    fldproclocnum = orggrafptr->proclocnum - fldprocglbnbr;
    fldprocglbnbr = orggrafptr->procglbnbr - fldprocglbnbr;
  }
  else
    fldproclocnum = orggrafptr->proclocnum;

  fldcommtypval = ((fldproclocnum >= 0) && (fldproclocnum < fldprocglbnbr)) ? DGRAPHFOLDCOMMRECV : DGRAPHFOLDCOMMSEND;
  if (vertinfoptrin != NULL)
    MPI_Type_size (vertinfotype, &vertinfosize);


  cheklocval = 0;
  if (fldcommtypval == DGRAPHFOLDCOMMRECV) {      /* If we are going to receive */
#ifdef SCOTCH_DEBUG_DGRAPH2
    if (fldgrafptr == NULL) {
      errorPrint ("dgraphFold: invalid parameters (1)");
      return     (1);
    }
    if (fldproccomm == MPI_COMM_NULL) {
      errorPrint ("dgraphFold: invalid parameters (2)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

    memSet (fldgrafptr, 0, sizeof (Dgraph));      /* Pre-initialize graph fields */

    fldgrafptr->proccomm   = fldproccomm;
    fldgrafptr->procglbnbr = fldprocglbnbr;
    fldgrafptr->proclocnum = fldproclocnum;
    fldgrafptr->flagval    = DGRAPHFREETABS | DGRAPHVERTGROUP | DGRAPHEDGEGROUP | DGRAPHFREECOMM; /* For premature freeing on error; do not free vhndloctab as it is grouped with vertloctab */

    if (memAllocGroup ((void **)                  /* Allocate folding structures */
                       &fldvertadjtab, (size_t) (orggrafptr->procglbnbr * DGRAPHFOLDCOMMNBR * sizeof (Gnum)),
                       &fldvertdlttab, (size_t) (orggrafptr->procglbnbr * DGRAPHFOLDCOMMNBR * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("dgraphFold: out of memory (1)");
      cheklocval = 1;
    }
    else if (memAllocGroup ((void **)             /* Allocate distributed graph private data */
                       &fldgrafptr->procdsptab, (size_t) ((fldprocglbnbr + 1) * sizeof (int)),
                       &fldgrafptr->proccnttab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->procngbtab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->procrcvtab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->procsndtab, (size_t) (fldprocglbnbr       * sizeof (int)), NULL) == NULL) {
      errorPrint ("dgraphFold: out of memory (2)");
      cheklocval = 1;
    }
    else if (dgraphFoldComm (orggrafptr, partval, &fldcommtypval, fldcommdattab, fldcommvrttab, /* Process can become a sender receiver */
                             fldgrafptr->proccnttab, &fldvertadjnbr, fldvertadjtab, fldvertdlttab) != 0) {
      errorPrint ("dgraphFold: cannot compute folding communications (1)");
      cheklocval = 1;
    }
    else {
      Gnum              fldvelolocnbr;

      if ((fldcommtypval & DGRAPHFOLDCOMMSEND) == 0) { /* If process is a normal receiver */
        int               i;

        for (i = 0, fldvertlocnbr = orggrafptr->proccnttab[orggrafptr->proclocnum];
             (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++)
          fldvertlocnbr += fldcommdattab[i].vertnbr;
        fldedgelocsiz = orggrafptr->edgelocsiz + orggrafptr->edgeglbsmx * i; /* Upper bound */
      }
      else {                                      /* Process is a sender receiver */
        fldvertlocnbr = fldcommvrttab[0] - orggrafptr->procvrttab[orggrafptr->proclocnum]; /* Communications will remove vertices */
        fldedgelocsiz = orggrafptr->vertloctax[fldvertlocnbr + orggrafptr->baseval] - orggrafptr->baseval;
      }
      fldvelolocnbr = (orggrafptr->veloloctax != NULL) ? fldvertlocnbr : 0;

      if (memAllocGroup ((void **)                /* Allocate distributed graph public data */
                         &fldgrafptr->vertloctax, (size_t) ((fldvertlocnbr + DGRAPHFOLDCOMMNBR + 1) * sizeof (Gnum)), /* More slots for received vertex arrays  */
                         &fldgrafptr->vnumloctax, (size_t) (fldvertlocnbr                           * sizeof (Gnum)),
                         &fldgrafptr->veloloctax, (size_t) (fldvelolocnbr                           * sizeof (Gnum)), NULL) == NULL) {
        errorPrint ("dgraphFold: out of memory (3)");
        cheklocval = 1;
      }
      else if (fldgrafptr->vertloctax -= orggrafptr->baseval,
               fldgrafptr->vnumloctax -= orggrafptr->baseval,
               fldgrafptr->vendloctax  = fldgrafptr->vertloctax + 1, /* Folded graph is compact */
               fldgrafptr->veloloctax  = ((fldvelolocnbr != 0) ? fldgrafptr->veloloctax - orggrafptr->baseval : NULL),
               fldedlolocsiz = ((orggrafptr->edloloctax != NULL) ? fldedgelocsiz : 0),
               memAllocGroup ((void **)
                              &fldgrafptr->edgeloctax, (size_t) (fldedgelocsiz * sizeof (Gnum)),
                              &fldgrafptr->edloloctax, (size_t) (fldedlolocsiz * sizeof (Gnum)), NULL) == NULL) {
        errorPrint ("dgraphFold: out of memory (4)");
        cheklocval = 1;
      }
      else {
        if (vertinfoptrin != NULL) {
          if ((*vertinfoptrout = (byte*) memAlloc (fldvertlocnbr * vertinfosize)) == NULL) {
            errorPrint ("dgraphFold: out of memory (5)");
            cheklocval = 1;
          }
          else {
            *vertinfoptrout -= (vertinfosize * orggrafptr->baseval);
          }
        }
        fldgrafptr->edgeloctax -= orggrafptr->baseval;
        fldgrafptr->edloloctax -= orggrafptr->baseval; /* Do not care about the validity of edloloctax at this stage */
      }
    }
  }
  else {                                          /* Process is a sender */
#ifdef SCOTCH_DEBUG_HDGRAPH2
    if (fldproccomm != MPI_COMM_NULL) {
      errorPrint ("dgraphFold2: invalid parameters (3)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_HDGRAPH2 */

    if (dgraphFoldComm (orggrafptr, partval, &fldcommtypval, fldcommdattab, fldcommvrttab, NULL, NULL, NULL, NULL) != 0) {
      errorPrint ("dgraphFold: cannot compute folding communications (2)");
      cheklocval = 1;
    }
  }

#ifdef SCOTCH_DEBUG_DGRAPH1                       /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, orggrafptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphFold: communication error (3)");
    chekglbval = 1;
  }
#else /* SCOTCH_DEBUG_DGRAPH1 */
  chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_DGRAPH1 */
  if (chekglbval != 0) {
    if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) {
      if (fldvertadjtab != NULL)
      memFree (fldvertadjtab);                    /* Free group leader */
      dgraphExit (fldgrafptr);
    }
    return (1);
  }

  if ((fldcommtypval & DGRAPHFOLDCOMMSEND) != 0) { /* If process is (also) a sender */
    Gnum              vertsndbas;
    Gnum              vertsndnbr;
    int               i;

    vertsndnbr = ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) ? (fldcommvrttab[0] - orggrafptr->procvrttab[orggrafptr->proclocnum]) : 0; /* If process is also a receiver, start sending after kept vertices */

    for (i = 0, requnbr = 0, vertsndbas = orggrafptr->baseval; /* For all send communications to perform */
         (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
      Gnum              edgelocbas;
      Gnum              edgelocsiz;
      int               procsndnum;               /* Rank of process to send to */

      vertsndbas += vertsndnbr;
      vertsndnbr  = fldcommdattab[i].vertnbr;
      edgelocbas  = orggrafptr->vertloctax[vertsndbas];
      edgelocsiz  = orggrafptr->vertloctax[vertsndbas + vertsndnbr] - edgelocbas; /* Graph is compact */
      procsndnum  = fldcommdattab[i].procnum;

      if (MPI_Isend (orggrafptr->vertloctax + vertsndbas, vertsndnbr + 1, GNUM_MPI, /* Send one more vertex to have end value */
                     procsndnum, TAGVERTLOCTAB * orggrafptr->procglbnbr + orggrafptr->proclocnum,
                     orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("dgraphFold: communication error (4,%d)", i);
        cheklocval = 1;
      }
      else if (MPI_Isend (orggrafptr->edgeloctax + edgelocbas, edgelocsiz, GNUM_MPI,
                          procsndnum, TAGEDGELOCTAB * orggrafptr->procglbnbr + orggrafptr->proclocnum,
                          orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("dgraphFold: communication error (5,%d)", i);
        cheklocval = 1;
      }
      else if ((orggrafptr->edloloctax != NULL) &&
               (MPI_Isend (orggrafptr->edloloctax + edgelocbas, edgelocsiz, GNUM_MPI,
                           procsndnum, TAGEDLOLOCTAB * orggrafptr->procglbnbr + orggrafptr->proclocnum,
                           orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("dgraphFold: communication error (6,%d)", i);
        cheklocval = 1;
      }
      else if ((orggrafptr->veloloctax != NULL) &&
               (MPI_Isend (orggrafptr->veloloctax + vertsndbas, vertsndnbr, GNUM_MPI,
                           procsndnum, TAGVELOLOCTAB * orggrafptr->procglbnbr + orggrafptr->proclocnum,
                           orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("dgraphFold: communication error (7,%d)", i);
        cheklocval = 1;
      }
      else if ((orggrafptr->vnumloctax != NULL) &&
               (MPI_Isend (orggrafptr->vnumloctax + vertsndbas, vertsndnbr, GNUM_MPI,
                           procsndnum, TAGVNUMLOCTAB * orggrafptr->procglbnbr + orggrafptr->proclocnum,
                           orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("dgraphFold: communication error (8,%d)", i);
        cheklocval = 1;
      }
      else if ((vertinfoptrin != NULL)  &&
               (MPI_Isend ((byte *) (vertinfoptrin) + vertsndbas * vertinfosize, vertsndnbr, vertinfotype,
                           procsndnum, TAGDATALOCTAB * orggrafptr->procglbnbr + orggrafptr->proclocnum,
                           orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("dgraphFold: communication error (9,%d)", i);
        cheklocval = 1;
      }
     }

    if ((fldcommtypval & DGRAPHFOLDCOMMRECV) == 0) { /* If process is a normal sender   */
      if (MPI_Waitall (requnbr, requtab, stattab) != MPI_SUCCESS) { /* Wait for all now */
        errorPrint  ("dgraphFold: communication error (10)");
        cheklocval = 1;
      }
    }                                             /* Communications of sender-receivers will be completed in the receiving phase */
  }

  if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) { /* If process is (also) a receiver */
    Gnum                orgvertlocnbr;
    Gnum                orgvertlocnnd;
    Gnum                fldvertlocnum;
    Gnum                fldedgelocnum;
    Gnum                fldvelolocsum;
    Gnum                fldvertrcvbas;
    Gnum                fldvertrcvnnd;
    int                 fldprocnum;
    int                 i;

    fldgrafptr->procvrttab = fldgrafptr->procdsptab; /* Graph does not have holes                               */
    fldgrafptr->procdsptab[0] = orggrafptr->baseval; /* Build private data of folded graph and array            */
    for (fldprocnum = 0; fldprocnum < fldprocglbnbr; fldprocnum ++) /* New subdomain indices start from baseval */
      fldgrafptr->procdsptab[fldprocnum + 1] = fldgrafptr->procdsptab[fldprocnum] + fldgrafptr->proccnttab[fldprocnum];

    requnbr = 0;                                  /* Optional communications without further processing at beginning of array */

    if ((fldcommtypval & DGRAPHFOLDCOMMSEND) == 0) { /* If process is a full receiver */
      Gnum                vertrcvbas;
      Gnum                vertrcvnbr;
      Gnum                orgvertlocmin;
      Gnum                orgvertlocmax;
      Gnum                fldvertlocadj;
      int                 procngbmin;
      int                 procngbmax;

      for (i = 0, vertrcvbas = orggrafptr->vertlocnnd, vertrcvnbr = 0; /* For all receive communications to perform */
           (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
        Gnum                procrcvnum;

        vertrcvbas += vertrcvnbr;
        vertrcvnbr  = fldcommdattab[i].vertnbr;
        procrcvnum  = fldcommdattab[i].procnum;

        if (MPI_Irecv (fldgrafptr->vertloctax + vertrcvbas + 1 + i, vertrcvnbr + 1, GNUM_MPI, /* Receive one more, after the end of previous array */
                       procrcvnum, TAGVERTLOCTAB * orggrafptr->procglbnbr + procrcvnum,
                       orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGVERTLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold: communication error (11,%d)", i);
          cheklocval = 1;
        }
        else if (MPI_Irecv (fldgrafptr->edgeloctax + orggrafptr->edgelocsiz + orggrafptr->baseval + orggrafptr->edgeglbsmx * i,
                            orggrafptr->edgeglbsmx, GNUM_MPI,
                            procrcvnum, TAGEDGELOCTAB * orggrafptr->procglbnbr + procrcvnum,
                            orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold: communication error (12,%d)", i);
          cheklocval = 1;
        }
        else if ((orggrafptr->edloloctax != NULL) &&
                 (MPI_Irecv (fldgrafptr->edloloctax + orggrafptr->edgelocsiz + orggrafptr->baseval + orggrafptr->edgeglbsmx * i,
                             orggrafptr->edgeglbsmx, GNUM_MPI,
                             procrcvnum, TAGEDLOLOCTAB * orggrafptr->procglbnbr + procrcvnum,
                             orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGEDLOLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS)) {
          errorPrint ("dgraphFold: communication error (13,%d)", i);
          cheklocval = 1;
        }
        else if ((orggrafptr->veloloctax != NULL) &&
                 (MPI_Irecv (fldgrafptr->veloloctax + vertrcvbas, vertrcvnbr, GNUM_MPI,
                             procrcvnum, TAGVELOLOCTAB * orggrafptr->procglbnbr + procrcvnum,
                             orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGVELOLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS)) {
          errorPrint ("dgraphFold: communication error (14,%d)", i);
          cheklocval = 1;
        }
        else if ((orggrafptr->vnumloctax != NULL) &&
                 (MPI_Irecv (fldgrafptr->vnumloctax + vertrcvbas, vertrcvnbr, GNUM_MPI,
                             procrcvnum, TAGVNUMLOCTAB * orggrafptr->procglbnbr + procrcvnum,
                             orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
          errorPrint ("dgraphFold: communication error (15,%d)", i);
          cheklocval = 1;
        }
        else if ((vertinfoptrin != NULL) &&
                 (MPI_Irecv ((byte *) *vertinfoptrout + vertrcvbas * vertinfosize, vertrcvnbr, vertinfotype,
                             procrcvnum, TAGDATALOCTAB * orggrafptr->procglbnbr + procrcvnum,
                             orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
          errorPrint ("dgraphFold: communication error (16,%d)", i);
          cheklocval = 1;
        }
      }

      orgvertlocnbr = orggrafptr->vertlocnbr;     /* Process all local vertices */
      orgvertlocnnd = orggrafptr->vertlocnnd;

      if (orggrafptr->vnumloctax == NULL) {       /* If original graph noes not have vertex numbers, create remote parts of vertex number array */
        Gnum              fldvertlocnum;
        Gnum              fldvertlocadj;
        int               i;

        fldvertlocnum = orgvertlocnnd;
        for (i = 0; (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
          Gnum              fldverttmpnnd;

          for (fldverttmpnnd = fldvertlocnum + fldcommdattab[i].vertnbr, fldvertlocadj = fldcommvrttab[i];
               fldvertlocnum < fldverttmpnnd; fldvertlocnum ++)
            fldgrafptr->vnumloctax[fldvertlocnum] = fldvertlocadj ++;
        }
      }

      for (procngbmin = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
           procngbmax - procngbmin > 1; ) {
        int               procngbnum;

        procngbnum = (procngbmax + procngbmin) / 2;
        if (fldvertadjtab[procngbnum] <= orggrafptr->procvrttab[orggrafptr->proclocnum])
          procngbmin = procngbnum;
        else
          procngbmax = procngbnum;
      }
      orgvertlocmin = fldvertadjtab[procngbmin];
      orgvertlocmax = fldvertadjtab[procngbmax];
      fldvertlocadj = fldvertdlttab[procngbmin];
      for (fldvertlocnum = fldedgelocnum = orggrafptr->baseval; /* Adjust local part of edge array */
           fldvertlocnum < orggrafptr->vertlocnnd; fldvertlocnum ++) {
        for ( ; fldedgelocnum < orggrafptr->vendloctax[fldvertlocnum]; fldedgelocnum ++) { /* Reorder end vertices */
          Gnum              orgvertlocend;

#ifdef SCOTCH_DEBUG_DGRAPH2
          if (fldedgelocnum >= (fldedgelocsiz + orggrafptr->baseval)) {
            errorPrint  ("dgraphFold: internal error (1)");
            return      (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

          orgvertlocend = orggrafptr->edgeloctax[fldedgelocnum];

          if ((orgvertlocend >= orgvertlocmin) && /* If end vertex is local */
              (orgvertlocend <  orgvertlocmax))
            fldgrafptr->edgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
          else {                                  /* End vertex is not local */
            int               procngbmin;
            int               procngbmax;

            for (procngbmin = 0, procngbmax = fldvertadjnbr;
                 procngbmax - procngbmin > 1; ) {
              int               procngbnum;

              procngbnum = (procngbmax + procngbmin) / 2;
              if (fldvertadjtab[procngbnum] <= orgvertlocend)
                procngbmin = procngbnum;
              else
                procngbmax = procngbnum;
            }
            fldgrafptr->edgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbmin];
          }
        }
      }

      fldvelolocsum = orggrafptr->velolocsum;     /* In case there are vertex loads: we keep all of existing load */
    }
    else {                                        /* Receiver process is also a sender */
      Gnum              orgvertlocmin;
      Gnum              orgvertlocmax;
      Gnum              fldvertlocadj;
      int               procngbmin;
      int               procngbmax;

      orgvertlocnbr = fldvertlocnbr;              /* Process only remaining local vertices */
      orgvertlocnnd = fldvertlocnbr + orggrafptr->baseval;

      for (procngbmin = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
           procngbmax - procngbmin > 1; ) {
        int               procngbnum;

        procngbnum = (procngbmax + procngbmin) / 2;
        if (fldvertadjtab[procngbnum] <= orggrafptr->procvrttab[orggrafptr->proclocnum])
          procngbmin = procngbnum;
        else
          procngbmax = procngbnum;
      }
      orgvertlocmin = fldvertadjtab[procngbmin];
      orgvertlocmax = fldvertadjtab[procngbmax];
      fldvertlocadj = fldvertdlttab[procngbmin];
      for (fldvertlocnum = fldedgelocnum = orggrafptr->baseval; /* Copy remaining local part of edge array */
           fldvertlocnum < orgvertlocnnd; fldvertlocnum ++) {
        for ( ; fldedgelocnum < orggrafptr->vendloctax[fldvertlocnum]; fldedgelocnum ++) { /* Reorder end vertices */
          Gnum              orgvertlocend;

#ifdef SCOTCH_DEBUG_DGRAPH2
          if (fldedgelocnum >= (fldedgelocsiz + orggrafptr->baseval)) {
            errorPrint  ("dgraphFold: internal error (2)");
            return      (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

          orgvertlocend = orggrafptr->edgeloctax[fldedgelocnum];

          if ((orgvertlocend >= orgvertlocmin) && /* If end vertex is local */
              (orgvertlocend <  orgvertlocmax))
            fldgrafptr->edgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
          else {
            int               procngbmin;
            int               procngbmax;

            for (procngbmin = 0, procngbmax = fldvertadjnbr;
                 procngbmax - procngbmin > 1; ) {
              int               procngbnum;

              procngbnum = (procngbmax + procngbmin) / 2;
              if (fldvertadjtab[procngbnum] <= orgvertlocend)
                procngbmin = procngbnum;
              else
                procngbmax = procngbnum;
            }
            fldgrafptr->edgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbmin];
          }
        }
      }

      if (orggrafptr->veloloctax != NULL)         /* If original graph has vertex loads                       */
        for (fldvertlocnum = orggrafptr->baseval, fldvelolocsum = 0; /* Accumulate load sum of remaining part */
             fldvertlocnum < orgvertlocnnd; fldvertlocnum ++)
          fldvelolocsum += orggrafptr->veloloctax[fldvertlocnum];

      fldcommdattab[0].procnum = -1;              /* Turn sender-receiver into normal receiver without any communications to perform */
    }

    if (orggrafptr->veloloctax != NULL)           /* If original graph has vertex loads             */
      memCpy (fldgrafptr->veloloctax + orggrafptr->baseval, /* Copy local part of vertex load array */
              orggrafptr->veloloctax + orggrafptr->baseval, orgvertlocnbr * sizeof (Gnum));

    if (orggrafptr->vnumloctax != NULL)           /* If original graph has vertex numbers             */
      memCpy (fldgrafptr->vnumloctax + orggrafptr->baseval, /* Copy local part of vertex number array */
              orggrafptr->vnumloctax + orggrafptr->baseval, orgvertlocnbr * sizeof (Gnum));
    if (vertinfoptrin != NULL) {                      /* If vertinfo exists                   */
      memCpy ((byte *) (*vertinfoptrout) + orggrafptr->baseval * vertinfosize, /* Copy local part */
              (byte *) (vertinfoptrin) + orggrafptr->baseval * vertinfosize, orgvertlocnbr * vertinfosize);
    }

    else {                                        /* Build local part of vertex number array */
      Gnum              fldvertlocnum;
      Gnum              fldvertlocadj;

      for (fldvertlocnum = orggrafptr->baseval,
           fldvertlocadj = orggrafptr->procvrttab[orggrafptr->proclocnum];
           fldvertlocnum < orgvertlocnnd; fldvertlocnum ++)
        fldgrafptr->vnumloctax[fldvertlocnum] = fldvertlocadj ++;
    }

    memCpy (fldgrafptr->vertloctax + orggrafptr->baseval, /* Copy local part of vertex array (since it is compact) */
            orggrafptr->vertloctax + orggrafptr->baseval, (orgvertlocnbr + 1) * sizeof (Gnum));

    for (i = 0, fldvertlocnum = orggrafptr->vertlocnnd + 1;
         (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
      if (MPI_Wait (&requtab[DGRAPHFOLDTAGVERTLOCTAB * DGRAPHFOLDCOMMNBR + i],
                    &stattab[DGRAPHFOLDTAGVERTLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
        errorPrint ("dgraphFold: communication error (15)");
        cheklocval = 1;
      }
      else {                                      /* Adjust first remote part of vertex array */
        Gnum              fldvertlocadj;
        Gnum              fldverttmpnnd;

        fldvertlocadj = fldgrafptr->vertloctax[fldvertlocnum - 1] - fldgrafptr->vertloctax[fldvertlocnum + i];
        fldvertidxtab[i] = fldgrafptr->vertloctax[fldvertlocnum + i];

        for (fldverttmpnnd = fldvertlocnum + fldcommdattab[i].vertnbr; fldvertlocnum < fldverttmpnnd; fldvertlocnum ++)
          fldgrafptr->vertloctax[fldvertlocnum] = fldgrafptr->vertloctax[fldvertlocnum + i + 1] + fldvertlocadj;
      }
    }

    for (i = 0, fldvertrcvbas = orggrafptr->vertlocnnd;
         (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++, fldvertrcvbas = fldvertrcvnnd) {
      fldvertrcvnnd    = fldvertrcvbas + fldcommdattab[i].vertnbr;
      fldedgenbrtab[i] = fldgrafptr->vertloctax[fldvertrcvnnd] - fldgrafptr->vertloctax[fldvertrcvbas];

      if (MPI_Wait (&requtab[DGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i],
                    &stattab[DGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
        errorPrint ("dgraphFold: communication error (16)");
        cheklocval = 1;
      }
      else if (cheklocval == 0) {                 /* Adjust remote part(s) of edge array */
        Gnum              orgvertlocmin;
        Gnum              orgvertlocmax;
        Gnum              fldvertlocadj;
        Gnum              fldvhallocmax;          /* Maximum current size of halo vertex array */
        Gnum              fldedgercvnum;
        int               procngbmin;
        int               procngbmax;
#ifdef SCOTCH_DEBUG_DGRAPH2
        int               fldedgercvnbr;

        MPI_Get_count (&stattab[DGRAPHFOLDTAGEDGELOCTAB * DGRAPHFOLDCOMMNBR + i], GNUM_MPI, &fldedgercvnbr);
        if (fldedgercvnbr != fldedgenbrtab[i]) {
          errorPrint ("dgraphFold: internal error (3)");
          return     (1);
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

        for (procngbmin = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
             procngbmax - procngbmin > 1; ) {
          int               procngbnum;

          procngbnum = (procngbmax + procngbmin) / 2;
          if (fldvertadjtab[procngbnum] <= fldcommvrttab[i])
            procngbmin = procngbnum;
          else
            procngbmax = procngbnum;
        }
        orgvertlocmin = fldvertadjtab[procngbmin];
        orgvertlocmax = fldvertadjtab[procngbmax];
        fldvertlocadj = fldvertdlttab[procngbmin];
        fldvhallocmax = -1;                       /* Reset halo vertex array for each remote part                                   */
        for (fldvertlocnum = fldvertrcvbas, fldedgelocnum = fldgrafptr->vertloctax[fldvertlocnum], /* Copy local part of edge array */
             fldedgercvnum = orggrafptr->edgelocsiz + orggrafptr->baseval + orggrafptr->edgeglbsmx * i;
             fldvertlocnum < fldvertrcvnnd; fldvertlocnum ++) {
          for ( ; fldedgelocnum < fldgrafptr->vendloctax[fldvertlocnum]; fldedgelocnum ++) { /* Reorder end vertices */
            Gnum              orgvertlocend;

#ifdef SCOTCH_DEBUG_DGRAPH2
            if (fldedgelocnum >= (fldedgelocsiz + orggrafptr->baseval)) {
              errorPrint  ("dgraphFold: internal error (4)");
              return      (1);
            }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

            orgvertlocend = fldgrafptr->edgeloctax[fldedgercvnum ++];

            if ((orgvertlocend >= orgvertlocmin) && /* If end vertex is local */
                (orgvertlocend <  orgvertlocmax))
              fldgrafptr->edgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
            else {
              int               procngbmin;
              int               procngbmax;

              for (procngbmin = 0, procngbmax = fldvertadjnbr;
                   procngbmax - procngbmin > 1; ) {
                int               procngbnum;

                procngbnum = (procngbmax + procngbmin) / 2;
                if (fldvertadjtab[procngbnum] <= orgvertlocend)
                  procngbmin = procngbnum;
                else
                  procngbmax = procngbnum;
              }
              fldgrafptr->edgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbmin];
            }
          }
        }
      }
    }

    memFree (fldvertadjtab);                      /* Free group leader */

#ifdef SCOTCH_DEBUG_DGRAPH2
    if (fldedgelocnum != fldgrafptr->vertloctax[fldvertlocnbr + orggrafptr->baseval]) {
      errorPrint ("dgraphFold: internal error (5)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
    fldedgelocsiz = fldedgelocnum - orggrafptr->baseval;

    if (orggrafptr->edloloctax != NULL) {         /* If graph has edge loads */
      Gnum              fldedlorcvbas;

      fldedlorcvbas = fldedgelocnum;              /* Put edge loads just after edge data */

      memCpy (fldgrafptr->edgeloctax + fldedlorcvbas, /* Copy local part of edge load array */
              orggrafptr->edloloctax + orggrafptr->baseval,
              (orggrafptr->vertloctax[orgvertlocnbr + orggrafptr->baseval] - orggrafptr->baseval) * sizeof (Gnum));
      fldedlorcvbas += orggrafptr->vertloctax[orgvertlocnbr + orggrafptr->baseval] - orggrafptr->baseval;

      for (i = 0 ; (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
        if (MPI_Wait (&requtab[DGRAPHFOLDTAGEDLOLOCTAB * DGRAPHFOLDCOMMNBR + i],
                      &stattab[DGRAPHFOLDTAGEDLOLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold: communication error (17)");
          cheklocval = 1;
        }
        else if (cheklocval == 0) {               /* Move edge load data into its final place */
#ifdef SCOTCH_DEBUG_DGRAPH2
          int               fldedlorcvnbr;

          MPI_Get_count (&stattab[DGRAPHFOLDTAGEDLOLOCTAB * DGRAPHFOLDCOMMNBR + i], GNUM_MPI, &fldedlorcvnbr);
          if (fldedlorcvnbr != fldedgenbrtab[i]) {
            errorPrint ("dgraphFold: internal error (6)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

          memMov (fldgrafptr->edgeloctax + fldedlorcvbas,
                  fldgrafptr->edloloctax + orggrafptr->edgelocsiz + orggrafptr->baseval + orggrafptr->edgeglbsmx * i,
                  fldedgenbrtab[i] * sizeof (Gnum));
          fldedlorcvbas += fldedgenbrtab[i];
        }
      }
      fldedlolocsiz = fldedgelocsiz;
    }

    memReallocGroup ((void *) (fldgrafptr->edgeloctax + orggrafptr->baseval),
                     &fldgrafptr->edgeloctax, (size_t) (fldedgelocsiz * sizeof (Gnum)),
                     &fldgrafptr->edloloctax, (size_t) (fldedlolocsiz * sizeof (Gnum)), NULL);
    fldgrafptr->edgeloctax -= orggrafptr->baseval;
    fldgrafptr->edloloctax  = (fldedlolocsiz != 0) ? (fldgrafptr->edgeloctax + fldedgelocsiz) : NULL; /* Start where we have copied edge load data */

    if (orggrafptr->veloloctax == NULL)           /* If no vertex loads, reset graph vertex load to number of vertices */
      fldvelolocsum = fldvertlocnbr;
    else {                                        /* Graph has vertex loads and load of local part has already been computed */
      Gnum                vertrcvbas;
      Gnum                vertrcvnnd;

      for (i = 0, vertrcvnnd = orggrafptr->vertlocnnd; /* For all receive communications to perform */
           (i < DGRAPHFOLDCOMMNBR) && (fldcommdattab[i].procnum != -1); i ++) {
        vertrcvbas  = vertrcvnnd;
        vertrcvnnd += fldcommdattab[i].vertnbr;

        if (MPI_Wait (&requtab[DGRAPHFOLDTAGVELOLOCTAB * DGRAPHFOLDCOMMNBR + i],
                      &stattab[DGRAPHFOLDTAGVELOLOCTAB * DGRAPHFOLDCOMMNBR + i]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold: communication error (18)");
          cheklocval = 1;
        }
        else {                                    /* Accumulate vertex loads for received vertex load array */
          for (fldvertlocnum = vertrcvbas; fldvertlocnum < vertrcvnnd; fldvertlocnum ++)
            fldvelolocsum += fldgrafptr->veloloctax[fldvertlocnum];
        }
      }
    }

    if (MPI_Waitall (requnbr, requtab, stattab) != MPI_SUCCESS) { /* Make sure graph data is safe because graph could be freed after routine completes */
      errorPrint  ("dgraphFold: communication error (19)");
      cheklocval = 1;
    }

    fldgrafptr->baseval    = orggrafptr->baseval;
    fldgrafptr->vertlocnbr = fldvertlocnbr;
    fldgrafptr->vertlocnnd = fldvertlocnbr + orggrafptr->baseval;
    fldgrafptr->velolocsum = fldvelolocsum;
    fldgrafptr->edgelocnbr = fldedgelocsiz;
    fldgrafptr->edgelocsiz = fldedgelocsiz;
    fldgrafptr->degrglbmax = orggrafptr->degrglbmax;
    if (dgraphBuild4 (fldgrafptr) != 0) {
      errorPrint ("dgraphFold: cannot build folded graph");
      dgraphExit (fldgrafptr);
      return     (1);
    }
#ifdef SCOTCH_DEBUG_DGRAPH2
    if (dgraphCheck (fldgrafptr) != 0) {          /* Check graph consistency */
      errorPrint ("dgraphFold: internal error (7)");
      dgraphExit (fldgrafptr);
      return     (1);
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  }

#ifdef SCOTCH_DEBUG_DGRAPH1                      /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, orggrafptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphFold: communication error (20)");
    chekglbval = 1;
  }
#else /* SCOTCH_DEBUG_DGRAPH1 */
  chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_DGRAPH1 */

  return (chekglbval);
}
