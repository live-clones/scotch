/* Copyright 2026 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : test_libparmetis.c                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Clement BARTHELEMY                      **/
/**                                                        **/
/**   FUNCTION   : This module tests the operation of      **/
/**                the libscotchmetis routines.            **/
/**                                                        **/
/**   DATES      : # Version 7.0  : from : 17 aug 2026     **/
/**                                 to   : 17 aug 2026     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include <math.h>
#include <stdio.h>
#if (((defined __STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || (defined HAVE_STDINT_H))
#include <stdint.h>
#endif /* (((defined __STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || (defined HAVE_STDINT_H)) */
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

#include "../libscotch/module.h"
#include "../libscotch/common.h"

#include "ptscotch.h"
#include "parmetis.h"                             /* Our "parmetis.h" file */

static File                 C_fileTab[1] = {      /* File array */
                              { FILEMODER } };

/**********************************/
/*                                */
/* Consistency checking routines. */
/*                                */
/**********************************/

/* checkOrder checks the produced permutation.
** It exits on error.
*/

static
void
checkOrder (
const SCOTCH_Num            baseval,
const SCOTCH_Num            vertlocnbr,
const SCOTCH_Num            vertglbnbr,
const SCOTCH_Num * const    permloctab,
MPI_Comm                    proccomm)
{
  int                 procglbnbr;
  int                 proclocnum;
  SCOTCH_Num          vertnum;
  SCOTCH_Num *        permtab;
  SCOTCH_Num *        peritab;
  int *               dspltab;
  int *               countab;

  const SCOTCH_Num    vertnnd = vertglbnbr + baseval;
  MPI_Comm_size (proccomm, &procglbnbr);
  MPI_Comm_rank (proccomm, &proclocnum);

  if (proclocnum == 0) {
    if (((permtab = malloc (vertglbnbr * 2 * sizeof (SCOTCH_Num))) == NULL) ||
        ((dspltab = malloc (procglbnbr * 2 * sizeof (int))) == NULL)) {
      SCOTCH_errorPrint ("checkOrder: out of memory");
      exit (EXIT_FAILURE);
    }
    peritab = permtab + vertglbnbr;
    countab = dspltab + procglbnbr;
    memset (peritab, ~0, vertglbnbr * sizeof (SCOTCH_Num));
  }

  MPI_Gather (&vertlocnbr, 1, MPI_INT, countab, 1, MPI_INT, 0, proccomm); /* Possible int/SCOTCH_Num size mismatch, ignore */

  if (proclocnum == 0) {
    dspltab[0] = 0;
    for (int procnum = 1; procnum < procglbnbr; procnum ++)
      dspltab[procnum] = dspltab[procnum - 1] + countab[procnum - 1];
  }

  MPI_Gatherv (permloctab, vertlocnbr, SCOTCH_NUM_MPI, permtab, countab, dspltab, SCOTCH_NUM_MPI, 0, proccomm);

  if (proclocnum != 0)
    return;

  for (vertnum = 0; vertnum < vertglbnbr; vertnum ++) {  /* Un-based traversal */
    SCOTCH_Num          permval;

    permval = permtab[vertnum];
    if ((permval <  baseval) ||
        (permval >= vertnnd)) {
      SCOTCH_errorPrint ("checkOrder: invalid permutation value");
      free (permtab);
      free (dspltab);
      exit (EXIT_FAILURE);
    }
    if (peritab[permval - baseval] != ~0) {
      SCOTCH_errorPrint ("checkOrder: duplicate permutation value");
      free (permtab);
      free (dspltab);
      exit (EXIT_FAILURE);
    }
    peritab[permval - baseval] = vertnum + baseval;
  }

  for (vertnum = 0; vertnum < vertglbnbr; vertnum ++) {
    if (peritab[vertnum] == ~0) {
      SCOTCH_errorPrint ("checkOrder: missing permutation value");
      free (permtab);
      free (dspltab);
      exit (EXIT_FAILURE);
    }
  }

  free (permtab);
  free (dspltab);
}

/* This routine checks that the produced
** partition is valid.
** It exits on error.
*/

static
void
checkPart (
const SCOTCH_Num            baseval,
const SCOTCH_Num            vertnbr,
const SCOTCH_Num * const    parttab,
const SCOTCH_Num            partnbr)
{
  SCOTCH_Num          vertnum;

  for (vertnum = 0; vertnum < vertnbr; vertnum ++) { /* Un-based traversal */
    SCOTCH_Num          partval;

    partval = parttab[vertnum];
    if ((partval <   baseval) ||
        (partval >= (baseval + partnbr))) {
      SCOTCH_errorPrint ("checkPart: invalid partition");
      exit (EXIT_FAILURE);
    }
  }
}

/*********************/
/*                   */
/* The main routine. */
/*                   */
/*********************/

int
main (
int                 argc,
char *              argv[])
{
  MPI_Comm            proccomm;
  int                 procglbnbr;                 /* Number of processes sharing graph data */
  int                 proclocnum;                 /* Number of this process                 */

  SCOTCH_Dgraph       grafdat;
  SCOTCH_Num          baseval;
  SCOTCH_Num          vertglbnbr;
  SCOTCH_Num          vertlocnbr;
  SCOTCH_Num          vertlocmax;
  SCOTCH_Num          procvrtval;
  SCOTCH_Num          edgecutval;
  SCOTCH_Num *        vertloctab;
  SCOTCH_Num *        veloloctab;
  SCOTCH_Num *        edgeloctab;
  SCOTCH_Num *        edloloctab;
  SCOTCH_Num *        parttab;
  SCOTCH_Num *        sizetab;
  SCOTCH_Num *        vtxdist;
#if (SCOTCH_METIS_VERSION == 3)
  SCOTCH_Num          fwgtval;
  float *             tpwgtab;
#endif /* (SCOTCH_METIS_VERSION == 3) */
#ifdef SCOTCH_PTHREAD
  int                 thrdreqlvl;
  int                 thrdprolvl;
#endif /* SCOTCH_PTHREAD */

  const SCOTCH_Num    partnbr = 9;
#if (SCOTCH_METIS_VERSION == 5)
  const double        awgttab[9] = { 0.10, 0.10, 0.05, 0.10, 0.20, 0.30, 0.05, 0.05, 0.05 };
  const SCOTCH_Num    nconval    = 1;
  const double        kbaltab[1] = { 0.05 };
  SCOTCH_Num          options[METIS_NOPTIONS];
#endif /* (SCOTCH_METIS_VERSION == 5) */

  SCOTCH_errorProg (argv[0]);

#ifdef SCOTCH_PTHREAD
  thrdreqlvl = MPI_THREAD_MULTIPLE;
  if (MPI_Init_thread (&argc, &argv, thrdreqlvl, &thrdprolvl) != MPI_SUCCESS) {
    SCOTCH_errorPrint ("main: Cannot initialize (1)");
    exit (EXIT_FAILURE);
  }
#else /* SCOTCH_PTHREAD */
  if (MPI_Init (&argc, &argv) != MPI_SUCCESS) {
    SCOTCH_errorPrint ("main: Cannot initialize (2)");
    exit (EXIT_FAILURE);
  }
#endif /* SCOTCH_PTHREAD */

  if (argc != 2) {
    SCOTCH_errorPrint ("usage: %s graph_file", argv[0]);
    exit (EXIT_FAILURE);
  }

  proccomm = MPI_COMM_WORLD;
  MPI_Comm_size (proccomm, &procglbnbr);          /* Get communicator data */
  MPI_Comm_rank (proccomm, &proclocnum);

  SCOTCH_randomReset ();

  if (MPI_Barrier (proccomm) != MPI_SUCCESS) {    /* Synchronize for debug */
    SCOTCH_errorPrint ("main: communication error (1)");
    exit (EXIT_FAILURE);
  }

  fileBlockInit (C_fileTab, 1);                   /* Set default stream pointers */
  fileBlockName (C_fileTab, 0) = argv[1];         /* Use provided file           */

  if (SCOTCH_dgraphInit (&grafdat, proccomm) != 0) {
    SCOTCH_errorPrint ("main: cannot initialize graph");
    exit (EXIT_FAILURE);
  }

  fileBlockOpenDist (C_fileTab, 1, procglbnbr, proclocnum, 0); /* Open all files */

  if (SCOTCH_dgraphLoad (&grafdat, fileBlockFile (C_fileTab, 0), -1, 0) != 0) {
    SCOTCH_errorPrint ("main: cannot load graph");
    exit (EXIT_FAILURE);
  }

  SCOTCH_dgraphData (&grafdat, &baseval, &vertglbnbr, &vertlocnbr, &vertlocmax, NULL,
                     &vertloctab, NULL, &veloloctab, NULL, NULL, NULL, NULL,
                     &edgeloctab, NULL, &edloloctab, NULL);

  if (((parttab = malloc (vertlocnbr           * sizeof (SCOTCH_Num))) == NULL) ||
      ((tpwgtab = malloc (partnbr              * sizeof (SCOTCH_Num))) == NULL) ||
      ((vtxdist = malloc ((procglbnbr + 1) * 3 * sizeof (SCOTCH_Num))) == NULL)) {
    SCOTCH_errorPrint ("main: out of memory");
    exit (EXIT_FAILURE);
  }
  sizetab = vtxdist + procglbnbr + 1;

  for (int i = 0; i < partnbr; i++)
    tpwgtab[i] = 1.0 / partnbr;

  if (MPI_Scan (&vertlocnbr, &procvrtval, 1, SCOTCH_NUM_MPI, MPI_SUM, proccomm) != MPI_SUCCESS) {
    SCOTCH_errorPrint ("main: communication error (2)");
    exit (EXIT_FAILURE);
  }
  vtxdist[0] = baseval;
  if (MPI_Allgather (&(SCOTCH_Num){procvrtval + baseval}, 1, SCOTCH_NUM_MPI, vtxdist + 1, 1, SCOTCH_NUM_MPI, proccomm) != MPI_SUCCESS) {
    SCOTCH_errorPrint ("main: communication error (3)");
    exit (EXIT_FAILURE);
  }

#if (SCOTCH_METIS_VERSION == 3)
  fwgtval = ((veloloctab != NULL) ? 2 : 0) +
            ((edloloctab != NULL) ? 1 : 0);

  if (SCOTCHMETISNAMES (ParMETIS_V3_PartKway) (vtxdist, vertloctab, edgeloctab, veloloctab, edloloctab, &fwgtval, &baseval, NULL, &partnbr, tpwgtab, NULL, NULL, &edgecutval, parttab, &proccomm) != METIS_OK) {
    SCOTCH_errorPrint ("main: error in ParMETIS_V3_PartKway");
    exit (EXIT_FAILURE);
  }

  checkPart (baseval, vertlocnbr, parttab, partnbr);

  if (SCOTCHMETISNAMES (ParMETIS_V3_NodeND) (vtxdist, vertloctab, edgeloctab, &baseval, NULL, parttab, sizetab, &proccomm) != METIS_OK) {
    SCOTCH_errorPrint ("main: error in ParMETIS_V3_NodeND");
    exit (EXIT_FAILURE);
  }

  checkOrder (baseval, vertlocnbr, vertglbnbr, parttab, proccomm);
#endif /* (SCOTCH_METIS_VERSION == 3) */

  if (MPI_Barrier (proccomm) != MPI_SUCCESS) {    /* Synchronize for debug */
    SCOTCH_errorPrint ("main:  communication error (4)");
    exit (EXIT_FAILURE);
  }

  free (parttab);
  free (tpwgtab);
  free (vtxdist);
  SCOTCH_dgraphExit (&grafdat);

  MPI_Finalize ();

  exit (EXIT_SUCCESS);
}
