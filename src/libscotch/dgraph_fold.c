/* Copyright 2007-2011,2014,2021,2023,2025,2026 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**                                 to   : 27 jun 2008     **/
/**                # Version 5.1  : from : 12 nov 2008     **/
/**                                 to   : 04 jan 2011     **/
/**                # Version 6.0  : from : 28 sep 2014     **/
/**                                 to   : 28 sep 2014     **/
/**                # Version 6.1  : from : 18 jun 2021     **/
/**                                 to   : 19 jun 2021     **/
/**                # Version 7.0  : from : 14 sep 2021     **/
/**                                 to   : 05 sep 2026     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

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

/* This routine builds a folded graph by merging graph
** data to the processes of the first half or to the
** second half of the communicator.
** The key value of the folded communicator is not
** changed as it is not relevant.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
dgraphFold (
const Dgraph * restrict const orggrafptr,
const int                     partval,            /*+ 0 for first half, 1 for second half                                        +*/
Dgraph * restrict const       fldgrafptr,
const void * restrict const   orgdataptr,         /*+ Un-based array of data which must be folded, e.g. coarmulttab              +*/
void ** const                 flddataptr,         /*+ Un-based array of data which must be folded, e.g. coarmulttab [norestrict] +*/
MPI_Datatype                  datatype)
{
  int                 fldprocnbr;
  int                 fldprocnum;                 /* Index of local process in folded communicator   */
  int                 fldproccol;                 /* Color of receiver or not wanted in communicator */
  MPI_Comm            fldproccomm;                /* Communicator of folded part                     */
  int                 o;

  fldprocnbr = (orggrafptr->procglbnbr + 1) / 2;
  fldprocnum = orggrafptr->proclocnum;
  if (partval == 1) {
    fldprocnum = fldprocnum - fldprocnbr;
    fldprocnbr = orggrafptr->procglbnbr - fldprocnbr;
  }
  fldproccol = ((fldprocnum >= 0) && (fldprocnum < fldprocnbr)) ? 0 : MPI_UNDEFINED;

  if (MPI_Comm_split (orggrafptr->proccomm, fldproccol, fldprocnum, &fldproccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphFold: communication error");
    return (1);
  }

  o = dgraphFold2 (orggrafptr, partval, fldgrafptr, fldproccomm, orgdataptr, flddataptr, datatype);
  fldgrafptr->pkeyglbval = fldproccol;            /* Key of folded communicator is always zero if no duplication occurs */

  return (o);
}

int
dgraphFold2 (
const Dgraph * restrict const orggrafptr,
const int                     partval,            /*+ 0 for first half, 1 for second half                          +*/
Dgraph * const                fldgrafptr,         /*+ Folded graph structure to fill [norestrict:async]            +*/
MPI_Comm                      fldproccomm,
const void * restrict const   orgdataptr,         /*+ Un-based array of data which must be kept, e.g. coarmulttab  +*/
void ** const                 flddataptr,         /*+ Un-based array of data which must be kept, e.g. coarmulttab  +*/
MPI_Datatype                  datatype)
{
  Gnum * restrict               orgvertloctax;    /* Pointer to (possible compacted) vertex array                     */ 
  Gnum * restrict               orgedgeloctax;    /* Pointer to (possible compacted) edge array                       */
  Gnum * restrict               orgedloloctax;    /* Pointer to (possible compacted) edge load array                  */
  int                           fldcommtypval;    /* Type of communication for this process                           */
  DgraphFoldCommData * restrict fldcommdattab;    /* Array of two communication data                                  */
  Gnum *                        fldcommvrttab;    /* Starting global send indices of communications [norestrict]      */
  Gnum *                        fldvertidxtab;    /* Start indices of vertex arrays [norestrict]                      */
  Gnum *                        fldedgeidxtab;    /* Start indices of edge arrays [norestrict]                        */
  Gnum *                        fldedgecnttab;    /* Number of edges exchanged during each communication [norestrict] */
  Gnum *                        fldedgecnptab;    /* Temporary save for fldedgecnttab for MPI standard [norestrict]   */
  Gnum                          fldvertlocnbr;    /* Number of vertices in local folded part                          */
  int                           fldprocglbnbr;
  int                           fldproclocnum;    /* Index of local process in folded communicator                    */
  int                           fldvertadjnbr;
  Gnum * restrict               fldvertadjtab;    /* Array of global start indices for adjustment slots               */
  Gnum * restrict               fldvertdlttab;    /* Array of index adjustments for original global indices           */
  int                           cheklocval;
  int                           chekglbval;
  int                           commmax;
  int                           commnbr;
  int                           requnbr;
  int                           requnum;
  MPI_Request * restrict        requtab;
  int                           datasiz;          /* Size of one information                                          */

  const Gnum                    orgprocvrtbas = orggrafptr->procvrttab[orggrafptr->proclocnum];

  if ((orggrafptr->flagval & DGRAPHHASVENDLOC) != 0) { /* If graph is not compact */
    if (dgraphCompact2 (orggrafptr, &orgvertloctax, &orgedgeloctax, &orgedloloctax) != 0) {
      errorPrint ("dgraphFold2: cannot compact graph");
      return (1);
    }
  }
  else {                                          /* Graph is compact; take arrays as is */
    orgvertloctax = orggrafptr->vertloctax;
    orgedgeloctax = orggrafptr->edgeloctax;
    orgedloloctax = orggrafptr->edloloctax;
  }

  fldprocglbnbr = (orggrafptr->procglbnbr + 1) / 2;
  if (partval == 1) {
    fldproclocnum = orggrafptr->proclocnum - fldprocglbnbr;
    fldprocglbnbr = orggrafptr->procglbnbr - fldprocglbnbr;
  }
  else
    fldproclocnum = orggrafptr->proclocnum;

  fldcommtypval = ((fldproclocnum >= 0) && (fldproclocnum < fldprocglbnbr)) ? DGRAPHFOLDCOMMRECV : DGRAPHFOLDCOMMSEND;
  if (orgdataptr != NULL)
    MPI_Type_size (datatype, &datasiz);

  cheklocval = 1;                                 /* Assume an error */

  fldcommdattab = NULL;                           /* In case of error */
  fldvertidxtab = NULL;
  if (fldcommtypval == DGRAPHFOLDCOMMRECV) {      /* If we are going to receive */
    Gnum                fldvelolocnbr;

#ifdef SCOTCH_DEBUG_DGRAPH2
    if (fldgrafptr == NULL) {
      errorPrint ("dgraphFold2: invalid parameters (1)");
      goto fail1;
    }
    if (fldproccomm == MPI_COMM_NULL) {
      errorPrint ("dgraphFold2: invalid parameters (2)");
      goto fail1;
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

    memSet (fldgrafptr, 0, sizeof (Dgraph));      /* Pre-initialize graph fields */

    fldgrafptr->proccomm   = fldproccomm;
    fldgrafptr->procglbnbr = fldprocglbnbr;
    fldgrafptr->proclocnum = fldproclocnum;
    fldgrafptr->flagval    = DGRAPHFREEALL | DGRAPHVERTGROUP | DGRAPHEDGEGROUP | DGRAPHFREECOMM; /* For premature freeing on error (folded communicator created by caller) */

    if (memAllocGroup ((void **) (void *)         /* Allocate distributed graph private data */
                       &fldgrafptr->procdsptab, (size_t) ((fldprocglbnbr + 1) * sizeof (Gnum)),
                       &fldgrafptr->proccnttab, (size_t) (fldprocglbnbr       * sizeof (Gnum)),
                       &fldgrafptr->procngbtab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->procrcvtab, (size_t) (fldprocglbnbr       * sizeof (int)),
                       &fldgrafptr->procsndtab, (size_t) (fldprocglbnbr       * sizeof (int)), NULL) == NULL) {
      errorPrint ("dgraphFold2: out of memory (1)");
      goto fail1;
    }

    if (dgraphFoldComm (orggrafptr, partval, &commmax, &fldcommtypval, &fldcommdattab, &fldcommvrttab, /* Process can become a sender receiver */
                        fldgrafptr->proccnttab, &fldvertadjnbr, &fldvertadjtab, &fldvertdlttab) != 0) {
      errorPrint ("dgraphFold2: cannot compute folding communications (1)");
      goto fail1;
    }

    if ((fldcommtypval & DGRAPHFOLDCOMMSEND) == 0) { /* If process is a normal receiver */
      int                 commnum;

      fldvertlocnbr = orggrafptr->vertlocnbr;     /* Account for local vertices */
      for (commnum = 0; (commnum < commmax) && (fldcommdattab[commnum].procnum != -1); commnum ++)
        fldvertlocnbr += fldcommdattab[commnum].vertnbr;
      commnbr = commnum;                          /* Record number of receives to perform for each data type */
    }
    else {                                        /* Process is a sender receiver         */
      Gnum                fldedgelocsiz;          /* Number of edges in folded graph      */
      Gnum                fldedlolocsiz;          /* Number of edge loads in folded graph */

      fldvertlocnbr = fldcommvrttab[0] - orgprocvrtbas; /* Communications will remove vertices */
      fldedgelocsiz = orgvertloctax[fldvertlocnbr + orggrafptr->baseval] - orggrafptr->baseval; /* Exact number of edges */
      fldgrafptr->edgelocnbr =
      fldgrafptr->edgelocsiz = fldedgelocsiz;

      fldedlolocsiz = ((orgedloloctax != NULL) ? fldedgelocsiz : 0);
      if ((fldgrafptr->edgeloctax = memAlloc ((fldedgelocsiz + fldedlolocsiz) * sizeof (Gnum))) == NULL) { /* Allocate single array for both edge arrays */
        errorPrint ("dgraphFold2: out of memory (2)");
        goto fail1;
      }
      fldgrafptr->edgeloctax -= orggrafptr->baseval; /* Do not care about the validity of edloloctax at this stage */
    }
    fldvelolocnbr = (orggrafptr->veloloctax != NULL) ? fldvertlocnbr : 0;

    if (memAllocGroup ((void **) (void *)       /* Allocate distributed graph public data */
                       &fldgrafptr->vertloctax, (size_t) ((fldvertlocnbr + 1) * sizeof (Gnum)),
                       &fldgrafptr->vnumloctax, (size_t) ( fldvertlocnbr      * sizeof (Gnum)),
                       &fldgrafptr->veloloctax, (size_t) ( fldvelolocnbr      * sizeof (Gnum)), NULL) == NULL) {
      errorPrint ("dgraphFold2: out of memory (3)");
      goto fail1;
    }
    fldgrafptr->vertloctax -= orggrafptr->baseval;
    fldgrafptr->vnumloctax -= orggrafptr->baseval;
    fldgrafptr->vendloctax  = fldgrafptr->vertloctax + 1; /* Folded graph is compact */
    fldgrafptr->veloloctax  = ((orggrafptr->veloloctax != NULL) ? (fldgrafptr->veloloctax - orggrafptr->baseval) : NULL);

    if (orgdataptr != NULL) {                     /* If extra data to fold */
      if ((*flddataptr = (byte *) memAlloc (fldvertlocnbr * datasiz)) == NULL) {
        errorPrint ("dgraphFold2: out of memory (4)");
        goto fail1;
      }
    }
  }
  else {                                          /* Process is a sender */
#ifdef SCOTCH_DEBUG_DGRAPH2
    if (fldproccomm != MPI_COMM_NULL) {
      errorPrint ("dgraphFold2: invalid parameters (3)");
      goto fail1;
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

    if (dgraphFoldComm (orggrafptr, partval, &commmax, &fldcommtypval, &fldcommdattab, &fldcommvrttab, NULL, NULL, NULL, NULL) != 0) {
      errorPrint ("dgraphFold2: cannot compute folding communications (2)");
      goto fail1;
    }
  }

  if (memAllocGroup ((void **) (void *)          /* Allocate folding data */
                     &fldvertidxtab, (size_t) (commmax * sizeof (Gnum)),
                     &fldedgeidxtab, (size_t) (commmax * sizeof (Gnum)),
                     &fldedgecnttab, (size_t) (commmax * sizeof (Gnum)),
                     &fldedgecnptab, (size_t) (commmax * sizeof (Gnum)),
                     &requtab,       (size_t) (commmax * DGRAPHFOLDTAGNBR * sizeof (MPI_Request)), NULL) == NULL) {
    errorPrint ("dgraphFold2: out of memory (5)");
    goto fail1;
  }
  for (requnum = 0, requnbr = commmax * DGRAPHFOLDTAGNBR; requnum < requnbr; requnum ++) /* In case of error */
    requtab[requnum] = MPI_REQUEST_NULL;

  cheklocval = 0;                                 /* Everything went well */
fail1:
#ifdef SCOTCH_DEBUG_DGRAPH1                       /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, orggrafptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphFold2: communication error (1)");
    chekglbval = 1;
  }
#else /* SCOTCH_DEBUG_DGRAPH1 */
  chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_DGRAPH1 */
  if (chekglbval != 0) {
    if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) {
      if (fldvertidxtab != NULL)
        memFree (fldvertidxtab);                  /* Free group leader */
      if (fldcommdattab != NULL)
        memFree (fldcommdattab);
      dgraphExit (fldgrafptr);
    }
    return (1);
  }

  cheklocval = 1;                                 /* Assume an error */

  requnbr = 0;                                    /* Communications without further processing are placed at beginning of array */

  if ((fldcommtypval & DGRAPHFOLDCOMMSEND) != 0) { /* If process is (also) a sender */
    Gnum                vertsndbas;
    Gnum                vertsndnbr;
    int                 commnum;

    vertsndnbr = ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) ? (fldcommvrttab[0] - orgprocvrtbas) : 0; /* If process is also a receiver, start sending after kept vertices */

    for (commnum = 0, vertsndbas = orggrafptr->baseval; /* For all send communications to perform */
         (commnum < commmax) && (fldcommdattab[commnum].procnum != -1); commnum ++) {
      vertsndbas += vertsndnbr;
      vertsndnbr  = fldcommdattab[commnum].vertnbr;

      fldvertidxtab[commnum] = vertsndbas;
      fldedgeidxtab[commnum] = orgvertloctax[vertsndbas];
      fldedgecnptab[commnum] =                    /* Save fldedgecnttab in temporary array to read it while MPI communication in progress */
      fldedgecnttab[commnum] = orgvertloctax[vertsndbas + vertsndnbr] - orgvertloctax[vertsndbas]; /* Graph is compact                    */
      if (MPI_Isend (&fldedgecnptab[commnum], 1, GNUM_MPI, fldcommdattab[commnum].procnum,
                     TAGFOLD + TAGVLBLLOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("dgraphFold2: communication error (2)");
fail2:                                            /* Receivers must fail collectively at fail3, senders at fail4 */
        if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0)
          goto fail3;
        else
          goto fail4;
      }
    }
    commnbr = commnum;                            /* Record number of receives to perform for each data type */

    for (commnum = 0; commnum < commnbr; commnum ++) { /* Send vertex index data */
      if (MPI_Isend (orgvertloctax + fldvertidxtab[commnum],
                     fldcommdattab[commnum].vertnbr, GNUM_MPI, fldcommdattab[commnum].procnum,
                     TAGFOLD + TAGVERTLOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("dgraphFold2: communication error (3)");
        goto fail2;
      }
    }
    for (commnum = 0; commnum < commnbr; commnum ++) { /* Send edge data */
      if (MPI_Isend (orgedgeloctax + fldedgeidxtab[commnum],
                     fldedgecnttab[commnum], GNUM_MPI, fldcommdattab[commnum].procnum,
                     TAGFOLD + TAGEDGELOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
        errorPrint ("dgraphFold2: communication error (4)");
        goto fail2;
      }
    }
    if (orggrafptr->veloloctax != NULL) {         /* If vertex load array present */
      for (commnum = 0; commnum < commnbr; commnum ++) { /* Send vertex load data */
        if  (MPI_Isend (orggrafptr->veloloctax + fldvertidxtab[commnum],
                        fldcommdattab[commnum].vertnbr, GNUM_MPI, fldcommdattab[commnum].procnum,
                        TAGFOLD + TAGVELOLOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold2: communication error (5)");
          goto fail2;
        }
      }
    }
    if (orgedloloctax != NULL) {                  /* If (compact) edge load array present             */
      for (commnum = 0; commnum < commnbr; commnum ++) { /* Send edge load data (in case of checking) */
        if (MPI_Isend (orgedloloctax + fldedgeidxtab[commnum],
                       fldedgecnttab[commnum], GNUM_MPI, fldcommdattab[commnum].procnum,
                       TAGFOLD + TAGEDLOLOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold2: communication error (6)");
          goto fail2;
        }
      }
    }
    for (commnum = 0; commnum < commnbr; commnum ++) { /* Send data we do not need to wait for */
      Gnum                vertsndbas;
      int                 vertsndnbr;
      int                 procsndnum;             /* Rank of process to send to */

      vertsndbas = fldvertidxtab[commnum];
      vertsndnbr = (int) fldcommdattab[commnum].vertnbr;
      procsndnum = fldcommdattab[commnum].procnum;

      if ((orggrafptr->vnumloctax != NULL) &&
          (MPI_Isend (orggrafptr->vnumloctax + vertsndbas,
                      vertsndnbr, GNUM_MPI, procsndnum,
                      TAGFOLD + TAGVNUMLOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("dgraphFold2: communication error (7)");
        goto fail2;
      }
      if ((orgdataptr != NULL)  &&
          (MPI_Isend ((byte *) orgdataptr + ((vertsndbas - orggrafptr->baseval) * datasiz),
                      vertsndnbr, datatype, procsndnum,
                      TAGFOLD + TAGDATALOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
        errorPrint ("dgraphFold2: communication error (8)");
        goto fail2;
      }
    }
  }                                               /* Communications of sender-receivers will be completed in the receiving phase */

  if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0) { /* If process is (also) a receiver */
    Gnum                orgvertlocnbr;
    Gnum                orgvertlocnnd;
    Gnum                orgvertlocmin;
    Gnum                orgvertlocmax;
    Gnum                fldvertlocadj;
    Gnum                fldvelolocsum;
    Gnum * restrict     fldedgeloctax;
    Gnum                fldedgelocnum;
    Gnum                fldedgelocnnd;
    int                 fldprocnum;
    int                 procngbmin;
    int                 procngbmax;
    int                 commnum;

    fldgrafptr->procvrttab = fldgrafptr->procdsptab; /* Graph does not have holes                               */
    fldgrafptr->procdsptab[0] = orggrafptr->baseval; /* Build private data of folded graph and array            */
    for (fldprocnum = 0; fldprocnum < fldprocglbnbr; fldprocnum ++) /* New subdomain indices start from baseval */
      fldgrafptr->procdsptab[fldprocnum + 1] = fldgrafptr->procdsptab[fldprocnum] + fldgrafptr->proccnttab[fldprocnum];

    if ((fldcommtypval & DGRAPHFOLDCOMMSEND) == 0) { /* If process is a normal receiver   */
      Gnum                fldedgelocsiz;          /* Number of edges in folded graph      */
      Gnum                fldedlolocsiz;          /* Number of edge loads in folded graph */
      Gnum                fldedgelocbas;
      Gnum                fldvertrcvbas;
      Gnum                fldvertrcvnbr;

      for (commnum = 0, fldvertrcvbas = orggrafptr->vertlocnnd, fldvertrcvnbr = 0; /* For all receive communications to perform */
           commnum < commnbr; commnum ++) {
        fldvertrcvbas += fldvertrcvnbr;
        fldvertrcvnbr  = fldcommdattab[commnum].vertnbr;

        fldvertidxtab[commnum] = fldvertrcvbas;
        if (MPI_Irecv (&fldedgecnttab[commnum], 1, GNUM_MPI, fldcommdattab[commnum].procnum, /* Receive edge counts */
                       TAGFOLD + TAGVLBLLOCTAB, orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGESIZ * commmax + commnum]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold2: communication error (9)");
          goto fail3;
        }
      }

      for (commnum = 0; commnum < commnbr; commnum ++) { /* Let these communications progress while we process the edge size messages */
        if (MPI_Irecv (fldgrafptr->vertloctax + fldvertidxtab[commnum],
                       fldcommdattab[commnum].vertnbr, GNUM_MPI, fldcommdattab[commnum].procnum,
                       TAGFOLD + TAGVERTLOCTAB, orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGVERT * commmax + commnum]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold2: communication error (10)");
          goto fail3;
        }
      }

      MPI_Waitall (commnbr, &requtab[DGRAPHFOLDTAGESIZ * commmax], MPI_STATUSES_IGNORE); /* Wait until all edge counts received */

      for (commnum = 0, fldedgelocbas = orgvertloctax[orggrafptr->vertlocnnd]; commnum < commnbr; commnum ++) {
        fldedgeidxtab[commnum] = fldedgelocbas;
        fldedgelocbas += fldedgecnttab[commnum]; /* Accumulate received edges in addition to local edges */
      }
      fldedgelocsiz = fldedgelocbas - orggrafptr->baseval; /* Get number of local and received edges */
      fldgrafptr->edgelocnbr =
      fldgrafptr->edgelocsiz = fldedgelocsiz;

      fldedlolocsiz = ((orgedloloctax != NULL) ? fldedgelocsiz : 0);
      if ((fldgrafptr->edgeloctax = memAlloc ((fldedgelocsiz + fldedlolocsiz) * sizeof (Gnum))) == NULL) { /* Allocate single array for both edge arrays */
        errorPrint ("dgraphFold2: out of memory (6)");
        goto fail3;
      }
      fldgrafptr->edgeloctax -= orggrafptr->baseval; /* Do not care about the validity of edloloctax at this stage */

      for (commnum = 0; commnum < commnbr; commnum ++) { /* Receive edge adjacency data */
        if (MPI_Irecv (fldgrafptr->edgeloctax + fldedgeidxtab[commnum],
                       fldedgecnttab[commnum], GNUM_MPI, fldcommdattab[commnum].procnum,
                       TAGFOLD + TAGEDGELOCTAB, orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGEDGE * commmax + commnum]) != MPI_SUCCESS) {
          errorPrint ("dgraphFold2: communication error (11)");
          goto fail3;
        }
      }

      if (orggrafptr->veloloctax != NULL) {       /* Post as group as will be waited for in case of checking */
        for (commnum = 0; commnum < commnbr; commnum ++) {
          if (MPI_Irecv (fldgrafptr->veloloctax + fldvertidxtab[commnum],
                         fldcommdattab[commnum].vertnbr, GNUM_MPI, fldcommdattab[commnum].procnum,
                         TAGFOLD + TAGVELOLOCTAB, orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGVELO * commmax + commnum]) != MPI_SUCCESS) {
            errorPrint ("dgraphFold2: communication error (12)");
            goto fail3;
          }
        }
      }
      if (orgedloloctax != NULL) {
        fldgrafptr->edloloctax = fldgrafptr->edgeloctax + fldgrafptr->edgelocnbr; /* Set start index of edge load array */

        for (commnum = 0; commnum < commnbr; commnum ++) { /* Receive edge load data */
          if (MPI_Irecv (fldgrafptr->edloloctax + fldedgeidxtab[commnum],
                         fldedgecnttab[commnum], GNUM_MPI, fldcommdattab[commnum].procnum,
                         TAGFOLD + TAGEDLOLOCTAB, orggrafptr->proccomm, &requtab[DGRAPHFOLDTAGEDLO * commmax + commnum]) != MPI_SUCCESS) {
            errorPrint ("dgraphFold2: communication error (13)");
            goto fail3;
          }
        }
      }
      for (commnum = 0; commnum < commnbr; commnum ++) { /* Receive data we do not need to wait for */
        int                 procrcvnum;
        Gnum                vertrcvnbr;

        procrcvnum = fldcommdattab[commnum].procnum;
        vertrcvnbr = fldcommdattab[commnum].vertnbr;
        if ((orggrafptr->vnumloctax != NULL) &&
            (MPI_Irecv (fldgrafptr->vnumloctax + fldvertidxtab[commnum],
                        vertrcvnbr, GNUM_MPI, procrcvnum,
                        TAGFOLD + TAGVNUMLOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
          errorPrint ("dgraphFold2: communication error (14)");
          goto fail3;
        }
        if ((orgdataptr != NULL) &&
            (MPI_Irecv ((byte *) (*flddataptr) + ((fldvertidxtab[commnum] - orggrafptr->baseval) * datasiz),
                        vertrcvnbr, datatype, procrcvnum,
                        TAGFOLD + TAGDATALOCTAB, orggrafptr->proccomm, &requtab[requnbr ++]) != MPI_SUCCESS)) {
          errorPrint ("dgraphFold2: communication error (15)");
          goto fail3;
        }
      }

      orgvertlocnbr = orggrafptr->vertlocnbr;     /* Process all local vertices */
      orgvertlocnnd = orggrafptr->vertlocnnd;

      if (orggrafptr->vnumloctax == NULL) {       /* If original graph does not have vertex numbers, create remote parts of vertex number array */
        Gnum                fldvertlocnum;
        Gnum                fldvertlocadj;
        int                 commnum;

        Gnum * restrict const fldvnumloctax = fldgrafptr->vnumloctax;

        for (commnum = 0, fldvertlocnum = orgvertlocnnd; commnum < commnbr; commnum ++) {
          Gnum                fldvertlocnnd;

          for (fldvertlocnnd = fldvertlocnum + fldcommdattab[commnum].vertnbr, fldvertlocadj = fldcommvrttab[commnum];
               fldvertlocnum < fldvertlocnnd; fldvertlocnum ++)
            fldvnumloctax[fldvertlocnum] = fldvertlocadj ++;
        }
      }

      fldedgelocnnd = orgvertloctax[orggrafptr->vertlocnnd];
      fldvelolocsum = orggrafptr->velolocsum;     /* In case there are vertex loads, we keep all of existing load */
    }
    else {                                        /* Receiver process is also a sender     */
      orgvertlocnbr = fldvertlocnbr;              /* Process only remaining local vertices */
      orgvertlocnnd = fldvertlocnbr + orggrafptr->baseval;

      if (orggrafptr->veloloctax != NULL) {       /* If original graph has vertex loads */
        Gnum                fldvertlocnum;

        for (fldvertlocnum = orggrafptr->baseval, fldvelolocsum = 0; /* Accumulate load sum of remaining part */
             fldvertlocnum < orgvertlocnnd; fldvertlocnum ++)
          fldvelolocsum += orggrafptr->veloloctax[fldvertlocnum];
      }
      fldedgelocnnd = orgvertloctax[orgvertlocnnd]; /* Reorder remaining local part of edge array */
      if (orgedloloctax != NULL)
        fldgrafptr->edloloctax = fldgrafptr->edgeloctax + fldgrafptr->edgelocnbr; /* Set start index of edge load array */

      commnbr = 0;                                /* Turn sender-receiver into normal receiver without any communications to perform */
    }

    for (procngbmin = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
         procngbmax - procngbmin > 1; ) {
      int                 procngbmed;

      procngbmed = (procngbmax + procngbmin) / 2;
      if (fldvertadjtab[procngbmed] <= orgprocvrtbas)
        procngbmin = procngbmed;
      else
        procngbmax = procngbmed;
    }
    orgvertlocmin = fldvertadjtab[procngbmin];
    orgvertlocmax = fldvertadjtab[procngbmax];
    fldvertlocadj = fldvertdlttab[procngbmin];
    fldedgeloctax = fldgrafptr->edgeloctax;
    for (fldedgelocnum = orggrafptr->baseval; fldedgelocnum < fldedgelocnnd; fldedgelocnum ++) {
      Gnum                orgvertlocend;

      orgvertlocend = orgedgeloctax[fldedgelocnum];

      if ((orgvertlocend >= orgvertlocmin) &&     /* If end vertex is local */
          (orgvertlocend <  orgvertlocmax))
        fldedgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
      else {                                      /* End vertex is not local */
        int                 procngbnum;
        int                 procngbmax;

        for (procngbnum = 0, procngbmax = fldvertadjnbr;
             procngbmax - procngbnum > 1; ) {
          int                 procngbmed;

          procngbmed = (procngbmax + procngbnum) / 2;
          if (fldvertadjtab[procngbmed] <= orgvertlocend)
            procngbnum = procngbmed;
          else
            procngbmax = procngbmed;
        }
        fldedgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbnum];
      }
    }

    if (orggrafptr->veloloctax != NULL)           /* If original graph has vertex loads             */
      memCpy (fldgrafptr->veloloctax + orggrafptr->baseval, /* Copy local part of vertex load array */
              orggrafptr->veloloctax + orggrafptr->baseval, orgvertlocnbr * sizeof (Gnum));

    if (orgedloloctax != NULL)                    /* If original graph has edge loads             */
      memCpy (fldgrafptr->edloloctax + orggrafptr->baseval, /* Copy local part of edge load array */
              orgedloloctax + orggrafptr->baseval, (orgvertloctax[orgvertlocnnd] - orggrafptr->baseval) * sizeof (Gnum));

    if (orggrafptr->vnumloctax != NULL)           /* If original graph has vertex numbers             */
      memCpy (fldgrafptr->vnumloctax + orggrafptr->baseval, /* Copy local part of vertex number array */
              orggrafptr->vnumloctax + orggrafptr->baseval, orgvertlocnbr * sizeof (Gnum));
    else {                                        /* Build local part of vertex number array */
      Gnum                fldvertlocnum;
      Gnum                fldvertlocadj;

      for (fldvertlocnum = orggrafptr->baseval, fldvertlocadj = orgprocvrtbas;
           fldvertlocnum < orgvertlocnnd; fldvertlocnum ++)
        fldgrafptr->vnumloctax[fldvertlocnum] = fldvertlocadj ++;
    }

    memCpy (fldgrafptr->vertloctax + orggrafptr->baseval, /* Copy local part of vertex array, since it is compact */
            orgvertloctax + orggrafptr->baseval, orgvertlocnbr * sizeof (Gnum)); /* Last value is not copied      */
    fldgrafptr->vertloctax[fldvertlocnbr + orggrafptr->baseval] = fldgrafptr->edgelocnbr + orggrafptr->baseval;

    if (orgdataptr != NULL)                       /* If additional data present */
      memCpy ((byte *) (*flddataptr), (byte *) orgdataptr, orgvertlocnbr * datasiz); /* Copy local part */

    for (commnum = 0; commnum < commnbr; commnum ++) { /* Wait for, and adjust, remote vertex sub-arrays */
      Gnum                fldvertlocnum;
      Gnum                fldvertlocnnd;
      Gnum                fldvertlocadj;
      int                 commidx;                /* Index of communication in request array */

      if (MPI_Waitany (commnbr, &requtab[DGRAPHFOLDTAGVERT * commmax], &commidx, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
        errorPrint ("dgraphFold2: communication error (16)");
        goto fail2;
      }

      Gnum * restrict const fldvertloctax = fldgrafptr->vertloctax;

      fldvertlocnum = fldvertidxtab[commidx];
      fldvertlocadj = fldedgeidxtab[commidx] - fldgrafptr->vertloctax[fldvertlocnum];

      for (fldvertlocnnd = fldvertlocnum + fldcommdattab[commidx].vertnbr; fldvertlocnum < fldvertlocnnd; fldvertlocnum ++)
        fldvertloctax[fldvertlocnum] += fldvertlocadj;
    }

    for (commnum = 0; commnum < commnbr; commnum ++) { /* Wait for, and adjust, remote edge sub-arrays */
      Gnum                orgvertlocmin;
      Gnum                orgvertlocmax;
      Gnum                fldvertlocadj;
      Gnum * restrict     fldedgeloctax;
      int                 procngbnum;
      int                 procngbmax;
      int                 commidx;                /* Index of communication in request array */
      MPI_Status          statdat;

      if (MPI_Waitany (commnbr, &requtab[DGRAPHFOLDTAGEDGE * commmax], &commidx, &statdat) != MPI_SUCCESS) {
        errorPrint ("dgraphFold2: communication error (17)");
        goto fail3;
      }

#ifdef SCOTCH_DEBUG_DGRAPH2
      int                 fldedgercvnbr;

      MPI_Get_count (&statdat, GNUM_MPI, &fldedgercvnbr);
      if (fldedgercvnbr != fldedgecnttab[commidx]) {
        errorPrint ("dgraphFold2: internal error (1)");
        goto fail3;
      }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

      for (procngbnum = 0, procngbmax = fldvertadjnbr; /* Initialize search accelerator */
           procngbmax - procngbnum > 1; ) {
        int                 procngbmed;

        procngbmed = (procngbmax + procngbnum) / 2;
        if (fldvertadjtab[procngbmed] <= fldcommvrttab[commidx])
          procngbnum = procngbmed;
        else
          procngbmax = procngbmed;
      }
      orgvertlocmin = fldvertadjtab[procngbnum];
      orgvertlocmax = fldvertadjtab[procngbmax];
      fldvertlocadj = fldvertdlttab[procngbnum];
      fldedgeloctax = fldgrafptr->edgeloctax;
      for (fldedgelocnum = fldedgeidxtab[commidx], fldedgelocnnd = fldedgelocnum + fldedgecnttab[commidx];
           fldedgelocnum < fldedgelocnnd; fldedgelocnum ++) { /* Reorder end vertices */
        Gnum                orgvertlocend;

#ifdef SCOTCH_DEBUG_DGRAPH2
        if (fldedgelocnum >= (fldgrafptr->edgelocnbr + orggrafptr->baseval)) {
          errorPrint ("dgraphFold2: internal error (2)");
          goto fail3;
        }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

        orgvertlocend = fldedgeloctax[fldedgelocnum];

        if ((orgvertlocend >= orgvertlocmin) &&   /* If end vertex is local */
            (orgvertlocend <  orgvertlocmax))
          fldedgeloctax[fldedgelocnum] = orgvertlocend + fldvertlocadj;
        else {
          int                 procngbnum;
          int                 procngbmax;

          for (procngbnum = 0, procngbmax = fldvertadjnbr;
               procngbmax - procngbnum > 1; ) {
            int                 procngbmed;

            procngbmed = (procngbmax + procngbnum) / 2;
            if (fldvertadjtab[procngbmed] <= orgvertlocend)
              procngbnum = procngbmed;
            else
              procngbmax = procngbmed;
          }
          fldedgeloctax[fldedgelocnum] = orgvertlocend + fldvertdlttab[procngbnum];
        }
      }
    }

    if (orggrafptr->veloloctax == NULL)           /* If no vertex loads, reset graph vertex load to number of vertices */
      fldvelolocsum = fldvertlocnbr;
    else {                                        /* Graph has vertex loads and load of local part has already been computed */
      for (commnum = 0; commnum < commnbr; commnum ++) { /* Accumulate vertex loads for received vertex load array */
        Gnum                fldvertlocnum;
        Gnum                fldvertlocnnd;
        int                 commidx;

        if (MPI_Waitany (commnbr, &requtab[DGRAPHFOLDTAGVELO * commmax], &commidx, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
          errorPrint ("dgraphFold2: communication error (18)");
          goto fail3;
        }

        for (fldvertlocnum = fldvertidxtab[commidx], fldvertlocnnd = fldvertlocnum + fldcommdattab[commidx].vertnbr;
             fldvertlocnum < fldvertlocnnd; fldvertlocnum ++)
          fldvelolocsum += fldgrafptr->veloloctax[fldvertlocnum];
      }
    }

    if ((fldcommtypval & DGRAPHFOLDCOMMSEND) == 0) { /* If process is a pure receiver */
      if (orgedloloctax != NULL) {
        if (MPI_Waitall (commnbr, &requtab[DGRAPHFOLDTAGEDLO * commmax], MPI_STATUSES_IGNORE) != MPI_SUCCESS) { /* Wait for edge load sub-arrays */
          errorPrint ("dgraphFold2: communication error (19)");
          goto fail3;
        }
      }
    }

    fldgrafptr->baseval    = orggrafptr->baseval;
    fldgrafptr->vertlocnbr = fldvertlocnbr;
    fldgrafptr->vertlocnnd = fldvertlocnbr + orggrafptr->baseval;
    fldgrafptr->velolocsum = fldvelolocsum;
    fldgrafptr->degrglbmax = orggrafptr->degrglbmax;
    if (dgraphBuild4 (fldgrafptr) != 0) {
      errorPrint ("dgraphFold2: cannot build folded graph");
      goto fail3;
    }

    cheklocval = 0;                               /* Everything went well for receivers */
fail3:
#ifdef SCOTCH_DEBUG_DGRAPH1                       /* Communication cannot be merged with a useful one */
    if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, fldproccomm) != MPI_SUCCESS) {
      errorPrint ("dgraphFold2: communication error (20)"); /* Communication among receivers only */
      chekglbval = 1;
    }
#else /* SCOTCH_DEBUG_DGRAPH1 */
    chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_DGRAPH1 */
    cheklocval = 1;                               /* Assume an error for receivers            */
    if (chekglbval != 0)                          /* All receivers fail before checking graph */
      goto fail4;

#ifdef SCOTCH_DEBUG_DGRAPH2
    if (dgraphCheck (fldgrafptr) != 0) {          /* Check graph consistency; vnumloctab is not checked so no need to wait for it */
      errorPrint ("dgraphFold2: internal error (3)");
      dgraphExit (fldgrafptr);
      goto fail4;
    }
#endif /* SCOTCH_DEBUG_DGRAPH2 */
  }

  memFree (fldcommdattab);                        /* Free group leader */
  fldcommdattab = NULL;                           /* In case of error  */

  cheklocval = 0;                                 /* Everything went well */

  if (MPI_Waitall (requnbr, requtab, MPI_STATUSES_IGNORE) != MPI_SUCCESS) { /* Wait for all graph data to arrive because graph could be freed afterwards */
    errorPrint ("dgraphFold2: communication error (21)");
    cheklocval = 1;
  }

  memFree (fldvertidxtab);                        /* Free group leader including request array */
  if (orgvertloctax != orggrafptr->vertloctax)
    memFree (orgvertloctax + orggrafptr->baseval);

fail4:
#ifdef SCOTCH_DEBUG_DGRAPH1                       /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, orggrafptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dgraphFold2: communication error (22)");
    chekglbval = 1;
  }
#else /* SCOTCH_DEBUG_DGRAPH1 */
  chekglbval = cheklocval;
#endif /* SCOTCH_DEBUG_DGRAPH1 */
  if (chekglbval != 0) {
    if ((fldcommtypval & DGRAPHFOLDCOMMRECV) != 0)
      dgraphExit (fldgrafptr);
  }

  return (chekglbval);
}
