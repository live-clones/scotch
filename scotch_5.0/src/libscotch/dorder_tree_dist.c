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
/**   NAME       : dorder_tree_dist.c                      **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles distributed         **/
/**                orderings.                              **/
/**                                                        **/
/**   DATES      : # Version 5.1  : from : 28 nov 2007     **/
/**                                 to     07 dec 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define DORDER

#include "module.h"
#include "common.h"
#include "dgraph.h"
#include "dorder.h"

/************************************/
/*                                  */
/* These routines handle orderings. */
/*                                  */
/************************************/

/* This function returns to all processes the
** number of distributed leaf column blocks
** possessed by the ordering.
** It returns:
** - >=0 : number of distributed column blocks.
** - <0  : on error.
*/

Gnum
dorderCblkDist (
const Dorder * restrict const ordeptr)
{
  Gnum                        cblklocnbr;
  Gnum                        cblkglbnbr;

  cblklocnbr = ordeptr->cblklocnbr;
  if (MPI_Allreduce (&cblklocnbr, &cblkglbnbr, 1, GNUM_MPI, MPI_SUM, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderCblkDist: communication error");
    return     ((Gnum) -1);
  }

  return (cblkglbnbr);
}

/* This function returns on all of the procesors the
** distributed part of the distributed structure of
** the given distributed ordering. The two array
** pointers which must be passed should both point to
** arrays of size dorderCblkDist().
** It returns:
** - 0   : if the distributed tree structure could be computed.
** - !0  : on error.
*/

int
dorderTreeDist (
const Dorder * restrict const ordeptr,
const Dgraph * restrict const grafptr,
Gnum * restrict const         treeglbtab,
Gnum * restrict const         sizeglbtab)
{
  const DorderLink * restrict linklocptr;
  Gnum                        cblklocnbr;
  int                         cblkloctmp;         /* MPI only supports int as count type */
  int                         cblkglbtmp;
  Gnum                        cblkglbnbr;
  Gnum                        cblkglbnum;
  int * restrict              cblkcnttab;
  int * restrict              cblkdsptab;
  Gnum * restrict             sortglbtab;         /* Array to post-order sort nodes */
  Gnum *                      sortglbsrc;
  Gnum *                      sortglbdst;
  Gnum * restrict             permglbtab;
  int                         procglbnbr;
  int                         procnum;
  int                         reduloctab[3];
  int                         reduglbtab[3];
#define treeloctab                  treeglbtab
#define sizeloctab                  sizeglbtab
#define ordeloctab                  sortglbtab

  cblklocnbr = ordeptr->cblklocnbr;
  if (MPI_Allreduce (&cblklocnbr, &cblkglbnbr, 1, GNUM_MPI, MPI_SUM, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (1)");
    return     (1);
  }

  MPI_Comm_size (ordeptr->proccomm, &procglbnbr);

  reduloctab[0] =
  reduloctab[1] =
  reduloctab[2] = 0;
  if (memAllocGroup ((void **) (void *)
                     &sortglbtab, (size_t) ( cblkglbnbr * 2  * sizeof (Gnum)),
                     &permglbtab, (size_t) ((cblkglbnbr + 1) * sizeof (Gnum)), /* TRICK: "+1" to handle root node in process 0 */
                     &cblkcnttab, (size_t) (procglbnbr       * sizeof (int)),
                     &cblkdsptab, (size_t) (procglbnbr       * sizeof (int)), NULL) == NULL) {
    errorPrint ("dorderTreeDist: out of memory");
    reduloctab[0] = 1;                            /* Memory error */
  }
  else {
    if (treeloctab != NULL)
      reduloctab[1] = 1;                          /* Compute the "or" of any array being non-null */
    if (sizeloctab != NULL) {
      reduloctab[2] = reduloctab[1];              /* Compute the "and" of any array being non-null */
      reduloctab[1] = 1;
    }
  }
#ifdef SCOTCH_DEBUG_DORDER1                       /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (reduloctab, reduglbtab, 3, MPI_INT, MPI_SUM, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (1)");
    reduglbtab[0] =                               /* Post-process error below      */
    reduglbtab[1] =                               /* Prevent Valgrind from yelling */
    reduglbtab[2] = 1;
  }
#else /* SCOTCH_DEBUG_DORDER1 */
  reduglbtab[0] = reduloctab[0];
  reduglbtab[1] = procglbnbr - 1 + reduloctab[1];
  reduglbtab[2] = procglbnbr - 1 + reduloctab[2];
#endif /* SCOTCH_DEBUG_DORDER1 */

  if (reduglbtab[1] != reduglbtab[2]) {           /* If not both arrays provided on each of the candidate processors */
    if (reduloctab[1] != reduloctab[2])
      errorPrint ("dorderTreeDist: invalid parameters (1)");
    reduglbtab[0] = 1;
  }
  if (reduglbtab[2] != procglbnbr) {
    errorPrint ("dorderTreeDist: invalid parameters (2)");
    reduglbtab[0] = 1;
  }
  if (reduglbtab[0] != 0) {
    if (sortglbtab != NULL)
      memFree (sortglbtab);                       /* Free group leader */
    return (1);
  }

  cblkloctmp = ordeptr->cblklocnbr;               /* MPI only supports int as count type */
  if (MPI_Allgather (&cblkloctmp, 1, MPI_INT, cblkcnttab, 1, MPI_INT, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (2)");
    return     (1);
  }
  for (procnum = cblkglbtmp = 0; procnum < procglbnbr; procnum ++) { /* Accumulate un-based global start indices for column blocks */
    cblkdsptab[procnum] = cblkglbtmp;
    cblkglbtmp         += cblkcnttab[procnum];
  }

  for (linklocptr = ordeptr->linkdat.nextptr;     /* For all nodes in local ordering structure */
       linklocptr != &ordeptr->linkdat; linklocptr = linklocptr->nextptr) {
    const DorderCblk * restrict cblklocptr;

    cblklocptr = (DorderCblk *) linklocptr;       /* TRICK: FIRST                    */
    if (cblklocptr->cblknum.proclocnum == ordeptr->proclocnum) { /* If node is local */
      Gnum                        cblklocnum;

      cblklocnum = cblklocptr->cblknum.cblklocnum;
#ifdef SCOTCH_DEBUG_DORDER2
      if ((cblklocnum < 0) || (cblklocnum >= ordeptr->cblklocnbr)) {
        errorPrint ("dorderTreeDist: internal error");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_DORDER2 */
      sizeloctab[cblklocnum] = cblklocptr->vnodglbnbr;
      treeloctab[cblklocnum] = (cblklocptr->fathnum.cblklocnum == -1) ? -1
                               : cblkdsptab[cblklocptr->fathnum.proclocnum] + cblklocptr->fathnum.cblklocnum;
      ordeloctab[cblklocnum] = cblklocptr->ordeglbval;
    }
  }

  if (MPI_Allgatherv (ordeloctab, cblkloctmp, GNUM_MPI, /* Spread start indices of column blocks in second half of sort array */
                      sortglbtab + cblkglbnbr, cblkcnttab, cblkdsptab, GNUM_MPI, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (3)");
    return     (1);
  }
  for (cblkglbnum = 0, sortglbdst = sortglbtab, sortglbsrc = sortglbtab + cblkglbnbr;
       cblkglbnum < cblkglbnbr; cblkglbnum ++) {
    Gnum                        ordelocval;

    ordelocval     = *sortglbsrc ++;
    *sortglbdst ++ = ordelocval;
    *sortglbdst ++ = cblkglbnum;
  }
  intSort2asc2 (sortglbtab, cblkglbnbr);          /* Sort local nodes by ascending inverse start index */

  permglbtab[0] = -1 - ordeptr->baseval;          /* TRICK: set cell for root father index */
  permglbtab ++;
  for (cblkglbnum = 0; cblkglbnum < cblkglbnbr; cblkglbnum ++) /* Build inverse column block permutation */
    permglbtab[sortglbtab[cblkglbnum * 2 + 1]] = cblkglbnum;

  if (MPI_Allgatherv (treeloctab, cblkloctmp, GNUM_MPI, /* Use sort array as temporary storage */
                      sortglbtab, cblkcnttab, cblkdsptab, GNUM_MPI, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (4)");
    return     (1);
  }
  for (cblkglbnum = 0; cblkglbnum < cblkglbnbr; cblkglbnum ++) /* Permute tree data */
    treeglbtab[permglbtab[cblkglbnum]] = permglbtab[sortglbtab[cblkglbnum]] + ordeptr->baseval;

  if (MPI_Allgatherv (sizeloctab, cblkloctmp, GNUM_MPI, /* Use sort array as temporary storage */
                      sortglbtab, cblkcnttab, cblkdsptab, GNUM_MPI, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (4)");
    return     (1);
  }
  for (cblkglbnum = 0; cblkglbnum < cblkglbnbr; cblkglbnum ++) /* Permute tree data */
    sizeglbtab[permglbtab[cblkglbnum]] = sortglbtab[cblkglbnum];

  memFree (sortglbtab);                           /* Free group leader */

  return (0);
}
