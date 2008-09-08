/* Copyright 2008 ENSEIRB, INRIA & CNRS
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
/**   NAME       : dgmap.c                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel static mapping       **/
/**                software.                               **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 12 jun 2008     **/
/**                                 to   : 28 aug 2008     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define DGMAP
#define SCOTCH_PTSCOTCH

#include <sys/time.h>

#include "module.h"
#include "common.h"
#include "ptscotch.h"
#include "dgmap.h"

/*
**  The static variables.
*/

static int                  C_partNbr = 2;        /* Default number of parts     */
static int                  C_paraNum = 0;        /* Number of parameters        */
static int                  C_paraNbr = 0;        /* No parameters for mapping   */
static int                  C_fileNum = 0;        /* Number of file in arg list  */
static int                  C_fileNbr = 4;        /* Number of files for mapping */
static File                 C_fileTab[C_FILENBR] = { /* File array               */
                              { "-", NULL, "r" },
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {     /* Usage */
  "dgmap [<input source file> [<input target file> [<output mapping file> [<output log file>]]]] <options>",
  "dgpart [<nparts>] [<input source file> [<output mapping file> [<output log file>]]] <options>",
  "  -h         : Display this help",
  "  -m<strat>  : Set parallel mapping strategy (see user's manual)",
  "  -r<num>    : Set root process for centralized files (default is 0)",
  "  -s<obj>    : Force unity weights on <obj>:",
  "                 e  : edges",
  "                 v  : vertices",
  "  -V         : Print program version and copyright",
  "  -v<verb>   : Set verbose mode to <verb>:",
  "                 m  : mapping information",
  "                 s  : strategy information",
  "                 t  : timing information",
  "",
  "See default strategy with option '-vs'",
  NULL };

/******************************/
/*                            */
/* This is the main function. */
/*                            */
/******************************/

int
main (
int                         argc,
char *                      argv[])
{
  SCOTCH_Dgraph       grafdat;                    /* Source graph            */
  SCOTCH_Num          grafflag;                   /* Source graph properties */
  SCOTCH_Arch         archdat;                    /* Target architecture     */
  SCOTCH_Strat        stradat;                    /* Mapping strategy        */
  SCOTCH_Dmapping     mapdat;                     /* Mapping data            */
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

  flagval = C_FLAGNONE;                           /* Default behavior */
  i = strlen (argv[0]);
  if ((i >= 5) && (strncmp (argv[0] + i - 5, "gpart", 5) == 0)) {
    flagval |= C_FLAGPART;
    C_paraNbr = 1;                                /* One more parameter       */
    C_fileNbr = 3;                                /* One less file to provide */
    errorProg ("dgpart");
  }
  else
    errorProg ("dgmap");

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

  grafflag = 0;                                   /* Use vertex and edge weights */
  if (SCOTCH_stratInit (&stradat) != 0) {
    errorPrint ("main: cannot initialize strategy");
    return     (1);
  }

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes                        */
    if ((argv[i][0] != '-') || (argv[i][1] == '\0') || (argv[i][1] == '.')) { /* If found a file name */
      if (C_paraNum < C_paraNbr) {                /* If number of parameters not reached              */
        if ((C_partNbr = atoi (argv[i])) < 1) {   /* Get the number of parts                          */
          errorPrint ("main: invalid number of parts (\"%s\")", argv[i]);
          exit       (1);
        }
        C_paraNum ++;
        continue;                                 /* Process the other parameters */
      }
      if (C_fileNum < C_fileNbr)                  /* A file name has been given */
        C_fileTab[C_fileNum ++].name = argv[i];
      else {
        errorPrint ("main: too many file names given");
        return     (1);
      }
    }
    else {                                        /* If found an option name */
      switch (argv[i][1]) {
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
        case 'M' :
        case 'm' :
          SCOTCH_stratExit (&stradat);
          SCOTCH_stratInit (&stradat);
          if ((SCOTCH_stratDgraphMap (&stradat, &argv[i][2])) != 0) {
            errorPrint ("main: invalid parallel mapping strategy");
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
        case 'S' :
        case 's' :                                /* Source graph parameters */
          for (j = 2; argv[i][j] != '\0'; j ++) {
            switch (argv[i][j]) {
              case 'E' :
              case 'e' :
                grafflag |= 2;                    /* Do not load edge weights */
                break;
              case 'V' :
              case 'v' :
                grafflag |= 1;                    /* Do not load vertex weights */
                break;
              default :
                errorPrint ("main: invalid source graph option (\"%c\")",
                            argv[i][j]);
                return     (1);
            }
          }
          break;
        case 'V' :
          fprintf (stderr, "dgmap/dgpart, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2008 ENSEIRB, INRIA & CNRS, France\n");
          fprintf (stderr, "This software is libre/free software under CeCILL-C -- see the user's manual for more information\n");
          return  (0);
        case 'v' :                                /* Output control info */
          for (j = 2; argv[i][j] != '\0'; j ++) {
            switch (argv[i][j]) {
              case 'M' :
              case 'm' :
                flagval |= C_FLAGVERBMAP;
                break;
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
                return (1);
            }
          }
          break;
        default :
          errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
          return     (1);
      }
    }
  }
  if ((flagval & C_FLAGPART) != 0) {              /* If program run as the partitioner            */
    C_fileTab[3].name = C_fileTab[2].name;        /* Put provided file names at their right place */
    C_fileTab[2].name = C_fileTab[1].name;
    C_fileTab[1].name = "-";
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

  SCOTCH_archInit (&archdat);                     /* Create architecture structure          */
  if ((flagval & C_FLAGPART) != 0)                /* If program run as the partitioner      */
    SCOTCH_archCmplt (&archdat, C_partNbr);       /* Create a complete graph of proper size */
  else
    SCOTCH_archLoad (&archdat, C_filepntrtgtinp); /* Read target architecture */

  clockStop (&runtime[0]);                        /* Get input time */
  clockInit (&runtime[1]);

#ifdef SCOTCH_DEBUG_ALL
  if ((flagval & C_FLAGDEBUG) != 0)
    MPI_Barrier (MPI_COMM_WORLD);
#endif /* SCOTCH_DEBUG_ALL */

  clockStart (&runtime[1]);

  SCOTCH_dgraphMapInit    (&grafdat, &mapdat, &archdat, NULL);
  SCOTCH_dgraphMapCompute (&grafdat, &mapdat, &stradat); /* Perform mapping */

  clockStop (&runtime[1]);                        /* Get computation time */

#ifdef SCOTCH_DEBUG_ALL
  if ((flagval & C_FLAGDEBUG) != 0)
    MPI_Barrier (MPI_COMM_WORLD);
#endif /* SCOTCH_DEBUG_ALL */

  clockStart (&runtime[0]);

  SCOTCH_dgraphMapSave (&grafdat, &mapdat, (proclocnum == protglbnum) ? C_filepntrmapout : NULL); /* Write mapping */

  clockStop (&runtime[0]);                        /* Get output time */

#ifdef SCOTCH_DEBUG_ALL
  if ((flagval & C_FLAGDEBUG) != 0)
    MPI_Barrier (MPI_COMM_WORLD);
#endif /* SCOTCH_DEBUG_ALL */

  if ((flagval & C_FLAGVERBTIM) != 0) {
    MPI_Type_contiguous (3, MPI_DOUBLE, &redutype);
    MPI_Type_commit     (&redutype);
    MPI_Op_create       ((MPI_User_function *) dgmapStatReduceOp, 1, &reduop);

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
      fprintf (C_filepntrlogout, "T\tMapping\tmin=%g\tmax=%g\tavg=%g\nT\tI/O\tmin=%g\tmax=%g\tavg=%g\nT\tTotal\tmin=%g\tmax=%g\tavg=%g\n",
               reduglbtab[0], reduglbtab[1], reduglbtab[2] / (double) procglbnbr,
               reduglbtab[3], reduglbtab[4], reduglbtab[5] / (double) procglbnbr,
               reduglbtab[6], reduglbtab[7], reduglbtab[8] / (double) procglbnbr);
    }
  }
  if (flagval & C_FLAGVERBMAP)
    SCOTCH_dgraphMapView (&grafdat, &mapdat, C_filepntrlogout);

  fileBlockClose (C_fileTab, C_FILENBR);          /* Always close explicitely to end eventual (un)compression tasks */

  SCOTCH_dgraphMapExit (&grafdat, &mapdat);
  SCOTCH_dgraphExit    (&grafdat);
  SCOTCH_stratExit     (&stradat);
  SCOTCH_archExit      (&archdat);

  MPI_Finalize ();
#ifdef COMMON_PTHREAD
  pthread_exit ((void *) 0);                      /* Allow potential (un)compression tasks to complete */
#endif /* COMMON_PTHREAD */
  return (0);
}

/* Reduction routine for statistics output.
*/

void
dgmapStatReduceOp (
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
