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
/**   NAME       : main_fax_mesh.c                         **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This is the test module for the block   **/
/**                symbolic factorization routine.         **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 17 nov 2003     **/
/**                                 to     17 nov 2003     **/
/**                # Version 3.0  : from : 02 mar 2004     **/
/**                                 to     02 mar 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "common.h"
#include "scotch.h"
#include "mesh.h"
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
  FILE *              meshfile;                   /* Mesh input stream     */
  Mesh                meshdat;                    /* Mesh structure        */
  Order               ordedat;                    /* Mesh ordering         */
  FILE *              ordefile;                   /* Ordering file         */
  SymbolMatrix        symbdat;                    /* Block factored matrix */
  FILE *              symbfile;                   /* Matrix output stream  */
  Dof                 deofdat;                    /* Matrix DOF structure  */
  double              matrnnz;                    /* Non-zeroes            */
  double              matropc;                    /* Operation count       */
  INT                 leafnbr;                    /* Number of leaves      */
  INT                 heigmin;                    /* Minimum height        */
  INT                 heigmax;                    /* Maximum height        */
  double              heigavg;                    /* Average height        */
  double              heigdlt;                    /* Deviation             */
  INT                 vnodnbr;                    /* Number of nodes       */
  INT                 velmbas;                    /* Base values           */
  INT                 vnodbas;
  INT                 baseval;
  int                 o;

  if ((argc < 4) || (argc > 5)) {
    errorPrint ("main_fax_mesh: usage: %s mesh_file ord_file display_file [ordering_strategy]", argv[0]);
    return     (1);
  }

  if (strcmp (argv[1], "-") == 0)
    meshfile = stdin;
  else if ((meshfile = fopen (argv[1], "r")) == NULL) {
    errorPrint ("main_fax_mesh: cannot open mesh file");
    return     (1);
  }

  meshInit (&meshdat);                           /* Initialize mesh structure */

  o = meshLoad (&meshdat, meshfile, -1);         /* Read mesh */
  if (meshfile != stdin)
    fclose (meshfile);
  if (o != 0) {
    errorPrint ("main_fax_mesh: cannot read mesh");
    return     (1);
  }

  SCOTCH_meshData (&meshdat, &velmbas, &vnodbas, NULL, &vnodnbr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  baseval = MIN (velmbas, vnodbas);

  dofInit     (&deofdat);
  dofConstant (&deofdat, baseval, vnodnbr, 1); /* One DOF per node */

  orderInit  (&ordedat);
  if (argc == 4)
    o = orderMesh (&ordedat, &meshdat);
  else
    o = orderMeshStrat (&ordedat, &meshdat, argv[4]);
  if (o != 0) {
    errorPrint ("main_fax_mesh: cannot order mesh");
    return     (1);
  }

  if (strcmp (argv[2], "-") == 0)
    ordefile = stdout;
  else if ((ordefile = fopen (argv[2], "w+")) == NULL) {
    errorPrint ("main_fax_mesh: cannot open ordering file");
    return     (1);
  }

  orderSave (&ordedat, ordefile);

  if (ordefile != stdout)
    fclose (ordefile);

  symbolInit    (&symbdat);
  symbolFaxMesh (&symbdat, &meshdat, &ordedat);
  symbolCheck   (&symbdat);
  symbolCost    (&symbdat, &deofdat, SYMBOLCOSTLDLT, &matrnnz, &matropc);
  symbolTree    (&symbdat, &deofdat, &leafnbr,
                 &heigmin, &heigmax, &heigavg, &heigdlt);

  printf ("cblknbr=%6ld\tbloknbr=%6ld\tratio=%5.3f\n",
          (long) symbdat.cblknbr, (long) symbdat.bloknbr,
          (double) ((double) symbdat.bloknbr / (double) symbdat.cblknbr));
  printf ("nnz=%e\topc=%e\n",
          matrnnz, matropc);
  printf ("leaf=%ld\thmin=%ld\thmax=%ld\thavg=%e\thdlt=%e\n",
          (long) leafnbr, (long) heigmin, (long) heigmax, heigavg, heigdlt);

/* symbolSave (&symbdat, stdout); */

  if (strcmp (argv[3], "-") == 0)
    symbfile = stdout;
  else if ((symbfile = fopen (argv[3], "w+")) == NULL) {
    errorPrint ("main_fax_mesh: cannot open symbol file");
    return     (1);
  }

  symbolDraw (&symbdat, symbfile);

  if (symbfile != stdout)
    fclose (symbfile);

  symbolExit (&symbdat);
  orderExit  (&ordedat);
  dofExit    (&deofdat);
  meshExit   (&meshdat);

  exit (0);
}
