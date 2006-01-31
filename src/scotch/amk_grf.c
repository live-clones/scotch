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
/**   NAME       : amk_grf.c                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the architecture description    **/
/**                file for any source graph.              **/
/**                                                        **/
/**   DATES      : # Version 3.0  : from : 06 jul 1995     **/
/**                                 to   : 02 oct 1995     **/
/**                # Version 3.1  : from : 26 mar 1996     **/
/**                                 to   : 26 mar 1996     **/
/**                # Version 3.2  : from : 23 apr 1997     **/
/**                                 to   : 03 jun 1998     **/
/**                # Version 3.3  : from : 15 may 1999     **/
/**                                 to   : 15 may 1999     **/
/**                # Version 3.4  : from : 03 feb 2000     **/
/**                                 to   : 03 feb 2000     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to   : 17 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define AMK_GRF

#include "common.h"
#include "scotch.h"
#include "amk_grf.h"

/*
**  The static variables.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* File array              */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "r" } };

static const char *         C_usageList[] = {     /* Usage */
  "amk_grf [<input source file> [<output target file>]] <options>",
  "  -b<strat>[{<params>}]  : Apply bipartitioning strategy <strat>",
  "  -h                     : Display this help",
  "  -l<file>               : Load vertex list from <file>",
  "  -V                     : Print program version and copyright",
  "",
  "Default option set is : '-Bhf{move=1000}/((load0=load)|(load0=0))?x;'",
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
  SCOTCH_Strat        bipastrat;                  /* Bipartitioning strategy                   */
  SCOTCH_Arch         archdat;                    /* Target (terminal) architecture            */
  SCOTCH_Graph        grafdat;                    /* Source graph to turn into architecture    */
  SCOTCH_Num          vertnbr;                    /* Number of vertices in graph               */
  SCOTCH_Num *        vlbltab;                    /* Pointer to vertex label array, if present */
  SCOTCH_Num          listnbr;                    /* Size of list array                        */
  SCOTCH_Num *        listtab;                    /* Pointer to list array                     */
  C_VertSort *        sorttab;                    /* Vertex label sort area                    */
  SCOTCH_Num          baseval;
  SCOTCH_Num          vertnum;
  SCOTCH_Num          listnum;
  int                 flag;                       /* Process flags */
  int                 i;

  errorProg ("amk_grf");

  intRandInit ();

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  flag = C_FLAGNONE;
  SCOTCH_stratInit (&bipastrat);

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
        case 'B' :                                /* Bipartitioning strategy */
        case 'b' :
          SCOTCH_stratExit (&bipastrat);
          SCOTCH_stratInit (&bipastrat);
          if ((SCOTCH_stratGraphBipart (&bipastrat, &argv[i][2])) != 0) {
            errorPrint ("main: invalid bipartitioning strategy");
            return     (1);
          }
          break;
        case 'H' :                                /* Give the usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'L' :                                /* Input vertex list */
        case 'l' :
          flag |= C_FLAGVRTINP;
          if (argv[i][2] != '\0')
            C_filenamevrtinp = &argv[i][2];
          break;
        case 'V' :
          fprintf (stderr, "amk_grf, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  SCOTCH_graphInit (&grafdat);                    /* Create graph structure            */
  SCOTCH_graphLoad (&grafdat, C_filepntrgrfinp, -1, 0); /* Load source graph           */
  SCOTCH_graphData (&grafdat,  &baseval, &vertnbr, NULL, NULL, NULL, /* Get graph data */
                    &vlbltab, NULL, NULL, NULL);

  listnbr = 0;                                    /* Initialize vertex list */
  listtab = NULL;
  if (flag & C_FLAGVRTINP) {                      /* If list of vertices provided */
    if ((intLoad (C_filepntrvrtinp, &listnbr) != 1) || /* Read list size          */
        (listnbr < 0)                               ||
        (listnbr > vertnbr)) {
      errorPrint ("main: bad list input (1)");
      return     (1);
    }
    if ((listtab = (SCOTCH_Num *) memAlloc (vertnbr * sizeof (SCOTCH_Num) + 1)) == NULL) {
      errorPrint ("main: out of memory (1)");
      return     (1);
    }
    for (vertnum = 0; vertnum < vertnbr; vertnum ++) { /* Read list data */
      if (intLoad (C_filepntrvrtinp, &listtab[vertnum]) != 1) {
        errorPrint ("main: bad list input (2)");
        return     (1);
      }
    }
    intSort1asc1 (listtab, vertnbr);
    for (vertnum = 0; vertnum < vertnbr - 1; vertnum ++) { /* Search for duplicates */
      if (listtab[vertnum] == listtab[vertnum + 1]) {
        errorPrint ("main: duplicate list labels");
        memFree    (listtab);
        return     (1);
      }
    }

    if (vlbltab != NULL) {                        /* If graph has vertex labels */
      if ((sorttab = (C_VertSort *) memAlloc (vertnbr * sizeof (C_VertSort))) == NULL) {
        errorPrint ("main: out of memory (2)");
        memFree    (listtab);
        return     (1);
      }
      for (vertnum = 0; vertnum < vertnbr; vertnum ++) { /* Initialize sort area */
        sorttab[vertnum].labl = vlbltab[vertnum];
        sorttab[vertnum].num  = vertnum;
      }
      intSort2asc1 (sorttab, vertnbr);            /* Sort by ascending labels */

      for (listnum = 0, vertnum = 0; listnum < listnbr; listnum ++) {  /* For all labels in list */
        while ((vertnum < vertnbr) && (sorttab[vertnum].labl < listtab[listnum]))
          vertnum ++;                             /* Search vertex graph with corresponding label */
        if ((vertnum >= vertnbr) ||               /* If label not found                           */
            (sorttab[vertnum].labl > listtab[listnum])) {
          errorPrint ("main: list label not in graph (%ld)", (long) listtab[listnum]);
          memFree    (sorttab);
          memFree    (listtab);
          return     (1);
        }
        listtab[listnum] = sorttab[vertnum ++].num; /* Replace label by number */
      }
      memFree (sorttab);                          /* Free sort area */
    }
  }

  SCOTCH_archInit  (&archdat);                    /* Initialize target architecture            */
  SCOTCH_archBuild (&archdat, &grafdat, listnbr, listtab, &bipastrat); /* Compute architecture */
  SCOTCH_archSave  (&archdat, C_filepntrtgtout);  /* Write target architecture                 */

#ifdef SCOTCH_DEBUG_MAIN1
  graphExit (&tgtgraph);                          /* Free target graph        */
  archExit  (&tgtarch);                           /* Free target architecture */
  stratExit (mapstrat);                           /* Free strategy string     */
  listExit  (&tgtlist);                           /* Free vertex list         */

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  return (0);
}
