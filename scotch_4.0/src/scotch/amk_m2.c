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
/**   NAME       : amk_m2.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the distance map for            **/
/**                bidimensional mesh graphs, to be used   **/
/**                to build the architecture description   **/
/**                files for these graphs.                 **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 21 apr 1994     **/
/**                                 to   : 21 apr 1994     **/
/**                # Version 2.0  : from : 12 jul 1994     **/
/**                                 to   : 12 nov 1994     **/
/**                # Version 3.0  : from : 17 jul 1995     **/
/**                                 to   : 19 sep 1995     **/
/**                # Version 3.2  : from : 31 may 1997     **/
/**                                 to   : 02 jun 1997     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                # Version 4.0  : from : 09 feb 2004     **/
/**                                 to   : 09 feb 2004     **/
/**                                                        **/
/**   NOTES      : # The vertices of the (dX,dY) mesh are  **/
/**                  numbered as terminals so that         **/
/**                  t (0,0)=0, t (1,0) = 1,               **/
/**                  t (dX - 1, 0) = dX - 1,               **/
/**                  t (0,1) = dX, and                     **/
/**                  t(x,y) = (y * dX) + x.                **/
/**                                                        **/
/**                # The nested dissection method should   **/
/**                  behave like the architecture built in **/
/**                  the mapper.                           **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define AMK_M2

#include "module.h"
#include "common.h"
#include "scotch.h"
#include "arch.h"
#include "arch_mesh.h"
#include "amk_m2.h"

/*
**  The static definitions.
*/

static int                  C_paraNum = 0;        /* Number of parameters       */
static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "amk_m2 <dimX> [<dimY> [<output target file>]] <options>",
  "  -h          : Display this help",
  "  -m<method>  : Decomposition method",
  "                  n  : Nested dissection (cut biggest dimension)",
  "                  o  : One-way dissection (y, then x)",
  "  -V          : Print program version and copyright",
  "",
  "Default option set is : '-Mn'",
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
  ArchMesh2        arch;                          /* Mesh dimensions            */
  ArchMesh2Dom     dom;                           /* Initial domain             */
  C_MethType       methtype;                      /* Bipartitioning method      */
  unsigned int     termnbr;                       /* Number of terminal domains */
  unsigned int     termmax;                       /* Maximum terminal number    */
  unsigned int *   termtab;                       /* Terminal numbers table     */
  unsigned int     x0, y0, x1, y1;
  unsigned int     i;

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  methtype  = C_METHNESTED;
  arch.c[0] =                                     /* Preset mesh dimensions */
  arch.c[1] = 1;

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_paraNum < 2) {                        /* If number of parameters not reached */
        if ((arch.c[C_paraNum ++] = atoi (argv[i])) < 1) { /* Get the dimension          */
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
        case 'M' :                                /* Use a built-in method */
        case 'm' :
          switch (argv[i][2]) {
            case 'N' :                            /* Nested dissection */
            case 'n' :
              methtype = C_METHNESTED;
              break;
            case 'O' :                            /* One-way dissection */
            case 'o' :
              methtype = C_METHONEWAY;
              break;
            default :
              errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
              return     (1);
          }
          break;
        case 'H' :                               /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'V' :
          fprintf (stderr, "amk_m2, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  dom.c[0][0] = 0;                                /* Set the initial domain */
  dom.c[0][1] = arch.c[0] - 1;
  dom.c[1][0] = 0;
  dom.c[1][1] = arch.c[1] - 1;

  termnbr = arch.c[0] * arch.c[1];                /* Compute number of terminals                                    */
  termmax = 0;                                    /* Maximum terminal value not known yet                           */
  if ((termtab = (unsigned int *) memAlloc (termnbr * sizeof (unsigned int))) == NULL) { /* Allocate terminal array */
    errorPrint ("main: out of memory");
    return     (1);
  }
  memset (termtab, -1, termnbr * sizeof (unsigned int)); /* Initilize mapping table */

  C_termBipart (&arch, &dom, 1, termtab, &termmax, /* Compute terminal numbers */
                (methtype == C_METHNESTED) ? archMesh2DomBipart : C_methBipartOne);

  fprintf (C_filepntrarcout, "deco\n0\n%u\t%u\n", /* Print file header                */
           termnbr,                               /* Print number of terminal domains */
           termmax);                              /* Print biggest terminal value     */
  for (i = 0; i < termnbr; i ++)                  /* For all terminals                */
    fprintf (C_filepntrarcout, "%u\t1\t%u\n",     /* Print terminal data              */
             i, termtab[i]);

  for (y0 = 0; y0 < arch.c[1]; y0 ++) {           /* For all vertices */
    for (x0 = 0; x0 < arch.c[0]; x0 ++) {
      for (y1 = 0; y1 <= y0; y1 ++) {             /* Compute distance to smaller vertices */
        for (x1 = 0; (x1 < arch.c[0]) && ((y1 < y0) || (x1 < x0)); x1 ++)
          fprintf (C_filepntrarcout,
                   ((x1 == 0) && (y1 == 0)) ? "%u" : " %u",
                   C_termDist (x0, y0, x1, y1));
      }
      fprintf (C_filepntrarcout, "\n");
    }
  }

#ifdef SCOTCH_DEBUG_C1
  memFree (termtab);                              /* Free terminal number array */

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_C1 */

  return (0);
}

/* This routine recursively determines the values
** of all the terminal vertices of the mesh domain,
** and puts them in table.
*/

void
C_termBipart (
ArchMesh2 *                 archptr,
ArchMesh2Dom *              domptr,
unsigned int                num,
unsigned int *              termtab,
unsigned int *              termmax,
int                     (*  methfunc) ())
{
  ArchMesh2Dom        dom0;
  ArchMesh2Dom        dom1;

  if (methfunc (archptr, domptr, &dom0, &dom1) == 0) { /* If we can bipartition                          */
    C_termBipart (archptr, &dom0, num + num,     termtab, termmax, methfunc); /* Bipartition recursively */
    C_termBipart (archptr, &dom1, num + num + 1, termtab, termmax, methfunc);
  }
  else {                                          /* If we have reached the end */
    termtab[domptr->c[1][0] * archptr->c[0] +     /* Set the terminal number    */
            domptr->c[0][0]] = num;
    if (*termmax < num)                           /* If we have reached a new maximum */
      *termmax = num;                             /* Record it                        */
  }
}

/* This function tries to split a rectangular domain into
** two subdomains using a one-way dissection method, by
** cutting first in the Y dimension, then in the X one.
** It returns:
** - 1  : if the domain has been bipartitioned.
** - 0  : else.
*/

int
C_methBipartOne (
const ArchMesh2 * const       archptr,
const ArchMesh2Dom * const    domptr,
ArchMesh2Dom * restrict const dom0ptr,
ArchMesh2Dom * restrict const dom1ptr)
{
  if ((domptr->c[0][0] == domptr->c[0][1]) &&     /* Return if cannot split more */
      (domptr->c[1][0] == domptr->c[1][1]))
    return (0);

  if (domptr->c[1][1] == domptr->c[1][0]) {       /* If the Y dimension cannot be cut */
    dom0ptr->c[0][0] = domptr->c[0][0];           /* Cut in the X dimension           */
    dom0ptr->c[0][1] = (domptr->c[0][0] + domptr->c[0][1]) / 2;
    dom1ptr->c[0][0] = dom0ptr->c[0][1] + 1;
    dom1ptr->c[0][1] = domptr->c[0][1];

    dom0ptr->c[1][0] = dom1ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = dom1ptr->c[1][1] = domptr->c[1][1];
  }
  else {                                          /* If the Y dimension can be cut, cut it */
    dom0ptr->c[0][0] = dom1ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = dom1ptr->c[0][1] = domptr->c[0][1];

    dom0ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = (domptr->c[1][0] + domptr->c[1][1]) / 2;
    dom1ptr->c[1][0] = dom0ptr->c[1][1] + 1;
    dom1ptr->c[1][1] = domptr->c[1][1];
  }

  return (1);
}
