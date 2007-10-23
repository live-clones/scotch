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
/**   NAME       : parmetis_dgraph_order.c                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the compatibility        **/
/**                library for the ParMeTiS ordering       **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 17 oct 2007     **/
/**                                 to     21 oct 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "common.h"
#include "ptscotch.h"
#include "parmetis.h"                             /* Our "parmetis.h" file */

/************************************/
/*                                  */
/* These routines are the C API for */
/* the ParMeTiS graph ordering      */
/* routine.                         */
/*                                  */
/************************************/

/*
**
*/

void
ParMETIS_V3_NodeND (
const int * const           vtxdist,
int * const                 xadj,
int * const                 adjncy,
const int * const           numflag,
const int * const           options,              /* Not used */
int * const                 order,
int * const                 sizes,                /* Of size twice the number of processors ; not used */
MPI_Comm *                  comm)
{
  MPI_Comm            proccomm;
  int                 procglbnbr;
  int                 proclocnum;
  SCOTCH_Dgraph       grafdat;                    /* Scotch distributed graph object to interface with libScotch    */
  SCOTCH_Dordering    ordedat;                    /* Scotch distributed ordering object to interface with libScotch */
  SCOTCH_Strat        stradat;
  SCOTCH_Num          vertlocnbr;
  SCOTCH_Num          edgelocnbr;

  if (sizeof (SCOTCH_Num) != sizeof (int)) {
    SCOTCH_errorPrint ("ParMETIS_V3_NodeND (as of SCOTCH): SCOTCH_Num type should equate to int");
    return;
  }

  proccomm = *comm;
  if (SCOTCH_dgraphInit (&grafdat, proccomm) != 0)
    return;

  MPI_Comm_size (proccomm, &procglbnbr);
  MPI_Comm_rank (proccomm, &proclocnum);
  vertlocnbr = vtxdist[proclocnum + 1] - vtxdist[proclocnum];
  edgelocnbr = xadj[vertlocnbr] - *numflag;

  memSet (sizes, ~0, 2 * procglbnbr * sizeof (int)); /* Array not used */

  if (SCOTCH_dgraphBuild (&grafdat, (SCOTCH_Num) *numflag,
                          vertlocnbr, vertlocnbr, xadj, xadj + 1, NULL, NULL,
                          edgelocnbr, edgelocnbr, adjncy, NULL, NULL) == 0) {
    SCOTCH_stratInit (&stradat);
#ifdef SCOTCH_DEBUG_ALL
    if (SCOTCH_dgraphCheck (&grafdat) == 0)       /* TRICK: next instruction called only if graph is consistent */
#endif /* SCOTCH_DEBUG_ALL */
    {
      if (SCOTCH_dgraphOrderInit (&grafdat, &ordedat) == 0) {
        SCOTCH_dgraphOrderCompute (&grafdat, &ordedat, &stradat);
        SCOTCH_dgraphOrderPerm    (&grafdat, &ordedat, order);
        SCOTCH_dgraphOrderExit    (&grafdat, &ordedat);
      }
    }
    SCOTCH_stratExit (&stradat);
  }
  SCOTCH_dgraphExit (&grafdat);
}
