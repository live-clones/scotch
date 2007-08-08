/* Copyright 2004,2007 INRIA
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
/**   NAME       : gmap.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a graph mapping software.       **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 05 jan 1993     **/
/**                                 to     12 may 1993     **/
/**                # Version 1.1  : from : 15 oct 1993     **/
/**                                 to     15 oct 1993     **/
/**                # Version 1.3  : from : 06 apr 1994     **/
/**                                 to     18 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     17 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to     18 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     02 oct 1995     **/
/**                # Version 3.1  : from : 07 nov 1995     **/
/**                                 to     25 apr 1996     **/
/**                # Version 3.2  : from : 24 sep 1996     **/
/**                                 to     26 may 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to   : 30 mar 1999     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                # Version 4.0  : from : 16 jan 2004     **/
/**                                 to   : 27 dec 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GMAP

#include "common.h"
#include "scotch.h"
#include "gmap.h"

/*
**  The static variables.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "r" },
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {     /* Usage */
  "map [<input source file> [<input target file> [<output mapping file> [<output log file>]]]] <options>",
  "  -h         : Display this help",
  "  -m<strat>  : Apply mapping strategy <strat>, made of methods:",
  "                 b  : Dual Recursive Bipartitioning",
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
  SCOTCH_Graph        grafdat;                    /* Source graph            */
  SCOTCH_Num          grafflag;                   /* Source graph properties */
  SCOTCH_Num          vertnbr;                    /* Source graph size       */
  SCOTCH_Arch         archdat;                    /* Target architecture     */
  SCOTCH_Strat        mapstrat;                   /* Mapping strategy        */
  SCOTCH_Mapping      mapdat;                     /* Mapping data            */
  Clock               runtime[2];                 /* Timing variables        */
  int                 flag;
  int                 i, j;

  errorProg ("gmap");

  intRandInit ();

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  flag     = C_FLAGNONE;                          /* Default behavior             */
  grafflag = 0;                                   /* Use vertex and edge weights  */
  SCOTCH_stratInit (&mapstrat);                   /* Set default mapping strategy */

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_fileNum < C_FILEARGNBR)               /* File name has been given */
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
        case 'M' :
        case 'm' :
          SCOTCH_stratExit (&mapstrat);
          SCOTCH_stratInit (&mapstrat);
          if ((SCOTCH_stratGraphMap (&mapstrat, &argv[i][2])) != 0) {
            errorPrint ("main: invalid mapping strategy");
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
          fprintf (stderr, "gmap, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004,2007 INRIA, France\n");
          fprintf (stderr, "This software is libre/free software under CeCILL-C -- see the user's manual for more information\n");
          return  (0);
        case 'v' :                                /* Output control info */
          for (j = 2; argv[i][j] != '\0'; j ++) {
            switch (argv[i][j]) {
              case 'M' :
              case 'm' :
                flag |= C_FLAGVERBMAP;
                break;
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

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      if ((C_fileTab[i].pntr = fopen (C_fileTab[i].name, C_fileTab[i].mode)) == NULL) { /* Open the file */
        errorPrint ("main: cannot open file (%d)", i);
        return     (1);
      }
    }
  }

  clockInit  (&runtime[0]);
  clockStart (&runtime[0]);

  SCOTCH_graphInit (&grafdat);                    /* Create graph structure         */
  SCOTCH_graphLoad (&grafdat, C_filepntrsrcinp, -1, grafflag); /* Read source graph */
  SCOTCH_graphSize (&grafdat, &vertnbr, NULL);    /* Get graph characteristics      */

  SCOTCH_archInit (&archdat);                     /* Create architecture structure */
  SCOTCH_archLoad (&archdat, C_filepntrtgtinp);   /* Read target architecture      */

  clockStop  (&runtime[0]);                       /* Get input time */
  clockInit  (&runtime[1]);
  clockStart (&runtime[1]);

  SCOTCH_graphMapInit    (&grafdat, &mapdat, &archdat, NULL);
  SCOTCH_graphMapCompute (&grafdat, &mapdat, &mapstrat); /* Perform mapping */

  clockStop  (&runtime[1]);                       /* Get computation time */
  clockStart (&runtime[0]);

  SCOTCH_graphMapSave (&grafdat, &mapdat, C_filepntrmapout); /* Write mapping */

  clockStop  (&runtime[0]);                       /* Get output time */

  if (flag & C_FLAGVERBSTR) {
    fprintf (C_filepntrlogout, "S\tStrat=");
    SCOTCH_stratSave (&mapstrat, C_filepntrlogout);
    putc ('\n', C_filepntrlogout);
  }
  if (flag & C_FLAGVERBTIM) {
    fprintf (C_filepntrlogout, "T\tTimeTotal=%g\nT\tTimeIO=%g\nT\tTimeComput=%g\n",
             (double) clockVal (&runtime[0]) +
             (double) clockVal (&runtime[1]),
             (double) clockVal (&runtime[0]),
             (double) clockVal (&runtime[1]));
  }
  if (flag & C_FLAGVERBMAP)
    SCOTCH_graphMapView (&grafdat, &mapdat, C_filepntrlogout);

#ifdef SCOTCH_DEBUG_MAIN1
  SCOTCH_mapExit   (&mapdat);
  SCOTCH_stratExit (&mapstrat);
  SCOTCH_archExit  (&archdat);
  SCOTCH_graphExit (&grafdat);

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  return (0);
}
