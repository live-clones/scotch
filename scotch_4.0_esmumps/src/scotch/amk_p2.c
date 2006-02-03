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
/**   NAME       : amk_p2.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the target architecture file    **/
/**                for a weighted path with two vertices   **/
/**                used to bipartition graphs in parts of  **/
/**                different sizes.                        **/
/**                                                        **/
/**   DATES      : # Version 3.0  : from : 17 jul 1995     **/
/**                                 to   : 17 jul 1995     **/
/**                # Version 3.2  : from : 02 jun 1997     **/
/**                                 to   : 02 jun 1997     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define AMK_P2

#include "common.h"
#include "scotch.h"
#include "amk_p2.h"

/*
**  The static variables.
*/

static int                  C_paraNum = 0;        /* Number of parameters       */
static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "amk_p2 <wght0> [<wght1> [<output target file>]] <options>",
  "  -h  : Display this help",
  "  -V  : Print program version and copyright",
  NULL };

/************************************/
/*                                  */
/* The main routine, which computes */
/* the decomposition.               */
/*                                  */
/************************************/

int
main (
int                         argc,
char *                      argv[])
{
  unsigned int        wght[2] = {1, 1};           /* Vertex weights */
  unsigned int        i;

  errorProg ("amk_p2");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_paraNum < 2) {                        /* If number of parameters not reached */
        if ((wght[C_paraNum ++] = atoi (argv[i])) < 1) { /* Get vertex weights           */
          errorPrint ("main: invalid weight (\"%s\")", argv[i]);
          return     (1);
        }
        continue;                                 /* Process remaining parameters */
      }
      if (C_fileNum < C_FILEARGNBR)               /* File name has been given */
        C_fileTab[C_fileNum ++].name = argv[i];
      else {
        errorPrint ("main: too many file names given");
        return     (1);
      }
    }
    else {                                       /* If found an option name */
      switch (argv[i][1]) {
        case 'H' :                               /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'V' :
          fprintf (stderr, "amk_p2, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004 INRIA, France\n");
          fprintf (stderr, "This software is libre/free software under LGPL -- see the user's manual for more information\n");
          return  (0);
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

  fprintf (C_filepntrtgtout, "deco\n0\n2\t3\n");  /* Print file header  */
  fprintf (C_filepntrtgtout, "0\t%u\t2\n", wght[0]); /* Print terminals */
  fprintf (C_filepntrtgtout, "1\t%u\t3\n", wght[1]);
  fprintf (C_filepntrtgtout, "1\n");              /* Print distance table */

#ifdef X_DEBUG_C1
  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* X_DEBUG_C1 */

  return (0);
}
