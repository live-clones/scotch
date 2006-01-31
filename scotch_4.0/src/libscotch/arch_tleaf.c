/* Copyright INRIA 2004
**
** This file is part of the Scotch distribution.
**
** The Scotch distribution is libre/free software; you can
** redistribute it and/or modify it under the terms of the
** GNU Lesser General Public License as published by the
** Free Software Foundation; either version 2.1 of the
** License, or (at your option) any later version.
**
** The Scotch distribution is distributed in the hope that
** it will be useful, but WITHOUT ANY WARRANTY; without even
** the implied warranty of MERCHANTABILITY or FITNESS FOR A
** PARTICULAR PURPOSE. See the GNU Lesser General Public
** License for more details.
**
** You should have received a copy of the GNU Lesser General
** Public License along with the Scotch distribution; if not,
** write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
**
** $Id$
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

  if ((intLoad (stream, &archptr->leafdep) +
       intLoad (stream, &archptr->clusdep) +
       intLoad (stream, &archptr->linkval) != 3) ||
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
