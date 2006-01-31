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
/**   NAME       : gmk_ub2.c                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the source graph for undirected **/
/**                de Bruijn graphs, to be used to build   **/
/**                the architecture description files for  **/
/**                these graphs.                           **/
/**                                                        **/
/**   DATES      : # Version 1.2  : from : 11 feb 1994     **/
/**                                 to   : 11 feb 1994     **/
/**                # Version 2.0  : from : 05 nov 1994     **/
/**                                 to     05 nov 1994     **/
/**                # Version 3.0  : from : 11 jul 1995     **/
/**                                 to     12 jul 1995     **/
/**                # Version 3.2  : from : 03 jun 1997     **/
/**                                 to   : 03 jun 1997     **/
/**                # Version 3.3  : from : 07 jun 1999     **/
/**                                 to   : 07 jun 1999     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GMK_UB2

#include "common.h"
#include "scotch.h"
#include "gmk_ub2.h"

#define ngbadd(v)                 if ((v) != vertnum) {           \
                                    int                   k;      \
                                    for (k = 0; k < ngbnbr; k ++) \
                                      if ((v) == ngbtab[k])       \
                                        break;                    \
                                    if (k == ngbnbr)              \
                                      ngbtab[ngbnbr ++] = (v);    \
                                  }

/*
**  The static definitions.
*/

static int                  C_paraNum = 0;        /* Number of parameters       */
static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* The file array          */
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "gmk_ub2 <dim> [<output source file>] <options>",
  "  -h  : Display this help",
  "  -V  : Print program version and copyright",
  NULL };

/****************************************/
/*                                      */
/* The main routine, which computes the */
/* source graph description.            */
/*                                      */
/****************************************/

int
main (
int                         argc,
char *                      argv[])
{
  SCOTCH_Num          ubdim = 1;                  /* Graph dimension             */
  SCOTCH_Num          ubnbr;                      /* Number of vertices          */
  SCOTCH_Num          ubbit;                      /* Most significant bit        */
  SCOTCH_Num          ngbtab[4];                  /* Array of neighbors          */
  int                 ngbnbr;                     /* Current number of neighbors */
  SCOTCH_Num          vertnum;                    /* Current vertex number       */
  int                 i, j;

  errorProg ("gmk_ub2");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_paraNum < 1) {                        /* If number of parameters not reached */
        if ((ubdim = (SCOTCH_Num) atol (argv[i])) < 1) { /* Get dimension                */
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
          fprintf (stderr, "gmk_ub2, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  ubnbr = 1 <<  ubdim;                            /* Compute number of vertices */
  ubbit = 1 << (ubdim - 1);                       /* Bit to add on the left     */

  fprintf (C_filepntrsrcout, "0\n%ld\t%ld\n0\t000\n",
           (long) ubnbr,                          /* Print number of vertices     */
           (long) (4 * ubnbr - 6));               /* Print number of edges (arcs) */

  for (vertnum = 0; vertnum < ubnbr; vertnum ++) { /* For all vertices         */
    ngbnbr = 0;                                   /* No neighbors defined yet  */
    ngbadd  ((vertnum << 1) & (ubnbr - 1));       /* Register vertex neighbors */
    ngbadd (((vertnum << 1) & (ubnbr - 1)) | 1);
    ngbadd  ((vertnum >> 1) & (ubnbr - 1));
    ngbadd (((vertnum >> 1) & (ubnbr - 1)) | ubbit);

    fprintf (C_filepntrsrcout, "%d", ngbnbr);     /* Output number of neighbors */
    for (j = 0; j < ngbnbr; j ++)
      fprintf (C_filepntrsrcout, "\t%ld", (long) ngbtab[j]);
    fprintf (C_filepntrsrcout, "\n");
  }

#ifdef SCOTCH_DEBUG_ALL
  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_ALL */

  return (0);
}
