/* Copyright 2020,2021,2023,2024 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : metis_graph_part_dual_f.c               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API of   **/
/**                the compatibility library for the       **/
/**                MeTiS dual graph processing routines.   **/
/**                                                        **/
/**   DATES      : # Version 6.1  : from : 11 may 2021     **/
/**                                 to   : 24 may 2021     **/
/**                # Version 7.0  : from : 21 jan 2023     **/
/**                                 to   : 11 aug 2024     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "module.h"
#include "common.h"
#include "scotch.h"
#include "metis.h"                                /* Our "metis.h" file */

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the graph dual routines.       */
/*                                    */
/**************************************/

FORTRAN (                                 \
SCOTCHMETISNAMESU (METIS_PARTMESHDUAL),   \
SCOTCHMETISNAMESL (metis_partmeshdual), ( \
const SCOTCH_Num * const    ne,           \
const SCOTCH_Num * const    nn,           \
const SCOTCH_Num * const    eptr,         \
const SCOTCH_Num * const    eind,         \
const SCOTCH_Num * const    vwgt,         \
const SCOTCH_Num * const    vsize,        \
const SCOTCH_Num * const    ncommon,      \
const SCOTCH_Num * const    nparts,       \
const double * const        tpwgts,       \
const SCOTCH_Num * const    options,      \
SCOTCH_Num * const          objval,       \
SCOTCH_Num * const          epart,        \
SCOTCH_Num * const          npart),       \
(ne, nn, eptr, eind, vwgt, vsize, ncommon, nparts, tpwgts, options, objval, epart, npart))
{
  SCOTCHMETISNAMES (METIS_PartMeshDual) (ne, nn, eptr, eind, vwgt, vsize, ncommon,
                                         nparts, tpwgts, options, objval, epart, npart);
}

/*
**
*/

FORTRAN (                                 \
SCOTCHMETISNAMEFU (METIS_PARTMESHDUAL),   \
SCOTCHMETISNAMEFL (metis_partmeshdual), ( \
const SCOTCH_Num * const    ne,           \
const SCOTCH_Num * const    nn,           \
const SCOTCH_Num * const    eptr,         \
const SCOTCH_Num * const    eind,         \
const SCOTCH_Num * const    vwgt,         \
const SCOTCH_Num * const    vsize,        \
const SCOTCH_Num * const    ncommon,      \
const SCOTCH_Num * const    nparts,       \
const double * const        tpwgts,       \
const SCOTCH_Num * const    options,      \
SCOTCH_Num * const          objval,       \
SCOTCH_Num * const          epart,        \
SCOTCH_Num * const          npart),       \
(ne, nn, eptr, eind, vwgt, vsize, ncommon, nparts, tpwgts, options, objval, epart, npart))
{
  SCOTCHMETISNAMES (METIS_PartMeshDual) (ne, nn, eptr, eind, vwgt, vsize, ncommon,
                                         nparts, tpwgts, options, objval, epart, npart);
}
