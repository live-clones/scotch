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
/**   NAME       : arch_tleaf.c                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the tree-leaf       **/
/**                target architecture.                    **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 01 dec 1992     **/
/**                                 to   : 24 mar 1993     **/
/**                # Version 1.2  : from : 04 feb 1994     **/
/**                                 to   : 11 feb 1994     **/
/**                # Version 1.3  : from : 20 apr 1994     **/
/**                                 to   : 20 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to   : 23 dec 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to   : 29 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     08 sep 1995     **/
/**                # Version 3.1  : from : 20 jul 1996     **/
/**                                 to     20 jul 1996     **/
/**                # Version 3.2  : from : 10 oct 1996     **/
/**                                 to     14 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 3.4  : from : 07 jun 2001     **/
/**                                 to     07 jun 2001     **/
/**                # Version 4.0  : from : 10 dec 2003     **/
/**                                 to     10 mar 2005     **/
/**                # Version 5.1  : from : 21 jan 2008     **/
/**                                 to     28 feb 2008     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH_TLEAF

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_tleaf.h"

/*******************************************/
/*                                         */
/* These are the tree-leaf graph routines. */
/*                                         */
/*******************************************/

/* This routine loads the
** tree leaf architecture.
** It returns:
** - 0   : if the architecture has been successfully read.
** - !0  : on error.
*/

int
archTleafArchLoad (
ArchTleaf * restrict const  archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchTleaf)    > sizeof (ArchDummy)) ||
      (sizeof (ArchTleafDom) > sizeof (ArchDomDummy))) {
    errorPrint ("archTleafArchLoad: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if ((intLoad (stream, &archptr->leafdep) != 1) ||
      (intLoad (stream, &archptr->clusdep) != 1) ||
      (intLoad (stream, &archptr->linkval) != 1) ||
      (archptr->leafdep < 1)                     ||
      (archptr->clusdep < 0)                     ||
      (archptr->clusdep > archptr->leafdep)      ||
      (archptr->linkval < 1)) {
    errorPrint ("archTleafArchLoad: bad input");
    return     (1);
  }

  return (0);
}

/* This routine saves the
** tree leaf architecture.
** It returns:
** - 0   : if the architecture has been successfully written.
** - !0  : on error.
*/

int
archTleafArchSave (
const ArchTleaf * const     archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchTleaf)    > sizeof (ArchDummy)) ||
      (sizeof (ArchTleafDom) > sizeof (ArchDomDummy))) {
    errorPrint ("archTleafArchSave: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if (fprintf (stream, "%ld %ld %ld ",
               (long) archptr->leafdep,
               (long) archptr->clusdep,
               (long) archptr->linkval) == EOF) {
    errorPrint ("archTleafSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function returns the smallest number
** of terminal domain included in the given
** domain.
*/

ArchDomNum
archTleafDomNum (
const ArchTleaf * const     archptr,
const ArchTleafDom * const  domptr)
{
  return ((domptr->leafnum << (archptr->leafdep - domptr->leaflvl)) - (1 << archptr->leafdep));  /* Return vertex number */
}

/* This function returns the terminal domain associated
** with the given terminal number in the architecture.
** It returns:
** - 0  : if label is valid and domain has been updated.
** - 1  : if label is invalid.
** - 2  : on error.
*/

int
archTleafDomTerm (
const ArchTleaf * const     archptr,
ArchTleafDom * const        domptr,
const ArchDomNum            domnum)
{
  if (domnum < (1 << archptr->leafdep)) {         /* If valid label */
    domptr->leaflvl = archptr->leafdep;           /* Set the domain */
    domptr->leafnum = domnum + (1 << archptr->leafdep);

    return (0);
  }

  return (1);                                     /* Cannot set domain */
}

/* This function returns the number of
** elements in the subtree domain.
*/

Anum 
archTleafDomSize (
const ArchTleaf * const     archptr,
const ArchTleafDom * const  domptr)
{
  return (1 << (archptr->leafdep - domptr->leaflvl));
}

/* This function returns the average
** distance between two tree leaf
** subdomains.
*/

Anum 
archTleafDomDist (
const ArchTleaf * const     archptr,
const ArchTleafDom * const  dom0ptr,
const ArchTleafDom * const  dom1ptr)
{
  Anum                clus0;
  Anum                clus1;
  Anum                dist0;

  if (dom0ptr->leafnum == dom1ptr->leafnum)       /* If same node */
    return (0);

  clus0 = dom0ptr->leafnum >> (dom0ptr->leaflvl - archptr->clusdep);
  clus1 = dom1ptr->leafnum >> (dom1ptr->leaflvl - archptr->clusdep);

  if (clus0 == clus1)                             /* If in same cluster */
    return (1);

  for (dist0 = 0; clus0 != clus1; ) {             /* As long as common ancestor not found */
    if (clus0 > clus1) {
      clus0 >>= 1;
      dist0 ++;                                   /* Increase distance between both clusters and ancestor */
    }
    else
      clus1 >>= 1;
  }
  return (dist0 * archptr->linkval);
}

/* This function sets the biggest
** domain available for this
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archTleafDomFrst (
const ArchTleaf * const       archptr,
ArchTleafDom * restrict const domptr)
{
  domptr->leaflvl = 0;
  domptr->leafnum = 1;

  return (0);
}

/* This routine reads domain information
** from the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archTleafDomLoad (
const ArchTleaf * const       archptr,
ArchTleafDom * restrict const domptr,
FILE * const                  stream)
{
  if (((intLoad (stream, &domptr->leaflvl) +
        intLoad (stream, &domptr->leafnum)) != 2) ||
      (domptr->leaflvl < 1)                       ||
      (domptr->leaflvl > archptr->leafdep)        ||
      (domptr->leafnum >= (1 << archptr->leafdep))) {
    errorPrint ("archTleafDomLoad: bad input");
    return     (1);
  }

  return (0);
}

/* This routine saves domain information
** to the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archTleafDomSave (
const ArchTleaf * const     archptr,
const ArchTleafDom * const  domptr,
FILE * const                stream)
{
  if (fprintf (stream, "%ld %ld ",
               (long) domptr->leaflvl,
               (long) domptr->leafnum) == EOF) {
    errorPrint ("archTleafDomSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function tries to split a tree leaf
** domain into two subdomains.
** It returns:
** - 0  : if bipartitioning succeeded.
** - 1  : if bipartitioning could not be performed.
** - 2  : on error.
*/

int
archTleafDomBipart (
const ArchTleaf * const       archptr,
const ArchTleafDom * const    domptr,
ArchTleafDom * restrict const dom0ptr,
ArchTleafDom * restrict const dom1ptr)
{
  if (domptr->leaflvl >= archptr->leafdep)        /* Return if cannot bipartition more */
    return (1);

  dom0ptr->leaflvl =                              /* Bipartition the domain */
  dom1ptr->leaflvl = domptr->leaflvl + 1;
  dom0ptr->leafnum = domptr->leafnum << 1;
  dom1ptr->leafnum = dom0ptr->leafnum + 1;

  return (0);
}

/* This function creates the MPI_Datatype for
** tree-leaf domains.
** It returns:
** - 0  : if type could be created.
** - 1  : on error.
*/

#ifdef SCOTCH_PTSCOTCH
int
archTleafDomMpiType (
const ArchTleaf * const       archptr,
MPI_Datatype * const          typeptr)
{
  MPI_Type_contiguous (2, ANUM_MPI, typeptr);

  return (0);
}
#endif /* SCOTCH_PTSCOTCH */
