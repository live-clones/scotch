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
/**   NAME       : dgord.c                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel sparse matrix        **/
/**                ordering software.                      **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 30 apr 2006     **/
/**                                 to   : 16 jun 2008     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define DGORD
#define SCOTCH_PTSCOTCH

#include <sys/time.h>

#include "module.h"
#include "common.h"
#include "ptscotch.h"
#include "dgord.h"

/*
**  The static and global definitions.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "dgord [<input source file> [<output ordering file> [<output log file>]]] <options>",
  "  -b         : Output block ordering data instead of plain ordering data",
  "  -h         : Display this help",
  "  -m<file>   : Save column block mapping data to <file>",
  "  -o<strat>  : Set parallel ordering strategy (see user's manual)",
  "  -r<num>    : Set root process for centralized files (default is 0)",
  "  -t<file>   : Save partitioning tree data to <file>",
  "  -V         : Print program version and copyright",
  "  -v<verb>   : Set verbose mode to <verb> :",
  "                 s  : strategy information",
  "                 t  : timing information",
  "",
  "See default strategy with option '-vs'",
  NULL };

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
  SCOTCH_Strat        stradat;
  SCOTCH_Dgraph       grafdat;
  SCOTCH_Dordering    ordedat;
  int                 procglbnbr;
  int                 proclocnum;
  int                 protglbnum;                 /* Root process        */
  Clock               runtime[2];                 /* Timing variables    */
  double              reduloctab[9];              /* 3 * (min, max, sum) */
  double              reduglbtab[9];
  MPI_Datatype        redutype;
  MPI_Op              reduop;
  int                 flagval;
  int                 i, j;
#ifdef SCOTCH_PTHREAD
  int                 thrdlvlreqval;
  int                 thrdlvlproval;
#endif /* SCOTCH_PTHREAD */

  errorProg ("dgord");

#ifdef SCOTCH_PTHREAD
  thrdlvlreqval = MPI_THREAD_MULTIPLE;
  if (MPI_Init_thread (&argc, &argv, thrdlvlreqval, &thrdlvlproval) != MPI_SUCCESS) {
    errorPrint ("main: Cannot initialize (1)");
    exit       (1);
  }
  if (thrdlvlreqval > thrdlvlproval) {
    errorPrint ("main: MPI implementation is not thread-safe: recompile without SCOTCH_PTHREAD");
    exit       (1);
  }
#else /* SCOTCH_PTHREAD */
  if (MPI_Init (&argc, &argv) != MPI_SUCCESS) {
    errorPrint ("main: Cannot initialize (2)");
    exit       (1);
  }
#endif /* SCOTCH_PTHREAD */

  MPI_Comm_size (MPI_COMM_WORLD, &procglbnbr);    /* Get communicator data */
  MPI_Comm_rank (MPI_COMM_WORLD, &proclocnum);
  protglbnum = 0;                                 /* Assume root process is process 0 */

  intRandInit ();

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  flagval = C_FLAGNONE;
  if (SCOTCH_stratInit (&stradat) != 0) {
    errorPrint ("main: cannot initialize strategy");
    return     (1);
  }

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes                        */
    if ((argv[i][0] != '-') || (argv[i][1] == '\0') || (argv[i][1] == '.')) { /* If found a file name */
      if (C_fileNum < C_FILEARGNBR)               /* File name has been given                         */
        C_fileTab[C_fileNum ++].name = argv[i];
      else {
        errorPrint ("main: too many file names given");
        return     (1);
      }
    }
    else {                                        /* If found an option name */
      switch (argv[i][1]) {
        case 'B' :
        case 'b' :
          flagval |= C_FLAGBLOCK;
          break;
#ifdef SCOTCH_DEBUG_ALL
        case 'D' :
        case 'd' :
          flagval |= C_FLAGDEBUG;
          break;
#endif /* SCOTCH_DEBUG_ALL */
        case 'H' :                                /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'M' :                                /* Output separator mapping */
        case 'm' :
          flagval |= C_FLAGMAPOUT;
          if (argv[i][2] != '\0')
            C_filenamemapout = &argv[i][2];
          break;
        case 'O' :                                /* Ordering strategy */
        case 'o' :
          SCOTCH_stratExit (&stradat);
          SCOTCH_stratInit (&stradat);
          if ((SCOTCH_stratDgraphOrder (&stradat, &argv[i][2])) != 0) {
            errorPrint ("main: invalid parallel ordering strategy");
            return     (1);
          }
          break;
        case 'R' :                                /* Root process (if necessary) */
        case 'r' :
          protglbnum = atoi (&argv[i][2]);
          if ((protglbnum < 0)           ||
              (protglbnum >= procglbnbr) ||
              ((protglbnum == 0) && (argv[i][2] != '0'))) {
            errorPrint ("main: invalid root process number");
            return     (1);
          }
          break;
        case 'T' :                                /* Output separator tree */
        case 't' :
          flagval |= C_FLAGTREOUT;
          if (argv[i][2] != '\0')
            C_filenametreout = &argv[i][2];
          break;
        case 'V' :
          fprintf (stderr, "dgord, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2007,2008 ENSEIRB, INRIA & CNRS, France\n");
          fprintf (stderr, "This software is libre/free software under CeCILL-C -- see the user's manual for more information\n");
          return  (0);
        case 'v' :                                /* Output control info */
          for (j = 2; argv[i][j] != '\0'; j ++) {
            switch (argv[i][j]) {
              case 'S' :
              case 's' :
                flagval |= C_FLAGVERBSTR;
                break;
              case 'T' :
              case 't' :
                flagval |= C_FLAGVERBTIM;
                break;
              default :
                errorPrint ("main: unprocessed parameter \"%c\" in \"%s\"",
                            argv[i][j], argv[i]);
                return     (1);
            }
          }
          break;
        default :
          errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
          return     (1);
      }
    }
  }

#ifdef SCOTCH_DEBUG_ALL
  if ((flagval & C_FLAGDEBUG) != 0) {
    fprintf (stderr, "Proc %4d of %d, pid %d\n", proclocnum, procglbnbr, getpid ());
    if (proclocnum == protglbnum) {               /* Synchronize on keybord input */
      char           c;

      printf ("Waiting for key press...\n");
      scanf ("%c", &c);
    }
    MPI_Barrier (MPI_COMM_WORLD);
  }
#endif /* SCOTCH_DEBUG_ALL */

  fileBlockOpenDist (C_fileTab, C_FILENBR, procglbnbr, proclocnum, protglbnum); /* Open all files */

  clockInit  (&runtime[0]);
  clockStart (&runtime[0]);

  if (SCOTCH_dgraphInit (&grafdat, MPI_COMM_WORLD) != 0) { /* Initialize distributed source graph */
    errorPrint ("main: cannot initialize distributed graph");
    return     (1);
  }
  if (SCOTCH_dgraphLoad (&grafdat, C_filepntrsrcinp, -1, 0) != 0) {
    errorPrint ("main: cannot load distributed graph");
    return     (1);
  }

  clockStop (&runtime[0]);                        /* Get input time */
  clockInit (&runtime[1]);

#ifdef SCOTCH_DEBUG_ALL
  if ((flagval & C_FLAGDEBUG) != 0)
    MPI_Barrier (MPI_COMM_WORLD);
#endif /* SCOTCH_DEBUG_ALL */

  clockStart (&runtime[1]);

  if (SCOTCH_dgraphOrderInit (&grafdat, &ordedat) != 0) { /* Initialize source graph */
    errorPrint ("main: cannot initialize distributed ordering");
    return     (1);
  }

  if (SCOTCH_dgraphOrderCompute (&grafdat, &ordedat, &stradat) != 0) { /* Initialize source graph */
    errorPrint ("main: cannot compute ordering");
    return     (1);
  }

  clockStop (&runtime[1]);                        /* Get ordering time */

#ifdef SCOTCH_DEBUG_ALL
  if ((flagval & C_FLAGDEBUG) != 0)
    MPI_Barrier (MPI_COMM_WORLD);
#endif /* SCOTCH_DEBUG_ALL */

  clockStart (&runtime[0]);

  if (proclocnum == protglbnum) {
    if ((flagval & C_FLAGBLOCK) == 0)
      SCOTCH_dgraphOrderSave (&grafdat, &ordedat, C_filepntrordout);
    else
      SCOTCH_dgraphOrderSaveBlock (&grafdat, &ordedat, C_filepntrordout);
    if ((flagval & C_FLAGMAPOUT) != 0)            /* If mapping wanted                   */
      SCOTCH_dgraphOrderSaveMap (&grafdat, &ordedat, C_filepntrmapout); /* Write mapping */
    if ((flagval & C_FLAGTREOUT) != 0)            /* If separator tree wanted            */
      SCOTCH_dgraphOrderSaveTree (&grafdat, &ordedat, C_filepntrtreout); /* Write tree   */
  }
  else {
    if ((flagval & C_FLAGBLOCK) == 0)
      SCOTCH_dgraphOrderSave (&grafdat, &ordedat, NULL);
    else
      SCOTCH_dgraphOrderSaveBlock (&grafdat, &ordedat, NULL);
    if ((flagval & C_FLAGMAPOUT) != 0)
      SCOTCH_dgraphOrderSaveMap (&grafdat, &ordedat, NULL);
    if ((flagval & C_FLAGTREOUT) != 0)
      SCOTCH_dgraphOrderSaveTree (&grafdat, &ordedat, NULL);
  }

  clockStop (&runtime[0]);

#ifdef SCOTCH_DEBUG_ALL
  if ((flagval & C_FLAGDEBUG) != 0)
    MPI_Barrier (MPI_COMM_WORLD);
#endif /* SCOTCH_DEBUG_ALL */

  if ((flagval & C_FLAGVERBTIM) != 0) {
    MPI_Type_contiguous (3, MPI_DOUBLE, &redutype);
    MPI_Type_commit     (&redutype);
    MPI_Op_create       ((MPI_User_function *) dgordStatReduceOp, 1, &reduop);

    reduloctab[0] =
    reduloctab[1] =
    reduloctab[2] = (double) clockVal (&runtime[1]);
    reduloctab[3] =
    reduloctab[4] =
    reduloctab[5] = (double) clockVal (&runtime[0]);
    reduloctab[6] =
    reduloctab[7] =
    reduloctab[8] = reduloctab[0] + reduloctab[3];

    MPI_Reduce    (reduloctab, reduglbtab, 3, redutype, reduop, protglbnum, MPI_COMM_WORLD);
    MPI_Op_free   (&reduop);
    MPI_Type_free (&redutype);
  }

  if (C_filepntrlogout != NULL) {
    if ((flagval & C_FLAGVERBSTR) != 0) {
      fprintf (C_filepntrlogout, "S\tStrat=");
      SCOTCH_stratSave (&stradat, C_filepntrlogout);
      putc ('\n', C_filepntrlogout);
    }
    if ((flagval & C_FLAGVERBTIM) != 0) {
      fprintf (C_filepntrlogout, "T\tOrder\tmin=%g\tmax=%g\tavg=%g\nT\tI/O\tmin=%g\tmax=%g\tavg=%g\nT\tTotal\tmin=%g\tmax=%g\tavg=%g\n",
               reduglbtab[0], reduglbtab[1], reduglbtab[2] / (double) procglbnbr,
               reduglbtab[3], reduglbtab[4], reduglbtab[5] / (double) procglbnbr,
               reduglbtab[6], reduglbtab[7], reduglbtab[8] / (double) procglbnbr);
    }
  }

  fileBlockClose (C_fileTab, C_FILENBR);          /* Always close explicitely to end eventual (un)compression tasks */

  SCOTCH_dgraphOrderExit (&grafdat, &ordedat);
  SCOTCH_dgraphExit      (&grafdat);
  SCOTCH_stratExit       (&stradat);

  MPI_Finalize ();
#ifdef COMMON_PTHREAD
  pthread_exit ((void *) 0);                      /* Allow potential (un)compression tasks to complete */
#endif /* COMMON_PTHREAD */
  return (0);
}

/* Reduction routine for statistics output.
*/

void
dgordStatReduceOp (
double *                    in,
double *                    inout,
int *                       len,
MPI_Datatype *              datatype)
{
  int                 i;

  for (i = 0; i < *len; i ++) {
    inout[3 * i]     = MIN (in[3 * i],      inout[3 * i]);
    inout[3 * i + 1] = MAX (in[3 * i + 1],  inout[3 * i + 1]);
    inout[3 * i + 2] =      in[3 * i + 2] + inout[3 * i + 2];
  }
}
