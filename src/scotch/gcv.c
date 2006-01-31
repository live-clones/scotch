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
/**   NAME       : gcv.c                                   **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                Bruno MARCUSSEAU (v3.1)                 **/
/**                                                        **/
/**   FUNCTION   : Part of a graph file converter.         **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 02 apr 1993     **/
/**                                 to     02 apr 1993     **/
/**                # Version 2.0  : from : 28 oct 1994     **/
/**                                 to     16 nov 1994     **/
/**                # Version 3.0  : from : 08 sep 1995     **/
/**                                 to     20 sep 1995     **/
/**                # Version 3.1  : from : 22 mar 1996     **/
/**                                 to     22 mar 1996     **/
/**                # Version 3.2  : from : 04 oct 1996     **/
/**                                 to     26 may 1997     **/
/**                # Version 3.3  : from : 06 oct 1998     **/
/**                                 to   : 21 dec 1998     **/
/**                # Version 3.4  : from : 05 oct 1999     **/
/**                                 to   : 03 feb 2000     **/
/**                # Version 4.0  : from : 29 nov 2003     **/
/**                                 to   : 19 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GCV

#include "common.h"
#include "scotch.h"
#include "gcv.h"

/*
**  The static and global variables.
*/

static int                  C_inpFormatType  = 0; /* Input graph format           */
static char *               C_inpFormatData  = "\0"; /* Pointer to auxiliary data */
static const C_Format       C_inpFormatTab[] = {  /* Table of input formats       */
                              { 'B',  SCOTCH_graphGeomLoadHabo },
                              { 'b',  SCOTCH_graphGeomLoadHabo },
                              { 'C',  SCOTCH_graphGeomLoadChac },
                              { 'c',  SCOTCH_graphGeomLoadChac },
                              { 'S',  SCOTCH_graphGeomLoadScot },
                              { 's',  SCOTCH_graphGeomLoadScot },
                              { '\0', NULL } };

static int                  C_outFormatType  = 2; /* Output graph format          */
static char *               C_outFormatData  = "\0"; /* Pointer to auxiliary data */
static C_Format             C_outFormatTab[] = {  /* Table of output formats      */
                              { 'C',  SCOTCH_graphGeomSaveChac },
                              { 'c',  SCOTCH_graphGeomSaveChac },
                              { 'S',  SCOTCH_graphGeomSaveScot },
                              { 's',  SCOTCH_graphGeomSaveScot },
                              { '\0', NULL } };

static int                  C_fileNum    = 0;     /* Number of file in arg list  */
static File                 C_fileTab[3] = {      /* File array                  */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "gcv [<input graph file> [<output graph file> [<output geometry file>]]] <options>",
  "  -h          : Display this help",
  "  -i<format>  : Select input file format",
  "                  b  : Boeing-Harwell format (matrices)",
  "                  c  : Chaco v2.0 format (adjacency)",
  "                  i  : Interface format (finite elements)",
  "                  h  : Hammond's format (adjacency)",
  "                  s  : Scotch v3.0 format (adjacency)",
  "                  t  : Simon's simple format (vertices & edges)",
  "  -o<format>  : Select output file format",
  "                  c  : Chaco v2.0 format (adjacency)",
  "                  s  : Scotch v3.0 format (adjacency)",
  "  -V          : Print program version and copyright",
  "",
  "Default option set is : '-Ib -Os'",
  NULL };

/*****************************/
/*                           */
/* This is the main function */
/*                           */
/*****************************/

int
main (
int                         argc,
char *                      argv[])
{
  SCOTCH_Graph        grafdat;
  SCOTCH_Geom         geomdat;
  int                 i, j;

  errorProg ("gcv");

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
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
        return     (1);
      }
    }
    else {                                       /* If found an option name */
      switch (argv[i][1]) {
        case 'H' :                               /* Give help */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'I' :                               /* Select input file type */
        case 'i' :
          for (j = 0; C_inpFormatTab[j].code != '\0'; j ++) { /* Find proper format code */
            if (C_inpFormatTab[j].code == argv[i][2]) {
              C_inpFormatType = j;
              C_inpFormatData = &argv[i][3];
              break;
            }
          }
          if (C_inpFormatTab[j].code == '\0') {
            errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
            return     (1);
          }
          break;
        case 'O' :                               /* Select input file type */
        case 'o' :
          for (j = 0; C_outFormatTab[j].code != '\0'; j ++) { /* Find proper format code */
            if (C_outFormatTab[j].code == argv[i][2]) {
              C_outFormatType = j;
              C_outFormatData = &argv[i][3];
              break;
            }
          }
          if (C_inpFormatTab[j].code == '\0') {
            errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
            return     (1);
          }
          break;
        case 'V' :
          fprintf (stderr, "gcv, version %s - F. Pellegrini\n", SCOTCH_VERSION);
          fprintf (stderr, "Copyright 2004 INRIA, France\n");
          fprintf (stderr, "This software is libre/free software under LGPL -- see the user's manual for more information\n");
          return  (0);
        default :
          errorPrint ("main: unprocessed option (\"%s\")", argv[i]);
          return     (1);
      }
    }
  }

  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      if ((C_fileTab[i].pntr = fopen (C_fileTab[i].name, C_fileTab[i].mode)) == NULL) { /* Open the file */
        errorPrint ("main: cannot open file (%d)", i);
        return     (1);
      }
    }
  }

  SCOTCH_graphInit (&grafdat);
  SCOTCH_geomInit  (&geomdat);
  C_inpFormatTab[C_inpFormatType].func (&grafdat, &geomdat, C_filepntrsrcinp, NULL, C_inpFormatData);
#ifdef SCOTCH_DEBUG_MAIN1
  if (SCOTCH_graphCheck (&grafdat) != 0) {
    errorPrint ("main: bad graph structure");
    return (1);
  }
#endif /* SCOTCH_DEBUG_MAIN1 */
  C_outFormatTab[C_outFormatType].func (&grafdat, &geomdat, C_filepntrsrcout, C_filepntrgeoout, C_outFormatData);

#ifdef SCOTCH_DEBUG_MAIN1
  SCOTCH_geomExit  (&geomdat);
  SCOTCH_graphExit (&grafdat);

  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  return (0);
}
