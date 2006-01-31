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
/**   NAME       : mesh_check.c                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the source mesh     **/
/**                functions.                              **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 29 dec 2001     **/
/**                                 to     11 may 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MESH

#include "module.h"
#include "common.h"
#include "graph.h"
#include "mesh.h"

/****************************************/
/*                                      */
/* These routines handle source meshes. */
/*                                      */
/****************************************/

/* This routine checks the consistency
** of the given mesh.
** It returns:
** - 0   : if mesh data are consistent.
** - !0  : on error.
*/

int
meshCheck (
const Mesh * const          meshptr)
{
  Gnum                vertnnd;                    /* Maximum vertex index                */
  Gnum                vertnum;                    /* Number of current vertex            */
  Gnum                veisnbr;                    /* Number of isolated element vertices */
  Gnum                velosum;                    /* Vertex element load sum             */
  Gnum                vnlosum;                    /* Vertex node load sum                */
  Gnum                degrmax;                    /* Maximum degree                      */

  if ((meshptr->velmbas > meshptr->velmnnd) ||
      (meshptr->vnodbas > meshptr->vnodnnd) ||
      ((meshptr->velmnnd != meshptr->vnodbas) &&
       (meshptr->vnodnnd != meshptr->velmbas))) {
    errorPrint ("meshCheck: invalid node and element numbers");
    return     (1);
  }

  vertnnd = meshptr->velmnbr + meshptr->vnodnbr + meshptr->baseval;
  degrmax = 0;
  veisnbr = 0;
  for (vertnum = meshptr->velmbas; vertnum < meshptr->velmnnd; vertnum ++) { /* For all element vertices */
    Gnum                degrval;
    Gnum                edgenum;

    if ((meshptr->verttax[vertnum] < meshptr->baseval)          ||
        (meshptr->vendtax[vertnum] < meshptr->verttax[vertnum])) {
      errorPrint ("meshCheck: invalid vertex arrays (1)");
      return     (1);
    }

    degrval = meshptr->vendtax[vertnum] - meshptr->verttax[vertnum];
    if (degrval > degrmax)
      degrmax = degrval;
    else if (degrval == 0)
      veisnbr ++;

    for (edgenum = meshptr->verttax[vertnum]; edgenum < meshptr->vendtax[vertnum]; edgenum ++) {
      Gnum                vertend;                /* Number of end vertex      */
      Gnum                edgeend;                /* Number of end vertex edge */

      vertend = meshptr->edgetax[edgenum];

      if ((vertend < meshptr->baseval) || (vertend >= vertnnd)) { /* If invalid edge end */
        errorPrint ("meshCheck: invalid edge array (1)");
        return     (1);
      }
      if ((vertend >= meshptr->velmbas) &&
          (vertend <  meshptr->velmnnd)) {
        errorPrint ("meshCheck: element vertices must not be connected together");
        return     (1);
      }

      for (edgeend = meshptr->verttax[vertend];   /* Search for matching arc */
           (edgeend < meshptr->vendtax[vertend]) && (meshptr->edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if (edgeend >= meshptr->vendtax[vertend]) {
        errorPrint ("meshCheck: arc data do not match (1)");
        return     (1);
      }
      for (edgeend ++;                            /* Search for duplicate arcs */
           (edgeend < meshptr->vendtax[vertend]) && (meshptr->edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if (edgeend < meshptr->vendtax[vertend]) {
        errorPrint ("meshCheck: duplicate arc (1)");
        return     (1);
      }
    }
  }
  if (veisnbr != meshptr->veisnbr) {
    errorPrint ("meshCheck: invalid number of isolated element vertices (1)");
    return     (1);
  }

  for (vertnum = meshptr->vnodbas; vertnum < meshptr->vnodnnd; vertnum ++) { /* For all node vertices */
    Gnum                edgenum;

    if ((meshptr->verttax[vertnum] < meshptr->baseval)          ||
        (meshptr->vendtax[vertnum] < meshptr->verttax[vertnum])) {
      errorPrint ("meshCheck: invalid vertex arrays (2)");
      return     (1);
    }
    if ((meshptr->vendtax[vertnum] - meshptr->verttax[vertnum]) > degrmax)
      degrmax = meshptr->vendtax[vertnum] - meshptr->verttax[vertnum];

    for (edgenum = meshptr->verttax[vertnum]; edgenum < meshptr->vendtax[vertnum]; edgenum ++) {
      Gnum                vertend;                /* Number of end vertex      */
      Gnum                edgeend;                /* Number of end vertex edge */

      vertend = meshptr->edgetax[edgenum];

      if ((vertend < meshptr->baseval) || (vertend >= vertnnd)) { /* If invalid edge end */
        errorPrint ("meshCheck: invalid edge array (2)");
        return     (1);
      }
      if ((vertend >= meshptr->vnodbas) &&
          (vertend <  meshptr->vnodnnd)) {
        errorPrint ("meshCheck: node vertices must not be connected together");
        return     (1);
      }

      for (edgeend = meshptr->verttax[vertend];   /* Search for matching arc */
           (edgeend < meshptr->vendtax[vertend]) && (meshptr->edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if (edgeend >= meshptr->vendtax[vertend]) {
        errorPrint ("meshCheck: arc data do not match (2)");
        return     (1);
      }
      for (edgeend ++;                            /* Search for duplicate arcs */
           (edgeend < meshptr->vendtax[vertend]) && (meshptr->edgetax[edgeend] != vertnum);
           edgeend ++) ;
      if (edgeend < meshptr->vendtax[vertend]) {
        errorPrint ("meshCheck: duplicate arc (2)");
        return     (1);
      }
    }
  }

  if (meshptr->velotax == NULL)                   /* Recompute node vertex load sum */
    velosum = meshptr->velmnnd - meshptr->velmbas;
  else {
    for (vertnum = meshptr->velmbas, velosum = 0;
         vertnum < meshptr->velmnnd; vertnum ++) {
      if (meshptr->velotax[vertnum] < 1) {
        errorPrint ("meshCheck: invalid element vertex load");
        return     (1);
      }
      velosum += meshptr->velotax[vertnum];
    }
  }
  if (velosum != meshptr->velosum) {
    errorPrint ("meshCheck: invalid element vertex load sum");
    return     (1);
  }

  if (meshptr->vnlotax == NULL)                   /* Recompute node vertex load sum */
    vnlosum = meshptr->vnodnnd - meshptr->vnodbas;
  else {
    for (vertnum = meshptr->vnodbas, vnlosum = 0;
         vertnum < meshptr->vnodnnd; vertnum ++) {
      if (meshptr->vnlotax[vertnum] < 1) {
        errorPrint ("meshCheck: invalid node vertex load");
        return     (1);
      }
      vnlosum += meshptr->vnlotax[vertnum];
    }
  }
  if (vnlosum != meshptr->vnlosum) {
    errorPrint ("meshCheck: invalid node vertex load sum");
    return     (1);
  }

  if (meshptr->degrmax < degrmax) {
    errorPrint ("meshCheck: invalid maximum degree");
    return     (1);
  }

  return (0);
}
