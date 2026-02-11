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
/**   NAME       : test_scotch_dgraph_order.c              **/
/**                                                        **/
/**   AUTHOR     : Clement BARTHELEMY                      **/
/**                                                        **/
/**                                                        **/
/**   FUNCTION   : This module tests the operation of the  **/
/**                SCOTCH_dgraphOrderCompute() and         **/
/**                SCOTCH_dgraphOrderComputeList()         **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 7.0  : from : 10 sep 2025     **/
/**                                 to   : 17 sep 2025     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include <mpi.h>

#include "../libscotch/module.h"
#include "../libscotch/common.h"

#include "ptscotch.h"

static File                 C_fileTab[1] = {      /* File array */
                              { FILEMODER } };

/*******************************/
/*                             */
/* This routine handles lists. */
/*                             */
/*******************************/

/* This routine fills a list array with a subset
** of based vertex indices.
** It returns :
** - 0   : if list array could be filled
** - !0  : on error.
*/

static
int
listFillRandom (
SCOTCH_Num                  listlocnbr,           /* Size of list to fill */
SCOTCH_Num * restrict       listloctab,           /* List area to fill    */
SCOTCH_Num                  baseval,              /* Base value           */
SCOTCH_Num                  vertlocnbr)           /* Local vertex range   */
{
  SCOTCH_Num          listlocidx;
  SCOTCH_Num          vertlocnum;                 /* Based local vertex index to be put in list */

  if (listlocnbr > vertlocnbr) {
    SCOTCH_errorPrint ("listFillRandom: invalid arguments");
    return (1);
  }

  if (listlocnbr == 0)                            /* If nothing to do */
    return (0);

  vertlocnum = baseval;                           /* If list is full, take all vertices */
  if (listlocnbr < vertlocnbr)                    /* Else select only a vertex range    */
    vertlocnum += SCOTCH_randomVal (vertlocnbr - listlocnbr + 1);

  for (listlocidx = 0; listlocidx < listlocnbr; listlocidx ++, vertlocnum ++)
    listloctab[listlocidx] = vertlocnum;

  return (0);
}

/***************************************/
/*                                     */
/* These routines handle permutations. */
/*                                     */
/***************************************/

static
SCOTCH_Num
permGather (
SCOTCH_Dgraph * const       grafptr,
SCOTCH_Dordering * const    dordptr,
SCOTCH_Num *                permtab,              /* Permutation array to fill with full permutation */
int                         proclocnum)           /* Index of this process                           */
{
  SCOTCH_Ordering     corddat;
  SCOTCH_Ordering *   cordptr;
  SCOTCH_Num          cblknbr;

  if (proclocnum == 0) {                          /* Gather ordering results on root process */
    if (SCOTCH_dgraphCorderInit (grafptr, &corddat, permtab, NULL, &cblknbr, NULL, NULL) != 0) {
      SCOTCH_errorPrint ("permGather: cannot initialize distributed ordering");
      return (1);
    }
    cordptr = &corddat;
  }
  else
    cordptr = NULL;

  if (SCOTCH_dgraphOrderGather (grafptr, dordptr, cordptr) != 0) {
    SCOTCH_errorPrint ("permGather: cannot gather distributed ordering");
    return (1);
  }

  if (cordptr != NULL)
    SCOTCH_dgraphCorderExit (grafptr, cordptr);

  return (0);
}

static
SCOTCH_Num
permCheckIdentity (
const SCOTCH_Num * const    permtab,
SCOTCH_Num                  baseval,
SCOTCH_Num                  vertnbr)
{
  SCOTCH_Num        vertnum;

  for (vertnum = 0; vertnum < vertnbr; vertnum ++) {
    if (permtab[vertnum] != vertnum + baseval)
      return (1);
  }

  return (0);
}

static
SCOTCH_Num
permCheckList (
const SCOTCH_Num * const    permtab,
SCOTCH_Num                  baseval,
SCOTCH_Num                  listlocnbr,           /* Number of local vertices in list  */
SCOTCH_Num *                listloctab,           /* Local list of vertices            */
SCOTCH_Num                  listglbnbr)           /* Global number of vertices in list */
{
  SCOTCH_Num          listlocidx;

  const SCOTCH_Num * const          permtax = permtab - baseval;

  for (listlocidx = 0; listlocidx < listlocnbr; listlocidx ++) /* For all vertices in list  */
    if (permtax[listloctab[listlocidx]] >= listglbnbr + baseval) { /* Must be ordered first */
      return (1);
  }

  return (0);
}

/*********************/
/*                   */
/* The main routine. */
/*                   */
/*********************/

int
main (
int                         argc,
char *                      argv[])
{
  MPI_Comm            proccomm;
  int                 procglbnbr;                 /* Number of processes sharing graph data */
  int                 proclocnum;                 /* Number of this process                 */
  SCOTCH_Dgraph       grafdat;
  SCOTCH_Dordering    dorddat;
  SCOTCH_Strat        stradat;
  SCOTCH_Num          baseval;
  SCOTCH_Num          vertglbnbr;
  SCOTCH_Num          vertlocnbr;
  SCOTCH_Num          listlocnbr;
  SCOTCH_Num *        listloctab;
  SCOTCH_Num          listglbnbr;
  SCOTCH_Num *        permtab;
#ifdef SCOTCH_PTHREAD
  int                 thrdreqlvl;
  int                 thrdprolvl;
#endif /* SCOTCH_PTHREAD */

  SCOTCH_errorProg (argv[0]);

#ifdef SCOTCH_PTHREAD
  thrdreqlvl = MPI_THREAD_MULTIPLE;
  if (MPI_Init_thread (&argc, &argv, thrdreqlvl, &thrdprolvl) != MPI_SUCCESS)
    SCOTCH_errorPrint ("main: Cannot initialize (1)");
#else /* SCOTCH_PTHREAD */
  if (MPI_Init (&argc, &argv) != MPI_SUCCESS)
    SCOTCH_errorPrint ("main: Cannot initialize (2)");
#endif /* SCOTCH_PTHREAD */

  if (argc != 2) {
    SCOTCH_errorPrint ("usage: %s graph_file", argv[0]);
    exit (EXIT_FAILURE);
  }

  proccomm = MPI_COMM_WORLD;
  MPI_Comm_size (proccomm, &procglbnbr);          /* Get communicator data */
  MPI_Comm_rank (proccomm, &proclocnum);

#ifdef SCOTCH_CHECK_NOAUTO
  fprintf (stderr, "Proc %2d of %2d, pid %d\n", proclocnum, procglbnbr, getpid ());

  if (proclocnum == 0) {                          /* Synchronize on keybord input */
    char           c;

    printf ("Waiting for key press...\n");
    scanf ("%c", &c);
  }
#endif /* SCOTCH_CHECK_NOAUTO */

  SCOTCH_randomReset ();

  if (MPI_Barrier (proccomm) != MPI_SUCCESS) {    /* Synchronize for debug */
    SCOTCH_errorPrint ("main: cannot communicate (1)");
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

  SCOTCH_dgraphData (&grafdat, &baseval, &vertglbnbr, &vertlocnbr, NULL, NULL, NULL, NULL, NULL,
                     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  if ((permtab = malloc (vertglbnbr * sizeof (SCOTCH_Num))) == NULL) {
    SCOTCH_errorPrint ("main: out of memory (1)");
    exit (EXIT_FAILURE);
  }

  SCOTCH_stratInit (&stradat);
  if (SCOTCH_stratDgraphOrderBuild (&stradat, 0, (SCOTCH_Num) procglbnbr, 0, 0.2) != 0) {
    SCOTCH_errorPrint ("main: cannot build ordering strategy");
    exit (EXIT_FAILURE);
  }

  if (SCOTCH_dgraphOrderInit (&grafdat, &dorddat) != 0) {
    SCOTCH_errorPrint ("main: cannot initialize ordering (1)");
    exit (EXIT_FAILURE);
  }

  if (SCOTCH_dgraphOrderComputeList (&grafdat, &dorddat, 0, NULL, &stradat) != 0) { /* Identity permutation */
    SCOTCH_errorPrint ("main: cannot compute graph ordering (1)");
    exit (EXIT_FAILURE);
  }

  memset (permtab, ~0, vertglbnbr * sizeof (SCOTCH_Num));
  if (permGather (&grafdat, &dorddat, permtab, proclocnum) != 0) {
    SCOTCH_errorPrint ("main: error checking ordering (1)");
    exit (EXIT_FAILURE);
  }
  if ((proclocnum == 0) &&
      (permCheckIdentity (permtab, baseval, vertglbnbr) != 0)) {
    SCOTCH_errorPrint ("main: invalid ordering (1)");
    exit (EXIT_FAILURE);
  }

  SCOTCH_dgraphOrderExit (&grafdat, &dorddat);

  if (SCOTCH_dgraphOrderInit (&grafdat, &dorddat) != 0) {
    SCOTCH_errorPrint ("main: cannot initialize ordering (2)");
    exit (EXIT_FAILURE);
  }

  if (proclocnum == 0)
    listlocnbr = vertlocnbr;                      /* Experiment with a full local list */
  else if (proclocnum == (procglbnbr - 1))
    listlocnbr = 0;                               /* And an empty local list */
  else
    listlocnbr = (9 * vertlocnbr) / 10;           /* And a partial local list */
  if ((listloctab = malloc (listlocnbr * sizeof (SCOTCH_Num))) == NULL) {
    SCOTCH_errorPrint ("main: out of memory (2)");
    exit (EXIT_FAILURE);
  }
  listFillRandom (listlocnbr, listloctab, baseval, vertlocnbr);

  if (SCOTCH_dgraphOrderComputeList (&grafdat, &dorddat, listlocnbr, listloctab, &stradat)) { /* List ordering */
    SCOTCH_errorPrint ("main: cannot compute graph ordering (2)");
    exit (EXIT_FAILURE);
  }

  if (MPI_Reduce (&listlocnbr, &listglbnbr, 1, SCOTCH_NUM_MPI, MPI_SUM, 0, proccomm) != MPI_SUCCESS) {
    SCOTCH_errorPrint ("main: cannot communicate (2)");
    exit (EXIT_FAILURE);
  }

  memset (permtab, ~0, vertglbnbr * sizeof (SCOTCH_Num));
  if (permGather (&grafdat, &dorddat, permtab, proclocnum) != 0) {
    SCOTCH_errorPrint ("main: error checking ordering (2)");
    exit (EXIT_FAILURE);
  }
  if ((proclocnum == 0) &&
      (permCheckList (permtab, baseval, listlocnbr, listloctab, listglbnbr) != 0)) {
    SCOTCH_errorPrint ("main: invalid ordering (2)");
    exit (EXIT_FAILURE);
  }

  SCOTCH_dgraphOrderExit (&grafdat, &dorddat);

  fileBlockClose (C_fileTab, 1);                  /* Always close explicitly to end eventual (un)compression tasks */

  if (MPI_Barrier (proccomm) != MPI_SUCCESS) {    /* Synchronize for debug */
    SCOTCH_errorPrint ("main: cannot communicate (3)");
    exit (EXIT_FAILURE);
  }

  SCOTCH_stratExit  (&stradat);
  SCOTCH_dgraphExit (&grafdat);
  free (listloctab);
  free (permtab);

  MPI_Finalize ();

  exit (EXIT_SUCCESS);
}
