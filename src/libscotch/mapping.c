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
/**   NAME       : mapping.c                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles (partial) mappings. **/
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
/**                                 to     30 mar 1999     **/
/**                # Version 3.4  : from : 11 sep 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 16 jan 2004     **/
/**                                 to     05 jan 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MAPPING

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"

/***********************************/
/*                                 */
/* These routines handle mappings. */
/*                                 */
/***********************************/

/* This routine builds a mapping.
** It returns:
** - 0   : if mapping successfully initialized.
** - !0  : on error.
*/

int
mapInit (
Mapping * restrict const      mappptr,
const Gnum                    baseval,
const Gnum                    vertnbr,
const Arch * restrict const   archptr,
Gnum * restrict const         parttab)
{
  ArchDom             domfrst;                    /* First, largest, domain          */
  Anum                domnmax;                    /* Maximum number of domains       */
  Gnum * restrict     parttmp;                    /* Temporary pointer to part array */

  archDomFrst (archptr, &domfrst);                /* Get first domain of target architecture */

  if (strncmp (archName (archptr), "var", 3) == 0) /* If target architecture is variable-sized */
    domnmax = (vertnbr > 1024) ? 1024 : vertnbr;  /* Pre-set number of domains                 */
  else {                                          /* Else if fixed architecture                */
    domnmax = archDomSize (archptr, &domfrst);    /* Get architecture size                     */

#ifdef SCOTCH_DEBUG_MAP2
    if (domnmax <= 0) {
      errorPrint ("mapInit: target architecture must have at least one domain");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_MAP2 */
  }

  mappptr->flagval = MAPNONE;
  mappptr->baseval = baseval;
  mappptr->vertnbr = vertnbr;
  mappptr->domnmax = domnmax + 1;                 /* +1 for empty domain in mapLoad */
  mappptr->domnnbr = 1;                           /* One domain in mapping to date  */
  mappptr->archdat = *archptr;

  if ((mappptr->domntab = (ArchDom *) memAlloc ((domnmax + 1) * sizeof (ArchDom))) == NULL) {
    errorPrint ("mapInit: out of memory (1)");
    return (1);
  }
  mappptr->domntab[0] = domfrst;                  /* Set first domain */

  parttmp = parttab;
  if (parttab == NULL) {
    mappptr->flagval |= MAPFREEPART;
    if ((parttmp = (Gnum *) memAlloc (vertnbr * sizeof (Gnum))) == NULL) {
      errorPrint ("mapInit: out of memory (2)");
      return     (1);
    }
  }
  mappptr->parttax = parttmp - baseval;
  memSet (parttmp, 0, vertnbr * sizeof (Anum));   /* All vertices mapped to first domain */

  return (0);
}

/* This routine frees the contents
** of the given mapping.
** It returns:
** - VOID  : in all cases.
*/

void
mapExit (
Mapping * const             mappptr)
{
  if ((mappptr->parttax != NULL) && ((mappptr->flagval & MAPFREEPART) != 0))
    memFree (mappptr->parttax + mappptr->baseval);
  if (mappptr->domntab != NULL)
    memFree (mappptr->domntab);

#ifdef SCOTCH_DEBUG_MAP2
  memSet (mappptr, ~0, sizeof (Mapping));
#endif /* SCOTCH_DEBUG_MAP2 */
}
