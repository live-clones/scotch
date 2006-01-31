/************************************************************/
/**                                                        **/
/**   NAME       : mmk_m3.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the source meshes for           **/
/**                tridimensional mesh source graphs.      **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 26 sep 2002     **/
/**                                 to   : 17 feb 2004     **/
/**                                                        **/
/**   NOTES      : # The nodes and elements of the         **/
/**                  (dX,dY,dZ) mesh are numbered so that  **/
/**                  t(0,0,0) = 0, t(1,0,0) = 1,           **/
/**                  t(dX - 1, 0, 0) = dX - 1, t(0,1,0) =  **/
/**                  dX, t (0, 0, 1) = dX * dY - 1,        **/
/**                  and t(x,y,z) = (z * dY + y) * dX + x. **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MMK_M3

#include "common.h"
#include "scotch.h"
#include "mmk_m3.h"

/*
**  The static definitions.
*/

static int                  C_paraNum = 0;        /* Number of parameters       */
static int                  C_fileNum = 0;        /* Number of file in arg list */
static File                 C_fileTab[C_FILENBR] = { /* The file array          */
                              { "-", NULL, "w" },
                              { "-", NULL, "w" } };

static const int            C_nghbTab[4] = { 8, 4, 2, 1 };

static const char *         C_usageList[] = {
  "mmk_m3 <dimX> [<dimY> [<dimZ> [<output mesh file>]]] <options>",
  "  -g<file>  : Output mesh geometry to <file>",
  "  -h        : Display this help",
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
  SCOTCH_Num          e[3] = { 1, 1, 1 };         /* Mesh element dimensions */
  SCOTCH_Num          n[3];                       /* Mesh node dimensions    */
  SCOTCH_Num          c[3];                       /* Vertex coordinates      */
  SCOTCH_Num          velmnbr;                    /* First node number       */
  uint                flag;                       /* Process flags           */
  int                 i;

  errorProg ("mmk_m3");

  flag = C_FLAGDEFAULT;                           /* Set default flags */

  if ((argc >= 2) && (argv[1][0] == '?')) {       /* If need for help */
    usagePrint (stdout, C_usageList);
    return     (0);
  }

  for (i = 0; i < C_FILENBR; i ++)                /* Set default stream pointers */
    C_fileTab[i].pntr = (C_fileTab[i].mode[0] == 'r') ? stdin : stdout;
  for (i = 1; i < argc; i ++) {                   /* Loop for all option codes */
    if ((argv[i][0] != '+') &&                    /* If found a file name      */
        ((argv[i][0] != '-') || (argv[i][1] == '\0'))) {
      if (C_paraNum < 3) {                        /* If number of parameters not reached */
        if ((e[C_paraNum ++] = atoi (argv[i])) < 1) { /* Get the dimension               */
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
        case 'G' :                                /* Output mesh geometry */
        case 'g' :
          flag |= C_FLAGGEOOUT;
          if (argv[i][2] != '\0')
            C_filenamegeoout = &argv[i][2];
          break;
        case 'H' :                                /* Give program usage message */
        case 'h' :
          usagePrint (stdout, C_usageList);
          return     (0);
        case 'V' :
          fprintf (stderr, "mmk_m3, version %s - F. Pellegrini\n", SCOTCH_VERSION);
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

  n[0] = e[0] + 1;
  n[1] = e[1] + 1;
  n[2] = e[2] + 1;
  velmnbr = e[0] * e[1] * e[2];

  fprintf (C_filepntrmshout, "1\n%ld\t%ld\t%ld\n0\t%ld\t000\n", /* Print mesh file header */
             (long) velmnbr,
             (long) (n[0] * n[1] * n[2]),
             (long) ((velmnbr + (n[0] * n[1] * n[2]) -
                                 (n[0] * n[1] + n[0] * n[2] + n[1] * n[2]) +
                                  n[0] + n[1] + n[2] - 1) * 8),
             (long) velmnbr);

  for (c[2] = 0; c[2] < e[2]; c[2] ++) {          /* Output element neighbor list */
    for (c[1] = 0; c[1] < e[1]; c[1] ++) {
      for (c[0] = 0; c[0] < e[0]; c[0] ++)
        fprintf (C_filepntrmshout, "8\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n", /* Output neighbors of element */
                  (long) ((c[2] * n[1] + c[1]) * n[0] + c[0]),
                  (long) ((c[2] * n[1] + c[1]) * n[0] + (c[0] + 1)),
                  (long) ((c[2] * n[1] + (c[1] + 1)) * n[0] + c[0]),
                  (long) ((c[2] * n[1] + (c[1] + 1)) * n[0] + (c[0] + 1)),
                  (long) (((c[2] + 1) * n[1] + c[1]) * n[0] + c[0]),
                  (long) (((c[2] + 1) * n[1] + c[1]) * n[0] + (c[0] + 1)),
                  (long) (((c[2] + 1) * n[1] + (c[1] + 1)) * n[0] + c[0]),
                  (long) (((c[2] + 1) * n[1] + (c[1] + 1)) * n[0] + (c[0] + 1)));
    }
  }
  for (c[2] = 0; c[2] < n[2]; c[2] ++) {          /* Output node neighbor list */
    for (c[1] = 0; c[1] < n[1]; c[1] ++) {
      for (c[0] = 0; c[0] < n[0]; c[0] ++) {
        fprintf (C_filepntrmshout, "%d",          /* Output number of neighboring elements */
                  C_nghbTab[(((c[0] != 0) && (c[0] != e[0])) ? 0 : 1) +
                            (((c[1] != 0) && (c[1] != e[1])) ? 0 : 1) +
                            (((c[2] != 0) && (c[2] != e[2])) ? 0 : 1)]);
        if (c[2] != 0) {                          /* Output neighbors of nodes */
          if (c[1] != 0) {
            if (c[0] != 0)
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) (((c[2] - 1) * e[1] + (c[1] - 1)) * e[0] + (c[0] - 1)));
            if (c[0] != e[0])
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) (((c[2] - 1) * e[1] + (c[1] - 1)) * e[0] + c[0]));
          }
          if (c[1] != e[1]) {
            if (c[0] != 0)
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) (((c[2] - 1) * e[1] + c[1]) * e[0] + (c[0] - 1)));
            if (c[0] != e[0])
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) (((c[2] - 1) * e[1] + c[1]) * e[0] + c[0]));
          }
        }
        if (c[2] != e[2]) {
          if (c[1] != 0) {
            if (c[0] != 0)
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) ((c[2] * e[1] + (c[1] - 1)) * e[0] + (c[0] - 1)));
            if (c[0] != e[0])
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) ((c[2] * e[1] + (c[1] - 1)) * e[0] + c[0]));
          }
          if (c[1] != e[1]) {
            if (c[0] != 0)
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) ((c[2] * e[1] + c[1]) * e[0] + (c[0] - 1)));
            if (c[0] != e[0])
              fprintf (C_filepntrmshout, "\t%ld",
                        (long) ((c[2] * e[1] + c[1]) * e[0] + c[0]));
          }
        }
        fprintf (C_filepntrmshout, "\n");
      }
    }
  }

  if (flag & C_FLAGGEOOUT) {                      /* If geometry is wanted       */
    fprintf (C_filepntrgeoout, "3\n%ld\n",        /* Output geometry file header */
              (long) (velmnbr + n[0] * n[1] * n[2]));

    for (c[2] = 0; c[2] < e[2]; c[2] ++) {        /* Output element coordinates */
      for (c[1] = 0; c[1] < e[1]; c[1] ++) {
        for (c[0] = 0; c[0] < e[0]; c[0] ++)
          fprintf (C_filepntrgeoout, "%ld\t%ld.5\t%ld.5\t%ld.5\n",
                    (long) (((c[2] * e[1]) + c[1]) * e[0] + c[0]),
                    (long) c[0],
                    (long) (e[1] - 1 - c[1]),
                    (long) c[2]);
      }
    }
    for (c[2] = 0; c[2] <= e[2]; c[2] ++) {       /* Output node coordinates */
      for (c[1] = 0; c[1] <= e[1]; c[1] ++) {
        for (c[0] = 0; c[0] <= e[0]; c[0] ++)
          fprintf (C_filepntrgeoout, "%ld\t%ld\t%ld\t%ld\n",
                    (long) (velmnbr + ((c[2] * n[1]) + c[1]) * n[0] + c[0]),
                    (long) c[0],
                    (long) (e[1] - c[1]),
                    (long) c[2]);
      }
    }
  }

#ifdef SCOTCH_DEBUG_MAIN1
  for (i = 0; i < C_FILENBR; i ++) {              /* For all file names     */
    if ((C_fileTab[i].name[0] != '-') ||          /* If not standard stream */
        (C_fileTab[i].name[1] != '\0')) {
      fclose (C_fileTab[i].pntr);                 /* Close the stream */
    }
  }
#endif /* SCOTCH_DEBUG_MAIN1 */

  return (0);
}
