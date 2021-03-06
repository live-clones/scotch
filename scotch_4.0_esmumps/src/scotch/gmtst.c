/************************************************************/
/**                                                        **/
/**   NAME       : gmtst.c                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This program computes statistics on     **/
/**                graph mappings.                         **/
/**                                                        **/
/**   DATES      : # Version 3.1  : from : 17 jul 1996     **/
/**                                 to     23 jul 1996     **/
/**                # Version 3.2  : from : 02 jun 1997     **/
/**                                 to   : 16 jul 1997     **/
/**                # Version 3.3  : from : 07 jun 1999     **/
/**                                 to   : 07 jun 1999     **/
/**                # Version 4.0  : from : 12 feb 2004     **/
/**                                 to     16 nov 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GMTST

#include "common.h"
#include "scotch.h"
#include "gmtst.h"

/*
**  The static variables.
*/

static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* The file array          */
                              { "-", NULL, "r" },
                              { "-", NULL, "r" },
                              { "-", NULL, "r" },
                              { "-", NULL, "w" } };

static const char *         C_usageList[] = {     /* Usage */
  "gmtst [<input source file> [<input target file> [<input mapping file> [<output data file>]]]] <options>",
  "  -h  : Display this help",
  "  -V  : Print program version and copyright",
  "",
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
  SCOTCH_Graph        grafdat;                    /* Source graph                      */
  SCOTCH_Num          vertnbr;                    /* Source graph size                 */
  SCOTCH_Num *        vlbltab;                    /* Source graph vertex label array   */
  SCOTCH_Arch         archdat;                    /* Target architecture               */
  SCOTCH_Num          archnbr;                    /* Size of the target architecture   */
  SCOTCH_Mapping      mappdat;                    /* Mapping data                      */
  int                 i;

  errorProg ("gmtst");

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
        case 'M' :                                /* No mapping flag */
        case 'm' :
          C_filenamemapinp = "-";                 /* Default name to avoid opening   */
          C_filepntrmapinp = NULL;                /* NULL file pointer means no file */
          break;
        case 'V' :
          fprintf (stderr, "gmtst, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  SCOTCH_graphInit (&grafdat);                    /* Create graph structure    */
  SCOTCH_graphLoad (&grafdat, C_filepntrsrcinp, -1, 0); /* Read source graph   */
  SCOTCH_graphData (&grafdat, NULL,               /* Get graph characteristics */
                    &vertnbr, NULL, NULL, NULL, &vlbltab,
                    NULL, NULL, NULL);

  SCOTCH_archInit (&archdat);                     /* Create architecture structure                    */
  SCOTCH_archLoad (&archdat, C_filepntrtgtinp);   /* Read target architecture                         */
  if (strcmp (SCOTCH_archName (&archdat), "term") == 0) { /* If target architecture is variable-sized */
    errorPrint ("main: variable-sized architectures cannot be mapped");
    return     (1);
  }

  archnbr = SCOTCH_archSize (&archdat);           /* Get architecture size */

  SCOTCH_graphMapInit (&grafdat, &mappdat, &archdat, NULL); /* Create mapping structure */
  if (SCOTCH_graphMapLoad (&grafdat, &mappdat, C_filepntrmapinp) != 0)
    errorPrint ("main: bad input (1)");

  SCOTCH_graphMapView (&grafdat, &mappdat, C_filepntrdatout); /* Display mapping statistics */

#ifdef SCOTCH_DEBUG_ALL
  SCOTCH_graphMapExit (&grafdat, &mappdat);
  SCOTCH_archExit     (&archdat);
  SCOTCH_graphExit    (&grafdat);

  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_ALL */

  return (0);
}
