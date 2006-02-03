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
/**   NAME       : mcv.c                                   **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a mesh file converter.          **/
/**                This module contains the main function. **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 19 jan 2004     **/
/**                                 to   : 19 jan 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MCV

#include "common.h"
#include "scotch.h"
#include "mcv.h"

/*
**  The static and global variables.
*/

static int                  C_inpFormatType  = 0; /* Input mesh format            */
static char *               C_inpFormatData  = "\0"; /* Pointer to auxiliary data */
static const C_Format       C_inpFormatTab[] = {  /* Table of input formats       */
                              { 'B',  SCOTCH_meshGeomLoadHabo },
                              { 'b',  SCOTCH_meshGeomLoadHabo },
                              { 'S',  SCOTCH_meshGeomLoadScot },
                              { 's',  SCOTCH_meshGeomLoadScot },
                              { '\0', NULL } };

static int                  C_outFormatType  = 0; /* Output mesh format           */
static char *               C_outFormatData  = "\0"; /* Pointer to auxiliary data */
static C_Format             C_outFormatTab[] = {  /* Table of output formats      */
                              { 'S',  SCOTCH_meshGeomSaveScot },
                              { 's',  SCOTCH_meshGeomSaveScot },
                              { '\0', NULL } };

static int                  C_fileNum    = 0;     /* Number of file in arg list  */
static File                 C_fileTab[3] = {      /* File array                  */
                              { "-", NULL, "r" },
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {
  "mcv [<input mesh file> [<output mesh file> [<output geometry file>]]] <options>",
  "  -h          : Display this help",
  "  -i<format>  : Select input file format",
  "                  b  : Boeing-Harwell format (elemental)",
  "                  s  : Scotch v4.0 format",
  "  -o<format>  : Select output file format",
  "                  s  : Scotch v4.0 format",
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
  SCOTCH_Mesh         meshdat;
  SCOTCH_Geom         geomdat;
  int                 i, j;

  errorProg ("mcv");

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
          fprintf (stderr, "mcv, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  SCOTCH_meshInit (&meshdat);
  SCOTCH_geomInit (&geomdat);
  C_inpFormatTab[C_inpFormatType].func (&meshdat, &geomdat, C_filepntrsrcinp, NULL, C_inpFormatData);
#ifdef SCOTCH_DEBUG_MAIN1
  if (SCOTCH_meshCheck (&meshdat) != 0) {
    errorPrint ("main: bad graph structure");
    return (1);
  }
#endif /* SCOTCH_DEBUG_MAIN1 */
  C_outFormatTab[C_outFormatType].func (&meshdat, &geomdat, C_filepntrsrcout, C_filepntrgeoout, C_outFormatData);

#ifdef SCOTCH_DEBUG_MAIN1
  SCOTCH_geomExit  (&geomdat);
  SCOTCH_graphExit (&meshdat);

  for (i = 0; i < C_FILENBR; i ++) {             /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||         /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  return (0);
}
