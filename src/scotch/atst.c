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
/**   NAME       : atst.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Target architecture graph analyzer.     **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 17 may 1994     **/
/**                                 to   : 17 may 1994     **/
/**                # Version 2.0  : from : 11 nov 1994     **/
/**                                 to   : 11 nov 1994     **/
/**                # Version 3.0  : from : 05 jul 1995     **/
/**                                 to   : 19 aug 1995     **/
/**                # Version 3.2  : from : 24 sep 1996     **/
/**                                 to   : 12 may 1998     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                # Version 4.0  : from : 09 feb 2004     **/
/**                                 to   : 23 nov 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ATST

#include "common.h"
#include "module.h"
#include "scotch.h"
#include "arch.h"
#include "arch_deco.h"
#include "arch_mesh.h"
#include "atst.h"

/*
**  The static variables.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "atst [<input target file> [<output data file>]] <options>",
  "  -h  : Display this help",
  "  -V  : Print program version and copyright",
  NULL };

/******************************/
/*                            */
/* This is the main function. */
/*                            */
/******************************/

int
main (argc, argv)
int                 argc;
char *              argv[];
{
  Arch                archdat;                    /* The architecture read */
  ArchDeco *          deco;                       /* Its decomposition     */
  ArchDecoDom         dom0;
  ArchDecoDom         dom1;
  Anum                dstval;
  Anum                dstmin;
  Anum                dstmax;
  Anum                dstsum;
  double              dstavg;
  double              dstdlt;
  int                 i;

  errorProg ("atst");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

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
        case 'V' :
          fprintf (stderr, "atst, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  archInit (&archdat);                            /* Initialize architecture structure */
  archLoad (&archdat, C_filepntrtgtinp);          /* Load architecture                 */
  if (strcmp (archName (&archdat), "deco") != 0) { /* If it is not a decomposition     */
    errorPrint ("main: architecture is not decomposition-defined");
    return     (1);
  }
  deco = (ArchDeco *) &archdat.data.dummy;        /* Point to the decomposition */

  dstmin = (Anum) (((unsigned long) ((Anum) -1)) >> 1); /* Set to maximum number in Anum */
  dstmax = 0;
  dstsum = 0;

  for (dom0.num = 1; dom0.num <= deco->domvertnbr; dom0.num ++) { /* For all pairs of vertices */
    if (archDecoDomSize (deco, &dom0) == 1) {     /* If vertex is a terminal                   */
      for (dom1.num = dom0.num + 1; dom1.num <= deco->domvertnbr; dom1.num ++) {
        if (archDecoDomSize (deco, &dom1) == 1) { /* If vertex is a terminal               */
          dstval = archDecoDomDist (deco, &dom0, &dom1); /* Compute distance between pairs */
          if (dstmin > dstval)
            dstmin = dstval;
          if (dstmax < dstval)
            dstmax = dstval;
          dstsum += dstval;                       /* Compute distance between pairs */
        }
      }
    }
  }
  dstavg = (deco->domtermnbr > 1)
           ? (double) dstsum / (double) (deco->domtermnbr * (deco->domtermnbr - 1) / 2)
           : 0.0L;
  dstdlt = 0.0L;
  for (dom0.num = 1; dom0.num <= deco->domvertnbr; dom0.num ++) { /* For all pairs of vertices */
    if (archDecoDomSize (deco, &dom0) == 1) {     /* If vertex is a terminal                   */
      for (dom1.num = dom0.num + 1; dom1.num <= deco->domvertnbr; dom1.num ++) {
        if (archDecoDomSize (deco, &dom1) == 1)   /* If vertex is a terminal */
          dstdlt += fabs (archDecoDomDist (deco, &dom0, &dom1) - dstavg);
      }
    }
  }
  if (deco->domtermnbr > 1)
    dstdlt /= (double) (deco->domtermnbr * (deco->domtermnbr - 1) / 2);

  fprintf (C_filepntrlogout, "A\tTerminals\tnbr=%u\n",
           deco->domtermnbr);
  fprintf (C_filepntrlogout, "A\tDistance\tmin=%u\tmax=%u\tavg=%g\tdlt=%g\n",
           dstmin, dstmax, dstavg, dstdlt);

#ifdef SCOTCH_DEBUG_MAIN1
  archExit (&archdat);

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  return (0);
}
