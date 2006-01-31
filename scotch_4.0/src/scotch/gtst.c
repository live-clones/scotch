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
/**   NAME       : gtst.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This program gives statistics on source **/
/**                graphs.                                 **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 31 oct 1994     **/
/**                                 to     03 nov 1994     **/
/**                # Version 3.0  : from : 15 sep 1995     **/
/**                                 to     19 sep 1995     **/
/**                # Version 3.2  : from : 03 jun 1997     **/
/**                                 to   : 25 may 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to   : 19 oct 1998     **/
/**                # Version 3.4  : from : 10 oct 1999     **/
/**                                 to     12 oct 1999     **/
/**                # Version 4.0  : from : 10 sep 2003     **/
/**                                 to   : 10 sep 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GTST

#include "common.h"
#include "scotch.h"
#include "gtst.h"

/*
**  The static definitions.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* The file array          */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "gtst [<input graph file> [<output data file>]] <options>",
  "  -h  : Display this help",
  "  -V  : Print program version and copyright",
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
  SCOTCH_Graph        grafdat;                    /* Source graph     */
  SCOTCH_Num          vertnbr;
  SCOTCH_Num          velomin;
  SCOTCH_Num          velomax;
  SCOTCH_Num          velosum;
  double              veloavg;
  double              velodlt;
  SCOTCH_Num          degrmin;
  SCOTCH_Num          degrmax;
  double              degravg;
  double              degrdlt;
  SCOTCH_Num          edgenbr;
  SCOTCH_Num          edlomin;
  SCOTCH_Num          edlomax;
  SCOTCH_Num          edlosum;
  double              edloavg;
  double              edlodlt;
  int                 i;

  errorProg ("gtst");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    exit       (0);
  }

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_fileNum < C_FILEARGNBR)               /* A file name has been given */
        C_fileTab[C_fileNum ++].name = argv[i];
      else {
        errorPrint ("main: too many file names given");
        exit       (1);
      }
    }
    else {                                        /* If found an option name */
      switch (argv[i][1]) {
        case 'H' :                                /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          exit       (0);
        case 'V' :
          fprintf (stderr, "gtst, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004 INRIA, France\n");
          fprintf (stderr, "This software is libre/free software under LGPL -- see the user's manual for more information\n");
          return  (0);
        default :
          errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
          exit       (1);
      }
    }
  }

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      if ((C_fileTab[i].pntr = fopen (C_fileTab[i].name, C_fileTab[i].mode)) == NULL) { /* Open the file */
        errorPrint ("main: cannot open file (%d)", i);
        exit       (1);
      }
    }
  }

  SCOTCH_graphInit  (&grafdat);
  SCOTCH_graphLoad  (&grafdat, C_filepntrsrcinp, -1, 0);
  SCOTCH_graphCheck (&grafdat);

  SCOTCH_graphSize  (&grafdat, &vertnbr, &edgenbr);
  SCOTCH_graphStat  (&grafdat, &velomin, &velomax, &velosum, &veloavg, &velodlt,
                     &degrmin, &degrmax, &degravg, &degrdlt,
                     &edlomin, &edlomax, &edlosum, &edloavg, &edlodlt);

  fprintf (C_filepntrdatout, "S\tVertex\tnbr=%u\n",
           vertnbr);
  fprintf (C_filepntrdatout, "S\tVertex load\tmin=%u\tmax=%u\tsum=%u\tavg=%g\tdlt=%g\n",
           velomin, velomax, velosum, veloavg, velodlt);
  fprintf (C_filepntrdatout, "S\tVertex degree\tmin=%u\tmax=%u\tsum=%u\tavg=%g\tdlt=%g\n",
           degrmin, degrmax, edgenbr, degravg, degrdlt);
  fprintf (C_filepntrdatout, "S\tEdge\tnbr=%u\n",
           edgenbr / 2);
  fprintf (C_filepntrdatout, "S\tEdge load\tmin=%u\tmax=%u\tsum=%u\tavg=%g\tdlt=%g\n",
           edlomin, edlomax, edlosum, edloavg, edlodlt);

#ifdef SCOTCH_DEBUG_MAIN1
  SCOTCH_graphExit (&grafdat);

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  exit (0);
}
