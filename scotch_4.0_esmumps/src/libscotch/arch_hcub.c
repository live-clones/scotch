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
/**   NAME       : arch_hcub.c                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the hypercube       **/
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
/**                # Version 3.1  : from : 11 jun 1996     **/
/**                                 to     11 jun 1996     **/
/**                # Version 3.2  : from : 21 sep 1996     **/
/**                                 to     14 may 1998     **/
/**                # Version 4.0  : from : 11 nov 2003     **/
/**                                 to     10 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH_HCUB

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_hcub.h"

/********************************************/
/*                                          */
/* These are the binary hypercube routines. */
/*                                          */
/********************************************/

/* This routine loads the binary
** hypercube architecture.
** It returns:
** - 0   : if the architecture has been successfully read.
** - !0  : on error.
*/

int
archHcubArchLoad (
ArchHcub * restrict const   archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchHcub)    > sizeof (ArchDummy)) ||
      (sizeof (ArchHcubDom) > sizeof (ArchDomDummy))) {
    errorPrint ("archHcubArchLoad: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if ((intLoad (stream, &archptr->dimmax) != 1) ||
      (archptr->dimmax < 1)                     ||
      (archptr->dimmax > (sizeof (archptr->dimmax) << 3))) {
    errorPrint ("archHcubArchLoad: bad input");
    return     (1);
  }

  return (0);
}

/* This routine saves the
** binary hypercube architecture.
** It returns:
** - 0   : if the architecture has been successfully written.
** - !0  : on error.
*/

int
archHcubArchSave (
const ArchHcub * const      archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchHcub)    > sizeof (ArchDummy)) ||
      (sizeof (ArchHcubDom) > sizeof (ArchDomDummy))) {
    errorPrint ("archHcubArchSave: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if (fprintf (stream, "%ld ", (long) archptr->dimmax) == EOF) {
    errorPrint ("archHcubArchSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function returns the smallest number
** of terminal domain included in the given
** domain.
*/

ArchDomNum
archHcubDomNum (
const ArchHcub * const      archptr,
const ArchHcubDom * const   domptr)
{
  return (domptr->bitset);                        /* Return vertex number */
}

/* This function returns the terminal domain associated
** with the given terminal number in the architecture.
** It returns:
** - 0  : if label is valid and domain has been updated.
** - 1  : if label is invalid.
** - 2  : on error.
*/

int
archHcubDomTerm (
const ArchHcub * const      archptr,
ArchHcubDom * const         domptr,
const ArchDomNum            domnum)
{
  if (domnum < (1 << archptr->dimmax)) {          /* If valid label */
    domptr->dimcur = 0;                           /* Set the domain */
    domptr->bitset = domnum;

    return (0);
  }

  return (1);                                     /* Cannot set domain */
}

/* This function returns the number of
** elements in the hypercube domain.
*/

Anum 
archHcubDomSize (
const ArchHcub * const      archptr,
const ArchHcubDom * const   domptr)
{
  return (1 << domptr->dimcur);
}

/* This function returns the average distance
** between two sub-hypercubes.
*/

Anum
archHcubDomDist (
const ArchHcub * const      archptr,
const ArchHcubDom * const   dom0ptr,
const ArchHcubDom * const   dom1ptr)
{
  Anum                i, j, k;

  if (dom0ptr->dimcur > dom1ptr->dimcur) {        /* Get smallest set dimension value */
    i = dom0ptr->dimcur;
    j = i - dom1ptr->dimcur;
  }
  else {
    i = dom1ptr->dimcur;
    j = i - dom0ptr->dimcur;
  }
  j /= 2;                                         /* For set/unset bits, assume 1/2 difference */

  for (k = (dom0ptr->bitset ^ dom1ptr->bitset) >> i, i = archptr->dimmax - i;
       i > 0;
       k >>= 1, i --)
    j += (k & 1);                                 /* Add Hamming difference on set dimensions */

  return (j);
}

/* This function sets the biggest
** domain available for this
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archHcubDomFrst (
const ArchHcub * const        archptr,
ArchHcubDom * restrict const  domptr)
{
  domptr->dimcur = archptr->dimmax;
  domptr->bitset = 0;

  return (0);
}

/* This routine reads domain information
** from the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archHcubDomLoad (
const ArchHcub * const        archptr,
ArchHcubDom * restrict const  domptr,
FILE * restrict const         stream)
{
  if ((intLoad (stream, &domptr->dimcur) +
       intLoad (stream, &domptr->bitset) != 2) ||
      (domptr->dimcur > archptr->dimmax)) {
    errorPrint ("archHcubDomLoad: bad input");
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
archHcubDomSave (
const ArchHcub * const      archptr,
const ArchHcubDom * const   domptr,
FILE * restrict const       stream)
{
  if (fprintf (stream, "%ld %ld ",
               (long) domptr->dimcur,
               (long) domptr->bitset) == EOF) {
    errorPrint ("archHcubDomSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function tries to split a hypercube
** domain into two subdomains.
** It returns:
** - 0  : if bipartitioning succeeded.
** - 1  : if bipartitioning could not be performed.
** - 2  : on error.
*/

int
archHcubDomBipart (
const ArchHcub * const        archptr,
const ArchHcubDom * const     domptr,
ArchHcubDom * restrict const  dom0ptr,
ArchHcubDom * restrict const  dom1ptr)
{
  if (domptr->dimcur <= 0)                        /* Return if cannot bipartition more */
    return (1);

  dom0ptr->dimcur =
  dom1ptr->dimcur = domptr->dimcur - 1;
  dom0ptr->bitset = domptr->bitset;
  dom1ptr->bitset = domptr->bitset | (1 << dom1ptr->dimcur);

  return (0);
}
