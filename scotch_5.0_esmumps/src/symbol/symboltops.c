/* Copyright 2004,2007 INRIA
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
/**   NAME       : symboltops.c                            **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This program produces a PostScript(tm)  **/
/**                file from a symbol matrix file.         **/
/**                                                        **/
/**   DATES      : # Version 0.1  : from : 21 mar 2002     **/
/**                                 to     21 mar 2002     **/
/**                # Version 3.0  : from : 29 sep 2004     **/
/**                                 to     29 sep 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "common.h"
#include "symbol.h"

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
  FILE *              symbfile;                   /* Symbol input stream      */
  SymbolMatrix        symbdat;                    /* Mesh graph               */
  FILE *              postfile;                   /* PostScript output stream */
  int                 errval;
  char *              errstr;

  errorProg ("symboltops");

  if (argc > 3) {
    errorPrint ("usage: symboltops symbol_file postscript_tm_file");
    return     (1);
  }

  errval = 0;
  errstr = NULL;

  symbfile = stdin;                               /* Assume standard streams */
  postfile = stdout;

  symbolInit (&symbdat);                          /* Initialize symbol structure */

  if (argc > 1) {                                 /* If input file name provided */
    if ((strcmp (argv[1], "-") != 0) &&
        ((symbfile = fopen (argv[1], "r")) == NULL)) {
      errval = 1;
      errstr = "cannot open symbol file";
    }
    else if (argc > 2) {                          /* If output file name provided */
      if ((strcmp (argv[2], "-") != 0) &&
          ((postfile = fopen (argv[2], "w+")) == NULL)) {
        errval = 1;
        errstr = "cannot open PostScript(tm) file";
      }
    }
  }

  if (errval == 0) {
    if (symbolLoad (&symbdat, symbfile) != 0) {   /* Read symbol data */
      errval = 1;
      errstr = "cannot read symbol file";
    }
    else if (symbolDraw (&symbdat, postfile) != 0) {
      errval = 1;
      errstr = "cannot write PostScript(tm) file";
    }
  }

  if (errstr != NULL)
    errorPrint (errstr);

  symbolExit (&symbdat);

  return (errval);
}
