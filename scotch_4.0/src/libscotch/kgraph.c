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
/**   NAME       : map.c                                   **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a bipartitioning mapper.        **/
/**                This module handles the k-way active    **/
/**                graph and save data structure handling  **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 03 oct 1997     **/
/**                                 to     26 may 1998     **/
/**                # Version 3.4  : from : 30 oct 2001     **/
/**                                 to     30 oct 2001     **/
/**                # Version 4.0  : from : 24 jun 2004     **/
/**                                 to     16 feb 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define KGRAPH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "kgraph.h"

/***********************************/
/*                                 */
/* Active graph handling routines. */
/*                                 */
/***********************************/

/* This routine builds the active graph
** corresponding to the given k-way
** partition parameters.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
kgraphInit (
Kgraph * restrict const         actgrafptr,       /* Active graph */
const Graph * restrict const    srcgrafptr,       /* Source graph */
const Mapping * restrict const  mappptr)          /* Mapping      */
{
  const Arch * restrict archptr;                  /* Pointer to mapping architecture */
  ArchDom               domfrst;                  /* First, largest domain           */
  Gnum                  domfrstload;              /* Load of first domain            */
  Anum                  termnum;

  actgrafptr->s          = *srcgrafptr;           /* Clone source graph */
  actgrafptr->s.flagval &= GRAPHFREETABS;
  actgrafptr->m          = *mappptr;              /* Clone mapping */

  if ((actgrafptr->comploadavg = (Gnum *) memAlloc (actgrafptr->m.domnnbr * sizeof (Gnum) * 2)) == NULL) {
    errorPrint ("kgraphInit: out of memory");
    return     (1);
  }
  actgrafptr->comploaddlt = actgrafptr->comploadavg + actgrafptr->m.domnnbr;

  archptr = &mappptr->archdat;
  archDomFrst (archptr, &domfrst);                /* Get first, largest domain */
  domfrstload = archDomWght (archptr, &domfrst);  /* Get its load              */

  actgrafptr->comploadavg[0] = (archDomWght (archptr, &actgrafptr->m.domntab[0]) * actgrafptr->s.velosum) / domfrstload;
  actgrafptr->comploaddlt[0] = actgrafptr->s.velosum - actgrafptr->comploadavg[0];
  for (termnum = 1; termnum < actgrafptr->m.domnnbr; termnum ++) {
    actgrafptr->comploadavg[termnum] = (archDomWght (archptr, &actgrafptr->m.domntab[termnum]) * actgrafptr->s.velosum) / domfrstload;
    actgrafptr->comploaddlt[termnum] = - actgrafptr->comploadavg[termnum];
  }

  actgrafptr->fronnbr  = 0;                       /* No frontier yet */
  actgrafptr->frontab  = NULL;
  actgrafptr->commload = 0;

  return (0);
}

/* This routine frees the contents
** of the given active graph and
** updates the mapping data accordingly.
** It returns:
** - VOID  : in all cases.
*/

void
kgraphExit (
Kgraph * restrict const     actgrafptr,           /* Active graph      */
Mapping * restrict const    mappptr)              /* Mapping to update */
{
  mappptr->domnmax = actgrafptr->m.domnmax;
  mappptr->domnnbr = actgrafptr->m.domnnbr;
  mappptr->domntab = actgrafptr->m.domntab;       /* Update pointer to domntab in case it has changed */

  memFree (actgrafptr->comploadavg);              /* Free load structures       */
  if (actgrafptr->frontab != NULL) {              /* Free frontier if it exists */
    memFree (actgrafptr->frontab);
    actgrafptr->frontab = NULL;
  }                                               /* Do not free the mapping, as it has been cloned */
}
