/* Copyright INRIA 2004
**
** This file is part of the Scotch distribution.
**
** The Scotch distribution is libre/free software; you can
** redistribute it and/or modify it under the terms of the
** GNU Lesser General Public License as published by the
** Free Software Foundation; either version 2.1 of the
** License, or (at your option) any later version.
**
** The Scotch distribution is distributed in the hope that
** it will be useful, but WITHOUT ANY WARRANTY; without even
** the implied warranty of MERCHANTABILITY or FITNESS FOR A
** PARTICULAR PURPOSE. See the GNU Lesser General Public
** License for more details.
**
** You should have received a copy of the GNU Lesser General
** Public License along with the Scotch distribution; if not,
** write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
**
** $Id$
*/
/************************************************************/
/**                                                        **/
/**   NAME       : gotst.c                                 **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                Bruno MARCUSSEAU (v3.1)                 **/
/**                                                        **/
/**   FUNCTION   : Graph symbolic factorizer.              **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 27 jan 2004     **/
/**                                 to   : 28 nov 2005     **/
/**                                                        **/
/**   NOTES      : # This module contains code that was    **/
/**                  formerly part of order_sf.c in        **/
/**                  libScotch 3.4                         **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GOTST

#include "common.h"
#include "scotch.h"
#include "gotst.h"

/*
**  The static and global variables.
*/

static int                  C_fileNum    = 0;     /* Number of file in arg list  */
static File                 C_fileTab[3] = {      /* File array                  */
                              { "-", NULL, "r" },
                              { "-", NULL, "r" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "gotst [<input graph file> [<input ordering file> [<output data file>]]] <options>",
  "  -h  : Display this help",
  "  -V  : Print program version and copyright",
  NULL };

/*****************************/
/*                           */
/* This is the main function */
/*                           */
/*****************************/

int
main (
int                         argc,
char *                      argv[])
{
  SCOTCH_Graph        grafdat;
  SCOTCH_Num          vertnbr;
  SCOTCH_Num *        verttab;
  SCOTCH_Num *        vendtab;
  SCOTCH_Num          edgenbr;
  SCOTCH_Num *        edgetab;
  SCOTCH_Num          baseval;
  SCOTCH_Ordering     ordedat;
  SCOTCH_Num *        peritab;
  int                 i;

  errorProg ("gotst");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_fileNum < C_FILEARGNBR)               /* A file name has been given */
        C_fileTab[C_fileNum ++].name = argv[i];
      else {
        errorPrint ("main: too many file names given");
        return     (1);
      }
    }
    else {                                       /* If found an option name */
      switch (argv[i][1]) {
        case 'H' :                               /* Give help */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'V' :
          fprintf (stderr, "gotst, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004 INRIA, France\n");
          fprintf (stderr, "This software is libre/free software under LGPL -- see the user's manual for more information\n");
          return  (0);
        default :
          errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
          return     (1);
      }
    }
  }

  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      if ((C_fileTab[i].pntr = fopen (C_fileTab[i].name, C_fileTab[i].mode)) == NULL) { /* Open the file */
        errorPrint ("main: cannot open file (%d)", i);
        return     (1);
      }
    }
  }

  SCOTCH_graphInit (&grafdat);
  SCOTCH_graphLoad (&grafdat, C_filepntrgrfinp, -1, 3);
  SCOTCH_graphData (&grafdat, &baseval, &vertnbr, &verttab, &vendtab, NULL, NULL, &edgenbr, &edgetab, NULL);
#ifdef SCOTCH_DEBUG_MAIN2
  if (vendtab != (verttab + 1)) {
    errorPrint ("main: graph should be compact");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_MAIN2 */
  if ((peritab = (SCOTCH_Num *) memAlloc (vertnbr * sizeof (SCOTCH_Num))) == NULL) {
    errorPrint ("main: out of memory");
    return     (1);
  }
  SCOTCH_graphOrderInit (&grafdat, &ordedat, NULL, peritab, NULL, NULL, NULL);
  SCOTCH_graphOrderLoad (&grafdat, &ordedat, C_filepntrordinp);

  C_factorView (vertnbr, verttab, edgenbr, edgetab, peritab, baseval, C_filepntrdatout);

#ifdef SCOTCH_DEBUG_MAIN1
  SCOTCH_orderExit (&ordedat);
  SCOTCH_graphExit (&grafdat);
  memFree          (peritab);

  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  return (0);
}

/*************************************/
/*                                   */
/* These routines compute statistics */
/* on orderings.                     */
/*                                   */
/*************************************/

static
int
C_factorView (
const SCOTCH_Num              vertnbr,
SCOTCH_Num * restrict         verttab,
const SCOTCH_Num              edgenbr,
SCOTCH_Num * restrict         edgetab,
SCOTCH_Num * restrict         peritab,
const SCOTCH_Num              baseval,
FILE * restrict const         stream)
{
  SCOTCH_Num * restrict   oradtab;                /* Ordering, adjusted     */
  C_FactorNode * restrict treetab;                /* Elimination tree table */
  SCOTCH_Num              vertdad;
  SCOTCH_Num              vertnum;
  SCOTCH_Num              heigmin;
  SCOTCH_Num              heigmax;
  SCOTCH_Num              heigtot;
  SCOTCH_Num              heignbr;
  double                  heigavg;
  double                  heigdlt;
  double                  opctot;
  SCOTCH_Num              opccol;
  SCOTCH_Num * restrict   mrktab;
  SCOTCH_Num * restrict   mlktab;
  SCOTCH_Num * restrict   rlktab;
  SCOTCH_Num              maxsub;
  SCOTCH_Num              maxlnz;
  SCOTCH_Num * restrict   xlnztab;
  SCOTCH_Num * restrict   xnzstab;
  SCOTCH_Num * restrict   nzsutab;
  int                     o;

  if (memAllocGroup ((void **)
                     &oradtab, (size_t) (vertnbr       * sizeof (SCOTCH_Num)),
                     &treetab, (size_t) (vertnbr       * sizeof (C_FactorNode)),
                     &mlktab,  (size_t) ((vertnbr + 1) * sizeof (SCOTCH_Num)),
                     &mrktab,  (size_t) ((vertnbr + 1) * sizeof (SCOTCH_Num)),
                     &rlktab,  (size_t) ((vertnbr + 1) * sizeof (SCOTCH_Num)),
                     &xlnztab, (size_t) ((vertnbr + 1) * sizeof (SCOTCH_Num)),
                     &xnzstab, (size_t) ((vertnbr + 1) * sizeof (SCOTCH_Num)), NULL) == NULL) {
    errorPrint ("C_factorView : out of memory (1)");
    return (1);
  }

  maxsub = (3 * (edgenbr + vertnbr) + 5000);      /* Start with a reasonably small subscript array */
  if ((nzsutab = (SCOTCH_Num *) memAlloc (maxsub * sizeof (SCOTCH_Num))) == NULL) {
    errorPrint ("C_factorView: out of memory");
    memFree    (oradtab);                         /* Free group leader */
    return     (1);
  }
  for ( ; ; ) {                                   /* Grow subscript array as much as we can */
    SCOTCH_Num * restrict    nzsutmp;


    maxlnz = maxsub + (maxsub / 2);               /* Compute bigger space           */
    if ((maxlnz < maxsub) ||                      /* Avoid overflow of size counter */
        ((nzsutmp = memRealloc (nzsutab, maxlnz)) == NULL))
      break;
    maxsub  = maxlnz;
    nzsutab = nzsutmp;
  }

  if (baseval != 0) {                             /* If base is not 1, adjust graph and permutation data */
    SCOTCH_Num              baseadj;
    SCOTCH_Num              edgenum;

    baseadj = - baseval;
    for (edgenum = 0; edgenum < (verttab[vertnbr] - baseval); edgenum ++)
      edgetab[edgenum] += baseadj;
    for (vertnum = 0; vertnum <= vertnbr; vertnum ++)
      verttab[vertnum] += baseadj;
  }
  if (baseval != 1) {                             /* If base is not 1, adjust graph and permutation data */
    SCOTCH_Num              baseadj;

    baseadj = 1 - baseval;
    for (vertnum = 0; vertnum < vertnbr; vertnum ++) /* Adjust inverse permutation */
      peritab[vertnum] += baseadj;
  }
  for (vertnum = 0; vertnum < vertnbr; vertnum ++) /* Build direct permutation */
    oradtab[peritab[vertnum] - 1] = vertnum + 1;

  if (C_smbfct (vertnbr, (SCOTCH_Num *) verttab, (SCOTCH_Num *) edgetab, /* orad and oriv have inverse meaning in SPARSPAK */
                peritab, oradtab, xlnztab, &maxlnz, xnzstab, nzsutab, &maxsub,
                mrktab, mlktab, rlktab)) {
    errorPrint ("graphFactorView: factored matrix too large");
    memFree    (nzsutab);
    memFree    (oradtab);                         /* Free group leader */
    return     (1);
  }

  memSet (treetab, 0, vertnbr * sizeof (C_FactorNode)); /* Initialize elimination tree array */
  for (vertnum = 0; vertnum < vertnbr; vertnum ++) {
    if (xlnztab[vertnum] < xlnztab[vertnum + 1]) { /* If node is not root or not isolated */
      vertdad = nzsutab[xnzstab[vertnum] - 1] - 1;
      treetab[vertnum].linkdad = treetab + vertdad; /* Link node in tree */
      treetab[vertnum].linkbro = treetab[vertdad].linkson;
      treetab[vertdad].linkson = treetab + vertnum;
    }
  }

  heigmin = SCOTCH_NUMMAX;
  heigmax = 0;
  heigtot = 0;
  heignbr = 0;
  heigavg = 0.0L;
  heigdlt = 0.0L;
  C_factorView2 (treetab + vertnbr - 1, 1, &heigmin, &heigmax, &heigtot, &heignbr, &heigavg, &heigdlt); /* Get height sum */
  heigavg = heigdlt / (double) heignbr;
  heigdlt = 0.0L;
  heignbr = 0;
  C_factorView2 (treetab + vertnbr - 1, 1, &heigmin, &heigmax, &heigtot, &heignbr, &heigavg, &heigdlt); /* Get delta */
  heigdlt /= (double) heignbr;

  o = (fprintf (stream, "O\tLeaf=%d\nO\tHeight min=%u\tmax=%u\tavg=%f\tdlt=%f (%5.2f)\n", /* Write tree height statistics */
                heignbr, heigmin, heigmax, heigavg, heigdlt, ((heigdlt / heigavg) * (double) 100.0L)) == EOF);

  for (vertnum = 0, opctot = 0.0L; vertnum < vertnbr; vertnum ++) {
    opccol  = xlnztab[vertnum + 1] - xlnztab[vertnum] + 1; /* Get number of nonzeros per column, plus diagonal */
    opctot += (double) opccol * (double) opccol;
  }

  o |= (fprintf (stream, "O\tNNZ=%e\tStor=%ld\nO\tOPC=%e\n",
                 (double) (maxlnz + vertnbr),
                 (long) (2 * vertnbr + 1 +        /* xlnz + xnzsub */
                 maxsub),                         /* + nzsub       */
                 opctot) == EOF);

  if (o != 0) {
    errorPrint ("C_factorView : bad output");
  }

  memFree (nzsutab);
  memFree (oradtab);                              /* Free group leader */

  return (o);
}

static
void
C_factorView2 (
C_FactorNode * restrict const nodeptr,
SCOTCH_Num                    level,
SCOTCH_Num * restrict const   heigmin,
SCOTCH_Num * restrict const   heigmax,
SCOTCH_Num * restrict const   heigtot,
SCOTCH_Num * restrict const   heignbr,
double * restrict const       heigavg,
double * restrict const       heigdlt)
{
  C_FactorNode * restrict nodeson;

  (* heigtot) ++;
  if (nodeptr->linkson != NULL) {                 /* If node has descendants */
    for (nodeson = nodeptr->linkson; nodeson != NULL; nodeson = nodeson->linkbro)
      C_factorView2 (nodeson, level + 1, heigmin, heigmax, heigtot, heignbr, heigavg, heigdlt);
  }
  else {
    (*heignbr) ++;
    *heigdlt += fabs ((double) level - *heigavg);
    if (level < *heigmin)
      *heigmin = level;
    if (level > *heigmax)
      *heigmax = level;
  }
}

/*****************************************************************
**********     SMBFCT ..... SYMBOLIC FACTORIZATION       *********
******************************************************************
*   PURPOSE - THIS ROUTINE PERFORMS SYMBOLIC FACTORIZATION
*   ON A PERMUTED LINEAR SYSTEM AND IT ALSO SETS UP THE
*   COMPRESSED DATA STRUCTURE FOR THE SYSTEM.
*
*   INPUT PARAMETERS -
*      NEQNS - NUMBER OF EQUATIONS.
*      (XADJ, ADJNCY) - THE ADJACENCY STRUCTURE.
*      (PERM, INVP) - THE PERMUTATION VECTOR AND ITS INVERSE.
*
*   UPDATED PARAMETERS -
*      MAXSUB - SIZE OF THE SUBSCRIPT ARRAY NZSUB.  ON RETURN,
*             IT CONTAINS THE NUMBER OF SUBSCRIPTS USED
*
*   OUTPUT PARAMETERS -
*      XLNZ - INDEX INTO THE NONZERO STORAGE VECTOR LNZ.
*      (XNZSUB, NZSUB) - THE COMPRESSED SUBSCRIPT VECTORS.
*      MAXLNZ - THE NUMBER OF NONZEROS FOUND.
*
*******************************************************************/

static
int
C_smbfct (
const SCOTCH_Num            neqns,
SCOTCH_Num * restrict       xadj,
SCOTCH_Num * restrict       adjncy,
SCOTCH_Num * restrict       perm,
SCOTCH_Num * restrict       invp,
SCOTCH_Num * restrict       xlnz,
SCOTCH_Num * restrict       maxlnz,
SCOTCH_Num * restrict       xnzsub,
SCOTCH_Num * restrict       nzsub,
SCOTCH_Num * restrict       maxsub,
SCOTCH_Num * restrict       marker,
SCOTCH_Num * restrict       mrglnk,
SCOTCH_Num * restrict       rchlnk)
{
  SCOTCH_Num node, rchm, mrgk, lmax, i, j, k, m, nabor, nzbeg, nzend;
  SCOTCH_Num kxsub, jstop, jstrt, mrkflg, inz, knz, flag;

  memSet (marker, 0, (neqns + 1) * sizeof (SCOTCH_Num));
  memSet (mrglnk, 0, (neqns + 1) * sizeof (SCOTCH_Num));

  /* Parameter adjustments */
  --marker;
  --mrglnk;
  --rchlnk;
  --nzsub;
  --xnzsub;
  --xlnz;
  --invp;
  --perm;
  --xadj;

  /* Function Body */
  flag = 0;
  nzbeg = 1;
  nzend = 0;
  xlnz[1] = 1;

  /* FOR EACH COLUMN KNZ COUNTS THE NUMBER OF NONZEROS IN COLUMN K ACCUMULATED IN RCHLNK. */
  for (k = 1; k <= neqns; ++k) {
    knz = 0;
    mrgk = mrglnk[k];
    mrkflg = 0;
    marker[k] = k;
    if (mrgk != 0) 
      marker[k] = marker[mrgk];
    xnzsub[k] = nzend;
    node = perm[k];

    if (xadj[node] >= xadj[node+1]) {
      xlnz[k+1] = xlnz[k];
      continue;
    }

    /* USE RCHLNK TO LINK THROUGH THE STRUCTURE OF A(*,K) BELOW DIAGONAL */
    rchlnk[k] = neqns+1;
    for (j=xadj[node]; j<xadj[node+1]; j++) {
      nabor = invp[adjncy[j] + 1];
      if (nabor <= k) 
        continue;
      rchm = k;

      do {
        m = rchm;
        rchm = rchlnk[m];
      } while (rchm <= nabor);

      knz++;
      rchlnk[m] = nabor;
      rchlnk[nabor] = rchm;
      if (marker[nabor] != marker[k])
        mrkflg = 1;
    }

    /* TEST FOR MASS SYMBOLIC ELIMINATION */
    lmax = 0;
    if (mrkflg != 0 || mrgk == 0 || mrglnk[mrgk] != 0)
      goto L350;
    xnzsub[k] = xnzsub[mrgk] + 1;
    knz = xlnz[mrgk + 1] - (xlnz[mrgk] + 1);
    goto L1400;


    /* LINK THROUGH EACH COLUMN I THAT AFFECTS L(*,K) */
L350:
    i = k;
    while ((i = mrglnk[i]) != 0) {
      inz = xlnz[i+1] - (xlnz[i]+1);
      jstrt = xnzsub[i] + 1;
      jstop = xnzsub[i] + inz;

      if (inz > lmax) { 
        lmax = inz;
        xnzsub[k] = jstrt;
      }

      /* MERGE STRUCTURE OF L(*,I) IN NZSUB INTO RCHLNK. */ 
      rchm = k;
      for (j = jstrt; j <= jstop; ++j) {
        nabor = nzsub[j];
        do {
          m = rchm;
          rchm = rchlnk[m];
        } while (rchm < nabor);

        if (rchm != nabor) {
          knz++;
          rchlnk[m] = nabor;
          rchlnk[nabor] = rchm;
          rchm = nabor;
        }
      }
    }

    /* CHECK IF SUBSCRIPTS DUPLICATE THOSE OF ANOTHER COLUMN */
    if (knz == lmax)
      goto L1400;

    /* OR IF TAIL OF K-1ST COLUMN MATCHES HEAD OF KTH */
    if (nzbeg > nzend)
      goto L1200;

    i = rchlnk[k];
    for (jstrt = nzbeg; jstrt <= nzend; ++jstrt) {
      if (nzsub[jstrt] < i)
        continue;

      if (nzsub[jstrt] == i)
        goto L1000;
      else 
        goto L1200;
    }
    goto L1200;

L1000:
    xnzsub[k] = jstrt;
    for (j = jstrt; j <= nzend; ++j) {
      if (nzsub[j] != i)
        goto L1200;

      i = rchlnk[i];
      if (i > neqns)
        goto L1400;
    }
    nzend = jstrt - 1;

    /* COPY THE STRUCTURE OF L(*,K) FROM RCHLNK TO THE DATA STRUCTURE (XNZSUB, NZSUB) */
L1200:
    nzbeg = nzend + 1;
    nzend += knz;

    if (nzend > *maxsub) {
      flag = 1; /* Out of memory */
      break;
    }

    i = k;
    for (j=nzbeg; j<=nzend; ++j) {
      i = rchlnk[i];
      nzsub[j] = i;
      marker[i] = k;
    }
    xnzsub[k] = nzbeg;
    marker[k] = k;

    /*
     * UPDATE THE VECTOR MRGLNK.  NOTE COLUMN L(*,K) JUST FOUND
     * IS REQUIRED TO DETERMINE COLUMN L(*,J), WHERE
     * L(J,K) IS THE FIRST NONZERO IN L(*,K) BELOW DIAGONAL.
     */
L1400:
    if (knz > 1) {
      kxsub = xnzsub[k];
      i = nzsub[kxsub];
      mrglnk[k] = mrglnk[i];
      mrglnk[i] = k;
    }

    xlnz[k + 1] = xlnz[k] + knz;
  }

  if (flag == 0) {
    *maxlnz = xlnz[neqns] - 1;
    *maxsub = xnzsub[neqns];
    xnzsub[neqns + 1] = xnzsub[neqns];
  }

  nzsub++;
  xnzsub++;
  xlnz++;
  invp++;
  perm++;
  xadj++;

  return (flag);
}
