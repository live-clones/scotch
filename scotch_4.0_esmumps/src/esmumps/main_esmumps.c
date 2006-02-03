/************************************************************/
/**                                                        **/
/**   NAME       : main_mumps.c                            **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This is the test module for the MUMPS   **/
/**                interface routine.                      **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 17 may 2001     **/
/**                                 to     17 may 2001     **/
/**                # Version 1.0  : from : 17 jun 2005     **/
/**                                 to     17 jun 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "common.h"
#include "scotch.h"
#include "graph.h"
#include "esmumps.h"

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
  Graph               grafdat;                    /* Graph to load */
  INT                 vertnbr;
  INT *               verttab;
  INT                 edgenbr;
  INT *               edgetab;
  INT *               lentab;
  INT *               nvtab;
  INT *               elentab;
  INT *               lasttab;
  INT                 pfree;
  INT                 ncmpa;
  INT                 vertnum;
  FILE *              stream;

  if (argc != 2) {
    errorPrint ("main_esmumps: usage: main_esmumps graph_file");
    return     (1);
  }

  graphInit (&grafdat);
  if ((stream = fopen (argv[1], "r")) == NULL) {
    errorPrint ("main_esmumps: cannot open graph file");
    graphExit  (&grafdat);
    return     (1);
  }
  if (graphLoad (&grafdat, stream, 1, 3) != 0) {  /* Base graph with base value 1, no loads */
    errorPrint ("main_esmumps: cannot open graph file");
    graphExit  (&grafdat);
    return     (1);
  }
  fclose (stream);

  graphData (&grafdat, NULL, &vertnbr, &verttab, NULL, NULL, NULL, &edgenbr, &edgetab, NULL);

  if ((lentab = (INT *) memAlloc (vertnbr * sizeof (INT))) == NULL) {
    errorPrint ("main_esmumps: out of memory (1)");
    graphExit  (&grafdat);
    return     (1);
  }
  for (vertnum = 0; vertnum < vertnbr; vertnum ++) {
    if (verttab[vertnum] == verttab[vertnum + 1]) {
      lentab[vertnum] = 0;
      verttab[vertnum] = 0;                       /* Graph structure no longer valid in Emilio */
    }
    else
      lentab[vertnum] = verttab[vertnum + 1] - verttab[vertnum];
  }

  if (((nvtab   = (INT *) memAlloc (vertnbr * sizeof (INT))) == NULL) ||
      ((elentab = (INT *) memAlloc (vertnbr * sizeof (INT))) == NULL) ||
      ((lasttab = (INT *) memAlloc (vertnbr * sizeof (INT))) == NULL)) {
    errorPrint ("main_esmumps: out of memory (2)");
    if (nvtab != NULL) {
      if (elentab != NULL)
        memFree (elentab);
      memFree (nvtab);
    }
    graphExit (&grafdat);
    return    (1);
  }

  pfree = edgenbr + 1;
  ESMUMPSF (&vertnbr, &edgenbr, verttab, &pfree,
            lentab, edgetab, nvtab, elentab, lasttab, &ncmpa);

  memFree   (lasttab);
  memFree   (elentab);
  memFree   (nvtab);
  memFree   (lentab);
  graphExit (&grafdat);

  if (ncmpa < 0) {
    errorPrint ("main_esmumps: error in ESMUMPSF (%d)", ncmpa);
    return     (1);
  }

  exit (0);
}
