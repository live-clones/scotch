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
/**   NAME       : target_vc.c                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the variable-sized  **/
/**                complete graph target architecture.     **/
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
/**                                 to     16 aug 1995     **/
/**                # Version 3.1  : from : 20 jul 1996     **/
/**                                 to     20 jul 1996     **/
/**                # Version 3.2  : from : 15 oct 1996     **/
/**                                 to     14 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 3.4  : from : 14 sep 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 05 nov 2003     **/
/**                                 to     05 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH_VCMPLT

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_vcmplt.h"

/*****************************************/
/*                                       */
/* These are the variable-sized complete */
/* graph handling routines.              */
/*                                       */
/*****************************************/

/* This function returns the smallest number
** of terminal domain included in the given
** domain.
*/

ArchDomNum
archVcmpltDomNum (
const ArchVcmplt * const    archptr,
const ArchVcmpltDom * const domptr)
{
  return (domptr->termnum);                       /* Return terminal number */
}

/* This function returns the terminal domain associated
** with the given terminal number in the architecture.
** It returns:
** - 0  : if label is valid and domain has been updated.
** - 1  : if label is invalid.
** - 2  : on error.
*/

int
archVcmpltDomTerm (
const ArchVcmplt * const    archptr,
ArchVcmpltDom * const       domptr,
const ArchDomNum            domnum)
{
  if (domnum != ARCHDOMNOTTERM) {                 /* If valid label */
    domptr->termnum = domnum;                     /* Set the domain */
    return (0);
  }

  return (1);                                     /* Cannot set domain */
}

/* This function returns the number of
** elements in the domain.
*/

Anum 
archVcmpltDomSize (
const ArchVcmplt * const    archptr,
const ArchVcmpltDom * const domptr)
{
  return (1);                                     /* All domains have same size for bipartitioning */
}

/* This function returns the average
** distance between two subdomains.
*/

Anum 
archVcmpltDomDist (
const ArchVcmplt * const    archptr,
const ArchVcmpltDom * const dom0ptr,
const ArchVcmpltDom * const dom1ptr)
{
  return ((dom0ptr->termnum == dom1ptr->termnum) ? 0 : 1); /* All distinct terminals are at distance 1 */
}

/* This function sets the biggest
** domain available for this
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archVcmpltDomFrst (
const ArchVcmplt * const        archptr,
ArchVcmpltDom * restrict const  domptr)
{
  domptr->termnum = 1;                            /* First terminal number */

  return (0);
}

/* This routine reads domain information
** from the given stream.
** It returns:
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archVcmpltDomLoad (
const ArchVcmplt * const        archptr,
ArchVcmpltDom * restrict const  domptr,
FILE * const                    stream)
{
  if (intLoad (stream, &domptr->termnum) != 1) {
    errorPrint ("archVcmpltDomLoad: bad input");
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
archVcmpltDomSave (
const ArchVcmplt * const    archptr,
const ArchVcmpltDom * const domptr,
FILE * const                stream)
{
  if (fprintf (stream, "%ld ",
               (long) domptr->termnum) == EOF) {
    errorPrint ("archVcmpltDomSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function splits a domain
** into two subdomains.
** It returns:
** - 0  : if bipartitioning succeeded.
** - 2  : on error.
*/

int
archVcmpltDomBipart (
const ArchVcmplt * const        archptr,
const ArchVcmpltDom * const     domptr,
ArchVcmpltDom * restrict const  dom0ptr,
ArchVcmpltDom * restrict const  dom1ptr)
{
  dom0ptr->termnum = domptr->termnum << 1;
  dom1ptr->termnum = dom0ptr->termnum + 1;

  return ((dom1ptr->termnum < domptr->termnum) ? 2 : 0); /* Return error on overflow */
}
