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
/**   NAME       : main_fax_graph.c                        **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This is the test module for the block   **/
/**                symbolic factorization routine.         **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 20 aug 1998     **/
/**                                 to     22 nov 2001     **/
/**                # Version 3.0  : from : 02 mar 2004     **/
/**                                 to     02 mar 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "common.h"
#include "scotch.h"
#include "graph.h"
#include "dof.h"
#include "symbol.h"
#include "order.h"
#include "fax.h"

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
  FILE *              graffile;                   /* Graph input stream    */
  Graph               grafmesh;                   /* Mesh graph            */
  Order               ordemesh;                   /* Mesh graph ordering   */
  FILE *              ordefile;                   /* Ordering file         */
  SymbolMatrix        matrsymb;                   /* Block factored matrix */
  Dof                 matrdeof;                   /* Matrix DOF structure  */
  double              matrnnz;                    /* Non-zeroes            */
  double              matropc;                    /* Operation count       */
  INT                 leafnbr;                    /* Number of leaves      */
  INT                 heigmin;                    /* Minimum height        */
  INT                 heigmax;                    /* Maximum height        */
  double              heigavg;                    /* Average height        */
  double              heigdlt;                    /* Deviation             */
  INT                 vertnbr;                    /* Number of vertices    */
  INT                 baseval;                    /* Base value            */
  FILE *              symbfile;                   /* Matrix output stream  */
  int                 o;

  if ((argc < 4) || (argc > 5)) {
    errorPrint ("main_fax_graph: usage: %s graph_file ord_file display_file [ordering_strategy]", argv[0]);
    return     (1);
  }

  if (strcmp (argv[1], "-") == 0)
    graffile = stdin;
  else if ((graffile = fopen (argv[1], "r")) == NULL) {
    errorPrint ("main_fax_graph: cannot open graph file");
    return     (1);
  }

  graphInit (&grafmesh);                          /* Initialize graph structure */

  o = graphLoad (&grafmesh, graffile, -1, 0);     /* Read graph */
  if (graffile != stdin)
    fclose (graffile);
  if (o != 0) {
    errorPrint ("main_fax_graph: cannot read graph");
    return     (1);
  }

  SCOTCH_graphData (&grafmesh, &baseval, &vertnbr, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  dofInit     (&matrdeof);
  dofConstant (&matrdeof, baseval, vertnbr, 1);   /* One DOF per node */

  orderInit  (&ordemesh);
  if (argc == 4)
    o = orderGraph (&ordemesh, &grafmesh);
  else
    o = orderGraphStrat (&ordemesh, &grafmesh, argv[4]);
  if (o != 0) {
    errorPrint ("main_fax_graph: cannot order graph");
    return     (1);
  }

  if (strcmp (argv[2], "-") == 0)
    ordefile = stdout;
  else if ((ordefile = fopen (argv[2], "w+")) == NULL) {
    errorPrint ("main_fax_graph: cannot open ordering file");
    return     (1);
  }

  orderSave (&ordemesh, ordefile);

  if (ordefile != stdout)
    fclose (ordefile);

  symbolInit     (&matrsymb);
  symbolFaxGraph (&matrsymb, &grafmesh, &ordemesh);
  symbolCheck    (&matrsymb);
  symbolCost     (&matrsymb, &matrdeof, SYMBOLCOSTLDLT, &matrnnz, &matropc);
  symbolTree     (&matrsymb, &matrdeof, &leafnbr,
                  &heigmin, &heigmax, &heigavg, &heigdlt);

  printf ("cblknbr=%6ld\tbloknbr=%6ld\tratio=%5.3f\n",
          (long) matrsymb.cblknbr, (long) matrsymb.bloknbr,
          (double) ((double) matrsymb.bloknbr / (double) matrsymb.cblknbr));
  printf ("nnz=%e\topc=%e\n",
          matrnnz, matropc);
  printf ("leaf=%ld\thmin=%ld\thmax=%ld\thavg=%e\thdlt=%e\n",
          (long) leafnbr, (long) heigmin, (long) heigmax, heigavg, heigdlt);

/* symbolSave (&matrsymb, stdout); */

  if (strcmp (argv[3], "-") == 0)
    symbfile = stdout;
  else if ((symbfile = fopen (argv[3], "w+")) == NULL) {
    errorPrint ("main_fax_graph: cannot open symbol file");
    return     (1);
  }

  symbolDraw (&matrsymb, symbfile);

  if (symbfile != stdout)
    fclose (symbfile);

  symbolExit (&matrsymb);
  orderExit  (&ordemesh);
  dofExit    (&matrdeof);
  graphExit  (&grafmesh);

  exit (0);
}
