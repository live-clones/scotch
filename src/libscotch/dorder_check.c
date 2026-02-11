/* Copyright 2025 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : dorder_check.c                          **/
/**                                                        **/
/**   AUTHOR     : Clement BARTHELEMY                      **/
/**                Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module checks the consistency of   **/
/**                distributed orderings.                  **/
/**                                                        **/
/**   DATES      : # Version 7.0  : from : 29 sep 2025     **/
/**                                 to   : 30 dec 2025     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "module.h"
#include "common.h"
#include "dgraph.h"
#include "dgraph_allreduce.h"
#include "order.h"
#include "dorder.h"

/************************************************/
/*                                              */
/* These routines check distributed orderings.  */
/*                                              */
/************************************************/

/* This routine checks the consistency
** of a direct or inverse permutation.
** It returns:
** - 0   : if permutation data is consistent.
** - !0  : on error.
*/

static
int
dorderCheck2 (
const Gnum                  baseval,
const Gnum                  vnodglbnbr,
Gnum *                      flagloctab,
const Gnum                  sortsndnbr,
Gnum *                      sortsndtab,
Gnum                        sortrcvsiz,
Gnum *                      sortrcvtab,
MPI_Comm                    proccomm,
const char * const          typestr)
{
  Gnum *            flagloctax;                   /* Based flag array                          */
  Gnum              perxlocbas;                   /* Start value for local permutation segment */
  Gnum              perxlocnnd;                   /* After-end value for permutation segment   */
  Gnum              perxlocnum;
  int               procglbnbr;                   /* Number of processes in communicator       */
  int               proclocnum;                   /* Local process number                      */
  int               procnum;
  int               cheklocval;
  int               chekglbval;

  MPI_Comm_size (proccomm, &procglbnbr);
  MPI_Comm_rank (proccomm, &proclocnum);

  perxlocbas = DATASCAN (vnodglbnbr, procglbnbr, proclocnum) + baseval; /* Spread evenly inverse permutation values for checking */
  perxlocnnd = DATASCAN (vnodglbnbr, procglbnbr, proclocnum + 1) + baseval;

  memSet (flagloctab, ~0, (perxlocnnd - perxlocbas) * sizeof (Gnum)); /* Initialize permutation flag array */
  flagloctax = flagloctab - perxlocbas;

  cheklocval = 0;                                 /* Assume everything will be all right */

  for (procnum = 0; procnum < procglbnbr; procnum ++) { /* For all sender processing elements */
    MPI_Request         requtab[2];
    MPI_Status          statdat;
    Gnum                perxsndbas;               /* Start inverse permutation value for segment     */
    Gnum                perxsndnnd;               /* After-end inverse permutation value for segment */
    int                 procsndnum;               /* Process to send to during this pass             */
    Gnum                sortsndbas;               /* Start index for sending segment                 */
    Gnum                sortsndnnd;               /* After-end index for sending segment             */
    int                 sortrcvnbr;               /* MPI count of received message; hence (int)      */
    int                 sortrcvnum;

    procsndnum = (proclocnum + procnum) % procglbnbr; /* Destination process for this round */

    perxsndbas = DATASCAN (vnodglbnbr, procglbnbr, procsndnum) + baseval;
    perxsndnnd = DATASCAN (vnodglbnbr, procglbnbr, procsndnum + 1) + baseval;

    if ((sortsndnbr == 0) ||                      /* If send array is empty                */
        (sortsndtab[0]              >= perxsndnnd) || /* Or its contents are out of bounds */
        (sortsndtab[sortsndnbr - 2] <  perxsndbas)) {
      sortsndbas =                                /* Noting to send */
      sortsndnnd = 0;
    }
    else {                                        /* Must find out what to send        */
      Gnum                sortsndmin;             /* Minimum index for sending segment */

      for (sortsndmin = 0, sortsndbas = sortsndnbr; /* Find start index by dichotomy               */
           sortsndbas - sortsndmin > 2; ) {       /* Dichotomy on array of number pairs; hence "2" */
        Gnum                sortsndmed;

        sortsndmed = ((sortsndmin + sortsndbas) / 4) * 2; /* TRICK: Allow for dichotomy on multiples of 2 (zero last bit) */
        if (sortsndtab[sortsndmed] < perxsndbas)
          sortsndmin = sortsndmed;
        else
          sortsndbas = sortsndmed;
      }
      while ((sortsndbas > 0) && (sortsndtab[sortsndbas - 2] >= perxsndbas)) /* Search for first instance */
        sortsndbas -= 2;

      for (sortsndmin = sortsndbas, sortsndnnd = sortsndnbr; /* Find after-end index by dichotomy  */
           sortsndnnd - sortsndmin > 2; ) {       /* Dichotomy on array of number pairs; hence "2" */
        Gnum                sortsndmed;

        sortsndmed = ((sortsndmin + sortsndnnd + 2) / 4) * 2; /* TRICK: Allow for dichotomy on multiples of 2 (zero last bit) */
        if (sortsndtab[sortsndmed] < perxsndnnd)
          sortsndmin = sortsndmed;
        else
          sortsndnnd = sortsndmed;
      }
      while ((sortsndnnd < sortsndnbr) && (sortsndtab[sortsndnnd] < perxsndnnd)) /* Search for last instance */
        sortsndnnd += 2;
    }

    if (MPI_Irecv (sortrcvtab, (int) sortrcvsiz, GNUM_MPI,
                   MPI_ANY_SOURCE, procnum, proccomm, &requtab[1]) != MPI_SUCCESS) {
      errorPrint ("dorderCheck2: communication error (1)");
      return (1);
    }
    if (MPI_Isend (sortsndtab + sortsndbas, (int) (sortsndnnd - sortsndbas), GNUM_MPI,
                   procsndnum, procnum, proccomm, &requtab[0]) != MPI_SUCCESS) {
      errorPrint ("dorderCheck2: communication error (2)");
      return (1);
    }
    if (MPI_Wait (&requtab[1], &statdat) != MPI_SUCCESS) { /* Wait for receive to complete before processing received array */
      errorPrint ("dorderCheck2: communication error (3)");
      return (1);
    }
    if (MPI_Get_count (&statdat, GNUM_MPI, &sortrcvnbr) != MPI_SUCCESS) { /* Wait for receive to complete before processing received array */
      errorPrint ("dorderCheck2: communication error (4)");
      return (1);
    }

    for (sortrcvnum = 0; sortrcvnum < sortrcvnbr; ) {
      Gnum                perxlocnum;

      perxlocnum = sortrcvtab[sortrcvnum ++];
#ifdef SCOTCH_DEBUG_DORDER2
      if ((perxlocnum <  perxlocbas) ||
          (perxlocnum >= perxlocnnd)) {
        errorPrint ("dorderCheck2: internal error");
        cheklocval = 1;                           /* We can exit nicely */
        goto abort;
      }
#endif /* SCOTCH_DEBUG_DORDER2 */
      if (flagloctax[perxlocnum] != ~0) {         /* If permutation index already assigned */
        errorPrint ("dorderCheck2: duplicate global %s permutation index", typestr);
        cheklocval = 1;                           /* We can exit nicely */
        goto abort;
      }
      flagloctax[perxlocnum] = sortrcvtab[sortrcvnum ++]; /* Record index in inverse permutation */
    }

    if (MPI_Wait (&requtab[0], &statdat) != MPI_SUCCESS) { /* Wait for send to complete before next pass */
      errorPrint ("dorderCheck2: communication error (5)");
      return (1);
    }
  }

  for (perxlocnum = perxlocbas; perxlocnum < perxlocnnd; perxlocnum ++) { /* For all indices in local permutation array */
    if (flagloctax[perxlocnum] == ~0) {           /* Unused index */
      errorPrint ("dorderCheck2: missing global %s permutation index", typestr);
      cheklocval = 1;                             /* We can exit nicely */
      goto abort;
    }
  }

abort:                                            /* Check whether all of the above succeeded */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderCheck2: communication error (6)");
    return (1);
  }

  return (chekglbval);
}

/* This routine checks the consistency
** of the given distributed ordering.
** It returns:
** - 0   : if ordering data is consistent.
** - !0  : on error.
*/

DGRAPHALLREDUCEMAXSUMOP (3, 1)

int
dorderCheck (
const Dorder * const  dordptr)
{
  Gnum              baseval;
  Gnum              cblklocnbr;                   /* Local number of column blocks in ordering         */
  Gnum              vnodlocnbr;                   /* Local number of node vertices in ordering         */
  Gnum              vnodglbnbr;                   /* Global number of node vertices in ordering        */
  Gnum              vnodglbmax;                   /* Maximum number of local node vertices             */
  DorderLink *      linklocptr;                   /* Pointer to traverse local column blocks           */
  Gnum *            flagloctab;                   /* Inverse permutation array used as flag array      */
  Gnum *            sortrcvtab;                   /* Receive array for inverse permutation pairs       */
  Gnum *            sortsndtab;                   /* Send array for inverse permutation pairs          */
  Gnum              sortsndnbr;                   /* Number of data in local send array                */
  Gnum              sortsndnum;
  Gnum              perircvnbr;                   /* Size of local inverse permutation segment         */
  Gnum              perircvbas;                   /* Start value for local inverse permutation segment */
  int               procglbnbr;                   /* Number of processes in communicator               */
  int               proclocnum;                   /* Local process number                              */
  int               cheklocval;
  int               chekglbval;
  Gnum              reduloctab[4];
  Gnum              reduglbtab[4];

  for (linklocptr = dordptr->linkdat.nextptr, vnodlocnbr = 0; /* Get local number of nodes */
       linklocptr != &dordptr->linkdat; linklocptr = linklocptr->nextptr) {
    const DorderCblk *  cblklocptr;

    cblklocptr = (DorderCblk *) linklocptr;
    if ((cblklocptr->typeval & DORDERCBLKLEAF) != 0) {
      Gnum                vnodloctmp;

      vnodloctmp = cblklocptr->data.leaf.vnodlocnbr;
      if (vnodloctmp < 0) {
        errorPrint ("dorderCheck: invalid column block size");
        vnodlocnbr = GNUMMAX;                     /* Set abort maximum value */
        break;
      }
      vnodlocnbr += vnodloctmp;                   /* Accumulate local number of vertices in leaves */
    }
  }

  MPI_Comm_size (dordptr->proccomm, &procglbnbr);
  MPI_Comm_rank (dordptr->proccomm, &proclocnum);
  if (proclocnum != dordptr->proclocnum) {
    errorPrint ("dorderCheck: inconsistent global ordering data (1)");
    vnodlocnbr = GNUMMAX;                         /* Set abort maximum value */
  }

  reduloctab[0] =   vnodlocnbr;                   /* Get maximum of send buffer sizes */
  reduloctab[1] =   dordptr->baseval;             /* Check consistency of baseval     */
  reduloctab[2] = - dordptr->baseval;
  reduloctab[3] =   vnodlocnbr;                   /* Accumulate number of node vertices */
  if (dgraphAllreduceMaxSum (reduloctab, reduglbtab, 3, 1, dordptr->proccomm) != 0) {
    errorPrint ("dorderCheck: communication error (1)");
    return (1);
  }
  vnodglbmax = reduglbtab[0];
  vnodglbnbr = reduglbtab[3];
  if (vnodglbmax == GNUMMAX)                      /* If need to abort */
    return (1);

  cheklocval = 0;                                 /* Assume everything will be all right */
  flagloctab = NULL;

  if ((vnodglbnbr    != dordptr->vnodglbnbr) ||   /* If inconsistent vnodglbnbr */
      (reduglbtab[2] != - reduglbtab[1])) {       /* Or if inconsistent baseval */
    errorPrint ("dorderCheck: inconsistent global ordering data (2)");
    cheklocval = 1;
    goto abort1;
  }

  perircvbas = DATASCAN (vnodglbnbr, procglbnbr, proclocnum); /* Spread evenly inverse permutation values for checking */
  perircvnbr = DATASCAN (vnodglbnbr, procglbnbr, proclocnum + 1) - perircvbas;
  if (memAllocGroup ((void **) (void *)
                     &flagloctab, (size_t) (perircvnbr * sizeof (Gnum)),
                     &sortsndtab, (size_t) (vnodlocnbr * sizeof (Gnum) * 2),
                     &sortrcvtab, (size_t) (vnodglbmax * sizeof (Gnum) * 2), NULL) == NULL) {
    errorPrint ("dorderCheck: out of memory");
    cheklocval = 1;
    goto abort1;
  }

  baseval = dordptr->baseval;
  for (linklocptr = dordptr->linkdat.nextptr, sortsndnbr = 0, cblklocnbr = 0; /* Fill send sort array */
       linklocptr != &dordptr->linkdat; linklocptr = linklocptr->nextptr) {
    const DorderCblk *  cblklocptr;               /* Pointer to current local column block */

    cblklocptr = (DorderCblk *) linklocptr;
    if (cblklocptr->cblknum.proclocnum == proclocnum) /* Count locally-rooted column blocks */
      cblklocnbr ++;

    if ((cblklocptr->typeval & DORDERCBLKLEAF) != 0) {
      Gnum                ordelocval;
      Gnum                nodeloctmp;
      Gnum                vnodloctmp;
      Gnum                vnodlocnum;

      nodeloctmp = cblklocptr->data.leaf.nodelocnbr;
      vnodloctmp = cblklocptr->data.leaf.vnodlocnbr;
      ordelocval = cblklocptr->data.leaf.ordelocval + baseval; /* Base ordering value */
      for (vnodlocnum = 0; vnodlocnum < vnodloctmp; vnodlocnum ++) {
        Gnum                perilocval;

        perilocval = cblklocptr->data.leaf.periloctab[vnodlocnum];
        if ((perilocval < baseval) ||             /* If based inverse permutation value is out of bounds */
            (perilocval >= (vnodglbnbr + baseval))) {
          errorPrint ("dorderCheck: invalid inverse permutation value");
          cheklocval = 1;
          goto abort1;
        }
#ifdef SCOTCH_DEBUG_DORDER2
        if (sortsndnbr >= (2 * vnodlocnbr)) {
          errorPrint ("dorderCheck: internal error (1)");
          cheklocval = 1;                         /* We can exit nicely */
          goto abort1;
        }
#endif /* SCOTCH_DEBUG_DORDER2 */
        sortsndtab[sortsndnbr ++] = ordelocval ++;
        sortsndtab[sortsndnbr ++] = perilocval;
      }

      if (nodeloctmp != 0) {                      /* If leaf has centralized column block node */
        Gnum                vnodleanbr;           /* Number of local node vertices in leaf     */
        const DorderNode *  nodelocptr;
        const DorderNode *  nodeloctnd;

        cblklocnbr += nodeloctmp;

        for (vnodleanbr = 0, nodelocptr = cblklocptr->data.leaf.nodeloctab,
             nodeloctnd = nodelocptr + cblklocptr->data.leaf.nodelocnbr;
             nodelocptr < nodeloctnd; nodelocptr ++) { /* For all local nodes */
          if (nodelocptr->fathnum == -1)          /* Count root only          */
            vnodleanbr += nodelocptr->vnodnbr;
        }
        if (vnodleanbr != cblklocptr->data.leaf.vnodlocnbr) {
          errorPrint ("dorderCheck: invalid number of node vertices in column block");
          cheklocval = 1;
          goto abort1;
        }
      }
    }
    else if ((cblklocptr->typeval & DORDERCBLKNEDI) != 0) {
      if ((cblklocptr->data.nedi.cblkglbnbr < 0) ||
          (cblklocptr->data.nedi.cblkglbnbr > 3)) {
        errorPrint ("dorderCheck: invalid number of descendent nodes");
        cheklocval = 1;
        goto abort1;
      }
    }
    else if ((cblklocptr->typeval & DORDERCBLKDICO) == 0) {
      errorPrint ("dorderCheck: invalid column block type");
      cheklocval = 1;
      goto abort1;
    }
  }
#ifdef SCOTCH_DEBUG_DORDER2
  if (sortsndnbr != (2 * vnodlocnbr)) {
    errorPrint ("dorderCheck: internal error (2)");
    cheklocval = 1;                               /* We can exit nicely */
    goto abort1;
  }
#endif /* SCOTCH_DEBUG_DORDER2 */

  if (cblklocnbr != dordptr->cblklocnbr) {
    errorPrint ("dorderCheck: invalid number of locally rooted column blocks");
    cheklocval = 1;
  }

abort1:                                           /* Check whether all of the above succeeded */
  if (MPI_Allreduce (&cheklocval, &chekglbval, 1, MPI_INT, MPI_MAX, dordptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderCheck: communication error (2)");
    return (1);
  }
  if (chekglbval != 0) {
    if (flagloctab != NULL)
      memFree (flagloctab);                       /* Free group leader */
    return (1);
  }

  intSort2asc1 (sortsndtab, sortsndnbr / 2);      /* Sort permutation send array */

  cheklocval = 1;                                 /* Assume an error */
  if (dorderCheck2 (baseval, vnodglbnbr, flagloctab, sortsndnbr, sortsndtab,
                    vnodglbmax * 2, sortrcvtab, dordptr->proccomm, "inverse") != 0)
    goto abort2;

  for (sortsndnum = 0; sortsndnum < sortsndnbr; sortsndnum += 2) { /* Swap global indices in send array for permutation */
    Gnum                tempval;

    tempval                    = sortsndtab[sortsndnum];
    sortsndtab[sortsndnum]     = sortsndtab[sortsndnum + 1];
    sortsndtab[sortsndnum + 1] = tempval;
  }
  intSort2asc1 (sortsndtab, sortsndnbr / 2);      /* Sort permutation send array */

  if (dorderCheck2 (baseval, vnodglbnbr, flagloctab, sortsndnbr, sortsndtab,
                    vnodglbmax * 2, sortrcvtab, dordptr->proccomm, "direct") != 0)
    goto abort2;

  cheklocval = 0;                                 /* Everything went all right */

abort2:
  memFree (flagloctab);                           /* Free group leader */

  return (cheklocval);
}
