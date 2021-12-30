/* Copyright 2008-2010,2012,2018,2019 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : parmetis_dgraph_part.c                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the compatibility        **/
/**                library for the ParMeTiS ordering       **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 5.1  : from : 19 jun 2008     **/
/**                                 to   : 30 jun 2010     **/
/**                # Version 6.0  : from : 13 sep 2012     **/
/**                                 to   : 18 may 2019     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
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

/* This routine converts an array of doubles
** into an array of proportionate integers.
** It returns:
** - void  : in all cases.
*/

#define EPSILON 1e-6F

static
void
_SCOTCH_ParMETIS_floatToInt (
const SCOTCH_Num            valunbr,
const float * const         flottab,
SCOTCH_Num * const          intetab)
{
  float               flotadj;
  SCOTCH_Num          i;

  for (i = 0, flotadj = 1.0; i < valunbr; i ++) {
    float               flotval;
    float               flottmp;

    flotval  = flottab[i];
    flotval *= flotadj;                         /* See if renormalization factor works        */
    flottmp  = flotval - floor (flotval + EPSILON); /* Determine its possible fractional part */
    if (fabs (flottmp) >= EPSILON) {            /* If a residual fractional part exists       */
      flottmp = flotadj / flottmp;              /* Incorporate it in renormalization factor   */
      flotadj = (flotadj * flottmp) / (float) intGcd ((SCOTCH_Num) round (flotadj), (SCOTCH_Num) round (flottmp));
    }
  }
  for (i = 0; i < valunbr; i ++)
    intetab[i] = (SCOTCH_Num) round (flottab[i] * flotadj);
}

int
SCOTCH_ParMETIS_V3_PartKway (
const SCOTCH_Num * const    vtxdist,
SCOTCH_Num * const          xadj,
SCOTCH_Num * const          adjncy,
SCOTCH_Num * const          vwgt,
SCOTCH_Num * const          adjwgt,
const SCOTCH_Num * const    wgtflag,
const SCOTCH_Num * const    numflag,
const SCOTCH_Num * const    ncon,                 /* Not used */
const SCOTCH_Num * const    nparts,
const float * const         tpwgts,
const float * const         ubvec,                /* Not used */
const SCOTCH_Num * const    options,              /* Not used */
SCOTCH_Num * const          edgecut,
SCOTCH_Num * const          part,
MPI_Comm *                  commptr)
{
  MPI_Comm            proccomm;
  int                 procglbnbr;
  int                 proclocnum;
  SCOTCH_Num          baseval;
  SCOTCH_Arch         archdat;
  SCOTCH_Dgraph       grafdat;                    /* Scotch distributed graph object to interface with libScotch   */
  SCOTCH_Dmapping     mappdat;                    /* Scotch distributed mapping object to interface with libScotch */
  SCOTCH_Strat        stradat;
  SCOTCH_Num          vertlocnbr;
  SCOTCH_Num *        veloloctab;
  SCOTCH_Num          edgelocnbr;
  SCOTCH_Num *        edloloctab;
  SCOTCH_Num *        twintab;                    /* Integer array of target weights                               */

  if ((twintab = malloc (*nparts * sizeof (SCOTCH_Num))) == NULL)
    return (METIS_ERROR_MEMORY);
  _SCOTCH_ParMETIS_floatToInt (*nparts, tpwgts, twintab);

  proccomm = *commptr;
  if (SCOTCH_dgraphInit (&grafdat, proccomm) != 0) {
    free   (twintab);
    return (METIS_ERROR);
  }

  MPI_Comm_size (proccomm, &procglbnbr);
  MPI_Comm_rank (proccomm, &proclocnum);
  baseval    = *numflag;
  vertlocnbr = vtxdist[proclocnum + 1] - vtxdist[proclocnum];
  edgelocnbr = xadj[vertlocnbr] - baseval;
  veloloctab = ((vwgt   != NULL) && ((*wgtflag & 2) != 0)) ? vwgt   : NULL;
  edloloctab = ((adjwgt != NULL) && ((*wgtflag & 1) != 0)) ? adjwgt : NULL;

  if (SCOTCH_dgraphBuild (&grafdat, baseval,
                          vertlocnbr, vertlocnbr, xadj, xadj + 1, veloloctab, NULL,
                          edgelocnbr, edgelocnbr, adjncy, NULL, edloloctab) == 0) {
    SCOTCH_stratInit (&stradat);
#ifdef SCOTCH_DEBUG_ALL
    if (SCOTCH_dgraphCheck (&grafdat) == 0)       /* TRICK: next instruction called only if graph is consistent */
#endif /* SCOTCH_DEBUG_ALL */
    {
      SCOTCH_archInit (&archdat);

      if ((SCOTCH_archCmpltw (&archdat, *nparts, twintab) == 0) &&
          (SCOTCH_dgraphMapInit (&grafdat, &mappdat, &archdat, part) == 0)) {
        SCOTCH_dgraphMapCompute (&grafdat, &mappdat, &stradat);

        SCOTCH_dgraphMapExit (&grafdat, &mappdat);
      }
      SCOTCH_archExit (&archdat);
    }
    SCOTCH_stratExit (&stradat);
  }
  SCOTCH_dgraphExit (&grafdat);

  *edgecut = 0;                                   /* TODO : compute real edge cut for people who might want it */

  free (twintab);

  if (baseval != 0) {                             /* MeTiS part array is based, Scotch is not */
    SCOTCH_Num          vertlocnum;

    for (vertlocnum = 0; vertlocnum < vertlocnbr; vertlocnum ++)
      part[vertlocnum] += baseval;
  }

  return (METIS_OK);
}

/*
**
*/

int
SCOTCH_ParMETIS_V3_PartGeomKway (
const SCOTCH_Num * const    vtxdist,
SCOTCH_Num * const          xadj,
SCOTCH_Num * const          adjncy,
SCOTCH_Num * const          vwgt,
SCOTCH_Num * const          adjwgt,
const SCOTCH_Num * const    wgtflag,
const SCOTCH_Num * const    numflag,
const SCOTCH_Num * const    ndims,                /* Not used */
const float * const         xyz,                  /* Not used */
const SCOTCH_Num * const    ncon,                 /* Not used */
const SCOTCH_Num * const    nparts,
const float * const         tpwgts,
const float * const         ubvec,
const SCOTCH_Num * const    options,              /* Not used */
SCOTCH_Num * const          edgecut,
SCOTCH_Num * const          part,
MPI_Comm *                  commptr)
{
  return (SCOTCH_ParMETIS_V3_PartKway (vtxdist, xadj, adjncy, vwgt, adjwgt, wgtflag, numflag, ncon, nparts, tpwgts, ubvec, options, edgecut, part, commptr));
}

/**********************/
/*                    */
/* ParMeTiS v3 stubs. */
/*                    */
/**********************/

#if (SCOTCH_PARMETIS_VERSION == 3)
#ifndef SCOTCH_METIS_PREFIX                       /* With "SCOTCH_" prefix, names already defined */

int
METISNAMEU (ParMETIS_V3_PartKway) (
const SCOTCH_Num * const    vtxdist,
SCOTCH_Num * const          xadj,
SCOTCH_Num * const          adjncy,
SCOTCH_Num * const          vwgt,
SCOTCH_Num * const          adjwgt,
const SCOTCH_Num * const    wgtflag,
const SCOTCH_Num * const    numflag,
const SCOTCH_Num * const    ncon,                 /* Not used */
const SCOTCH_Num * const    nparts,
const float * const         tpwgts,
const float * const         ubvec,                /* Not used */
const SCOTCH_Num * const    options,              /* Not used */
SCOTCH_Num * const          edgecut,
SCOTCH_Num * const          part,
MPI_Comm *                  commptr)
{
  return (SCOTCH_ParMETIS_V3_PartKway (vtxdist, xadj, adjncy, vwgt, adjwgt, wgtflag, numflag,
                                       ncon, nparts, tpwgts, ubvec, options, edgecut, part, commptr));
}

/*
**
*/

int
METISNAMEU (ParMETIS_V3_PartGeomKway) (
const SCOTCH_Num * const    vtxdist,
SCOTCH_Num * const          xadj,
SCOTCH_Num * const          adjncy,
SCOTCH_Num * const          vwgt,
SCOTCH_Num * const          adjwgt,
const SCOTCH_Num * const    wgtflag,
const SCOTCH_Num * const    numflag,
const SCOTCH_Num * const    ndims,                /* Not used */
const float * const         xyz,                  /* Not used */
const SCOTCH_Num * const    ncon,                 /* Not used */
const SCOTCH_Num * const    nparts,
const float * const         tpwgts,
const float * const         ubvec,
const SCOTCH_Num * const    options,              /* Not used */
SCOTCH_Num * const          edgecut,
SCOTCH_Num * const          part,
MPI_Comm *                  commptr)
{
  return (SCOTCH_ParMETIS_V3_PartGeomKway (vtxdist, xadj, adjncy, vwgt, adjwgt, wgtflag, numflag,
                                           ndims, xyz, ncon, nparts, tpwgts, ubvec, options, edgecut, part, commptr));
}

#endif /* SCOTCH_METIS_PREFIX */
#endif /* (SCOTCH_PARMETIS_VERSION == 3) */
