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
/**   NAME       : test_arch_deco.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module tests the decomposition-    **/
/**                defined building routine on a path      **/
/**                graph.                                  **/
/**                                                        **/
/**   DATES      : # Version 7.0  : from : 20 aug 2026     **/
/**                                 to   : 25 aug 2026     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE               600
#endif /* _XOPEN_SOURCE */
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K                             /* For POSIX pthread_barrier_t */
#endif /* __USE_XOPEN2K */

#include "../libscotch/module.h"
#include "../libscotch/common.h"
#include "../libscotch/fibo.h"
#include "../libscotch/parser.h"
#include "../libscotch/graph.h"
#include "../libscotch/arch.h"
#include "../libscotch/arch_deco_build.h"
#include "../libscotch/bgraph.h"
#include "../libscotch/bgraph_bipart_st.h"

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
  Context             contdat;
  Gnum                listnbr;
  Gnum *              listtab;
  Strat *             straptr;
  Arch                archdat;
  ArchDom *           domntab;
  Graph               grafdat;
  Gnum *              verttab;
  Gnum                vertnbr;
  Gnum                vertnum;
  Gnum                vnumnum;
  Gnum *              edgetab;
  Gnum                edgenbr;

  errorProg (argv[0]);

  if ((argc < 2) || (argc > 3)) {
    errorPrint ("usage: %s path_size [terminal_ratio]", argv[0]);
    exit       (EXIT_FAILURE);
  }

  if ((vertnbr = (Gnum) atoi (argv[1])) < 1) {
    SCOTCH_errorPrint ("main: invalid number of vertices (\"%s\")", argv[1]);
    exit (EXIT_FAILURE);
  }

  contextInit   (&contdat);
  contextCommit (&contdat);

  if (argc == 2)
    listnbr = vertnbr;
  else {
    double              termrat;

    termrat = (double) atof (argv[2]);
    listnbr = (Anum) (((double) vertnbr) * termrat);

    if ((listnbr <= 0) || (listnbr > vertnbr)) {
      SCOTCH_errorPrint ("main: invalid terminal ratio (\"%s\")", argv[2]);
      exit (EXIT_FAILURE);
    }
  }

  if (memAllocGroup ((void **) (void *)
                     &verttab, (size_t) ((vertnbr + 1) * sizeof (Gnum)),
                     &edgetab, (size_t) ((vertnbr - 1) * sizeof (Gnum) * 2),
                     &listtab, (size_t) (vertnbr       * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("main: out of memory (1)");
    exit       (EXIT_FAILURE);
  }

  intAscn (listtab, vertnbr, 0);
  intPerm (listtab, vertnbr, &contdat);

  edgenbr = 0;                                    /* Build 0-based, compact path graph arrays */
  for (vertnum = 0; vertnum < vertnbr; vertnum ++) {
    verttab[vertnum] = edgenbr;

    if (vertnum > 0)
      edgetab[edgenbr ++] = vertnum - 1;
    if (vertnum < (vertnbr - 1))
      edgetab[edgenbr ++] = vertnum + 1;
  }
  verttab[vertnum] = edgenbr;

  graphInit (&grafdat);                           /* Create path graph from scratch */
  grafdat.baseval = 0;
  grafdat.vertnbr = vertnbr;
  grafdat.vertnnd = vertnbr;
  grafdat.verttax = verttab;
  grafdat.vendtax = verttab + 1;
  grafdat.velosum = vertnbr;
  grafdat.edgenbr = edgenbr;
  grafdat.edgetax = edgetab;
  grafdat.edlosum = edgenbr;
  grafdat.degrmax = 2;

  archInit (&archdat);
  straptr = stratInit (&bgraphbipartststratab, "(m{vert=50,low=h{pass=10},asc=f{move=100,bal=0.1}}f{move=100,bal=0.05})(/((load0=load)|(load0=0))?x;)");

  if (archDecoArchBuild (&archdat, &grafdat, listnbr, listtab, straptr, &contdat) != 0) {
    errorPrint ("main: cannot build architecture");
    exit       (EXIT_FAILURE);
  }

  stratExit (straptr);

  if ((domntab = memAlloc (2 * sizeof (ArchDom))) == NULL) {
    errorPrint ("main: out of memory (2)");
    exit       (EXIT_FAILURE);
  }

  for (vnumnum = 0; vnumnum < listnbr; vnumnum ++) { /* For all terminals */
    Anum                termnum;
    Anum                vnumend;

    termnum = listtab[vnumnum];
    if (archDomTerm (&archdat, &domntab[0], termnum) != 0) { /* Set terminal domain */
      errorPrint ("main: invalid terminal domain (1)");
      exit       (EXIT_FAILURE);
    }

    for (vnumend = 0; vnumend < listnbr; vnumend ++) {
      Anum                termend;
      Anum                distval;

      termend = listtab[vnumend];
      if (archDomTerm (&archdat, &domntab[1], termend) != 0) { /* Set end terminal domain */
        errorPrint ("main: invalid terminal domain (2)");
        exit       (EXIT_FAILURE);
      }

      distval = archDomDist (&archdat, &domntab[0], &domntab[1]); /* Get distance between path terminal domains   */
      if (distval != abs (termnum - termend)) {   /* If distance not equal to difference between terminal indices */
        errorPrint ("main: invalid terminal distance");
        exit       (EXIT_FAILURE);
      }
    }
  }

  memFree  (domntab);
  memFree  (verttab);                             /* Free graph group leader */
  archExit (&archdat);

  contextExit (&contdat);

  exit (EXIT_SUCCESS);
}
