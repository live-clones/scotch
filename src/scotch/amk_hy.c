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
/**   NAME       : amk_hy.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the distance map for hypercube  **/
/**                graphs, to be used to build the archi-  **/
/**                tecture description files for these     **/
/**                graphs.                                 **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 14 nov 1994     **/
/**                                 to   : 14 nov 1994     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to   : 19 sep 1995     **/
/**                # Version 3.2  : from : 31 may 1997     **/
/**                                 to   : 02 jun 1997     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 02 oct 1998     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define AMK_HY

#include "common.h"
#include "scotch.h"
#include "amk_hy.h"

/*
**  The static definitions.
*/

static int                  C_paraNum = 0;        /* Number of parameters       */
static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* The file array          */
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "amk_hy <dim> [<output target file>] <options>",
  "  -h  : Display this help",
  "  -V  : Print program version and copyright",
  NULL };

/*************************************************/
/*                                               */
/* The main routine, which computes the distance */
/* triangular table.                             */
/*                                               */
/*************************************************/

int
main (
int                         argc,
char *                      argv[])
{
  SCOTCH_Num          hdim;                       /* Hypercube dimension          */
  SCOTCH_Num          hnbr;                       /* Number of hypercube vertices */
  SCOTCH_Num          hmax;                       /* Number of domains            */
  SCOTCH_Num          hv0, hv1;
  SCOTCH_Num          i, j;

  errorProg ("amk_hy");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  hdim = 1;                                       /* Preset hypercube dimension */

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_paraNum < 1) {                        /* If number of parameters not reached */
        if ((hdim = atoi (argv[i])) < 1) {        /* Get the dimension                   */
          errorPrint ("main: invalid dimension (\"%s\")", argv[i]);
          return     (1);
        }
        C_paraNum ++;
        continue;                                 /* Process the other parameters */
      }
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
        case 'V' :
          fprintf (stderr, "amk_hy, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  hnbr =  1 << hdim;                              /* Compute number of terminals */
  hmax = (1 << (hdim + 1)) - 1;                   /* Maximum terminal value      */

  fprintf (C_filepntrtgtout, "deco\n0\n%u\t%u\n", /* Print the file header:           */
           hnbr,                                  /* Print number of terminal domains */
           hmax);                                 /* Print the biggest terminal value */

  for (i = 0; i < hnbr; i ++)                     /* For all vertices */
    fprintf (C_filepntrtgtout, "%u\t1\t%u\n",
             i,                                   /* Print terminal label  */
             hnbr + i);                           /* Print terminal number */

  for (hv0 = 1; hv0 < hnbr; hv0 ++) {             /* For all vertices */
    for (hv1 = 0; hv1 < hv0; hv1 ++) {
      for (i = hv0 ^ hv1, j = 0; i > 0; i >>=1)
        j += (i & 1);
      fprintf (C_filepntrtgtout,
               (hv1 == 0) ? "%u" : " %u", j);
    }
    fprintf (C_filepntrtgtout, "\n");
  }

#ifdef SCOTCH_DEBUG_C1
  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_C1 */

  return (0);
}
