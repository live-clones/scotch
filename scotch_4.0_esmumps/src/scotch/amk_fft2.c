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
/**   NAME       : amk_fft2.c                              **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the distance map for FFT        **/
/**                graphs, to be used to build the archi-  **/
/**                tecture description files for these     **/
/**                graphs.                                 **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 19 apr 1994     **/
/**                                 to   : 20 apr 1994     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to   : 19 sep 1995     **/
/**                # Version 3.2  : from : 07 may 1997     **/
/**                                 to   : 07 may 1997     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 02 oct 1998     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define AMK_FFT2

#include "common.h"
#include "scotch.h"
#include "amk_fft2.h"

/*
**  The static and global definitions.
*/

static int                  C_paraNum = 0;        /* Number of parameters       */
static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* The file array          */
                              { "-", NULL, "w" } };

static C_VertDist *         C_distaTab;           /* Pointer to distance map table */
static C_Queue              C_distaQueue;         /* Distance queue                */

static const char *         C_usageList[] = {     /* Usage list */
  "amk_fft2 [<dim> [<output target file>]] <options>",
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
  SCOTCH_Num          fdim;                       /* FFT dimension               */
  SCOTCH_Num          fnbr;                       /* Number of FFT vertices      */
  SCOTCH_Num          fmax;                       /* Maximum terminal number     */
  SCOTCH_Num          fmsk;                       /* Position bit mask           */
  C_Vertex            v, w, x;                    /* A FFT vertex (lvl, pos)     */
  SCOTCH_Num          b, d;                       /* Mask and bit variables      */
  SCOTCH_Num          i;                          /* Loop counter                */
  SCOTCH_Num          t;                          /* Vertex terminal value       */

  errorProg ("amk_fft2");

  fdim = 2;

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
        if ((fdim = atoi (argv[i])) < 1) {        /* Get the dimension                   */
          errorPrint ("main: invalid dimension (\"%s\")", argv[i]);
          return     (1);
        }
        C_paraNum ++;
        continue;                                 /* Process the other parameters */
      }
      if (C_fileNum < C_FILENBR)                  /* A file name has been given */
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
          fprintf (stderr, "amk_fft2, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  fnbr = (fdim + 1) * (1 << fdim);                /* Compute number of vertices      */
  fmax = (1 << (fdim * 2 - 1)) - 1;               /* Compute maximum terminal number */
  fmsk = (1 << fdim) - 1;                         /* Get maximum position number     */

  fprintf (C_filepntrarcout, "deco\n0\n%ld\t%ld\n", /* Print file header              */
           (long) fnbr,                           /* Print number of terminal domains */
           (long) fmax);                          /* Print the biggest terminal value */

  for (v.lvl = 0; v.lvl <= fdim; v.lvl ++) {      /* For all vertices */
    for (v.pos = 0; v.pos <= fmsk; v.pos ++) {
      for (i = v.lvl, b = 1 << (fdim - 1), t = 1; /* Recurse through the vertical + horizontal cuts */
           i <= fdim;
           i ++, b >>= 1) {
        t <<= 1;                                  /* Vertical cut: tell if vertex is in left or...   */
        t |= (v.pos & b) ? 1 : 0;                 /* right part from the position heaviest bits      */
        t <<= 1;                                  /* Vertex is still in upper part of horizontal cut */
      }
      if (v.lvl == 0)                             /* If vertex is in the first level...              */
        t >>= 2;                                  /* We have gone one step too far                   */
      else {                                      /* Else                                            */
        t |= 1;                                   /* This time vertex is in the lower part           */
        t <<= (v.lvl - 1);                        /* Make space for the chain bipartition            */
        t |= v.pos & ((1 << (v.lvl - 1)) - 1);    /* Bipartition the chain following the lowest bits */
      }

      printf (((v.lvl == fdim) && (v.pos == fmsk)) /* Print terminal domain number */
               ? "%u\n\n" : "%u ", t);
    }
  }

  if ((C_queueInit (&C_distaQueue, fnbr) != 0) || /* Allocate distance array */
      ((C_distaTab = (C_VertDist *) memAlloc (fnbr * sizeof (C_VertDist))) == NULL)) {
    errorPrint ("main: out of memory");
    return     (1);
  }

  for (v.lvl = 0; v.lvl <= fdim; v.lvl ++) {      /* For all vertices */
    for (v.pos = 0; v.pos <= fmsk; v.pos ++) {
      for (i = 0; i < fnbr; i ++)                 /* Initialize the vertex table */
        C_distaTab[i].queued = 0;                 /* Vertex not queued yet       */

      C_distaRoot (&v);                           /* Set the queue with root v */

      while (C_distaGet (&w, &d)) {               /* As long as the queue is not empty */
        C_distaTab[C_vertLabl (&w)].dist = d;     /* Keep the distance information     */

        d ++;                                     /* Search for neighbors at next level */
        if (w.lvl > 0) {                          /* Add new neighbors to the queue     */
          x.lvl = w.lvl - 1;
          x.pos = w.pos;
          C_distaPut (&x, d);
          x.pos = w.pos ^ (1 << (w.lvl - 1));
          C_distaPut (&x, d);
        }
        if (w.lvl < fdim) {
          x.lvl = w.lvl + 1;
          x.pos = w.pos;
          C_distaPut (&x, d);
          x.pos = w.pos ^ (1 << w.lvl);
          C_distaPut (&x, d);
        }
      }

      if (v.lvl + v.pos > 0) {                    /* Print the distance triangular map line */
        fprintf (C_filepntrarcout, "%ld",
                 (long) C_distaTab[0].dist);
        for (i = 1; i < (v.lvl << fdim) + v.pos; i ++)
          fprintf (C_filepntrarcout, " %ld",
                   (long) C_distaTab[i].dist);
        fprintf (C_filepntrarcout, "\n");
      }
    }
  }

#ifdef SCOTCH_DEBUG_C1
  C_queueExit (&C_distaQueue);
  memFree     (C_distaTab);

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_C1 */

  return (0);
}
