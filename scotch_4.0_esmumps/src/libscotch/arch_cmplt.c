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
/**   NAME       : target_cm.c                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the complete graph  **/
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
/**                                 to     13 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 09 jan 2004     **/
/**                                 to     10 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH_CMPLT

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_cmplt.h"

/******************************************/
/*                                        */
/* These are the complete graph routines. */
/*                                        */
/******************************************/

/* This routine loads the complete
** graph architecture.
** It returns:
** - 0   : if the architecture has been successfully read.
** - !0  : on error.
*/

int
archCmpltArchLoad (
ArchCmplt * restrict const  archptr,
FILE * restrict const       stream)
{
  long                numnbr;

#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchCmplt)    > sizeof (ArchDummy)) ||
      (sizeof (ArchCmpltDom) > sizeof (ArchDomDummy))) {
    errorPrint ("archCmpltArchLoad: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if ((fscanf (stream, "%ld", &numnbr) != 1) ||
      (numnbr < 1)) {
    errorPrint ("archCmpltArchLoad: bad input");
    return     (1);
  }
  archptr->numnbr = (Anum) numnbr;

  return (0);
}

/* This routine saves the
** complete graph architecture.
** It returns:
** - 0   : if the architecture has been successfully written.
** - !0  : on error.
*/

int
archCmpltArchSave (
const ArchCmplt * const     archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchCmplt)    > sizeof (ArchDummy)) ||
      (sizeof (ArchCmpltDom) > sizeof (ArchDomDummy))) {
    errorPrint ("archCmpltArchSave: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if (fprintf (stream, "%ld ", (long) archptr->numnbr) == EOF) {
    errorPrint ("archCmpltArchSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function returns the smallest number
** of terminal domain included in the given
** domain.
*/

ArchDomNum
archCmpltDomNum (
const ArchCmplt * const     archptr,
const ArchCmpltDom * const  domptr)
{
  return (domptr->nummin);                        /* Return vertex number */
}

/* This function returns the terminal domain associated
** with the given terminal number in the architecture.
** It returns:
** - 0  : if label is valid and domain has been updated.
** - 1  : if label is invalid.
** - 2  : on error.
*/

int
archCmpltDomTerm (
const ArchCmplt * const     archptr,
ArchCmpltDom * const        domptr,
const ArchDomNum            domnum)
{
  if (domnum < archptr->numnbr) {                 /* If valid label */
    domptr->nummin = domnum;                      /* Set the domain */
    domptr->numnbr = 1;

    return (0);
  }

  return (1);                                     /* Cannot set domain */
}

/* This function returns the number of
** elements in the complete domain.
*/

Anum 
archCmpltDomSize (
const ArchCmplt * const     archptr,
const ArchCmpltDom * const  domptr)
{
  return (domptr->numnbr);
}

/* This function returns the average
** distance between two complete
** subdomains.
*/

Anum 
archCmpltDomDist (
const ArchCmplt * const     archptr,
const ArchCmpltDom * const  dom0ptr,
const ArchCmpltDom * const  dom1ptr)
{
  return (((dom0ptr->nummin == dom1ptr->nummin) && /* All domains are at distance 1 */
           (dom0ptr->numnbr == dom1ptr->numnbr)) ? 0 : 1); /* If they are different */
}

/* This function sets the biggest
** domain available for this
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archCmpltDomFrst (
const ArchCmplt * const         archptr,
ArchCmpltDom * restrict const   domptr)
{
  domptr->nummin = 0;
  domptr->numnbr = archptr->numnbr;

  return (0);
}

/* This routine reads domain information
** from the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archCmpltDomLoad (
const ArchCmplt * const       archptr,
ArchCmpltDom * restrict const domptr,
FILE * const                  stream)
{
  long                nummin;
  long                numnbr;

  if ((fscanf (stream, "%ld%ld",
               &nummin,
               &numnbr) != 2) ||
      (numnbr < 1)            ||
      (numnbr + nummin > (long) archptr->numnbr)) {
    errorPrint ("archCmpltDomLoad: bad input");
    return     (1);
  }
  domptr->nummin = (Anum) nummin;
  domptr->numnbr = (Anum) numnbr;

  return (0);
}

/* This routine saves domain information
** to the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archCmpltDomSave (
const ArchCmplt * const     archptr,
const ArchCmpltDom * const  domptr,
FILE * const                stream)
{
  if (fprintf (stream, "%ld %ld ",
               (long) domptr->nummin,
               (long) domptr->numnbr) == EOF) {
    errorPrint ("archCmpltDomSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function tries to split a complete
** graph domain into two subdomains.
** It returns:
** - 0  : if bipartitioning succeeded.
** - 1  : if bipartitioning could not be performed.
** - 2  : on error.
*/

int
archCmpltDomBipart (
const ArchCmplt * const       archptr,
const ArchCmpltDom * const    domptr,
ArchCmpltDom * restrict const dom0ptr,
ArchCmpltDom * restrict const dom1ptr)
{
  if (domptr->numnbr <= 1)                        /* Return if cannot bipartition more */
    return (1);

  dom0ptr->nummin = domptr->nummin;               /* Bipartition vertices */
  dom0ptr->numnbr = domptr->numnbr / 2;
  dom1ptr->nummin = domptr->nummin + dom0ptr->numnbr;
  dom1ptr->numnbr = domptr->numnbr - dom0ptr->numnbr;

  return (0);
}
