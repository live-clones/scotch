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
/**   NAME       : test_scotch_arch_build.c                **/
/**                                                        **/
/**   AUTHOR     : Charles PELLEGRINI                      **/
/**                Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module tests the semantic          **/
/**                correctness of the decomposition-       **/
/**                defined architectures built by          **/
/**                SCOTCH_archBuild0().                    **/
/**                                                        **/
/**   DATES      : # Version 7.0  : from : 04 jul 2026     **/
/**                                 to   : 19 aug 2026     **/
/**                                                        **/
/**   NOTES      : # This test builds a "deco 0"           **/
/**                  architecture a from weighted path     **/
/**                  graph whose edge loads force the      **/
/**                  recursive bipartition as follows:     **/
/**                  - All loads lie within a factor of    **/
/**                    two of each other, so, after        **/
/**                    inversion by graphIelo(), cutting   **/
/**                    any single path edge is always      **/
/**                    cheaper than cutting two: parts     **/
/**                    remain contiguous;                  **/
/**                  - Within every contiguous interval,   **/
/**                    the median edge carries the         **/
/**                    strictly largest load, hence the    **/
/**                    strictly cheapest cut: every        **/
/**                    bipartition severs the median       **/
/**                    edge.                               **/
/**                  Consequently, in a correctly built    **/
/**                  architecture, two singleton leaves    **/
/**                  that are siblings in the              **/
/**                  decomposition tree (domains 2k and    **/
/**                  2k+1) are the two ends of the last    **/
/**                  edge cut, so their terminal labels    **/
/**                  are adjacent path vertices. The test  **/
/**                  checks this property, the leaf label  **/
/**                  bijection, and the invariance of the  **/
/**                  whole architecture with respect to    **/
/**                  the graph base value, by parsing the  **/
/**                  saved architecture (of "deco 1"       **/
/**                  format).                              **/
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

#include "scotch.h"

#define PATHMAX                     16            /* Maximum path graph size            */
#define DOMNMAX                     64            /* Maximum decomposition tree size    */
#define BASENBR                     3             /* Number of base values to test with */

/*
**  The type and structure definitions.
*/

/*+ The parsed "deco 1" architecture structure:
    every tree domain num (based at 1, so that
    sons of domain k are 2k and 2k+1), has its
    terminal label, subdomain size and weight.  +*/

typedef struct C_ArchDeco_ {
  SCOTCH_Num                termnbr;              /* Number of terminal domains */
  SCOTCH_Num                domnnbr;              /* Number of domains          */
  SCOTCH_Num                labltab[DOMNMAX];
  SCOTCH_Num                sizetab[DOMNMAX];
  SCOTCH_Num                wghttab[DOMNMAX];
} C_ArchDeco;

/*
**  The static and global variables.
*/

static const SCOTCH_Num     sizetab[] = { 3, 4, 6, 8, 12 };
static const SCOTCH_Num     basetab[BASENBR] = { 0, 1, 100000 };

/**************************************/
/*                                    */
/* The architecture testing routines. */
/*                                    */
/**************************************/

/* This routine gives every edge of a path interval a
** weight such that the median edge of each sub-interval
** carries the strictly largest weight of the interval.
** Weights span in the [109, 199] interval, that is,
** within a factor of two.
*/

static
void
C_archBuild2 (
SCOTCH_Num * const          wghttab,              /*+ Edge load array [vertnbr - 1] +*/
const SCOTCH_Num            vertmin,              /*+ First vertex of interval      +*/
const SCOTCH_Num            vertnnd,              /*+ After-last vertex of interval +*/
const SCOTCH_Num            levlnum)              /*+ Recursion level               +*/
{
  SCOTCH_Num          vertmed;

  if ((vertnnd - vertmin) < 2)                    /* If single vertex, nothing to do */
    return;

  vertmed = vertmin + (vertnnd - vertmin) / 2;    /* Cut edge is (vertmed - 1, vertmed)                                          */
  wghttab[vertmed - 1] = 1 << (levlnum + 1);      /* Make sure edge weight difference is strictly greater than 1 */
  C_archBuild2 (wghttab, vertmin, vertmed, levlnum - 1);
  C_archBuild2 (wghttab, vertmed, vertnnd, levlnum - 1);
}

/* This routine builds a "deco 0" architecture from a
** weighted path graph of the given size and base value.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
int
C_archBuild (
SCOTCH_Arch * const         archptr,
const SCOTCH_Num            baseval,              /* Graph base value        */
const SCOTCH_Num            vertnbr)              /* Number of path vertices */
{
  SCOTCH_Graph              grafdat;
  SCOTCH_Strat              stradat;
  SCOTCH_Num                verttab[PATHMAX + 1];
  SCOTCH_Num                edgetab[2 * (PATHMAX - 1)];
  SCOTCH_Num                edlotab[2 * (PATHMAX - 1)];
  SCOTCH_Num                wghttab[PATHMAX - 1];
  SCOTCH_Num                vertnum;
  SCOTCH_Num                edgenbr;

  SCOTCH_randomReset ();                          /* Same random state for all base values */

  SCOTCH_archInit (archptr);

  C_archBuild2 (wghttab, 0, vertnbr, vertnbr - 1); /* Compute forced-median edge weights */

  edgenbr = 0;                                    /* Build compact path graph arrays */
  for (vertnum = 0; vertnum < vertnbr; vertnum ++) {
    verttab[vertnum] = edgenbr + baseval;

    if (vertnum > 0) {
      edgetab[edgenbr] = vertnum - 1 + baseval;
      edlotab[edgenbr] = wghttab[vertnum - 1];
      edgenbr ++;
    }
    if (vertnum < (vertnbr - 1)) {
      edgetab[edgenbr] = vertnum + 1 + baseval;
      edlotab[edgenbr] = wghttab[vertnum];
      edgenbr ++;
    }
  }
  verttab[vertnum] = edgenbr + baseval;

  SCOTCH_graphInit (&grafdat);
  if (SCOTCH_graphBuild (&grafdat, baseval, vertnbr, verttab, NULL, NULL, NULL,
                         edgenbr, edgetab, edlotab) != 0) {
    SCOTCH_errorPrint ("C_archBuild: cannot build graph");
    return (1);
  }

  SCOTCH_stratInit (&stradat);                    /* Default target architecture bipartitioning strategy */
  if (SCOTCH_archBuild0 (archptr, &grafdat, 0, NULL, &stradat) != 0) {
    SCOTCH_errorPrint ("C_archBuild: cannot build architecture");
    return (1);
  }

  SCOTCH_graphExit (&grafdat);

  return (0);
}

/* This routine parses the "deco 1" architecture from
** the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
int
C_archLoad (
C_ArchDeco * const          darcptr,              /* Test architecture to fill-in       */
const SCOTCH_Num            vertnbr,              /* Number of vertices in architecture */
FILE *                      fileptr)
{
  char                      nametab[32];
  SCOTCH_Num                decoval;
  SCOTCH_Num                domnnum;
  SCOTCH_Num                distnbr;
  SCOTCH_Num                distval;
  int                       o;

  nametab[31] = '\0';
  if ((fscanf (fileptr, "%31s" SCOTCH_NUMSTRING SCOTCH_NUMSTRING SCOTCH_NUMSTRING,
               nametab, &decoval, &darcptr->termnbr, &darcptr->domnnbr) != 4) ||
      (strcmp (nametab, "deco") != 0)                                         ||
      (decoval != 1)) {
    SCOTCH_errorPrint ("C_archLoad: invalid input (1)");
    return (1);
  }
  if ((darcptr->termnbr != vertnbr) ||
      (darcptr->domnnbr <  vertnbr) ||
      (darcptr->domnnbr >  DOMNMAX)) {
    SCOTCH_errorPrint ("C_archLoad: invalid input (2)");
    return (1);
  }
  for (domnnum = 0; domnnum < darcptr->domnnbr; domnnum ++) {
    if (fscanf (fileptr, "" SCOTCH_NUMSTRING SCOTCH_NUMSTRING SCOTCH_NUMSTRING,
                &darcptr->labltab[domnnum], &darcptr->sizetab[domnnum], &darcptr->wghttab[domnnum]) != 3) {
      SCOTCH_errorPrint ("C_archLoad: invalid input (3)");
      return (1);
    }
  }

  o = 0;
  for (distnbr = (darcptr->domnnbr * (darcptr->domnnbr - 1)) / 2; distnbr > 0; distnbr --)
    o |= (fscanf (fileptr, SCOTCH_NUMSTRING, &distval) != 1);
  if (o != 0) {
    SCOTCH_errorPrint ("C_archLoad: invalid input (4)");
    return (1);
  }

  return (0);
}

/* This routine checks that the given architecture
** is a proper decomposition of the forced path graph:
** - the root domain holds all the terminals;
** - the terminal domains hold each path vertex exactly once;
** - sibling terminal domains hold adjacent path vertices.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
int
C_archCheck (
const C_ArchDeco * const    darcptr,
const SCOTCH_Num            baseval,
const SCOTCH_Num            vertnbr)
{
  int                       flagtab[PATHMAX];
  SCOTCH_Num                termnbr;
  SCOTCH_Num                domnnum;
  SCOTCH_Num                lablval;

  if (darcptr->sizetab[0] != vertnbr) {           /* If root domain does not hold all terminals */
    SCOTCH_errorPrint ("C_archCheck: invalid root size");
    return (1);
  }

  memset (flagtab, 0, PATHMAX * sizeof (int));

  for (domnnum = 0, termnbr = 0; domnnum < darcptr->domnnbr; domnnum ++) {
    if (darcptr->sizetab[domnnum] != 1)
      continue;

    termnbr ++;
    lablval = darcptr->labltab[domnnum];

    if ((lablval <   baseval) ||
        (lablval >= (baseval + vertnbr))) {
      SCOTCH_errorPrint ("C_archCheck: invalid terminal label");
      return (1);
    }
    if (flagtab[lablval - baseval] ++ != 0) {
      SCOTCH_errorPrint ("C_archCheck: duplicate terminal label");
      return (1);
    }
  }
  if (termnbr != vertnbr) {
    SCOTCH_errorPrint ("C_archCheck: invalid number of terminals");
    return (1);
  }

  for (domnnum = 2; domnnum < darcptr->domnnbr; domnnum ++) { /* For all sibling pairs (2k, 2k+1) */
    if ((darcptr->sizetab[domnnum - 1] != 1) ||   /* Consider pairs of terminal domains only      */
        (darcptr->sizetab[domnnum]     != 1))
      continue;

    if (abs (darcptr->labltab[domnnum] - darcptr->labltab[domnnum - 1]) != 1) { /* Labels must be path-adjacent */
      SCOTCH_errorPrint ("C_archCheck: non-adjacent terminal domains");
      return (1);
    }
    domnnum ++;                                   /* Pair of terminals has been consumed */
  }

  return (0);
}

/* This routine checks that two parsed architectures built
** from the same path graph at different base values are
** identical.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
int
C_archCompare (
const C_ArchDeco * const    dar0ptr,              /* First architecture  */
const SCOTCH_Num            bas0val,              /* First base value    */
const C_ArchDeco * const    dar1ptr,              /* Second architecture */
const SCOTCH_Num            bas1val)              /* Second base value   */
{
  SCOTCH_Num          domnnum;

  if (dar0ptr->domnnbr != dar1ptr->domnnbr) {
    SCOTCH_errorPrint ("C_archCompare: invalid number of domains");
    return (1);
  }
  for (domnnum = 0; domnnum < dar0ptr->domnnbr; domnnum ++) {
    if ((dar0ptr->sizetab[domnnum] != dar1ptr->sizetab[domnnum]) ||
        (dar0ptr->wghttab[domnnum] != dar1ptr->wghttab[domnnum]) ||
        ((dar0ptr->sizetab[domnnum] > 0) &&
         ((dar0ptr->labltab[domnnum] - bas0val) != (dar1ptr->labltab[domnnum] - bas1val)))) {
      SCOTCH_errorPrint ("C_archCompare: invalid domain");
      return (1);
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
int                         argc,
char *                      argv[])
{
  C_ArchDeco          darctab[BASENBR];
  int                 vertidx;

  SCOTCH_errorProg (argv[0]);

  if (argc != 2) {
    SCOTCH_errorPrint ("usage: %s architecture_file", argv[0]);
    exit (EXIT_FAILURE);
  }

  for (vertidx = 0; vertidx < (int) (sizeof (sizetab) / sizeof (sizetab[0])); vertidx ++) {
    int                 baseidx;                  /* Index of test base value */
    SCOTCH_Num          vertnbr;

    vertnbr = sizetab[vertidx];
    for (baseidx = 0; baseidx < BASENBR; baseidx ++) {
      SCOTCH_Arch         archdat;
      FILE *              fileptr;

      if (C_archBuild (&archdat, basetab[baseidx], vertnbr) != 0) {
        SCOTCH_errorPrint ("main: cannot build architecture");
        exit (EXIT_FAILURE);
      }

      if ((fileptr = fopen (argv[1], "w")) == NULL) {
        SCOTCH_errorPrint ("main: cannot open architecture file (1)");
        exit (EXIT_FAILURE);
      }
      if (SCOTCH_archSave (&archdat, fileptr) != 0) {
        SCOTCH_errorPrint ("main: cannot save architecture");
        exit (EXIT_FAILURE);
      }
      fclose (fileptr);

      if ((fileptr = fopen (argv[1], "r")) == NULL) {
        SCOTCH_errorPrint ("main: cannot open architecture file (2)");
        exit (EXIT_FAILURE);
      }
      if (C_archLoad (&darctab[baseidx], vertnbr, fileptr) != 0) {
        SCOTCH_errorPrint ("main: cannot load architecture");
        exit (EXIT_FAILURE);
      }
      fclose (fileptr);

      if (C_archCheck (&darctab[baseidx], basetab[baseidx], vertnbr) != 0) {
        SCOTCH_errorPrint ("main: invalid architecture (1)");
        exit (EXIT_FAILURE);
      }

      if ((baseidx > 0) &&
          (C_archCompare (&darctab[0], basetab[0], &darctab[baseidx], basetab[baseidx]) != 0)) {
        SCOTCH_errorPrint ("main: invalid architecture (2)");
        exit (EXIT_FAILURE);
      }
    }
  }

  exit (EXIT_SUCCESS);
}
