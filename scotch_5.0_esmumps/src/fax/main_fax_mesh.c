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
