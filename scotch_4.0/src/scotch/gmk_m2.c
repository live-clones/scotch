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
/**   NAME       : gmk_m2.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the source graph for 2D mesh    **/
/**                graphs.                                 **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 30 oct 1994     **/
/**                                 to     08 nov 1994     **/
/**                # Version 3.0  : from : 11 jul 1995     **/
/**                                 to     02 oct 1995     **/
/**                # Version 3.2  : from : 03 jun 1997     **/
/**                                 to   : 03 jun 1997     **/
/**                # Version 3.3  : from : 06 oct 1998     **/
/**                                 to   : 06 oct 1998     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 18 may 2004     **/
/**                                                        **/
/**   NOTES      : # The vertices of the (dX,dY) mesh are  **/
/**                  numbered as terminals so that         **/
/**                  t(0,0) = 0, t(1,0) = 1,               **/
/**                  t(dX - 1, 0) = dX - 1, t(0,1) = dX,   **/
/**                  and t(x,y) = (y * dX) + x.            **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GMK_M2

#include "common.h"
#include "scotch.h"
#include "gmk_m2.h"

/*
**  The static definitions.
*/

static int                  C_paraNum = 0;        /* Number of parameters       */
static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* The file array          */
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "gmk_m2 <dimX> [<dimY> [<output source file>]] <options>",
  "  -b<val>   : Set base value for output (0 or 1)",
  "  -e        : Build a 8-neighbor grid rather than a 4-neighbor one",
  "  -g<file>  : Output the geometry to <file>",
  "  -h        : Display this help",
  "  -t        : Build a torus rather than a mesh",
  "  -V        : Print program version and copyright",
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
  uint                flagval;                    /* Process flags      */
  SCOTCH_Num          baseval;                    /* Base value         */
  SCOTCH_Num          d[2] = { 1, 1 };            /* Mesh dimensions    */
  SCOTCH_Num          c[2];                       /* Vertex coordinates */
  int                 i;

  errorProg ("gmk_m2");

  flagval = C_FLAGDEFAULT;                        /* Set default flags */
  baseval = 0;

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
        if ((d[C_paraNum ++] = atoi (argv[i])) < 1) { /* Get the dimension               */
          errorPrint ("main: invalid dimension (\"%s\")", argv[i]);
          return     (1);
        }
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
        case 'B' :                                /* Set base value */
        case 'b' :
          baseval = (SCOTCH_Num) atol (&argv[i][2]);
          if ((baseval < 0) || (baseval > 1)) {
            errorPrint ("main: invalid base value (%ld)", (long) baseval);
          }
          break;
        case 'E' :                                /* Build a finite-element grid */
        case 'e' :
          flagval |= C_FLAGELEM;
          break;
        case 'G' :                                /* Output the geometry */
        case 'g' :
          flagval |= C_FLAGGEOOUT;
          if (argv[i][2] != '\0')
            C_filenamegeoout = &argv[i][2];
          break;
        case 'H' :                                /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'T' :                                /* Build a torus */
        case 't' :
          flagval |= C_FLAGTORUS;
          break;
        case 'V' :
          fprintf (stderr, "acpl, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  if (flagval & C_FLAGELEM) {                     /* Build a torus */
    errorPrint ("main: elements not supported");
    return     (1);
  }

  if (flagval & C_FLAGTORUS) {                    /* Build a torus */
    fprintf (C_filepntrsrcout, "0\n%ld\t%ld\n%ld\t000\n",
             (long) (d[0] * d[1]),                /* Print number of vertices        */
             (long) ((4 * d[0] * d[1])             - /* Print number of edges (arcs) */
                     ((d[0] < 3) ? (2 * d[1]) : 0) -
                     ((d[1] < 3) ? (2 * d[0]) : 0)),
             (long) baseval);

    for (c[1] = 0; c[1] < d[1]; c[1] ++) {        /* Output neighbor list */
      for (c[0] = 0; c[0] < d[0]; c[0] ++) {
        fprintf (C_filepntrsrcout, "%ld",
                 (long) (((d[0] > 2) ? 3 : d[0]) + /* Output number of neighbors */
                         ((d[1] > 2) ? 3 : d[1]) - 2));
        if (d[1] > 2)
          fprintf (C_filepntrsrcout, "\t%ld",     /* Output the neighbors */
                   (long) (((c[1] + d[1] - 1) % d[1]) * d[0] + c[0] + baseval));
        if (d[0] > 2)
          fprintf (C_filepntrsrcout, "\t%ld",
                   (long) ((c[1] * d[0] + (c[0] + d[0] - 1) % d[0]) + baseval));
        if (d[0] > 1)
          fprintf (C_filepntrsrcout, "\t%ld",
                   (long) (c[1] * d[0] + ((c[0] + 1) % d[0]) + baseval));
        if (d[1] > 1)
          fprintf (C_filepntrsrcout, "\t%ld",
                   (long) (((c[1] + 1) % d[1]) * d[0] + c[0] + baseval));
        fprintf (C_filepntrsrcout, "\n");
      }
    }
  }
  else {                                          /* Build a mesh */
    fprintf (C_filepntrsrcout, "0\n%ld\t%ld\n%ld\t000\n",
             (long) (d[0] * d[1]),
             (long) ((d[0] * d[1] * 2 - (d[0] + d[1])) * 2),
             (long) baseval);

    for (c[1] = 0; c[1] < d[1]; c[1] ++) {        /* Output neighbor list */
      for (c[0] = 0; c[0] < d[0]; c[0] ++) {
        fprintf (C_filepntrsrcout, "%d",
                 ((c[0] == 0)          ? 0 : 1) + /* Output number of neighbors */
                 ((c[0] == (d[0] - 1)) ? 0 : 1) +
                 ((c[1] == 0)          ? 0 : 1) +
                 ((c[1] == (d[1] - 1)) ? 0 : 1));
        if (c[1] != 0)                            /* Output the neighbors */
          fprintf (C_filepntrsrcout, "\t%ld",
                   (long) ((c[1] - 1) * d[0] + c[0] + baseval));
        if (c[0] != 0)
          fprintf (C_filepntrsrcout, "\t%ld",
                   (long) (c[1] * d[0] + (c[0] - 1) + baseval));
        if (c[0] != (d[0] - 1))
          fprintf (C_filepntrsrcout, "\t%ld",
                   (long) (c[1] * d[0] + (c[0] + 1) + baseval));
        if (c[1] != (d[1] - 1))
          fprintf (C_filepntrsrcout, "\t%ld",
                   (long) ((c[1] + 1) * d[0] + c[0] + baseval));
        fprintf (C_filepntrsrcout, "\n");
      }
    }
  }

  if (flagval & C_FLAGGEOOUT) {                   /* If geometry is wanted       */
   fprintf (C_filepntrgeoout, "2\n%ld\n",         /* Output geometry file header */
            (long) (d[0] * d[1]));

    for (c[1] = 0; c[1] < d[1]; c[1] ++) {        /* Output mesh coordinates */
      for (c[0] = 0; c[0] < d[0]; c[0] ++)
        fprintf (C_filepntrgeoout, "%ld\t%ld\t%ld\n",
                 (long) (c[1] * d[0] + c[0] + baseval),
                 (long) c[0],
                 (long) (d[1] - 1 - c[1]));
    }
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
