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
/**   NAME       : mesh_io.c                               **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the input/output        **/
/**                routines for the mesh structure.        **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 19 oct 2003     **/
/**                                 to     25 oct 2003     **/
/**                                                        **/
/**   NOTES      : # This code has been taken verbatim     **/
/**                  from the mesh I/O routines of         **/
/**                  Scotch. Consequently, mesh formats    **/
/**                  are identical in both projects,       **/
/**                  although edge weights are not used    **/
/**                  by these meshes.                      **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MESH_IO

#include "common.h"
#include "mesh.h"

/*******************************/
/*                             */
/* The mesh handling routines. */
/*                             */
/*******************************/

/*+ This routine reads the given mesh
*** structure from the given stream.
*** Edge loads, whenever present, are
*** always discarded.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
meshLoad (
Mesh * restrict const       meshptr,
FILE * restrict const       stream)
{
  INT                   versval;                  /* File version value                 */
  INT                   propval;                  /* File flag value                    */
  char                  proptab[4];               /* Decomposed file flag value         */
  INT                   velmbas;                  /* Base number for elements           */
  INT                   velmnbr;                  /* Number of elements                 */
  INT                   vnodbas;                  /* Base number for nodes              */
  INT                   vnodnbr;                  /* Number of nodes                    */
  INT                   vlblmax;                  /* Maximum vertex label number        */
  INT * restrict        vlbltax;                  /* Based access to vertex label array */
  INT * restrict        verttax;                  /* Based access to vertex array       */
  INT * restrict        velotax;                  /* Based access to vertex load array  */
  INT * restrict        edgetax;                  /* Based access to edge array         */
  INT                   edgennd;                  /* Index of end of edge array         */
  INT                   edgenum;                  /* Index of current edge              */
  INT                   degrval;                  /* Degree of current vertex           */
  INT                   vertbastab[2];
  INT                   vertnndtab[2];
  INT                   edgeadjtab[2];
  int                   i;

  memset (meshptr, 0, sizeof (Mesh));

  if ((intLoad (stream, &versval) != 1) ||        /* Read version number */
      (versval != 1)) {
    errorPrint ("meshLoad: bad input (1)");
    return     (1);
  }

  if ((intLoad (stream, &velmnbr)          +      /* Read rest of header */
       intLoad (stream, &vnodnbr)          +
       intLoad (stream, &meshptr->edgenbr) +
       intLoad (stream, &velmbas)          +
       intLoad (stream, &vnodbas)          +
       intLoad (stream, &propval) != 6) ||
      (velmnbr < 0)                     ||
      (vnodnbr < 0)                     ||
      (velmbas < 0)                     ||
      (vnodbas < 0)                     ||
      (propval < 0)                     ||
      (propval > 111)                   ||
      (((velmbas + velmnbr) != vnodbas) &&
       ((vnodbas + vnodnbr) != velmbas))) {
    errorPrint ("meshLoad: bad input (2)");
    return     (1);
  }
  sprintf (proptab, "%3.3d", (int) propval);      /* Compute file properties */
  proptab[0] -= '0';                              /* Vertex labels flag      */
  proptab[1] -= '0';                              /* Edge weights flag       */
  proptab[2] -= '0';                              /* Vertex loads flag       */

  meshptr->baseval = MIN (velmbas, vnodbas);      /* Get file mesh base value */
  meshptr->vertnbr = (velmnbr + vnodnbr);
  meshptr->velmnbr = velmnbr;
  meshptr->velmbas = velmbas;
  meshptr->velmnnd = velmbas + velmnbr;
  meshptr->vnodnbr = vnodnbr;
  meshptr->vnodbas = vnodbas;
  meshptr->vnodnnd = vnodbas + vnodnbr;
  meshptr->edgesiz = meshptr->edgenbr;

  if (((meshptr->verttab = memAlloc ((meshptr->vertnbr + 1) * sizeof (INT))) == NULL) ||
      ((proptab[0] != 0) &&
       ((meshptr->vlbltab = memAlloc (meshptr->vertnbr * sizeof (INT))) == NULL)) ||
      ((proptab[2] != 0) &&
       ((meshptr->velotab = memAlloc (meshptr->vertnbr * sizeof (INT))) == NULL)) ||
      ((meshptr->edgetab = memAlloc (meshptr->edgenbr * sizeof (INT))) == NULL)) {
    errorPrint ("meshLoad: out of memory (1)");
    meshFree   (meshptr);
    return     (1);
  }
  meshptr->velosum = 0;                           /* No load accumulated yet */
  meshptr->degrmax = 0;                           /* No maximum degree yet   */

  if (meshptr->velmbas <= meshptr->vnodbas) {     /* If elements first */
    vertbastab[0] = meshptr->velmbas;
    vertnndtab[0] = meshptr->velmnnd;
    edgeadjtab[0] = meshptr->vnodbas - meshptr->baseval;
    vertbastab[1] = meshptr->vnodbas;
    vertnndtab[1] = meshptr->vnodnnd;
    edgeadjtab[1] = meshptr->velmbas - meshptr->baseval;
  }
  else {
    vertbastab[0] = meshptr->vnodbas;
    vertnndtab[0] = meshptr->vnodnnd;
    edgeadjtab[0] = meshptr->velmbas - meshptr->baseval;
    vertbastab[1] = meshptr->velmbas;
    vertnndtab[1] = meshptr->velmnnd;
    edgeadjtab[1] = meshptr->vnodbas - meshptr->baseval;
  }

  verttax = meshptr->verttab - meshptr->baseval;  /* Compute based indices */
  velotax = (meshptr->velotab != NULL) ? meshptr->velotab - meshptr->baseval : NULL;
  vlbltax = (meshptr->vlbltab != NULL) ? meshptr->vlbltab - meshptr->baseval : NULL;
  vlblmax = meshptr->vertnbr + meshptr->baseval - 1; /* No vertex labels known */
  edgetax = meshptr->edgetab - meshptr->baseval;
  edgenum = meshptr->baseval;
  edgennd = meshptr->edgenbr + meshptr->baseval;

  for (i = 0; i < 2; i ++) {                      /* For both kinds of vertices */
    INT                 vertbas;
    INT                 vertnnd;
    INT                 edgeadj;
    INT                 vertnum;

    vertbas = vertbastab[i];
    vertnnd = vertnndtab[i];
    edgeadj = edgeadjtab[i];

    for (vertnum = vertbas; vertnum < vertnnd; vertnum ++) { /* For all vertices of same kind */
      if (vlbltax != NULL) {                      /* If must read label                       */
        INT                 vlblval;

        if (intLoad (stream, &vlblval) != 1) {    /* Read label data */
          errorPrint ("meshLoad: bad input (3)");
          meshFree   (meshptr);
          return     (1);
        }
        vlbltax[vertnum] = vlblval + vertbas;     /* Adjust vertex label      */
        if (vlbltax[vertnum] > vlblmax)           /* Get maximum vertex label */
          vlblmax = vlbltax[vertnum];
      }
      if (proptab[2] != 0) {                      /* If must read vertex load */
        INT                 veloval;

        if (intLoad (stream, &veloval) != 1) {    /* Read vertex load data */
          errorPrint ("meshLoad: bad input (4)");
          meshFree   (meshptr);
          return     (1);
        }
        velotax[vertnum]  = veloval;
        meshptr->velosum += veloval;
      }
      if (intLoad (stream, &degrval) != 1) {      /* Read vertex degree */
        errorPrint ("meshLoad: bad input (5)");
        meshFree   (meshptr);
        return     (1);
      }
      if (meshptr->degrmax < degrval)             /* Set maximum degree */
        meshptr->degrmax = degrval;

      verttax[vertnum] = edgenum;                 /* Set index in edge array */
      degrval += edgenum;
      if (degrval > edgennd) {                    /* Check if edge array overflows */
        errorPrint ("meshLoad: invalid arc count (1)");
        meshFree   (meshptr);
        return     (1);
      }

      for ( ; edgenum < degrval; edgenum ++) {
        INT                 edloval;
        INT                 edgeval;

        if (proptab[1] != 0) {                    /* If must read edge load        */
          if (intLoad (stream, &edloval) != 1) {  /* Read edge load data (useless) */
            errorPrint ("meshLoad: bad input (6)");
            meshFree   (meshptr);
            return     (1);
          }
        }
        if (intLoad (stream, &edgeval) != 1) {    /* Read edge data */
          errorPrint ("meshLoad: bad input (7)");
          meshFree   (meshptr);
          return     (1);
        }
        edgetax[edgenum] = edgeval + edgeadj;
      }
    }
  }
  meshptr->verttab[meshptr->vertnbr] = meshptr->edgenbr + meshptr->baseval; /* Set end of edge array */

  if (edgenum != edgennd) {                       /* Check if number of edges is valid */
    errorPrint ("meshLoad: invalid arc count (2)");
    meshFree   (meshptr);
    return     (1);
  }

  if (meshptr->velotab == NULL) {                 /* If vertex loads not computed     */
    meshptr->vnlosum = meshptr->vnodnbr;          /* Node load is number of nodes     */
    meshptr->velosum = meshptr->vertnbr;          /* Total load is number of vertices */
  }
  else {                                          /* Compute sum of node loads */
    INT                 vertnum;
    INT                 vnlosum;

    for (vertnum = meshptr->vnodbas - meshptr->baseval, vnlosum = 0;
         vertnum < meshptr->vnodnnd - meshptr->baseval; vertnum ++)
      vnlosum += meshptr->velotab[vertnum];

    meshptr->vnlosum = vnlosum;
  }

  if (meshptr->vlbltab != NULL) {                 /* If vertex label renaming necessary */
    errorPrint ("meshLoad: vertex labels not yet supported");
    return     (1);
  }

  return (0);
}

/*+ This routine saves the given mesh
*** structure to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
meshSave (
const Mesh * restrict const meshptr,
FILE * restrict const       stream)
{
  INT                   vertnum;
  const INT * restrict  verttax;
  const INT * restrict  velotax;
  const INT * restrict  vlbltax;
  const INT * restrict  edgetax;                  /* Based access to edge array */
  INT                   edgenum;
  char                  propstr[4];               /* Property string */
  int                   o;

  propstr[0] = (meshptr->vlbltab != NULL) ? '1' : '0'; /* Set property string */
  propstr[1] = '0';                               /* No edge loads written    */
  propstr[2] = (meshptr->velotab != NULL) ? '1' : '0';
  propstr[3] = '\0';

  if (fprintf (stream, "1\n%ld\t%ld\t%ld\n%ld\t%ld\t%3s\n", /* Write file header */
               (long) meshptr->velmnbr,
               (long) meshptr->vnodnbr,
               (long) meshptr->edgenbr,
               (long) meshptr->velmbas,
               (long) meshptr->vnodbas,
               propstr) == EOF) {
    errorPrint ("meshSave: bad output (1)");
    return     (1);
  }

  verttax = meshptr->verttab - meshptr->baseval;  /* Compute based indices */
  velotax = (meshptr->velotab != NULL) ? meshptr->velotab - meshptr->baseval : NULL;
  vlbltax = (meshptr->vlbltab != NULL) ? meshptr->vlbltab - meshptr->baseval : NULL;
  edgetax = meshptr->edgetab - meshptr->baseval;
  for (vertnum = meshptr->baseval, o = 0;
       (vertnum < (meshptr->vertnbr + meshptr->baseval)) && (o == 0); vertnum ++) {
    if (vlbltax != NULL)                          /* Write vertex label if necessary */
      o  = (fprintf (stream, "%ld\t", (long) vlbltax[vertnum]) == EOF);
    if (velotax != NULL)                          /* Write vertex load if necessary */
      o |= (fprintf (stream, "%ld\t", (long) velotax[vertnum]) == EOF);
    o |= (fprintf (stream, "%ld", (long) (verttax[vertnum + 1] - verttax[vertnum])) == EOF); /* Write vertex degree */

    for (edgenum = verttax[vertnum];
         (edgenum < verttax[vertnum + 1]) && (o == 0); edgenum ++) {
      o |= (putc ('\t', stream) == EOF);
      o |= (intSave (stream,                      /* Write edge end */
                     (vlbltax != NULL) ? vlbltax[edgetax[edgenum]] : edgetax[edgenum]) != 1);
    }
    o |= (putc ('\n', stream) == EOF);
  }

  if (o != 0)
    errorPrint ("meshSave: bad output (2)");

  return (o);
}
