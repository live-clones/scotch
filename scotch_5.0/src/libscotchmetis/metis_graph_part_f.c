/* Copyright 2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : metis_graph_part_f.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This file contains the Fortran API of   **/
/**                the compatibility library for the       **/
/**                MeTiS partitioning routines.            **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 10 sep 2006     **/
/**                                 to     07 jun 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "common.h"
#include "scotch.h"
#include "metis.h"                                /* Our "metis.h" file */

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the graph ordering routines.   */
/*                                    */
/**************************************/

/*
**
*/

FORTRAN (                                       \
METIS_PARTGRAPHKWAY, metis_partgraphkway, (     \
const int * const           n,                  \
const int * const           xadj,               \
const int * const           adjncy,             \
const int * const           vwgt,               \
const int * const           adjwgt,             \
const int * const           wgtflag,            \
const int * const           numflag,            \
const int * const           nparts,             \
const int * const           options,            \
int * const                 edgecut,            \
int * const                 part),              \
(n, xadj, adjncy, vwgt, adjwgt, wgtflag, numflag, nparts, options, edgecut, part))
{
  METIS_PartGraphKway (n, xadj, adjncy, vwgt, adjwgt, wgtflag, numflag, nparts, options, edgecut, part);
}

/*
**
*/

FORTRAN (                                             \
METIS_PARTGRAPHRECURSIVE, metis_partgraphrecursive, ( \
const int * const           n,                        \
const int * const           xadj,                     \
const int * const           adjncy,                   \
const int * const           vwgt,                     \
const int * const           adjwgt,                   \
const int * const           wgtflag,                  \
const int * const           numflag,                  \
const int * const           nparts,                   \
const int * const           options,                  \
int * const                 edgecut,                  \
int * const                 part),                    \
(n, xadj, adjncy, vwgt, adjwgt, wgtflag, numflag, nparts, options, edgecut, part))
{
  METIS_PartGraphRecursive (n, xadj, adjncy, vwgt, adjwgt, wgtflag, numflag, nparts, options, edgecut, part);
}

/*
**
*/

FORTRAN (                                       \
METIS_PARTGRAPHVKWAY, metis_partgraphvkway, (   \
const int * const           n,                  \
const int * const           xadj,               \
const int * const           adjncy,             \
const int * const           vwgt,               \
const int * const           vsize,              \
const int * const           wgtflag,            \
const int * const           numflag,            \
const int * const           nparts,             \
const int * const           options,            \
int * const                 volume,             \
int * const                 part),              \
(n, xadj, adjncy, vwgt, vsize, wgtflag, numflag, nparts, options, volume, part))
{
  METIS_PartGraphVKway (n, xadj, adjncy, vwgt, vsize, wgtflag, numflag, nparts, options, volume, part);
}
