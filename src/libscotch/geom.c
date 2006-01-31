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
/**   NAME       : geom.c                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the geometric       **/
/**                source graph functions.                 **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 12 dec 1998     **/
/**                                 to     21 dec 1998     **/
/**                # Version 4.0  : from : 18 dec 2001     **/
/**                                 to     26 nov 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define GEOM

#include "module.h"
#include "common.h"
#include "geom.h"

/********************************************/
/*                                          */
/* These routines handle geometrical gdata. */
/*                                          */
/********************************************/

/* This routine initializes a geometrical
** data structure.
** It returns:
** - 0  : in all cases.
*/

int
geomInit (
Geom * restrict const       geomptr)
{
  geomptr->dimnnbr = 0;                           /* Initialize geometry */
  geomptr->geomtab = NULL;

  return (0);
}

/* This routine frees a geometrical graph structure.
** It returns:
** - VOID  : in all cases.
*/

void
geomExit (
Geom * restrict const       geomptr)
{
  if (geomptr->geomtab != NULL)
    memFree (geomptr->geomtab);

  geomptr->dimnnbr = 0;
  geomptr->geomtab = NULL;
}
