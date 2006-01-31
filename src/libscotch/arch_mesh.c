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
/**   NAME       : arch_mesh.c                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the mesh graph      **/
/**                target architectures.                   **/
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
/**                # Version 3.1  : from : 22 jul 1996     **/
/**                                 to     23 jul 1996     **/
/**                # Version 3.2  : from : 16 oct 1996     **/
/**                                 to     14 may 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     01 oct 1998     **/
/**                # Version 4.0  : from : 09 jan 2004     **/
/**                                 to     10 mar 2005     **/
/**                                                        **/
/**   NOTES      : # The vertices of the (dX,dY) mesh are  **/
/**                  numbered as terminals so that         **/
/**                  t(0,0) = 0, t(1,0) = 1,               **/
/**                  t(dX - 1, 0) = dX - 1, t(0,1) = dX,   **/
/**                  and t(x,y) = (y * dX) + x.            **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define ARCH_MESH

#include "module.h"
#include "common.h"
#include "arch.h"
#include "arch_mesh.h"

/***********************************************/
/*                                             */
/* These are the 2-dimensional mesh routines. */
/*                                             */
/***********************************************/

/* This routine loads the
** bidimensional mesh architecture.
** It returns:
** - 0   : if the architecture has been successfully read.
** - !0  : on error.
*/

int
archMesh2ArchLoad (
ArchMesh2 * restrict const  archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchMesh2)    > sizeof (ArchDummy)) ||
      (sizeof (ArchMesh2Dom) > sizeof (ArchDomDummy))) {
    errorPrint ("archMesh2ArchLoad: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if ((intLoad (stream, &archptr->c[0]) +
       intLoad (stream, &archptr->c[1]) != 2) ||
      (archptr->c[0] < 1) || (archptr->c[1] < 1)) {
    errorPrint ("archMesh2ArchLoad: bad input");
    return     (1);
  }

  return (0);
}

/* This routine saves the
** bidimensional mesh architecture.
** It returns:
** - 0   : if the architecture has been successfully written.
** - !0  : on error.
*/

int
archMesh2ArchSave (
const ArchMesh2 * const     archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchMesh2)    > sizeof (ArchDummy)) ||
      (sizeof (ArchMesh2Dom) > sizeof (ArchDomDummy))) {
    errorPrint ("archMesh2ArchSave: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if (fprintf (stream, "%ld %ld ",
               (long) archptr->c[0],
               (long) archptr->c[1]) == EOF) {
    errorPrint ("archMesh2ArchSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function returns the smallest number
** of terminal domain included in the given
** domain.
*/

ArchDomNum
archMesh2DomNum (
const ArchMesh2 * const     archptr,
const ArchMesh2Dom * const  domptr)
{
  return ((domptr->c[1][0] * archptr->c[0]) + domptr->c[0][0]); /* Return vertex number */
}

/* This function returns the terminal domain associated
** with the given terminal number in the architecture.
** It returns:
** - 0  : if label is valid and domain has been updated.
** - 1  : if label is invalid.
** - 2  : on error.
*/

int
archMesh2DomTerm (
const ArchMesh2 * const     archptr,
ArchMesh2Dom * const        domptr,
const ArchDomNum            domnum)
{
  if (domnum < (archptr->c[0] * archptr->c[1])) { /* If valid label */
    domptr->c[0][0] =                             /* Set the domain */
    domptr->c[0][1] = domnum % archptr->c[0];
    domptr->c[1][0] =
    domptr->c[1][1] = domnum / archptr->c[0];

    return (0);
  }

  return (1);                                     /* Cannot set domain */
}

/* This function returns the number of
** elements in the rectangular domain.
*/

Anum 
archMesh2DomSize (
const ArchMesh2 * const     archptr,
const ArchMesh2Dom * const  domptr)
{
  return ((domptr->c[0][1] - domptr->c[0][0] + 1) *
          (domptr->c[1][1] - domptr->c[1][0] + 1));
}

/* This function returns the average
** distance between two rectangular
** domains (in fact the distance between
** the centers of the domains).
*/

Anum 
archMesh2DomDist (
const ArchMesh2 * const    archptr,
const ArchMesh2Dom * const dom0ptr,
const ArchMesh2Dom * const dom1ptr)
{
  return (((abs (dom0ptr->c[0][0] + dom0ptr->c[0][1] -
                 dom1ptr->c[0][0] - dom1ptr->c[0][1]) + 1) / 2) +
          ((abs (dom0ptr->c[1][0] + dom0ptr->c[1][1] -
                 dom1ptr->c[1][0] - dom1ptr->c[1][1]) + 1) / 2));
}

/* This function sets the biggest
** domain available for this
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archMesh2DomFrst (
const ArchMesh2 * const       archptr,
ArchMesh2Dom * restrict const domptr)
{
  domptr->c[0][0] =
  domptr->c[1][0] = 0;
  domptr->c[0][1] = archptr->c[0] - 1;
  domptr->c[1][1] = archptr->c[1] - 1;

  return (0);
}

/* This routine reads domain information
** from the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archMesh2DomLoad (
const ArchMesh2 * const       archptr,
ArchMesh2Dom * restrict const domptr,
FILE * restrict const         stream)
{
  if (intLoad (stream, &domptr->c[0][0]) +
      intLoad (stream, &domptr->c[1][0]) +
      intLoad (stream, &domptr->c[0][1]) +
      intLoad (stream, &domptr->c[1][1]) != 4) {
    errorPrint ("archMesh2DomLoad: bad input");
    return     (1);
  }

  return (0);
}

/* This routine saves domain information
** to the given stream.
** - 0   : on success.
** - !0  : on error.
*/

int
archMesh2DomSave (
const ArchMesh2 * const     archptr,
const ArchMesh2Dom * const  domptr,
FILE * restrict const       stream)
{
  if (fprintf (stream, "%ld %ld %ld %ld ",
               (long) domptr->c[0][0], (long) domptr->c[1][0],
               (long) domptr->c[0][1], (long) domptr->c[1][1]) == EOF) {
    errorPrint ("archMesh2DomSave: bad output");
    return     (1);
  }

  return (0);
}

/* These functions try to split a rectangular
** domain into two subdomains.
** It returns:
** - 0  : if bipartitioning succeeded.
** - 1  : if bipartitioning could not be performed.
** - 2  : on error.
*/

int
archMesh2DomBipart (
const ArchMesh2 * const       archptr,
const ArchMesh2Dom * const    domptr,
ArchMesh2Dom * restrict const dom0ptr,
ArchMesh2Dom * restrict const dom1ptr)
{
  if ((domptr->c[0][0] == domptr->c[0][1]) &&     /* Return if cannot bipartition more */
      (domptr->c[1][0] == domptr->c[1][1]))
    return (1);

  if ((domptr->c[0][1] - domptr->c[0][0]) >       /* Split domain in two along largest dimension */
      (domptr->c[1][1] - domptr->c[1][0])) {
    dom0ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = (domptr->c[0][0] + domptr->c[0][1]) / 2;
    dom1ptr->c[0][0] = dom0ptr->c[0][1] + 1;
    dom1ptr->c[0][1] = domptr->c[0][1];
    dom0ptr->c[1][0] = dom1ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = dom1ptr->c[1][1] = domptr->c[1][1];
  }
  else {
    dom0ptr->c[0][0] = dom1ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = dom1ptr->c[0][1] = domptr->c[0][1];
    dom0ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = (domptr->c[1][0] + domptr->c[1][1]) / 2;
    dom1ptr->c[1][0] = dom0ptr->c[1][1] + 1;
    dom1ptr->c[1][1] = domptr->c[1][1];
  }

  return (0);
}

int
archMesh2DomBipartO (
const ArchMesh2 * const       archptr,
const ArchMesh2Dom * const    domptr,
ArchMesh2Dom * restrict const dom0ptr,
ArchMesh2Dom * restrict const dom1ptr)
{
  if ((domptr->c[0][0] == domptr->c[0][1]) &&     /* Return if cannot bipartition more */
      (domptr->c[1][0] == domptr->c[1][1]))
    return (1);

  if (domptr->c[1][1] == domptr->c[1][0]) {       /* If the Y dimension cannot be cut */
    dom0ptr->c[0][0] = domptr->c[0][0];           /* Cut in the X dimension           */
    dom0ptr->c[0][1] = (domptr->c[0][0] + domptr->c[0][1]) / 2;
    dom1ptr->c[0][0] = dom0ptr->c[0][1] + 1;
    dom1ptr->c[0][1] = domptr->c[0][1];
    dom0ptr->c[1][0] = dom1ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = dom1ptr->c[1][1] = domptr->c[1][1];
  }
  else {                                          /* If the Y dimension can be cut, cut it */
    dom0ptr->c[0][0] = dom1ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = dom1ptr->c[0][1] = domptr->c[0][1];
    dom0ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = (domptr->c[1][0] + domptr->c[1][1]) / 2;
    dom1ptr->c[1][0] = dom0ptr->c[1][1] + 1;
    dom1ptr->c[1][1] = domptr->c[1][1];
  }

  return (0);
}

int
archMesh2DomBipartU (
const ArchMesh2 * const       archptr,
const ArchMesh2Dom * const    domptr,
ArchMesh2Dom * restrict const dom0ptr,
ArchMesh2Dom * restrict const dom1ptr)
{
  if ((domptr->c[0][0] == domptr->c[0][1]) &&     /* Return if cannot bipartition more */
      (domptr->c[1][0] == domptr->c[1][1]))
    return (1);

  if ((domptr->c[0][1] - domptr->c[0][0]) >       /* Split domain unevenly along largest dimension */
      (domptr->c[1][1] - domptr->c[1][0])) {
    dom0ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = (domptr->c[0][0] + domptr->c[0][1] + domptr->c[0][1]) / 3;
    dom1ptr->c[0][0] = dom0ptr->c[0][1] + 1;
    dom1ptr->c[0][1] = domptr->c[0][1];
    dom0ptr->c[1][0] = dom1ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = dom1ptr->c[1][1] = domptr->c[1][1];
  }
  else {
    dom0ptr->c[0][0] = dom1ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = dom1ptr->c[0][1] = domptr->c[0][1];
    dom0ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = (domptr->c[1][0] + domptr->c[1][1] + domptr->c[1][1]) / 3;
    dom1ptr->c[1][0] = dom0ptr->c[1][1] + 1;
    dom1ptr->c[1][1] = domptr->c[1][1];
  }

  return (0);
}

/***********************************************/
/*                                             */
/* These are the 3-dimensional mesh routines. */
/*                                             */
/***********************************************/

/* This routine loads the
** tridimensional mesh architecture.
** It returns:
** - 0   : if the architecture has been successfully read.
** - !0  : on error.
*/

int
archMesh3ArchLoad (
ArchMesh3 * restrict const  archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchMesh3)    > sizeof (ArchDummy)) ||
      (sizeof (ArchMesh3Dom) > sizeof (ArchDomDummy))) {
    errorPrint ("archMesh3ArchLoad: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if ((intLoad (stream, &archptr->c[0]) +
       intLoad (stream, &archptr->c[1]) +
       intLoad (stream, &archptr->c[2]) != 3) ||
      (archptr->c[0] < 1) || (archptr->c[1] < 1) || (archptr->c[2] < 1)) {
    errorPrint ("archMesh3ArchLoad: bad input");
    return     (1);
  }

  return (0);
}

/* This routine saves the
** tridimensional mesh architecture.
** It returns:
** - 0   : if the architecture has been successfully written.
** - !0  : on error.
*/

int
archMesh3ArchSave (
const ArchMesh3 * const     archptr,
FILE * restrict const       stream)
{
#ifdef SCOTCH_DEBUG_ARCH1
  if ((sizeof (ArchMesh3)    > sizeof (ArchDummy)) ||
      (sizeof (ArchMesh3Dom) > sizeof (ArchDomDummy))) {
    errorPrint ("archMesh3ArchSave: invalid type specification");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_ARCH1 */

  if (fprintf (stream, "%ld %ld %ld ",
               (long) archptr->c[0], (long) archptr->c[1], (long) archptr->c[2]) == EOF) {
    errorPrint ("archMesh3ArchSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function returns the smallest number
** of terminal domain included in the given
** domain.
*/

ArchDomNum
archMesh3DomNum (
const ArchMesh3 * const     archptr,
const ArchMesh3Dom * const  domptr)
{
  return ((((domptr->c[2][0]  * archptr->c[1]) +  /* Return the vertex number */
             domptr->c[1][0]) * archptr->c[0]) +
             domptr->c[0][0]);
}

/* This function returns the terminal domain associated
** with the given terminal number in the architecture.
** It returns:
** - 0  : if label is valid and domain has been updated.
** - 1  : if label is invalid.
** - 2  : on error.
*/

int
archMesh3DomTerm (
const ArchMesh3 * const     archptr,
ArchMesh3Dom * const        domptr,
const ArchDomNum            domnum)
{
  if (domnum < (archptr->c[0] * archptr->c[1] * archptr->c[2])) { /* If valid label */
    domptr->c[0][0] =                             /* Set the domain                 */
    domptr->c[0][1] = domnum % archptr->c[0];
    domptr->c[1][0] =
    domptr->c[1][1] = (domnum / archptr->c[0]) % archptr->c[1];
    domptr->c[2][0] =
    domptr->c[2][1] = domnum / (archptr->c[0] * archptr->c[1]);

    return (0);
  }

  return (1);                                     /* Cannot set domain */
}

/* This function returns the number of
** elements in the cubic domain.
*/

Anum 
archMesh3DomSize (
const ArchMesh3 * const     archptr,
const ArchMesh3Dom * const  domptr)
{
  return ((domptr->c[0][1] - domptr->c[0][0] + 1) *
          (domptr->c[1][1] - domptr->c[1][0] + 1) *
          (domptr->c[2][1] - domptr->c[2][0] + 1));
}

/* This function returns the average distance
** between two cubic domains (in fact the
** distance between the centers of the domains).
*/

Anum 
archMesh3DomDist (
const ArchMesh3 * const     archptr,
const ArchMesh3Dom * const  dom0ptr,
const ArchMesh3Dom * const  dom1ptr)
{
  return (((abs (dom0ptr->c[0][0] + dom0ptr->c[0][1] -
                 dom1ptr->c[0][0] - dom1ptr->c[0][1]) + 1) / 2) +
          ((abs (dom0ptr->c[1][0] + dom0ptr->c[1][1] -
                 dom1ptr->c[1][0] - dom1ptr->c[1][1]) + 1) / 2) +
          ((abs (dom0ptr->c[2][0] + dom0ptr->c[2][1] -
                 dom1ptr->c[2][0] - dom1ptr->c[2][1]) + 1) / 2));
}

/* This function sets the biggest
** domain available for this
** architecture.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archMesh3DomFrst (
const ArchMesh3 * const       archptr,
ArchMesh3Dom * restrict const domptr)
{
  domptr->c[0][0] =
  domptr->c[1][0] =
  domptr->c[2][0] = 0;
  domptr->c[0][1] = archptr->c[0] - 1;
  domptr->c[1][1] = archptr->c[1] - 1;
  domptr->c[2][1] = archptr->c[2] - 1;

  return (0);
}

/* This routine reads domain information
** from the given stream.
** It returns:
** - 0   : on success.
** - !0  : on error.
*/

int
archMesh3DomLoad (
const ArchMesh3 * const       archptr,
ArchMesh3Dom * restrict const domptr,
FILE * restrict const         stream)
{
  if (intLoad (stream, &domptr->c[0][0]) +
      intLoad (stream, &domptr->c[1][0]) +
      intLoad (stream, &domptr->c[2][0]) +
      intLoad (stream, &domptr->c[0][1]) +
      intLoad (stream, &domptr->c[1][1]) +
      intLoad (stream, &domptr->c[2][1]) != 6) {
    errorPrint ("archMesh3DomLoad: bad input");
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
archMesh3DomSave (
const ArchMesh3 * const     archptr,
const ArchMesh3Dom * const  domptr,
FILE * restrict const       stream)
{
  if (fprintf (stream, "%ld %ld %ld %ld %ld %ld ",
               (long) domptr->c[0][0], (long) domptr->c[1][0], (long) domptr->c[2][0],
               (long) domptr->c[0][1], (long) domptr->c[1][1], (long) domptr->c[2][1]) == EOF) {
    errorPrint ("archMesh3DomSave: bad output");
    return     (1);
  }

  return (0);
}

/* This function tries to split a cubic
** domain into two subdomains.
** It returns:
** - 0  : if bipartitioning succeeded.
** - 1  : if bipartitioning could not be performed.
** - 2  : on error.
*/

int
archMesh3DomBipart (
const ArchMesh3 * const       archptr,
const ArchMesh3Dom * const    domptr,
ArchMesh3Dom * restrict const dom0ptr,
ArchMesh3Dom * restrict const dom1ptr)
{
  int                 i;

  if ((domptr->c[0][0] == domptr->c[0][1]) &&     /* Return if cannot bipartition more */
      (domptr->c[1][0] == domptr->c[1][1]) &&
      (domptr->c[2][0] == domptr->c[2][1]))
    return (1);

  i = ((domptr->c[1][1] - domptr->c[1][0]) >      /* Find largest dimension */
       (domptr->c[0][1] - domptr->c[0][0]))
    ? 1 : 0;
  if  ((domptr->c[2][1] - domptr->c[2][0]) >
       (domptr->c[i][1] - domptr->c[i][0]))
    i = 2;

  if (i == 0) {                                   /* Split domain in two along largest dimension */
    dom0ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = (domptr->c[0][0] + domptr->c[0][1]) / 2;
    dom1ptr->c[0][0] = dom0ptr->c[0][1] + 1;
    dom1ptr->c[0][1] = domptr->c[0][1];

    dom0ptr->c[1][0] = dom1ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = dom1ptr->c[1][1] = domptr->c[1][1];

    dom0ptr->c[2][0] = dom1ptr->c[2][0] = domptr->c[2][0];
    dom0ptr->c[2][1] = dom1ptr->c[2][1] = domptr->c[2][1];
  }
  else if (i == 1) {
    dom0ptr->c[0][0] = dom1ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = dom1ptr->c[0][1] = domptr->c[0][1];

    dom0ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = (domptr->c[1][0] + domptr->c[1][1]) / 2;
    dom1ptr->c[1][0] = dom0ptr->c[1][1] + 1;
    dom1ptr->c[1][1] = domptr->c[1][1];

    dom0ptr->c[2][0] = dom1ptr->c[2][0] = domptr->c[2][0];
    dom0ptr->c[2][1] = dom1ptr->c[2][1] = domptr->c[2][1];
  }
  else {
    dom0ptr->c[0][0] = dom1ptr->c[0][0] = domptr->c[0][0];
    dom0ptr->c[0][1] = dom1ptr->c[0][1] = domptr->c[0][1];

    dom0ptr->c[1][0] = dom1ptr->c[1][0] = domptr->c[1][0];
    dom0ptr->c[1][1] = dom1ptr->c[1][1] = domptr->c[1][1];

    dom0ptr->c[2][0] = domptr->c[2][0];
    dom0ptr->c[2][1] = (domptr->c[2][0] + domptr->c[2][1]) / 2;
    dom1ptr->c[2][0] = dom0ptr->c[2][1] + 1;
    dom1ptr->c[2][1] = domptr->c[2][1];
  }

  return (0);
}
