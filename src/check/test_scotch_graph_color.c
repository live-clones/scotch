/* Copyright 2012,2014,2018,2025,2026 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : test_scotch_graph_color.c               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module tests the operation of      **/
/**                the SCOTCH_graphColor() routine.        **/
/**                                                        **/
/**   DATES      : # Version 6.0  : from : 06 jan 2012     **/
/**                                 to   : 22 may 2018     **/
/**                # Version 7.0  : from : 04 jul 2025     **/
/**                                 to   : 14 jan 2026     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include <stdio.h>
#if (((defined __STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || (defined HAVE_STDINT_H))
#include <stdint.h>
#endif /* (((defined __STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || (defined HAVE_STDINT_H)) */
#include <stdlib.h>
#include <string.h>

#include "../libscotch/module.h"
#include "../libscotch/common.h"

#include "scotch.h"

/*************************************/
/*                                   */
/* The consistency checking routine. */
/*                                   */
/*************************************/

static
int
checkColor (
const SCOTCH_Graph * const  grafptr,
const SCOTCH_Num * const    colotab)
{
  SCOTCH_Num          baseval;
  SCOTCH_Num          vertnbr;
  SCOTCH_Num          vertnnd;
  SCOTCH_Num          vertnum;
  SCOTCH_Num *        verttab;
  const SCOTCH_Num *  verttax;
  SCOTCH_Num *        vendtab;
  const SCOTCH_Num *  vendtax;
  SCOTCH_Num *        edgetab;
  const SCOTCH_Num *  edgetax;
  const SCOTCH_Num *  colotax;

  SCOTCH_graphData (grafptr, &baseval, &vertnbr, &verttab, &vendtab, NULL, NULL, NULL, &edgetab, NULL);

  verttax = verttab - baseval;
  vendtax = vendtab - baseval;
  edgetax = edgetab - baseval;
  colotax = colotab - baseval;

  for (vertnum = baseval, vertnnd = vertnbr + baseval;
       vertnum < vertnnd; vertnum ++) {
    SCOTCH_Num          edgenum;
    SCOTCH_Num          coloval;

    coloval = colotax[vertnum];
    for (edgenum = verttax[vertnum]; edgenum < vendtax[vertnum]; edgenum ++) {
      SCOTCH_Num          vertend;

      vertend = edgetax[edgenum];
      if (colotax[vertend] == coloval) {
        SCOTCH_errorPrint ("checkColor: invalid coloring");
        return (1);
      }
    }
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
int                 argc,
char *              argv[])
{
  FILE *              fileptr;
  SCOTCH_Graph        grafdat;
  SCOTCH_Num          vertnbr;
  SCOTCH_Num          vertnum;
  SCOTCH_Num          colonbr;
  SCOTCH_Num          colonum;
  SCOTCH_Num *        colotab;
  SCOTCH_Num *        cnbrtab;

  SCOTCH_errorProg (argv[0]);

  if (argc != 2) {
    SCOTCH_errorPrint ("usage: %s graph_file", argv[0]);
    exit (EXIT_FAILURE);
  }

  if (SCOTCH_graphInit (&grafdat) != 0) {         /* Initialize source graph */
    SCOTCH_errorPrint ("main: cannot initialize graph");
    exit (EXIT_FAILURE);
  }

  if ((fileptr = fopen (argv[1], "r")) == NULL) {
    SCOTCH_errorPrint ("main: cannot open file");
    exit (EXIT_FAILURE);
  }

  if (SCOTCH_graphLoad (&grafdat, fileptr, -1, 0) != 0) { /* Read source graph */
    SCOTCH_errorPrint ("main: cannot load graph");
    exit (EXIT_FAILURE);
  }

  fclose (fileptr);

  SCOTCH_graphSize (&grafdat, &vertnbr, NULL);

  if ((colotab = malloc (vertnbr * sizeof (SCOTCH_Num))) == NULL) {
    SCOTCH_errorPrint ("main: out of memory (1)");
    exit (EXIT_FAILURE);
  }

  if ((cnbrtab = malloc (vertnbr * sizeof (SCOTCH_Num))) == NULL) {
    SCOTCH_errorPrint ("main: out of memory (1)");
    exit (EXIT_FAILURE);
  }
  memset (cnbrtab, 0, vertnbr * sizeof (SCOTCH_Num));

  if (SCOTCH_graphColor (&grafdat, colotab, &colonbr, 0) != 0) {
    SCOTCH_errorPrint ("main: cannot color graph");
    exit (EXIT_FAILURE);
  }

  if (checkColor (&grafdat, colotab) != 0)        /* Verify coloring validity */
    exit (EXIT_FAILURE);

  printf ("Number of colors: %ld\n", (long) colonbr);

  for (vertnum = 0; vertnum < vertnbr; vertnum ++) /* Sum-up color histogram */
    cnbrtab[colotab[vertnum]] ++;

  for (colonum = 0; colonum < colonbr; colonum ++)
    printf ("Color %5ld: %ld\n",
            (long) colonum,
            (long) cnbrtab[colonum]);

  free (cnbrtab);
  free (colotab);
  SCOTCH_graphExit (&grafdat);

  exit (EXIT_SUCCESS);
}
