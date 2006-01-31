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
/**   NAME       : gord.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a sparse matrix graph ordering  **/
/**                software.                               **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 24 aug 1996     **/
/**                                 to   : 21 aug 1998     **/
/**                # Version 3.3  : from : 07 oct 1998     **/
/**                                 to   : 31 may 1999     **/
/**                # Version 3.4  : from : 07 oct 1998     **/
/**                                 to   : 03 feb 2000     **/
/**                # Version 4.0  : from : 02 feb 2002     **/
/**                                 to   : 08 jan 2006     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GORD

#include "common.h"
#include "scotch.h"
#include "gord.h"

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
  "gord [<input source file> [<output ordering file> [<output log file>]]] <options>",
  "  -h         : Display this help",
  "  -m<file>   : Save column block mapping data to <file>",
  "  -o<strat>  : Set ordering strategy (see user's manual)",
  "  -t<file>   : Save partitioning tree data to <file>",
  "  -V         : Print program version and copyright",
  "  -v<verb>   : Set verbose mode to <verb> :",
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
  SCOTCH_Num          vertnbr;                    /* Number of vertices */
  SCOTCH_Graph        grafdat;                    /* Source graph       */
  SCOTCH_Strat        ordestrat;                  /* Ordering strategy  */
  SCOTCH_Ordering     ordedat;                    /* Graph ordering     */
  SCOTCH_Num *        permtab;                    /* Permutation array  */
  Clock               runtime[2];                 /* Timing variables   */
  int                 flag;
  int                 i, j;

  errorProg ("gord");

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
          if ((SCOTCH_stratGraphOrder (&ordestrat, &argv[i][2])) != 0) {
            errorPrint ("main: invalid ordering strategy");
            return     (1);
          }
          break;
        case 'T' :                                /* Output separator tree */
        case 't' :
          flag |= C_FLAGTREOUT;
          if (argv[i][2] != '\0')
            C_filenametreout = &argv[i][2];
          break;
        case 'V' :
          fprintf (stderr, "gord, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004 INRIA, France\n");
          fprintf (stderr, "This software is libre/free software under LGPL -- see the user's manual for more information\n");
          return  (0);
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

  SCOTCH_graphInit (&grafdat);                    /* Create graph structure    */
  SCOTCH_graphLoad (&grafdat, C_filepntrsrcinp, -1, 2); /* Read source graph   */
  SCOTCH_graphSize (&grafdat, &vertnbr, NULL);    /* Get graph characteristics */

  clockStop  (&runtime[0]);                       /* Get input time */
  clockInit  (&runtime[1]);
  clockStart (&runtime[1]);

  if ((permtab = (SCOTCH_Num *) memAlloc (vertnbr * sizeof (SCOTCH_Num))) == NULL) {
    errorPrint ("main: out of memory");
    return     (1);
  }
  SCOTCH_graphOrderInit    (&grafdat, &ordedat, permtab, NULL, NULL, NULL, NULL); /* Create ordering */
  SCOTCH_graphOrderCompute (&grafdat, &ordedat, &ordestrat); /* Perform ordering */

  clockStop (&runtime[1]);                        /* Get ordering time */

#ifdef SCOTCH_DEBUG_ALL
  if (SCOTCH_graphOrderCheck (&grafdat, &ordedat) != 0)
    return (1);
#endif /* SCOTCH_DEBUG_ALL */

  clockStart (&runtime[0]);

  SCOTCH_graphOrderSave (&grafdat, &ordedat, C_filepntrordout);  /* Write ordering    */
  if (flag & C_FLAGMAPOUT)                        /* If mapping wanted                */
    SCOTCH_graphOrderSaveMap (&grafdat, &ordedat, C_filepntrmapout); /* Write mapping */
  if (flag & C_FLAGTREOUT)                        /* If separator tree wanted         */
    SCOTCH_graphOrderSaveTree (&grafdat, &ordedat, C_filepntrmapout); /* Write tree   */

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

  SCOTCH_graphOrderExit (&grafdat, &ordedat);
  SCOTCH_stratExit      (&ordestrat);
  SCOTCH_graphExit      (&grafdat);
  memFree               (permtab);

#ifdef SCOTCH_DEBUG_ALL
  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_ALL */

  return (0);
}
