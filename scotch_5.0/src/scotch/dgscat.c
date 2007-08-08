/* Copyright 2007 INRIA
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
/**   NAME       : dgscat.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This program distributes a source graph **/
/**                across processors and saves it as a     **/
/**                distributed source graph.               **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 17 may 2007     **/
/**                                 to   : 23 jun 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define DGSCAT
#define SCOTCH_PTSCOTCH

#include "common.h"
#include "ptscotch.h"
#include "dgscat.h"

/*
**  The static and global definitions.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "dgscat [<input (centralized) source file> [<output distributed source file>]] <options>",
  "  -c       : Check the input graph after loading",
  "  -h       : Display this help",
  "  -r<num>  : Set root process for centralized files (default is 0)",
  "  -V       : Print program version and copyright",
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
  int                 thrdlvlreqval;
  int                 thrdlvlproval;
  SCOTCH_Dgraph       grafdat;
  int                 procglbnbr;
  int                 proclocnum;
  int                 protglbnum;                 /* Root process */
  int                 flagval;
  int                 i;

  thrdlvlreqval = MPI_THREAD_MULTIPLE;
  if (MPI_Init_thread (&argc, &argv, thrdlvlreqval, &thrdlvlproval) != MPI_SUCCESS) {
    errorPrint ("main: Cannot initialize (1)");
    exit       (1);
  }
  if (thrdlvlreqval > thrdlvlproval) {
    errorPrint ("main: Cannot initialize (2)");
    exit       (1);
  }

  MPI_Comm_size (MPI_COMM_WORLD, &procglbnbr);    /* Get communicator data */
  MPI_Comm_rank (MPI_COMM_WORLD, &proclocnum);
  protglbnum = 0;                                 /* Assume root process is process 0 */

  errorProg ("dgscat");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  flagval = C_FLAGNONE;

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
    else {                                        /* If found an option name */
      switch (argv[i][1]) {
        case 'C' :
        case 'c' :
          flagval |= C_FLAGCHECK;
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
        case 'V' :
        case 'v' :
          fprintf (stderr, "dgscat, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2007 INRIA, France\n");
          fprintf (stderr, "This software is libre/free software under CeCILL-C -- see the user's manual for more information\n");
          return  (0);
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

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names */
    if (fileNameDistExpand (&C_fileTab[i].name, procglbnbr, proclocnum, protglbnum) != 0) { /* If cannot allocate new name */
      errorPrint ("main: cannot create file name (%d)", i);
      return     (1);
    }
    if (C_fileTab[i].name == NULL)                /* If inexisting stream because not root process and centralized stream */
      C_fileTab[i].pntr = NULL;
    else {
      if ((C_fileTab[i].name[0] != '-') ||        /* If not standard stream, open it */
          (C_fileTab[i].name[1] != '\0')) {
        if ((C_fileTab[i].pntr = fopen (C_fileTab[i].name, C_fileTab[i].mode)) == NULL) { /* Open the file */
          errorPrint ("main: cannot open file (%d)", i);
          return     (1);
        }
      }
    }
  }

  SCOTCH_dgraphInit (&grafdat, MPI_COMM_WORLD);
  SCOTCH_dgraphLoad (&grafdat, C_filepntrsrcinp, -1, 0);

  if ((flagval & C_FLAGCHECK) != 0)
    SCOTCH_dgraphCheck (&grafdat);

  SCOTCH_dgraphSave (&grafdat, C_filepntrsrcout);

#ifdef SCOTCH_DEBUG_ALL
  SCOTCH_dgraphExit (&grafdat);

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name != NULL) &&            /* If existing stream     */
        ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
         (C_fileTab[i].name[1] != '\0'))) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_ALL */

  MPI_Finalize ();
  exit         (0);
}
