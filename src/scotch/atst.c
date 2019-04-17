/* Copyright 2004,2007,2008,2010-2012,2014,2018,2019 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**                # Version 5.0  : from : 23 dec 2007     **/
/**                                 to   : 16 mar 2008     **/
/**                # Version 5.1  : from : 01 jul 2010     **/
/**                                 to   : 14 feb 2011     **/
/**                # Version 6.0  : from : 01 jan 2012     **/
/**                                 to   : 17 apr 2019     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ATST

#include "module.h"
#include "common.h"
#include "scotch.h"
#include "atst.h"

/*
**  The static variables.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { FILEMODER },
                              { FILEMODEW } };

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
  SCOTCH_Arch         archdat;
  SCOTCH_Num          archsiz;
  SCOTCH_ArchDom      dom0dat;
  SCOTCH_Num          dom0num;
  SCOTCH_ArchDom      dom1dat;
  SCOTCH_Num          dom1num;
  SCOTCH_Num          distval;
  SCOTCH_Num          distmin;
  SCOTCH_Num          distmax;
  SCOTCH_Num          distsum;
  double              distavg;
  double              distdlt;
  int                 i;

  errorProg ("atst");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  fileBlockInit (C_fileTab, C_FILENBR);           /* Set default stream pointers */

  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes                        */
    if ((argv[i][0] != '-') || (argv[i][1] == '\0') || (argv[i][1] == '.')) { /* If found a file name */
      if (C_fileNum < C_FILEARGNBR)               /* File name has been given                         */
        fileBlockName (C_fileTab, C_fileNum ++) = argv[i];
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
          fprintf (stderr, "atst, version " SCOTCH_VERSION_STRING "\n");
          fprintf (stderr, SCOTCH_COPYRIGHT_STRING "\n");
          fprintf (stderr, "This software is libre/free software under CeCILL-C -- see the user's manual for more information\n");
          return  (0);
        default :
          errorPrint ("main: unprocessed option '%s'", argv[i]);
          return     (1);
      }
    }
  }

  fileBlockOpen (C_fileTab, C_FILENBR);           /* Open all files */

  SCOTCH_archInit (&archdat);                     /* Initialize architecture structure */
  if (SCOTCH_archLoad (&archdat, C_filepntrtgtinp) != 0) { /* Load architecture        */
    SCOTCH_errorPrint ("main: cannot load architecture");
    return (1);
  }

  SCOTCH_archDomFrst (&archdat, &dom0dat);
  archsiz = SCOTCH_archDomSize (&archdat, &dom0dat);

  distmin = (SCOTCH_Num) (((unsigned long) ((SCOTCH_Num) -1)) >> 1); /* Set to maximum number in Anum */
  distmax = 0;
  distsum = 0;

  for (dom0num = 0; dom0num < archsiz; dom0num ++) { /* For all pairs of vertices */
    SCOTCH_archDomTerm (&archdat, &dom0dat, dom0num);
   
    for (dom1num = 0; dom1num < archsiz; dom1num ++) {
      SCOTCH_archDomTerm (&archdat, &dom1dat, dom1num);

      distval = SCOTCH_archDomDist (&archdat, &dom0dat, &dom1dat); /* Compute distance between pairs */
      if (distmin > distval)
        distmin = distval;
      if (distmax < distval)
        distmax = distval;
      distsum += distval;
    }
  }
  distavg = (archsiz > 1)
            ? (double) distsum / ((double) archsiz * (double) (archsiz - 1) / 2.0)
            : 0.0L;
  distdlt = 0.0L;

  for (dom0num = 0; dom0num < archsiz; dom0num ++) { /* For all pairs of vertices */
    SCOTCH_archDomTerm (&archdat, &dom0dat, dom0num);
   
    for (dom1num = 0; dom1num < archsiz; dom1num ++) {
      SCOTCH_archDomTerm (&archdat, &dom1dat, dom1num);
      distdlt += fabs ((double) SCOTCH_archDomDist (&archdat, &dom0dat, &dom1dat) - distavg);
    }
  }
  if (archsiz > 1)
    distdlt /= (double) archsiz * (double) (archsiz - 1) / 2.0;

  fprintf (C_filepntrlogout, "A\tTerminals\tnbr=" SCOTCH_NUMSTRING "\n",
           archsiz);
  fprintf (C_filepntrlogout, "A\tDistance\tmin=" SCOTCH_NUMSTRING "\tmax=" SCOTCH_NUMSTRING "\tavg=%g\tdlt=%g\n",
           distmin, distmax, distavg, distdlt);

  fileBlockClose (C_fileTab, C_FILENBR);          /* Always close explicitely to end eventual (un)compression tasks */

  SCOTCH_archExit (&archdat);

  return (0);
}
