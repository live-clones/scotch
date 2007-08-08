/* Copyright 2007 INRIA
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
/**   NAME       : dgraph_match_tools.c                    **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : This file implements common functions   **/
/**                used by the two matching algorithms.    **/
/**                Also implements a verification method   **/
/**                to validate matching results.           **/
/**                                                        **/
/**   DATES      : # Version 0.5  : from : 04 dec 2006     **/
/**                                 to   : 19 feb 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <mpi.h>

#include "module.h"
#include "common.h"
#include "graph.h"
#include "dgraph.h"
#include "dgraph_coarsen.h"
#include "dgraph_match.h"
#include "dgraph_match_tools.h"

/*
** Verification tool
*/
int
dgraphMatchCheck (
const Dgraph * restrict const           finegraph, /* Graph to match                       */
const DgraphMatchParam * restrict const param)    /* Tables concerning the vertices states */
{
  MPI_Datatype          coarmultype;
  int                   procnum;
  Gnum                  vertnum;
  Gnum                  vertmax;
  Gnum                  finevertfrstnum;
  Gnum *                accesstax;
  DgraphCoarsenMulti *  othermulttax;
  Gnum                  coarmultnbr;
  Gnum                  coarmultmax;
  DgraphCoarsenMulti *  multtax;
  MPI_Request *         sendrequests;
  MPI_Status            status;
  Gnum                  checklocval;
  Gnum                  checkglbval;

  MPI_Type_contiguous (2, GNUM_MPI, &coarmultype); /* Define type for MPI transfer */
  MPI_Type_commit (&coarmultype);

  coarmultnbr = param->coarmultnum;               /* Copy variable to preserve "const" qualifier */
  MPI_Allreduce (&coarmultnbr, &coarmultmax, 1, GNUM_MPI, MPI_MAX, finegraph->proccomm);

  memAllocGroup ((void **)
                 &accesstax, finegraph->vertlocnbr * sizeof (Gnum),
                 &othermulttax, coarmultmax * sizeof (DgraphCoarsenMulti),
                 &sendrequests, finegraph->procngbnbr * sizeof (MPI_Request),
                 NULL);
  accesstax -= finegraph->baseval;
  othermulttax -= finegraph->baseval;

  for (vertnum = finegraph->baseval ; vertnum < finegraph->vertlocnnd ; ++ vertnum)
    accesstax[vertnum] = -1; /* Initialise tab */

  for (procnum = 0 ; procnum < finegraph->procngbnbr ; ++procnum) { /* Send our coarmulttax to all neighboors */
    int flag;

    MPI_Isend (param->coarmulttax + finegraph->baseval, coarmultnbr - finegraph->baseval,
               coarmultype, finegraph->procngbtab[procnum],
               TAGCOARDEBUG, finegraph->proccomm, &sendrequests[procnum]); /* Send message */
    MPI_Test (&sendrequests[procnum], &flag, MPI_STATUS_IGNORE); /* Commit send */
  }

  checklocval = 0;
  finevertfrstnum = finegraph->procdsptab[finegraph->proclocnum];
  for (procnum = -1, multtax = param->coarmulttax, vertmax = coarmultnbr;
       procnum < finegraph->procngbnbr ; ++ procnum) {
    if (procnum >= 0) {
      int coarnbr;
      if (MPI_Recv (othermulttax + finegraph->baseval, coarmultmax, coarmultype,
                    finegraph->procngbtab[procnum], TAGCOARDEBUG, finegraph->proccomm, &status) != MPI_SUCCESS)
        errorPrint ("dgraphMatchCheck: internal error");
      vertmax = coarnbr = 0;
      MPI_Get_count (&status, coarmultype, &coarnbr);
      vertmax = coarnbr + finegraph->baseval;
      multtax = othermulttax;
    }
    for (vertnum = finegraph->baseval ; vertnum < vertmax ; ++vertnum) {
      int i;
      for (i = 0 ; i < 2; ++i) {
        Gnum finevertnum;

        finevertnum = multtax[vertnum].vertnum[i];
        if (!dgraphVertexLocal (finegraph, finevertnum))
          continue;

        if ((accesstax[finevertnum - finevertfrstnum + finegraph->baseval] >= 0) &&
            ((i == 0) || ((i==1) && (multtax[vertnum].vertnum[0] != finevertnum)))) {
          fprintf (stderr, "vertnum : %ld (%d)\n", (long)vertnum, procnum);
          errorPrint ("dgraphMatchCheck : internal error (1)");
          checklocval = 1;
        }
        if (procnum >= 0)
          accesstax[finevertnum - finevertfrstnum + finegraph->baseval] =
            vertnum + finegraph->procdsptab[finegraph->procngbtab[procnum]] - finegraph->baseval;
        else
          accesstax[finevertnum - finevertfrstnum + finegraph->baseval] =
            vertnum + finevertfrstnum - finegraph->baseval;
      }
    }
  }

  for (vertnum = finegraph->baseval ; vertnum < finegraph->vertlocnnd ; ++ vertnum) {
    if (accesstax[vertnum] < 0) {
      fprintf (stderr, "vertnum : %ld / %ld\n", (long)vertnum, (long)finegraph->vertlocnbr);
      errorPrint ("dgraphMatchCheck : internal error (2)");
      checklocval = 2;
      continue;
    }
    if (dgraphVertexProc(finegraph, accesstax[vertnum]) != param->finefoldtax[vertnum]) {
      int myprocnum;
      myprocnum = dgraphVertexProc(finegraph, accesstax[vertnum]);
      errorPrint ("dgraphMatchCheck : internal error (3)");
      checklocval = 3;
      continue;
    }
  }
  MPI_Waitall (finegraph->procngbnbr, sendrequests, MPI_STATUSES_IGNORE); /* wait for sending comms completion */
  MPI_Type_free (&coarmultype);                   /* Free type */

  MPI_Allreduce (&checklocval, &checkglbval, 1, GNUM_MPI, MPI_MAX, finegraph->proccomm);
  memFree (accesstax + finegraph->baseval);

  return (checkglbval);
}


int
dgraphMatchConvertInit (const Dgraph * restrict const grafptr,
                        DgraphMatchConvert * restrict convert)
{
  Gnum gstnbr;                                    /* Number of ghost vertices */
  Gnum edgenum;
  Gnum vertlocnum;
  Gnum procsidnum;

  gstnbr = grafptr->vertgstnbr - grafptr->vertlocnbr;

/*   for (convert->coarhashmsk = 31; convert->coarhashmsk < gstnbr; */
/*        convert->coarhashmsk = convert->coarhashmsk * 2 + 1) ; */
/*   convert->coarhashmsk = convert->coarhashmsk * 4 + 3; */
/*   convert->coarhashnbr = convert->coarhashmsk + 1; */

  if (memAllocGroup ((void**)
                     &convert->gst2glbtab, (gstnbr * sizeof (Gnum)),
/*                      &convert->glb2gsttab, (convert->coarhashnbr * sizeof (Gnum)), */
                     NULL) == NULL) {
    errorPrint ("dgraphMatchSyncInitConvert : Allocation Failed");
    return (1);
  }
  convert->gst2glbtab -= grafptr->vertlocnnd;

  for (procsidnum = 0, vertlocnum = grafptr->baseval;    /* Only browse frontier vertices */
       procsidnum < grafptr->procsidnbr; ) {

    while ((procsidnum < grafptr->procsidnbr) && (grafptr->procsidtab[procsidnum] < 0)) {
      vertlocnum -= (Gnum) grafptr->procsidtab[procsidnum];
      procsidnum ++;
    }
    if (procsidnum >= grafptr->procsidnbr)
      break;
    for (edgenum = grafptr->vertloctax[vertlocnum]; edgenum < grafptr->vendloctax[vertlocnum] ; ++ edgenum) {
      if (!dgraphVertexLocal (grafptr, grafptr->edgeloctax[edgenum])) {   /* Vertex is non-local */
	convert->gst2glbtab[grafptr->edgegsttax[edgenum]] = grafptr->edgeloctax[edgenum];
      }
    }
    while ((procsidnum < grafptr->procsidnbr) && (grafptr->procsidtab[procsidnum] >= 0))
      procsidnum ++;
  }

  return (0);
}

void
dgraphMatchConvertExit (const Dgraph * restrict const grafptr,
                        DgraphMatchConvert * restrict convert)
{
  memFree (convert->gst2glbtab + grafptr->vertlocnnd);
}
