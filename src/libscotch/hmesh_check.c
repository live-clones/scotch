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
/**   NAME       : hmesh_check.c                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the halo source     **/
/**                mesh functions.                         **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 12 sep 2002     **/
/**                                 to     11 may 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define HMESH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"
#include "hmesh.h"

/****************************************/
/*                                      */
/* These routines handle source meshes. */
/*                                      */
/****************************************/

/* This routine checks the consistency
** of the given halo mesh.
** It returns:
** - 0   : if halo mesh data are consistent.
** - !0  : on error.
*/

int
hmeshCheck (
const Hmesh * const         meshptr)
{
  Gnum                vnhlsum;

  if ((meshptr->vnohnnd < meshptr->m.vnodbas) ||
      (meshptr->vnohnnd > meshptr->m.vnodnnd)) {
    errorPrint ("hmeshCheck: invalid halo node numbers");
    return     (1);
  }
 
  if (meshCheck (&meshptr->m) != 0) {
    errorPrint ("hmeshCheck: invalid non-halo mesh structure");
    return     (1);
  }

  if (meshptr->vehdtax != meshptr->m.vendtax) {
    Gnum                veihnbr;
    Gnum                velmnum;

    for (velmnum = meshptr->m.velmbas, veihnbr = 0; /* For all element vertices */
         velmnum < meshptr->m.velmnnd; velmnum ++) {
      if ((meshptr->vehdtax[velmnum] < meshptr->m.verttax[velmnum]) ||
          (meshptr->vehdtax[velmnum] > meshptr->m.vendtax[velmnum])) {
        errorPrint ("hmeshCheck: invalid non-halo end vertex array");
        return     (1);
      }
      if (meshptr->vehdtax[velmnum] == meshptr->m.verttax[velmnum])
        veihnbr ++;
    }
    if (veihnbr != meshptr->veihnbr) {
      errorPrint ("hmeshCheck: invalid number of halo-isolated element vertices (1)");
      return     (1);
    }
  }
  else {
    if (meshptr->veihnbr != 0) {
      errorPrint ("hmeshCheck: invalid number of halo-isolated element vertices (2)");
      return     (1);
    }
  }

  if (meshptr->m.vnlotax == NULL)                 /* Recompute non-halo node vertex load sum */
    vnhlsum = meshptr->vnohnnd - meshptr->m.vnodbas;
  else {
    Gnum                vnodnum;

    for (vnodnum = meshptr->m.vnodbas, vnhlsum = 0;
         vnodnum < meshptr->vnohnnd; vnodnum ++)
      vnhlsum += meshptr->m.vnlotax[vnodnum];
  }
  if (vnhlsum != meshptr->vnhlsum) {
    errorPrint ("hmeshCheck: invalid non-halo vertex load sum");
    return     (1);
  }

  return (0);
}
