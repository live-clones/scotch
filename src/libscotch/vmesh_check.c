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
/**   NAME       : vmesh_check.c                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the consistency    **/
/**                checker for separation meshes.          **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 21 mar 2003     **/
/**                                 to     11 may 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VMESH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "vmesh.h"

/*************************/
/*                       */
/* These routines handle */
/* separator meshes.     */
/*                       */
/*************************/

/* This routine checks the consistency
** of the given separator mesh.
** It returns:
** - 0   : if mesh data are consistent.
** - !0  : on error.
*/

int
vmeshCheck (
const Vmesh * const         meshptr)
{
  Gnum                velmnum;                    /* Number of current element vertex  */
  Gnum                vnodnum;                    /* Number of current node vertex     */
  Gnum                fronnum;                    /* Number of current frontier vertex */
  int * restrict      frontax;                    /* Frontier flag array               */
  Gnum                ecmpsize[2];                /* Elements never in separator       */
  Gnum                ncmpsize[3];
  Gnum                ncmpload[3];

  if ((meshptr->ecmpsize[0] + meshptr->ecmpsize[1]) > meshptr->m.velmnbr) {
    errorPrint ("vmeshCheck: invalid element balance");
    return     (1);
  }
  if (meshptr->ncmploaddlt != (meshptr->ncmpload[0] - meshptr->ncmpload[1])) {
    errorPrint ("vmeshCheck: invalid node balance");
    return     (1);
  }

  ecmpsize[0] =
  ecmpsize[1] = 0;
  for (velmnum = meshptr->m.velmbas; velmnum < meshptr->m.velmnnd; velmnum ++) {
    Gnum                edgecut[3];               /* Array of cut edges     */
    Gnum                partnum;                  /* Part of current vertex */
    Gnum                eelmnum;                  /* Number of current edge */

    partnum = meshptr->parttax[velmnum];
    if ((partnum < 0) || (partnum > 1)) {
      errorPrint ("vmeshCheck: invalid part array (1)");
      return     (1);
    }
    ecmpsize[partnum] ++;

    if ((partnum != 0) &&
        (meshptr->m.verttax[velmnum] == meshptr->m.vendtax[velmnum])) {
      errorPrint ("vmeshCheck: isolated element not in part 0");
      return     (1);
    }

    edgecut[0] =
    edgecut[1] =
    edgecut[2] = 0;
    for (eelmnum = meshptr->m.verttax[velmnum];
         eelmnum < meshptr->m.vendtax[velmnum]; eelmnum ++)
      edgecut[meshptr->parttax[meshptr->m.edgetax[eelmnum]]] ++;

    if (partnum == 2) {
      if ((edgecut[0] != 0) || (edgecut[1] != 0)) {
        errorPrint ("vmeshCheck: separator element not surrounded by separator nodes");
        return     (1);
      }
    }
    else {
      if (edgecut[1 - partnum] != 0) {
        errorPrint ("vmeshCheck: element should be in separator (%ld)", (long) velmnum);
        return     (1);
      }
    }
  }
  if ((meshptr->ecmpsize[0] != ecmpsize[0]) ||
      (meshptr->ecmpsize[1] != ecmpsize[1])) {
    errorPrint ("vmeshCheck: invalid element parameters");
    return     (1);
  }

  ncmpload[0] =
  ncmpload[1] =
  ncmpload[2] = 0;
  ncmpsize[0] =
  ncmpsize[1] =
  ncmpsize[2] = 0;
  for (vnodnum = meshptr->m.vnodbas; vnodnum < meshptr->m.vnodnnd; vnodnum ++) {
    Gnum                edgecut[3];               /* Array of cut edges     */
    Gnum                partnum;                  /* Part of current vertex */
    Gnum                enodnum;                  /* Number of current edge */

    partnum = meshptr->parttax[vnodnum];
    if ((partnum < 0) || (partnum > 2)) {
      errorPrint ("vmeshCheck: invalid part array (2)");
      return     (1);
    }

    ncmpsize[partnum] ++;
    ncmpload[partnum] += (meshptr->m.vnlotax == NULL) ? 1 : meshptr->m.vnlotax[vnodnum];

    edgecut[0] =
    edgecut[1] =
    edgecut[2] = 0;
    for (enodnum = meshptr->m.verttax[vnodnum];
         enodnum < meshptr->m.vendtax[vnodnum]; enodnum ++)
      edgecut[meshptr->parttax[meshptr->m.edgetax[enodnum]]] ++;

#ifdef SCOTCH_DEBUG_VMESH3
    if (partnum == 2) {
      if ((edgecut[0] == 0) ||
          (edgecut[1] == 0))
        errorPrint ("vmeshCheck: no-use separator vertex%s (%ld)", /* Warning only */
                    ((meshptr->levlnum == 0) ? " at level 0" : ""),
                    (long) vnodnum);
    }
    else {
#else
    if (partnum != 2) {
#endif /* SCOTCH_DEBUG_VMESH3 */
      if (edgecut[1 - partnum] != 0) {
        errorPrint ("vmeshCheck: node should be in separator (%ld)", (long) vnodnum);
        return     (1);
      }
    }
  }
  if ((meshptr->ncmpload[0] != ncmpload[0]) ||
      (meshptr->ncmpload[1] != ncmpload[1]) ||
      (meshptr->ncmpload[2] != ncmpload[2]) ||
      (meshptr->ncmpsize[0] != ncmpsize[0]) ||
      (meshptr->ncmpsize[1] != ncmpsize[1]) ||
      (meshptr->fronnbr     != ncmpsize[2])) {
    errorPrint ("vmeshCheck: invalid node parameters");
    return     (1);
  }

  if ((meshptr->fronnbr < 0) ||
      (meshptr->fronnbr > meshptr->m.vnodnbr)) {
    errorPrint ("vmeshCheck: invalid number of frontier vertices");
    return     (1);
  }
  if ((frontax = memAlloc (meshptr->m.vnodnbr * sizeof (int))) == NULL) {
    errorPrint ("vmeshCheck: out of memory");
    return     (1);
  }
  memSet (frontax, 0, meshptr->m.vnodnbr * sizeof (int));
  frontax -= meshptr->m.vnodbas;

  for (fronnum = 0; fronnum < meshptr->fronnbr; fronnum ++) {
    Gnum                vnodnum;

    vnodnum = meshptr->frontab[fronnum];

    if ((vnodnum <  meshptr->m.vnodbas) ||
        (vnodnum >= meshptr->m.vnodnnd)) {
      errorPrint ("vmeshCheck: invalid vertex in frontier array");
      memFree    (frontax + meshptr->m.vnodbas);
      return     (1);
    }
    if (meshptr->parttax[vnodnum] != 2) {
      errorPrint ("vmeshCheck: invalid frontier array");
      memFree    (frontax + meshptr->m.vnodbas);
      return     (1);
    }
    if (frontax[vnodnum] != 0) {
      errorPrint ("vmeshCheck: duplicate node in frontier array");
      memFree    (frontax + meshptr->m.vnodbas);
      return     (1);
    }
    frontax[vnodnum] = 1;
  }
  memFree (frontax + meshptr->m.vnodbas);

  return (0);
}
