/* Copyright 2018 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : arch_cmpltw_mpi.c                       **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the weighted        **/
/**                complete graph target architecture.     **/
/**                                                        **/
/**   DATES      : # Version 7.0  : from : 18 feb 2018     **/
/**                                 to   : 19 feb 2018     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH_CMPLTW_MPI

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_mpi.h"
#include "arch_cmpltw_mpi.h"

/*****************************************/
/*                                       */
/* These are the weighted complete graph */
/* routines.                             */
/*                                       */
/*****************************************/

/* This function creates the MPI_Datatype for
** the weighted complete graph domain.
** It returns:
** - 0  : if type could be created.
** - 1  : on error.
*/

int
archCmpltwDomMpiType (
const ArchCmpltw * const      archptr,
MPI_Datatype * const          typeptr)
{
  return (archDomMpiTypeAnum (3, typeptr));
}
