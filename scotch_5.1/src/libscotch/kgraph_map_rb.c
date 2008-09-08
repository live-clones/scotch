/* Copyright 2004,2007,2008 ENSEIRB, INRIA & CNRS
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
/**   NAME       : kgraph_map_rb.c                         **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module performs the Dual Recursive **/
/**                Bipartitioning mapping algorithm.       **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 31 mar 1993     **/
/**                                 to     31 mar 1993     **/
/**                # Version 1.0  : from : 04 oct 1993     **/
/**                                 to     06 oct 1993     **/
/**                # Version 1.1  : from : 15 oct 1993     **/
/**                                 to     15 oct 1993     **/
/**                # Version 1.3  : from : 09 apr 1994     **/
/**                                 to     11 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     17 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to     18 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     19 oct 1995     **/
/**                # Version 3.1  : from : 30 oct 1995     **/
/**                                 to     14 jun 1996     **/
/**                # Version 3.2  : from : 23 aug 1996     **/
/**                                 to     07 sep 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     08 dec 1998     **/
/**                # Version 3.4  : from : 01 jun 2001     **/
/**                                 to     07 nov 2001     **/
/**                # Version 4.0  : from : 12 jan 2004     **/
/**                                 to     06 mar 2005     **/
/**                # Version 5.1  : from : 22 nov 2007     **/
/**                                 to     25 jun 2008     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define KGRAPH_MAP_RB

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "bgraph.h"
#include "bgraph_bipart_st.h"
#include "kgraph.h"
#include "kgraph_map_rb.h"

/*
**  The static variables.
*/

static KgraphMapRbPoolLink  kgraphmaprbpooldummy; /* Dummy links for pool routines; TRICK */

/************************************/
/*                                  */
/* These routines handle job pools. */
/*                                  */
/************************************/

/* This routine initializes the contents
** of the given pool.
** It returns:
** - VOID  : in all cases.
*/

static
void
kgraphMapRbPoolInit (
KgraphMapRbPool * restrict const  poolptr,
KgraphMapRbPolicy                 polival)
{
  poolptr->poollink.prev = 
  poolptr->poollink.next = &kgraphmaprbpooldummy;
  poolptr->polival       = polival;
}

/* This routine frees the contents of
** the given job pool.
** It returns:
** - VOID  : in all cases.
*/

static
void
kgraphMapRbPoolExit (
KgraphMapRbPool * restrict const  poolptr)
{
  KgraphMapRbJob *    jobptr;

  for (jobptr  = (KgraphMapRbJob *) poolptr->poollink.next; /* For all jobs in pool */
       jobptr != (KgraphMapRbJob *) &kgraphmaprbpooldummy;
       jobptr  = (KgraphMapRbJob *) jobptr->poollink.next)
    graphFree (&jobptr->grafdat);                 /* Free job graph, if not clone of original graph */
}

/* This routine adds a job to the given pool.
** It returns:
** - VOID  : in all cases.
*/

static
void
kgraphMapRbPoolAdd (
KgraphMapRbPool * const     poolptr,
KgraphMapRbJob * const      jobptr)
{
  jobptr->poollink.prev        = &poolptr->poollink; /* Link job in pool: TRICK */
  jobptr->poollink.next        = poolptr->poollink.next;
  jobptr->poolflag             = 1;               /* Job is in pool    */
  jobptr->poolptr              = poolptr;         /* Point to the pool */
  poolptr->poollink.next->prev = &jobptr->poollink;
  poolptr->poollink.next       = &jobptr->poollink;
}

/* This routine deletes a job from the given pool.
** It returns:
** - VOID  : in all cases.
*/

static
void
kgraphMapRbPoolDel (
KgraphMapRbPool * const     poolptr,
KgraphMapRbJob * const      jobptr)
{
  jobptr->poolflag            = 0;                /* Job is no longer in pool         */
  jobptr->poollink.next->prev = jobptr->poollink.prev; /* Remove job from pool; TRICK */
  jobptr->poollink.prev->next = jobptr->poollink.next;
}

/* This routine gets the best job
** available from the given pool,
** according to the given policy.
** It returns:
** - !NULL  : pointer to the job.
** - NULL   : if the pool is empty.
*/

static
KgraphMapRbJob *
kgraphMapRbPoolGet (
KgraphMapRbPool * const     poolptr)
{
  KgraphMapRbJob *    jobbest;                    /* Best job found */
  KgraphMapRbJob *    jobptr;

  jobbest = (KgraphMapRbJob *) poolptr->poollink.next;  /* Get first job in pool */
  for (jobptr  = jobbest;                         /* For all jobs in pool        */
       jobptr != (KgraphMapRbJob *) &kgraphmaprbpooldummy;
       jobptr  = (KgraphMapRbJob *) jobptr->poollink.next) {
    if (jobptr->priolvl > jobbest->priolvl)       /* If the current job has stronger priority */
      jobbest = jobptr;                           /* Select it as the best job                */
  }

  if (jobbest != (KgraphMapRbJob *) &kgraphmaprbpooldummy) /* If job found        */
    kgraphMapRbPoolDel (poolptr, jobbest);        /* Remove it from pool          */
  else                                            /* Dummy job means no job found */
    jobbest = NULL;

  return (jobbest);
}

/* This routine adds a job to the given pool
** as the first bipartitioning job.
** It returns:
** - VOID  : in all cases.
*/

static
void
kgraphMapRbPoolNew (
KgraphMapRbPool * const     poolptr,              /* New pool        */
KgraphMapRbJob * const      jobptr)               /* Job to be added */

{
  switch (poolptr->polival) {                     /* Set job priority value */
    case KGRAPHMAPRBPOLIRANDOM :
      jobptr->prioval =
      jobptr->priolvl = intRandVal (INTVALMAX);
      break;
    case KGRAPHMAPRBPOLILEVEL   :
    case KGRAPHMAPRBPOLINGLEVEL :
      jobptr->prioval = jobptr->grafdat.vertnbr;
      jobptr->priolvl = 0;
      break;
    case KGRAPHMAPRBPOLISIZE   :
    case KGRAPHMAPRBPOLINGSIZE :
      jobptr->prioval =
      jobptr->priolvl = jobptr->grafdat.vertnbr;
      break;
    case KGRAPHMAPRBPOLIOLD :
      jobptr->prioval = 0;
      jobptr->priolvl = 1;
      break;
#ifdef SCOTCH_DEBUG_KGRAPH2
    default :
      errorPrint ("kgraphMapRbPoolNew: unknown job selection policy");
      jobptr->prioval = 0;
      jobptr->priolvl = 0;
      return;
#endif /* SCOTCH_DEBUG_KGRAPH2 */
  }

  kgraphMapRbPoolAdd (poolptr, jobptr);           /* Add job to pool */
}

/* This routine updates the given job
** table with the given job data.
** This routine can be called only if
** the parent jobs of the vertices to
** be updated still exist.
** It returns:
** - VOID  : in all cases.
*/

static
void
kgraphMapRbPoolUpdt (
KgraphMapRbPool * const           poolptr,              /* Current pool                     */
const Graph * restrict const      srcgrafptr,           /* Pointer to original source graph */
const Mapping * const             mapptr,               /* Current mapping                  */
KgraphMapRbJob * restrict const   joboldptr,            /* Job to be removed                */
KgraphMapRbJob * restrict const   jobnewptr,            /* Job to be added                  */
KgraphMapRbJob * const            jobtab)               /* Job table                        */
{
  const Anum * restrict   mapparttax;             /* Based pointer to part array      */
  const Graph * restrict  jobgrafptr;             /* Pointer to job source graph      */
  Gnum                    jobvertnum;             /* Number of current vertex         */
  KgraphMapRbJob *        joblistptr;             /* List of jobs unlinked from pools */
  Gnum                    jobpriolvl;             /* New job priority level           */

  switch (poolptr->polival) {                     /* Set job priority value */
    case KGRAPHMAPRBPOLIRANDOM :
      jobnewptr->prioval =
      jobnewptr->priolvl = intRandVal (INTVALMAX);
      break;
    case KGRAPHMAPRBPOLILEVEL :
      jobnewptr->priolvl = joboldptr->priolvl + 1;
    case KGRAPHMAPRBPOLINGLEVEL :
      jobnewptr->prioval = joboldptr->prioval - 1;
      break;
    case KGRAPHMAPRBPOLISIZE :
      jobnewptr->priolvl = jobnewptr->grafdat.vertnbr;
    case KGRAPHMAPRBPOLINGSIZE :
      jobnewptr->prioval = jobnewptr->grafdat.vertnbr;
      break;
    case KGRAPHMAPRBPOLIOLD :
      jobnewptr->priolvl = (jobnewptr->grafdat.vnumtax == NULL) /* If new graph is original graph */
                           ? joboldptr->priolvl   /* Take same priority as before                 */
                           : ((joboldptr->priolvl ^ 1) << 1) + /* Else compute it "old-style"     */
                             ((joboldptr->grafdat.vnumtax == NULL)
                              ? ((jobnewptr->grafdat.vnumtax[0] == 0) ? 0 : 1)
                              : ((jobnewptr->grafdat.vnumtax[0] == joboldptr->grafdat.vnumtax[0]) ? 0 : 1));
      break;
#ifdef SCOTCH_DEBUG_KGRAPH2
    default :
      errorPrint ("kgraphMapRbPoolUpdt: unknown job selection policy");
      jobnewptr->prioval = 0;
      jobnewptr->priolvl = 0;
      return;
#endif /* SCOTCH_DEBUG_KGRAPH2 */
  }

  if (poolptr->polival >= KGRAPHMAPRBPOLINEIGHBOR) { /* If neighbors have to be updated */
    mapparttax = mapptr->parttax;
    jobgrafptr = &jobnewptr->grafdat;             /* Point to job source graph          */
    joblistptr = NULL;                            /* No temporarily unlinked jobs yet   */
    jobpriolvl = 0;                               /* Priority level not yet set         */

    for (jobvertnum = jobgrafptr->baseval; jobvertnum < jobgrafptr->vertnnd; jobvertnum ++) {
      Gnum                srcvertnum;             /* Source graph vertex number */
      const Gnum *        srcedgeptr;             /* Pointer to current edge    */

      srcvertnum = (jobgrafptr->vnumtax == NULL) ? jobvertnum : jobgrafptr->vnumtax[jobvertnum];

      if ((srcgrafptr->vendtax[srcvertnum] - srcgrafptr->verttax[srcvertnum]) == /* If vertex is internal, skip it */
          (jobgrafptr->vendtax[jobvertnum] - jobgrafptr->verttax[jobvertnum]))
        continue;

      for (srcedgeptr = srcgrafptr->edgetax + srcgrafptr->verttax[srcvertnum];
           srcedgeptr < srcgrafptr->edgetax + srcgrafptr->vendtax[srcvertnum];
           srcedgeptr ++) {
        KgraphMapRbJob * restrict jobnghbptr;     /* (Old ?) job of neighbor vertex */

        jobnghbptr = &jobtab[mapparttax[*srcedgeptr]]; /* Get pointer to neighboring job */

        if ((jobnghbptr->poolflag != 0)                 && /* If neighbor in active job          */
            (jobnghbptr->prioval >  jobnewptr->prioval) && /* Over which we have gained priority */
            (jobnghbptr->prioval <= joboldptr->prioval)) {
          jobnghbptr->priolvl ++;                 /* Update neighbor priority */
        }
        if ((jobnghbptr->poolflag == 0) ||        /* If neighbor is fully known           */
            (jobnghbptr->prioval < jobnewptr->prioval)) /* Or has stronger priority value */
          jobpriolvl ++;                          /* Then we should be processed          */
      }
    }
    jobnewptr->priolvl = jobpriolvl;              /* Then we should be processed */
  }

  kgraphMapRbPoolAdd (poolptr, jobnewptr);        /* Add job to pool */
}

/*
** This routine removes the vertices of the
** given job from the given job table.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

static
void
kgraphMapRbPoolRemv (
KgraphMapRbPool * const       poolptr,            /* Current pool                     */
const Graph * restrict const  srcgrafptr,         /* Pointer to original source graph */
Mapping * const               mapptr,             /* Current mapping                  */
KgraphMapRbJob * const        joboldptr,          /* Job to be removed                */
GraphPart * const             joboldparttax,      /* Job part array                   */
const GraphPart               joboldpartnum,      /* Number of part to remove         */
KgraphMapRbJob * const        jobtab)             /* Job table                        */
{
  const Anum * restrict   mapparttax;             /* Based pointer to part array           */
  const Graph * restrict  jobgrafptr;             /* Pointer to job source graph           */
  const Gnum * restrict   srcedgetax;             /* Pointer to edge array of source graph */
  Gnum                    jobvertnum;             /* Number of current vertex              */
  KgraphMapRbJob *        joblistptr;             /* List of jobs unlinked from pools      */

  if (poolptr->polival >= KGRAPHMAPRBPOLINEIGHBOR) { /* If neighbors have to be modified */
    mapparttax = mapptr->parttax;
    srcedgetax = srcgrafptr->edgetax;             /* Point to edge array of source graph */
    jobgrafptr = &joboldptr->grafdat;             /* Point to job source graph           */
    joblistptr = NULL;                            /* No temporarily unlinked jobs yet    */

    for (jobvertnum = jobgrafptr->baseval; jobvertnum < jobgrafptr->vertnnd; jobvertnum ++) {
      Gnum                srcvertnum;             /* Source graph vertex number */
      Gnum                srcedgenum;             /* Source graph edge number   */

      if (joboldparttax[jobvertnum] != joboldpartnum) /* If vertex is not in the right part */
        continue;

      srcvertnum = (jobgrafptr->vnumtax == NULL) ? jobvertnum : jobgrafptr->vnumtax[jobvertnum];

      for (srcedgenum = srcgrafptr->verttax[srcvertnum]; srcedgenum < srcgrafptr->vendtax[srcvertnum]; srcedgenum ++) {
        KgraphMapRbJob *    jobnghbptr;           /* (Old ?) job of neighbor vertex */

        jobnghbptr = &jobtab[mapparttax[srcedgetax[srcedgenum]]]; /* Get pointer to neighboring job */

        if ((jobnghbptr->poolflag != 0) &&        /* If neighbor is in active job          */
            (jobnghbptr->prioval < joboldptr->prioval)) { /* Which had a stronger priority */
          jobnghbptr->priolvl ++;                 /* Update neighbor priority              */
        }
      }
    }
  }
}

/********************************************/
/*                                          */
/* This is the entry point for the Dual     */
/* Recursive Bipartitioning mapping method. */
/*                                          */
/********************************************/

/* This routine runs the Dual Recursive
** Bipartitioning algorithm.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
kgraphMapRb (
Kgraph * restrict const                 grafptr,
const KgraphMapRbParam * restrict const paraptr)
{
  ArchDom            domnsub[2];                 /* Initial subdomains                                    */
  Anum               jobnbr;                     /* Current number of jobs in job table                   */
  Anum               jobmax;                     /* Maximum number of jobs in table (i.e. size)           */
  KgraphMapRbJob *   jobtab;                     /* Job table                                             */
  KgraphMapRbJob *   joborgptr;                  /* Pointer to current job                                */
  KgraphMapRbJob     joborgdat;                  /* Aera to save original job data                        */
  Anum               jobsubnum[2];               /* Number of subjob slots in job array                   */
  Gnum               jobsubsiz[2];               /* Sizes of subjobs                                      */
  KgraphMapRbPool    pooldat[2];                 /* Job pools                                             */
  KgraphMapRbPool *  poolptr[2];                 /* Pointers to job pools                                 */
  KgraphMapRbPool *  pooltmp;                    /* Temporary variable for pool swapping                  */
  Mapping            mapdat;                     /* Other mapping used by pools                           */
  Mapping *          mapptr[2];                  /* Pointers to previous and current mappings             */
  Mapping *          maptmp;                     /* Temporary pointer for mapping swapping                */
  Bgraph             actgraph;                   /* Active bipartitioning graph                           */
  Gnum               actvertnum;                 /* Current vertex number                                 */
  Graph *            srcgrafptr;                 /* Pointer to source graph if mapping distances required */
  int                cocyflag;                   /* Flag set if cocycle data relevant                     */
  int                varsflag;                   /* Flag set if variable-sized architecture               */
  int                i;

  grafptr->m.domnnbr    = 1;                      /* Only one valid domain to date */
  grafptr->m.domntab[0] = grafptr->m.domnorg;     /* All vertices are mapped to it */
  memSet (grafptr->m.parttax + grafptr->m.baseval, 0, grafptr->s.vertnbr * sizeof (ArchDomNum)); /* Initialize partition data */

  switch (archDomBipart (&grafptr->m.archdat, &grafptr->m.domnorg, &domnsub[0], &domnsub[1])) {
    case 1 :                                      /* If nothing to bipartition */
      return (0);                                 /* Return without error      */
    case 2 :                                      /* On error                  */
      errorPrint ("kgraphMapRb: cannot bipartition domain (1)");
      return     (1);
  }

  cocyflag = 1;                                   /* Assume cocycle data are relevant                                      */
  if (strstr (archName (&grafptr->m.archdat), "cmplt") != NULL) /* If target architecture is some flavor of complete graph */
    cocyflag = 0;                                 /* Do not account for cocycle data                                       */
  varsflag = 0;
  if (strncmp (archName (&grafptr->m.archdat), "var", 3) == 0) /* If target architecture is variable sized */
    varsflag = 1;
  srcgrafptr = (cocyflag == 0) ? NULL : &grafptr->s; /* If no need for external data */

#ifdef SCOTCH_DEBUG_KGRAPH2
  jobmax = 1;                                     /* Set minimum size in order to test resizing */
#else /* SCOTCH_DEBUG_KGRAPH2 */
  jobmax = grafptr->m.domnmax;                    /* Set size of job array */
#endif /* SCOTCH_DEBUG_KGRAPH2 */
  if ((jobtab = (KgraphMapRbJob *) memAlloc (jobmax * sizeof (KgraphMapRbJob))) == NULL) {
    errorPrint ("kgraphMapRb: out of memory (1)");
    return     (1);
  }

  kgraphMapRbPoolInit (&pooldat[0], (cocyflag == 0) ? KGRAPHMAPRBPOLILEVEL : paraptr->poli); /* Initialize first pool (done first for success of kgraphMapRbExit()) */

  poolptr[0] = &pooldat[0];                       /* Point to first pool  */
  if ((paraptr->flagjobtie != 0) || (cocyflag == 0)) /* If pools are tied */
    poolptr[1] = poolptr[0];                      /* They are the same    */
  else {
    kgraphMapRbPoolInit (&pooldat[1], (cocyflag == 0) ? KGRAPHMAPRBPOLILEVEL : paraptr->poli); /* Allocate another pool */
    poolptr[1] = &pooldat[1];
  }

  mapptr[0] =                                     /* First mapping is graph mapping */
  mapptr[1] = &grafptr->m;                        /* Assume mappings are tied       */
  if ((paraptr->flagmaptie == 0) && (cocyflag == 1)) { /* If mappings are not tied  */
    mapptr[1]      = &mapdat;                     /* Point to second mapping        */

    mapdat.flagval = MAPNONE;                     /* Partition data is shared */
    mapdat.baseval = grafptr->s.baseval;
    mapdat.vertnbr = grafptr->s.vertnbr;
    mapdat.parttax = grafptr->m.parttax;          /* Partition data is shared       */
    mapdat.domnmax = grafptr->m.domnmax;          /* Will not be used anyway        */
    mapdat.domnnbr = 1;                           /* Will not be used anyway        */
    mapdat.domntab = NULL;                        /* In case first allocation fails */
    if ((mapdat.domntab = (ArchDom *) memAlloc (grafptr->m.domnmax * sizeof (ArchDom))) == NULL) {
      errorPrint      ("kgraphMapRb: out of memory (2)");
      kgraphMapRbExit (1, jobtab, poolptr, mapptr, grafptr);
      return          (1);
    }
    mapdat.domntab[0] = grafptr->m.domnorg;       /* Initialize partition data */
  }

  jobnbr = 1;                                     /* One job in table yet */
  jobtab[0].domorg    = grafptr->m.domnorg;       /* Build first job      */
  jobtab[0].domsub[0] = domnsub[0];
  jobtab[0].domsub[1] = domnsub[1];
  jobtab[0].grafdat   = grafptr->s;               /* Clone original graph as first job graph */
  jobtab[0].grafdat.flagval &= ~GRAPHFREETABS;    /* Do not free its arrays on exit          */
  kgraphMapRbPoolNew (poolptr[0], &jobtab[0]);    /* Add initial job                         */

  while (! kgraphMapRbPoolEmpty (poolptr[0])) {   /* For all non-empty pools                */
    while ((joborgptr = kgraphMapRbPoolGet (poolptr[0])) != NULL) { /* For all jobs in pool */
      Anum * restrict     mapparttax;             /* Part array of mapping to be updated    */

      joborgdat = *joborgptr;                     /* Save current job data (clone graph)    */

      if ((bgraphInit (&actgraph, &joborgdat.grafdat, /* Create active graph */
                       srcgrafptr, mapptr[0], joborgdat.domsub) != 0) ||
          (bgraphBipartSt (&actgraph, paraptr->strat) != 0)) { /* Perform bipartitioning */
        errorPrint      ("kgraphMapRb: cannot bipartition job");
        kgraphMapRbExit (jobnbr, jobtab, poolptr, mapptr, grafptr);
        return          (1);
      }

      jobsubnum[0] = joborgptr - jobtab;          /* Get current (and first son) job slot number (before possible move of pointers) */
      jobsubsiz[0] = actgraph.compsize0;
      if (jobnbr == jobmax) {                     /* If all job slots busy                        */
        if (kgraphMapRbResize (&jobmax, &jobtab, poolptr, mapptr) != 0) { /* And if cannot resize */
          errorPrint      ("kgraphMapRb: cannot resize structures");
          kgraphMapRbExit (jobnbr, jobtab, poolptr, mapptr, grafptr);
          return          (1);
        }
      }
      jobsubnum[1] = jobnbr ++;                   /* Get slot number of new subdomain */
      jobsubsiz[1] = actgraph.s.vertnbr - actgraph.compsize0;
      mapptr[0]->domntab[jobsubnum[1]] = grafptr->m.domnorg; /* Copy original domain to new subdomain as old mapping shares parttax with new */

      mapparttax = mapptr[1]->parttax;
      if (mapptr[1] == mapptr[0]) {               /* If mappings are tied, update new fraction */
        if (actgraph.s.vnumtax != NULL) {
          for (actvertnum = actgraph.s.baseval; actvertnum < actgraph.s.vertnnd; actvertnum ++)
            if (actgraph.parttax[actvertnum] == 1)
              mapparttax[actgraph.s.vnumtax[actvertnum]] = jobsubnum[1];
        }
        else {
          for (actvertnum = actgraph.s.baseval; actvertnum < actgraph.s.vertnnd; actvertnum ++)
            if (actgraph.parttax[actvertnum] == 1)
              mapparttax[actvertnum] = jobsubnum[1];
        }
      }
      else {
        if (actgraph.s.vnumtax != NULL) {
          for (actvertnum = actgraph.s.baseval; actvertnum < actgraph.s.vertnnd; actvertnum ++)
            mapparttax[actgraph.s.vnumtax[actvertnum]] = jobsubnum[actgraph.parttax[actvertnum]];
        }
        else {
          for (actvertnum = 0; actvertnum < actgraph.s.vertnbr; actvertnum ++)
            mapparttax[actvertnum] = jobsubnum[actgraph.parttax[actvertnum]];
        }
      }

      if (varsflag == 0) {
        for (i = 1; i >= 0; i --) {               /* For both subdomains, in inverse order */
          if (jobsubsiz[i] > 0) {                 /* If subdomain is not empty             */
            jobtab[jobsubnum[i]].poolflag    = 0; /* Activate job slot                     */
            jobtab[jobsubnum[i]].domorg      =
            mapptr[1]->domntab[jobsubnum[i]] = joborgdat.domsub[i]; /* Update terminal domain array */

            switch (archDomBipart (&grafptr->m.archdat, &jobtab[jobsubnum[i]].domorg,
                                   &jobtab[jobsubnum[i]].domsub[0],
                                   &jobtab[jobsubnum[i]].domsub[1])) {
              case 0 :                            /* Add re-shaped job to other pool */
                graphInducePart (&actgraph.s, actgraph.parttax, jobsubsiz[i], (GraphPart) i, &jobtab[jobsubnum[i]].grafdat);
                kgraphMapRbPoolUpdt (poolptr[1], &grafptr->s, mapptr[1], &joborgdat, &jobtab[jobsubnum[i]], jobtab);
                break;
              case 1 :                            /* New domain is terminal */
                kgraphMapRbPoolRemv (poolptr[1], &grafptr->s, mapptr[1], &joborgdat, actgraph.parttax, (GraphPart) i, jobtab);
#ifdef SCOTCH_DEBUG_KGRAPH2
                jobtab[jobsubnum[i]].poollink.prev = /* Prevent Valgrind from yelling when doing kgraphMapRbResize() */
                jobtab[jobsubnum[i]].poollink.next = NULL;
#endif /* SCOTCH_DEBUG_KGRAPH2 */
                break;
#ifdef SCOTCH_DEBUG_KGRAPH2
              case 2 :                            /* On error */
                errorPrint      ("kgraphMapRb: cannot bipartition domain (2)");
                kgraphMapRbExit (jobnbr, jobtab, poolptr, mapptr, grafptr);
                return          (1);
#endif /* SCOTCH_DEBUG_KGRAPH2 */
            }
          }
          else {                                  /* Subdomain is empty  */
            jobtab[jobsubnum[i]].poolflag = 1;    /* Inactivate job slot */
            jobtab[jobsubnum[i]].poollink.prev = &kgraphmaprbpooldummy; /* Will not be considered in resizing */
            archDomTerm (&grafptr->m.archdat, &mapptr[1]->domntab[jobsubnum[i]], archDomNum (&grafptr->m.archdat, &joborgdat.domsub[i])); /* set terminal domain */
          }
        }
      }
      else {                                      /* If variable-sized architecture        */
        for (i = 1; i >= 0; i --) {               /* For both subdomains, in inverse order */
          mapptr[1]->domntab[jobsubnum[i]] = joborgdat.domsub[i]; /* Update domain array   */
          if ((jobsubsiz[i] > 1) && (jobsubsiz[1 - i] > 0)) { /* If worth going on         */
            jobtab[jobsubnum[i]].domorg      =
            mapptr[1]->domntab[jobsubnum[i]] = joborgdat.domsub[i]; /* Update terminal domain array */

            switch (archDomBipart (&grafptr->m.archdat, &jobtab[jobsubnum[i]].domorg,
                                   &jobtab[jobsubnum[i]].domsub[0],
                                   &jobtab[jobsubnum[i]].domsub[1])) {
              case 0 :                            /* Add re-shaped job to other pool */
                graphInducePart (&actgraph.s, actgraph.parttax, jobsubsiz[i], (GraphPart) i, &jobtab[jobsubnum[i]].grafdat);
                kgraphMapRbPoolUpdt (poolptr[1], &grafptr->s, mapptr[1], &joborgdat, &jobtab[jobsubnum[i]], jobtab);
                break;
              default :                           /* If domain is terminal or on error */
                errorPrint      ("kgraphMapRb: cannot bipartition domain (2)");
                kgraphMapRbExit (jobnbr, jobtab, poolptr, mapptr, grafptr);
                return          (1);
            }
          }
          else {                                  /* No use going on further */
            kgraphMapRbPoolRemv (poolptr[1], &grafptr->s, mapptr[1], &joborgdat, actgraph.parttax, (GraphPart) i, jobtab);
#ifdef SCOTCH_DEBUG_KGRAPH2
            jobtab[jobsubnum[i]].poollink.prev = /* Prevent Valgrind from yelling when doing kgraphMapRbResize() */
            jobtab[jobsubnum[i]].poollink.next = NULL;
#endif /* SCOTCH_DEBUG_KGRAPH2 */
          }
        }
      }

      bgraphExit (&actgraph);                     /* Free active graph data   */
      graphExit  (&joborgdat.grafdat);            /* Free (cloned) graph data */
    }

    maptmp     = mapptr[0];                       /* Swap current and next levels */
    mapptr[0]  = mapptr[1];
    mapptr[1]  = maptmp;
    pooltmp    = poolptr[0];
    poolptr[0] = poolptr[1];
    poolptr[1] = pooltmp;
  }

  kgraphMapRbExit (jobnbr, jobtab, poolptr, mapptr, grafptr); /* Free internal structures  */
  grafptr->m.domnmax = jobmax;                    /* Set maximum number of domains */
  grafptr->m.domnnbr = jobnbr;                    /* Set number of domains         */

  return (0);
}

/**********************************************/
/*                                            */
/* These routines handle internal structures. */
/*                                            */
/**********************************************/

/* This routine frees all of the internal arrays
** involved in the DRB algorithms. Great care
** should be taken that this routine always
** succeeds, whatever part of the algorithm it
** is called from.
** It returns:
** - VOID  : in all cases.
*/

static
void
kgraphMapRbExit (
const Anum                  jobnbr,               /* Current number of jobs/domains */
KgraphMapRbJob * const      jobtab,               /* Pointer to job table           */
KgraphMapRbPool ** const    poolptr,              /* Pointer to pool pointer array  */
Mapping ** const            maptab,               /* Pointers to mapping array      */
Kgraph * restrict const     grafptr)              /* Pointer to graph               */
{
  kgraphMapRbPoolExit (poolptr[0]);               /* Always free pool contents */
  if (poolptr[1] != poolptr[0])
    kgraphMapRbPoolExit (poolptr[1]);

  memFree (jobtab);                               /* Always free job array */

  if (maptab[0] != maptab[1]) {                   /* If mappings not tied         */
    if (maptab[0] != &grafptr->m) {               /* If graph mapping out of date */
      memCpy  (grafptr->m.domntab, maptab[0]->domntab, jobnbr * sizeof (ArchDom));
      memFree (maptab[0]->domntab);               /* Free mapping data */
    }
    else {                                        /* Graph mapping is up to date */
      if (maptab[1]->domntab != NULL)             /* Free other mapping data     */
        memFree (maptab[1]->domntab);    
    }
  }
}

/* This routine doubles the size all of the arrays
** involved in handling the target architecture,
** to make room for new domains of variable-sized
** architectures.
** It returns:
** - 0   : if resize succeeded.
** - !0  : if out of memory.
*/

static
int
kgraphMapRbResize (
Anum * const                jobmax,               /* Pointer to maximum number of jobs */
KgraphMapRbJob ** const     jobptr,               /* Pointer to job table pointer      */
KgraphMapRbPool ** const    poolptr,              /* Pointer to job pool array         */
Mapping * restrict * const  mapptr)               /* Pointers to mapping array         */
{
  KgraphMapRbJob * restrict joboldtab;            /* Pointer to old job array */

  joboldtab = *jobptr;                            /* Save old job table address */
  if ((*jobptr = (KgraphMapRbJob *) memRealloc (joboldtab, 2 * *jobmax * sizeof (KgraphMapRbJob))) == NULL) {
    errorPrint ("kgraphMapRbResize: out of memory (1)");
    *jobptr = joboldtab;
    return (1);
  }

  if (*jobptr != joboldtab) {                     /* If job array moved              */
    KgraphMapRbJob *          joboldtnd;          /* Pointer to end of old job array */
    Anum                      jobnum;             /* Temporary job index             */
    size_t                    jobdlt;             /* Address delta value             */

    joboldtnd = joboldtab + *jobmax;
    jobdlt = (byte *) *jobptr - (byte *) joboldtab; /* Compute delta between addresses */

    for (jobnum = 0; jobnum < *jobmax; jobnum ++) {
      if (((*jobptr)[jobnum].poollink.prev >= (KgraphMapRbPoolLink *) joboldtab) && /* If old pointers within bounds of old array, adjust them */
          ((*jobptr)[jobnum].poollink.prev <  (KgraphMapRbPoolLink *) joboldtnd))
        (*jobptr)[jobnum].poollink.prev = (KgraphMapRbPoolLink *) ((byte *) (*jobptr)[jobnum].poollink.prev + jobdlt);
      if (((*jobptr)[jobnum].poollink.next >= (KgraphMapRbPoolLink *) joboldtab) &&
          ((*jobptr)[jobnum].poollink.next <  (KgraphMapRbPoolLink *) joboldtnd))
        (*jobptr)[jobnum].poollink.next = (KgraphMapRbPoolLink *) ((byte *) (*jobptr)[jobnum].poollink.next + jobdlt);
    }
    if (poolptr[0]->poollink.next != &kgraphmaprbpooldummy) /* Update first pool pointer */
      poolptr[0]->poollink.next = (KgraphMapRbPoolLink *) ((byte *) poolptr[0]->poollink.next + jobdlt);
    if (poolptr[0] != poolptr[1]) {         /* If job pools not tied                        */
      if (poolptr[1]->poollink.next != &kgraphmaprbpooldummy) /* Update second pool pointer */
        poolptr[1]->poollink.next = (KgraphMapRbPoolLink *) ((byte *) poolptr[1]->poollink.next + jobdlt);
    }
  }

  *jobmax *= 2;                                   /* Double job slot limit */

  if (mapptr[0]->domnmax < *jobmax) {             /* If need to increase size of domain array */
    ArchDom *                 domtmp;             /* Temporary pointer to domain array        */

    domtmp = mapptr[0]->domntab;
    if ((mapptr[0]->domntab = (ArchDom *) memRealloc (mapptr[0]->domntab, *jobmax * sizeof (ArchDom))) == NULL) {
      errorPrint ("kgraphMapRbResize: out of memory (2)");
      mapptr[0]->domntab = domtmp;
      return (1);
    }
    mapptr[0]->domnmax = *jobmax;
  }
  if ((mapptr[1]->domntab != mapptr[0]->domntab) && /* If mappings not tied                    */
      (mapptr[1]->domnmax < *jobmax)) {           /* and need to increase size of domain array */
    ArchDom *                 domtmp;             /* Temporary pointer to domain array         */

    domtmp = mapptr[1]->domntab;
    if ((mapptr[1]->domntab = (ArchDom *) memRealloc (mapptr[1]->domntab, *jobmax * sizeof (ArchDom))) == NULL) {
      errorPrint ("kgraphMapRbResize: out of memory (3)");
      mapptr[1]->domntab = domtmp;
      return (1);
    }
    mapptr[1]->domnmax = *jobmax;
  }

  return (0);
}
