/* Copyright 2007,2008,2023,2026 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**                                 to   : 09 may 2008     **/
/**                # Version 7.0  : from : 17 jan 2023     **/
/**                                 to   : 25 aug 2026     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

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

static
int
dorderCblkDist2 (
const Dorder * restrict const ordeptr,
Gnum * restrict const         dblklocptr,         /*+ Pointer to local number of locally-rooted distributed column blocks  +*/
Gnum * restrict const         dblkglbptr)         /*+ Pointer to global number of locally-rooted distributed column blocks +*/
{
  const DorderLink * restrict linklocptr;
  Gnum                        dblklocnbr;         /* Local number of locally-rooted distributed column blocks  */
  Gnum                        dblkglbnbr;         /* Global number of locally-rooted distributed column blocks */

  for (linklocptr = ordeptr->linkdat.nextptr, dblklocnbr = 0; /* For all nodes in local ordering structure */
       linklocptr != &ordeptr->linkdat; linklocptr = linklocptr->nextptr) {
    const DorderCblk * restrict cblklocptr;

    cblklocptr = (DorderCblk *) linklocptr;       /* TRICK: FIRST */
    if (cblklocptr->cblknum.proclocnum == ordeptr->proclocnum) {
#ifdef SCOTCH_DEBUG_DORDER2
      Gnum                        cblklocnum;

      cblklocnum = cblklocptr->cblknum.cblklocnum;
      if ((cblklocnum < 0) || (cblklocnum >= ordeptr->cblklocnbr)) {
        errorPrint ("dorderCblkDist2: internal error");
        return (1);
      }
#endif /* SCOTCH_DEBUG_DORDER2 */
      dblklocnbr ++;
    }
  }

  if (MPI_Allreduce (&dblklocnbr, &dblkglbnbr, 1, GNUM_MPI, MPI_SUM, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderCblkDist2: communication error");
    return (1);
  }

  *dblklocptr = dblklocnbr;
  *dblkglbptr = dblkglbnbr;

  return (0);
}

Gnum
dorderCblkDist (
const Dorder * restrict const ordeptr)
{
  Gnum                dblklocnbr;
  Gnum                dblkglbnbr;

  if (dorderCblkDist2 (ordeptr, &dblklocnbr, &dblkglbnbr) != 0)
    return (1);

  return (dblkglbnbr);
}

/* This function returns on all of the procesors the
** distributed part of the distributed structure of
** the given distributed ordering. The two array
** pointers which must be passed should both point to
** arrays of size dorderCblkDist(). Tree node indices
** are based, while the father index of fatherless
** nodes (tree roots) is always -1.
** It returns:
** - 0   : if the distributed tree structure could be computed.
** - !0  : on error.
*/

int
dorderTreeDist (
const Dorder * restrict const ordeptr,
Gnum * restrict const         treeglbtab,
Gnum * restrict const         sizeglbtab)
{
  const DorderLink * restrict linklocptr;
  Gnum * restrict             dataloctab;
  Gnum * restrict             dataglbtab;
  Gnum                        dblklocnum;
  Gnum                        dblklocnbr;         /* Local number of distributed column blocks  */
  Gnum                        dblkglbnbr;         /* Global number of distributed column blocks */
  Gnum                        dblkglbnum;
  Gnum                        dbl1glbnum;
  Gnum                        dbl2glbnum;
  int * restrict              dblkcnttab;
  int * restrict              dblkdsptab;
  int                         dblkdspidx;
  int *                       cblkdsptab;         /* TRICK: continues dblkcnttab [norestrict]   */
  Gnum                        cblkglbtmp;
  Gnum * restrict             srt1glbtab;
  Gnum * restrict             srt2glbtab;
  Gnum * restrict             sizeglbtax;         /* Based access to sizeglbtab                 */
  Gnum * restrict             treeglbtax;         /* Based access to treeglbtab                 */
  int                         procglbnbr;
  int                         procnum;
  Gnum                        reduloctab[3];
  Gnum                        reduglbtab[3];

  const Gnum                        baseval = ordeptr->baseval;

  if (dorderCblkDist2 (ordeptr, &dblklocnbr, &dblkglbnbr) != 0)
    return (1);

  MPI_Comm_size (ordeptr->proccomm, &procglbnbr);

  reduloctab[0] =
  reduloctab[1] =
  reduloctab[2] = 0;
  if (memAllocGroup ((void **) (void *)
                     &dblkcnttab, (size_t) ( procglbnbr      * sizeof (int)),
                     &dblkdsptab, (size_t) ( procglbnbr      * sizeof (int)), /* TRICK: cblkdsptab used as secondary array after cblkcnttab */
                     &cblkdsptab, (size_t) ((procglbnbr + 1) * sizeof (int)), /* TRICK: have an array at least of size 2                    */
                     &dataloctab, (size_t) ( dblklocnbr * 4  * sizeof (Gnum)),
                     &dataglbtab, (size_t) ( dblkglbnbr * 4  * sizeof (Gnum)),
                     &srt1glbtab, (size_t) ( dblkglbnbr * 2  * sizeof (Gnum)), /* TRICK: one more slot for root node                  */
                     &srt2glbtab, (size_t) ( dblkglbnbr * 2  * sizeof (Gnum)), NULL) == NULL) { /* TRICK: one more slot for root node */
    errorPrint ("dorderTreeDist: out of memory");
    reduloctab[0] = 1;                            /* Memory error */
  }
  else {
    if (treeglbtab != NULL)
      reduloctab[1] = 1;                          /* Compute the "or" of any array being non-null */
    if (sizeglbtab != NULL) {
      reduloctab[2] = reduloctab[1];              /* Compute the "and" of any array being non-null */
      reduloctab[1] = 1;
    }
  }
#ifdef SCOTCH_DEBUG_DORDER1                       /* Communication cannot be merged with a useful one */
  if (MPI_Allreduce (reduloctab, reduglbtab, 3, GNUM_MPI, MPI_SUM, ordeptr->proccomm) != MPI_SUCCESS) {
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
    if (dblkcnttab != NULL)
      memFree (dblkcnttab);                       /* Free group leader */
    return (1);
  }

  cblkdsptab[0] = (int) dblklocnbr;               /* MPI only supports int as count type     */
  cblkdsptab[1] = (int) ordeptr->cblklocnbr;      /* TRICK: cblkdsptab is at least of size 2 */
  if (MPI_Allgather (cblkdsptab, 2, MPI_INT, dblkcnttab, 2, MPI_INT, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (2)");
    return (1);
  }
  for (procnum = cblkglbtmp = 0; procnum < procglbnbr; procnum ++) { /* Accumulate un-based global start indices for all column blocks */
    cblkdsptab[procnum] = cblkglbtmp;
    dblkcnttab[procnum] = dblkcnttab[2 * procnum] * 4; /* Four times for dataloctab */
    cblkglbtmp         += dblkcnttab[2 * procnum + 1];
  }
  for (procnum = dblkdspidx = 0; procnum < procglbnbr; procnum ++) { /* Accumulate un-based global start indices for distributed column blocks */
    dblkdsptab[procnum] = dblkdspidx;
    dblkdspidx         += dblkcnttab[procnum];
  }

  for (linklocptr = ordeptr->linkdat.nextptr, dblklocnum = 0; /* For all nodes in local ordering structure */
       linklocptr != &ordeptr->linkdat; linklocptr = linklocptr->nextptr) {
    const DorderCblk * restrict cblklocptr;

    cblklocptr = (DorderCblk *) linklocptr;       /* TRICK: FIRST                    */
    if (cblklocptr->cblknum.proclocnum == ordeptr->proclocnum) { /* If node is local */
      dataloctab[4 * dblklocnum]     = cblkdsptab[ordeptr->proclocnum] + cblklocptr->cblknum.cblklocnum; /* Global index of column block      */
      dataloctab[4 * dblklocnum + 1] = cblklocptr->ordeglbval; /* Start inverse permutation index for column block                            */
      dataloctab[4 * dblklocnum + 2] = cblkdsptab[cblklocptr->fathnum.proclocnum] + cblklocptr->fathnum.cblklocnum; /* Global index of father */
      dataloctab[4 * dblklocnum + 3] = cblklocptr->vnodglbnbr; /* Size of column block subtree */
      dblklocnum ++;
    }
  }
  if (MPI_Allgatherv (dataloctab, 4 * dblklocnbr, GNUM_MPI, dataglbtab, dblkcnttab, dblkdsptab, GNUM_MPI, ordeptr->proccomm) != MPI_SUCCESS) {
    errorPrint ("dorderTreeDist: communication error (3)");
    return (1);
  }

  for (dbl1glbnum = 0; dbl1glbnum < dblkglbnbr; dbl1glbnum ++) {
    srt1glbtab[2 * dbl1glbnum]     = dataglbtab[4 * dbl1glbnum + 1]; /* Start inverse permutation index for column block */
    srt1glbtab[2 * dbl1glbnum + 1] = dataglbtab[4 * dbl1glbnum]; /* Global index of column block                         */
  }
  intSort2asc2 (srt1glbtab, dblkglbnbr);          /* Sort nodes by ascending inverse start index to get permutation of column block indices */
  for (dbl1glbnum = 0; dbl1glbnum < dblkglbnbr; dbl1glbnum ++) {
    srt1glbtab[2 * dbl1glbnum]     = srt1glbtab[2 * dbl1glbnum + 1]; /* Global index of column block */
    srt1glbtab[2 * dbl1glbnum + 1] = dbl1glbnum + baseval; /* Base global column block numbers       */
  }
  intSort2asc2 (srt1glbtab, dblkglbnbr);          /* Sort nodes by ascending column block index to match with the ones of dataglbtab */

  for (dbl2glbnum = 0; dbl2glbnum < dblkglbnbr; dbl2glbnum ++) {
    srt2glbtab[2 * dbl2glbnum]     = dataglbtab[4 * dbl2glbnum + 2]; /* Global index of father */
    srt2glbtab[2 * dbl2glbnum + 1] = dbl2glbnum;  /* Index of column block data                */
  }
  intSort2asc2 (srt2glbtab, dblkglbnbr);          /* Sort father indices by ascending column block indices */
#ifdef SCOTCH_DEBUG_DORDER2
  if (srt2glbtab[0] != -1) {                      /* If tree has no root */
    errorPrint ("dorderTreeDist: internal error (1)");
    memFree    (dblkcnttab);                      /* Free group leader */
    return (1);
  }
  if ((dblkglbnbr > 1) && (srt2glbtab[2] == -1)) { /* If tree has multiple roots */
    errorPrint ("dorderTreeDist: internal error (2)");
    memFree    (dblkcnttab);                      /* Free group leader */
    return (1);
  }
#endif /* SCOTCH_DEBUG_DORDER2 */
  for (dbl2glbnum = 1, dbl1glbnum = 0; dbl2glbnum < dblkglbnbr; ) { /* Replace in block data the father column block indices by the new permuted indices */
    if (srt2glbtab[2 * dbl2glbnum] == srt1glbtab[2 * dbl1glbnum])
      dataglbtab[4 * srt2glbtab[2 * (dbl2glbnum ++) + 1] + 2] = srt1glbtab[2 * dbl1glbnum + 1]; /* Propagate based global column block numbers */
    else {
#ifdef SCOTCH_DEBUG_DORDER2
      if ((srt2glbtab[2 * dbl2glbnum] < srt1glbtab[2 * dbl1glbnum]) || /* If column block index not found in table */
          (dbl1glbnum >= (dblkglbnbr - 1))) {
        errorPrint ("dorderTreeDist: internal error (3)");
        memFree    (dblkcnttab);                  /* Free group leader */
        return (1);
      }
#endif /* SCOTCH_DEBUG_DORDER2 */
      dbl1glbnum ++;
    }
  }

  for (dbl2glbnum = 0; dbl2glbnum < dblkglbnbr; dbl2glbnum ++) {
    srt2glbtab[2 * dbl2glbnum]     = dataglbtab[4 * dbl2glbnum]; /* Global index of column block */
    srt2glbtab[2 * dbl2glbnum + 1] = dbl2glbnum;  /* Index of column block data                  */
  }
  intSort2asc2 (srt2glbtab, dblkglbnbr);          /* Sort father indices by ascending column block indices */
  treeglbtax = treeglbtab - baseval;
  sizeglbtax = sizeglbtab - baseval;
  for (dblkglbnum = 0; dblkglbnum < dblkglbnbr; dblkglbnum ++) {
#ifdef SCOTCH_DEBUG_DORDER2
    if (srt1glbtab[2 * dblkglbnum] != srt2glbtab[2 * dblkglbnum]) {
      errorPrint ("dorderTreeDist: internal error (4)");
      memFree    (dblkcnttab);                    /* Free group leader */
      return (1);
    }
#endif /* SCOTCH_DEBUG_DORDER2 */
    treeglbtax[srt1glbtab[2 * dblkglbnum + 1]] = dataglbtab[4 * srt2glbtab[2 * dblkglbnum + 1] + 2]; /* Based column block numbers   */
    sizeglbtax[srt1glbtab[2 * dblkglbnum + 1]] = dataglbtab[4 * srt2glbtab[2 * dblkglbnum + 1] + 3]; /* Size of column block subtree */
  }

  memFree (dblkcnttab);                           /* Free group leader */

  return (0);
}
