/* Copyright 2020,2021,2023-2026 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : metis_graph_dual.c                      **/
/**                                                        **/
/**   AUTHOR     : Marc FUENTES                            **/
/**                Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the compatibility        **/
/**                library for the MeTiS partitioning      **/
/**                routines containing routines relative   **/
/**                to dual graphs                          **/
/**                                                        **/
/**   DATES      : # Version 6.1  : from : 01 sep 2020     **/
/**                                 to   : 28 may 2021     **/
/**                # Version 7.0  : from : 21 jan 2023     **/
/**                                 to   : 01 apr 2026     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "scotch.h"
#include "metis.h"                                /* Our "metis.h" file */
#include "metis_graph_dual.h"                     /* Our "metis.h" file */

/* This routine creates a (SCOTCH_)Mesh structure
** from the partial mesh connectivity data that is
** passed to it.
** It returns:
** - METIS_OK      : if the mesh has been successfully built.
** - METIS_ERROR*  : on error.
*/

int
SCOTCHMETISNAMES (METIS_MeshToDual) (
const SCOTCH_Num * const    ne,
const SCOTCH_Num * const    nn,
const SCOTCH_Num * const    eptr,
const SCOTCH_Num * const    eind,
const SCOTCH_Num * const    ncommon,
const SCOTCH_Num * const    nuimflag,
SCOTCH_Num ** const         xadj,
SCOTCH_Num ** const         adjncy)
{
  SCOTCH_Mesh         meshdat;
  SCOTCH_Graph        grafdat;
  SCOTCH_Num          baseval;
  SCOTCH_Num          vertnbr;
  SCOTCH_Num *        verttab;
  SCOTCH_Num *        vendtab;
  SCOTCH_Num          edgenbr;
  SCOTCH_Num *        edgetab;
  int                 o;

  *xadj = NULL;                                   /* Assume something will go wrong */

  SCOTCH_meshInit  (&meshdat);
  SCOTCH_graphInit (&grafdat);

  edgenbr = eptr[*ne] - *nuimflag;                /* Number of arcs in element array */
  if (SCOTCH_meshBuildElem (&meshdat, *nuimflag, *nuimflag, *ne, *nn,
                            eptr, eptr + 1, NULL, NULL, NULL, edgenbr, eind) != 0) {
    SCOTCH_errorPrint ("SCOTCH_METIS_MeshToDual: cannot create mesh");
    return            (METIS_ERROR);
  }
  o = SCOTCH_meshGraphDual (&meshdat, &grafdat, *ncommon);
  SCOTCH_meshExit (&meshdat);                     /* Mesh structure is no longer needed */
  if (o != 0) {
    SCOTCH_errorPrint ("SCOTCH_METIS_MeshToDual: cannot create graph from mesh");
    return            (METIS_ERROR);
  }

  SCOTCH_graphData (&grafdat, &baseval, &vertnbr, &verttab, &vendtab, NULL, NULL, &edgenbr, &edgetab, NULL);

  if (((*xadj   = malloc ((vertnbr + 1) * sizeof (SCOTCH_Num))) == NULL) || /* Do not use libScotch memory allocation as freed by user */
      ((*adjncy = malloc (edgenbr       * sizeof(SCOTCH_Num)))  == NULL)) {
    SCOTCH_errorPrint ("SCOTCH_METIS_MeshToDual: out of memory");
    if (*xadj != NULL)
      free (*xadj);
    SCOTCH_graphExit (&grafdat);
    return           (METIS_ERROR_MEMORY);
  }

  memCpy (*xadj,   verttab, (vertnbr + 1) * sizeof (SCOTCH_Num));
  memCpy (*adjncy, edgetab,  edgenbr      * sizeof (SCOTCH_Num));
  SCOTCH_graphExit (&grafdat);

  return (METIS_OK);
}

/*
**
*/

int
SCOTCHMETISNAMEC (METIS_MeshToDual) (
const SCOTCH_Num * const    ne,
const SCOTCH_Num * const    nn,
const SCOTCH_Num * const    eptr,
const SCOTCH_Num * const    eind,
const SCOTCH_Num * const    ncommon,
const SCOTCH_Num * const    nuimflag,
SCOTCH_Num ** const         xadj,
SCOTCH_Num ** const         adjncy)
{
  return (SCOTCHMETISNAMES (METIS_MeshToDual) (ne, nn, eptr, eind, ncommon, nuimflag, xadj, adjncy));
}
