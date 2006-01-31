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
/**   NAME       : bgraph_store.c                          **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the save data      **/
/**                structure handling routines for bipar-  **/
/**                tition graphs.                          **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 17 oct 1998     **/
/**                                 to     17 oct 1998     **/
/**                # Version 4.0  : from : 18 dec 2001     **/
/**                                 to     16 jun 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BGRAPH_STORE

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "bgraph.h"

/**********************************/
/*                                */
/* Store graph handling routines. */
/*                                */
/**********************************/

/* This routine builds a save structure
** for the given active graph.
** It returns:
** - 0   : if allocation succeeded.
** - !0  : on error.
*/

int
bgraphStoreInit (
const Bgraph * const        grafptr,
BgraphStore * const         storptr)
{
  Gnum                savsize;

  savsize = (grafptr->s.vertnnd - grafptr->s.baseval) * (sizeof (GraphPart) + sizeof (Gnum)); /* Compute size for frontier and part arrays */

  if ((storptr->datatab = (byte *) memAlloc (savsize)) == NULL) { /* Allocate save structure */
    errorPrint ("bgraphStoreInit: out of memory");
    return     (1);
  }

  return (0);
}

/* This routine frees a save structure.
** It returns:
** - VOID  : in all cases.
*/

void
bgraphStoreExit (
BgraphStore * const         storptr)
{
  memFree (storptr->datatab);
#ifdef SCOTCH_DEBUG_BGRAPH2
  storptr->datatab = NULL;
#endif /* SCOTCH_DEBUG_BGRAPH2 */
}

/* This routine saves partition data from the
** given active graph to the given save structure.
** It returns:
** - VOID  : in all cases.
*/

void
bgraphStoreSave (
const Bgraph * const        grafptr,
BgraphStore * const         storptr)
{
  Gnum                vertnbr;                    /* Number of vertices in graph        */
  byte *              parttab;                    /* Pointer to part data save area     */
  byte *              frontab;                    /* Pointer to frontier data save area */

  storptr->fronnbr      = grafptr->fronnbr;       /* Save partition parameters */
  storptr->compload0dlt = grafptr->compload0dlt;
  storptr->compsize0    = grafptr->compsize0;
  storptr->commload     = grafptr->commload;
  storptr->commgainextn = grafptr->commgainextn;

  frontab = storptr->datatab;                     /* Compute data offsets within save structure */
  parttab = frontab + grafptr->fronnbr * sizeof (Gnum);

  vertnbr = grafptr->s.vertnnd - grafptr->s.baseval;
  memCpy (frontab, grafptr->frontab, grafptr->fronnbr * sizeof (Gnum));
  memCpy (parttab, grafptr->parttax + grafptr->s.baseval, vertnbr * sizeof (GraphPart));
}

/* This routine updates partition data of the
** given active graph, using the given save graph.
** It returns:
** - VOID  : in all cases.
*/

void
bgraphStoreUpdt (
Bgraph * const              grafptr,
const BgraphStore * const   storptr)
{
  Gnum                vertnbr;                    /* Number of vertices in graph        */
  byte *              frontab;                    /* Pointer to frontier data save area */
  byte *              parttab;                    /* Pointer to part data save area     */

  grafptr->fronnbr      = storptr->fronnbr;       /* Load partition parameters */
  grafptr->compload0    = storptr->compload0dlt + grafptr->compload0avg;
  grafptr->compload0dlt = storptr->compload0dlt;
  grafptr->compsize0    = storptr->compsize0;
  grafptr->commload     = storptr->commload;
  grafptr->commgainextn = storptr->commgainextn;

  frontab = storptr->datatab;                     /* Compute data offsets within save structure */
  parttab = frontab + grafptr->fronnbr * sizeof (Gnum);

  vertnbr = grafptr->s.vertnnd - grafptr->s.baseval;
  memCpy (grafptr->frontab, frontab, grafptr->fronnbr * sizeof (Gnum));
  memCpy (grafptr->parttax + grafptr->s.baseval, parttab, vertnbr * sizeof (GraphPart));

#ifdef SCOTCH_DEBUG_BGRAPH2
  if (bgraphCheck (grafptr) != 0)
    errorPrint ("bgraphStoreUpdt: inconsistent graph data");
#endif /* SCOTCH_DEBUG_BGRAPH2 */
}
