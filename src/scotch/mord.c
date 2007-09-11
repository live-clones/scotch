/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : mord.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a mesh reordering software.     **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 15 nov 2002     **/
/**                                 to   : 06 jan 2006     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MORD

#include "common.h"
#include "scotch.h"
#include "mord.h"

/*
**  The static and global definitions.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "mord [<input source mesh file> [<output ordering file> [<output log file>]]] <options>",
  "  -h         : Display this help",
  "  -m<file>   : Save column block mapping data to <file>",
  "  -o<strat>  : Use mesh ordering strategy <strat> (see user's manual)",
  "               (see default strategy with option '-vs')",
  "  -t<file>   : Save partitioning tree data to <file>",
  "  -V         : Print program version and copyright",
  "  -v<verb>   : Set verbose mode to <verb> :",
  "                 o  : ordering information",
  "                 s  : strategy information",
  "                 t  : timing information",
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
  SCOTCH_Num          vnodnbr;                    /* Number of nodes   */
  SCOTCH_Mesh         meshdat;                    /* Source graph      */
  SCOTCH_Strat        ordestrat;                  /* Ordering strategy */
  SCOTCH_Ordering     ordedat;                    /* Graph ordering    */
  SCOTCH_Num *        permtab;                    /* Permutation array */
  Clock               runtime[2];                 /* Timing variables  */
  int                 flag;
  int                 i, j;

  errorProg ("mord");

  intRandInit ();

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  flag = C_FLAGNONE;
  SCOTCH_stratInit (&ordestrat);

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
        case 'H' :                                /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'M' :                                /* Output separator mapping */
        case 'm' :
          flag |= C_FLAGMAPOUT;
          if (argv[i][2] != '\0')
            C_filenamemapout = &argv[i][2];
          break;
        case 'O' :                                /* Ordering strategy */
        case 'o' :
          SCOTCH_stratExit (&ordestrat);
          SCOTCH_stratInit (&ordestrat);
          if ((SCOTCH_stratMeshOrder (&ordestrat, &argv[i][2])) != 0) {
            errorPrint ("main: invalid ordering strategy");
            return     (1);
          }
          break;
        case 'V' :
          fprintf (stderr, "mord, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004,2007 ENSEIRB, INRIA & CNRS, France\n");
          fprintf (stderr, "This software is libre/free software under CeCILL-C -- see the user's manual for more information\n");
          return  (0);
          break;
        case 'v' :                               /* Output control info */
          for (j = 2; argv[i][j] != '\0'; j ++) {
            switch (argv[i][j]) {
              case 'S' :
              case 's' :
                flag |= C_FLAGVERBSTR;
                break;
              case 'T' :
              case 't' :
                flag |= C_FLAGVERBTIM;
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

  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      if ((C_fileTab[i].pntr = fopen (C_fileTab[i].name, C_fileTab[i].mode)) == NULL) { /* Open the file */
        errorPrint ("main: cannot open file (%d)", i);
        return     (1);
      }
    }
  }

  clockInit  (&runtime[0]);
  clockStart (&runtime[0]);

  SCOTCH_meshInit (&meshdat);                     /* Create mesh structure */
  SCOTCH_meshLoad (&meshdat, C_filepntrsrcinp, -1); /* Read source mesh    */
  SCOTCH_meshSize (&meshdat, NULL, &vnodnbr, NULL); /* Get number of nodes */

  clockStop  (&runtime[0]);                       /* Get input time */
  clockInit  (&runtime[1]);
  clockStart (&runtime[1]);

  if ((permtab = (SCOTCH_Num *) memAlloc (vnodnbr * sizeof (SCOTCH_Num))) == NULL) {
    errorPrint ("main: out of memory");
    return     (1);
  }
  SCOTCH_meshOrderInit    (&meshdat, &ordedat, permtab, NULL, NULL, NULL, NULL); /* Create ordering */
  SCOTCH_meshOrderCompute (&meshdat, &ordedat, &ordestrat); /* Perform ordering */

  clockStop (&runtime[1]);                        /* Get ordering time */

#ifdef SCOTCH_DEBUG_ALL
  if (SCOTCH_meshOrderCheck (&meshdat, &ordedat) != 0)
    return (1);
#endif /* SCOTCH_DEBUG_ALL */

  clockStart (&runtime[0]);

  SCOTCH_meshOrderSave (&meshdat, &ordedat, C_filepntrordout); /* Write ordering     */
  if (flag & C_FLAGMAPOUT)                        /* If mapping wanted               */
    SCOTCH_meshOrderSaveMap (&meshdat, &ordedat, C_filepntrmapout); /* Write mapping */

  clockStop  (&runtime[0]);                       /* Get output time */

  if (flag & C_FLAGVERBSTR) {
    fprintf (C_filepntrlogout, "S\tStrat=");
    SCOTCH_stratSave (&ordestrat, C_filepntrlogout);
    putc ('\n', C_filepntrlogout);
  }
  if (flag & C_FLAGVERBTIM) {
    fprintf (C_filepntrlogout, "T\tOrder\t\t%g\nT\tI/O\t\t%g\nT\tTotal\t\t%g\n",
             (double) clockVal (&runtime[1]),
             (double) clockVal (&runtime[0]),
             (double) clockVal (&runtime[0]) +
             (double) clockVal (&runtime[1]));
  }

  SCOTCH_meshOrderExit (&meshdat, &ordedat);
  SCOTCH_stratExit     (&ordestrat);
  SCOTCH_meshExit      (&meshdat);
  memFree              (permtab);

  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                /* Close the stream */
    }
  }

  return (0);
}

/*******************************************/
/*                                         */
/* Stubs to avoid including target module. */
/*                                         */
/*******************************************/

SCOTCH_Num
T_domWght (
const void * const          arch,
const void * const          dom)
{
  errorPrint ("T_domWghtMord: internal error");
  return     (1);
}

SCOTCH_Num
_SCOTCHTdomWght (
const void * const          arch,
const void * const          dom)
{
  errorPrint ("T_domWghtMord: internal error");
  return     (1);
}

SCOTCH_Num
T_domDist (
const void * const          arch,
const void * const          dom0,
const void * const          dom1)
{
  errorPrint ("T_domDistMord: internal error");
  return     (1);
}

SCOTCH_Num
_SCOTCHTdomDist (
const void * const          arch,
const void * const          dom0,
const void * const          dom1)
{
  errorPrint ("T_domDistMord: internal error");
  return     (1);
}
