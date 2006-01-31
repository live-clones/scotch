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
/**   NAME       : arch_vhcub.c                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the variable-sized  **/
/**                hypercube target architecture.          **/
/**                                                        **/
/**   DATES      : # Version 3.4  : from : 08 nov 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 04 nov 2003     **/
/**                                 to     04 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH_VHCUB

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_vhcub.h"

/********************************/
/*                              */
/* These are the variable-sized */
/* hypercube handling routines. */
/*                              */
/********************************/

/* This function returns the smallest number
** of terminal domain included in the given
** domain.
*/

ArchDomNum
archVhcubDomNum (
const ArchVhcub * const     archptr,
const ArchVhcubDom * const  domptr)
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
archVhcubDomTerm (
const ArchVhcub * const     archptr,
ArchVhcubDom * const        domptr,
const ArchDomNum            domnum)
{
  Anum                termnum;
  Anum                termlvl;

  if (domnum != ARCHDOMNOTTERM) {                 /* If valid label   */
    domptr->termnum = domnum;                     /* Set the domain   */
    for (termnum = domnum, termlvl = 0; termnum > 1; /* Compute level */
         termnum >>= 1, termlvl ++) ;
    domptr->termlvl = termnum;                    /* Set level */

    return (0);
  }

  return (1);                                     /* Cannot set domain */
}

/* This function returns the number of
** elements in the domain.
*/

Anum 
archVhcubDomSize (
const ArchVhcub * const     archptr,
const ArchVhcubDom * const  domptr)
{
  return (1);                                     /* All domains have same size for bipartitioning */
}

/* This function returns the average
** distance between two subdomains.
*/

Anum 
archVhcubDomDist (
const ArchVhcub * const     archptr,
const ArchVhcubDom * const  dom0ptr,
const ArchVhcubDom * const  dom1ptr)
{
  Anum                dom0num;
  Anum                dom1num;
  Anum                distval;

  if (dom0ptr->termlvl > dom1ptr->termlvl) {
    dom0num = dom0ptr->termnum >> (dom0ptr->termlvl - dom1ptr->termlvl);
    dom1num = dom1ptr->termnum;
    distval = (dom0ptr->termlvl - dom1ptr->termlvl) >> 1; /* One half of unknown bits */
  }
  else {
    dom0num = dom0ptr->termnum;
    dom1num = dom1ptr->termnum >> (dom1ptr->termlvl - dom0ptr->termlvl);
    distval = (dom1ptr->termlvl - dom0ptr->termlvl) >> 1; /* One half of unknown bits */
  }

  for (dom0num ^= dom1num; dom0num != 0;          /* Compute number of bit differences */
       distval += (dom0num & 1), dom0num >>= 1) ;

  return (distval);
}

/* This function sets the biggest
** domain available for this
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archVhcubDomFrst (
const ArchVhcub * const       archptr,
ArchVhcubDom * restrict const domptr)
{
  domptr->termlvl = 0;                            /* First terminal number */
  domptr->termnum = 1;

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
archVhcubDomLoad (
const ArchVhcub * const       archptr,
ArchVhcubDom * restrict const domptr,
FILE * const                  stream)
{
  if (((intLoad (stream, &domptr->termlvl) +
        intLoad (stream, &domptr->termnum)) != 2)  ||
      (domptr->termlvl < 0)                        ||
      (domptr->termnum <  (1 <<  domptr->termlvl)) ||
      (domptr->termnum >= (1 << (domptr->termlvl + 1)))) {
    errorPrint ("archVhcubDomLoad: bad input");
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
archVhcubDomSave (
const ArchVhcub * const     archptr,
const ArchVhcubDom * const  domptr,
FILE * const                stream)
{
  if (fprintf (stream, "%ld %ld ",
               (long) domptr->termlvl,
               (long) domptr->termnum) == EOF) {
    errorPrint ("archVhcubDomSave: bad output");
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
archVhcubDomBipart (
const ArchVhcub * const       archptr,
const ArchVhcubDom * const    domptr,
ArchVhcubDom * restrict const dom0ptr,
ArchVhcubDom * restrict const dom1ptr)
{
  dom0ptr->termlvl =                              /* Bipartition the domain */
  dom1ptr->termlvl = domptr->termlvl + 1;
  dom0ptr->termnum = domptr->termnum << 1;
  dom1ptr->termnum = dom0ptr->termnum + 1;

  return ((dom1ptr->termnum < domptr->termnum) ? 2 : 0); /* Return error on overflow */
}
