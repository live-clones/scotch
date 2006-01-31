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
/**   NAME       : vgraph_separate_vw.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module outputs the state of the    **/
/**                current partition on the form of a      **/
/**                Scotch mapping file.                    **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 18 may 2004     **/
/**                                 to     18 may 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VGRAPH_SEPARATE_VW

#include "module.h"
#include "common.h"
#include "gain.h"
#include "graph.h"
#include "vgraph.h"
#include "vgraph_separate_vw.h"

/*
**  The static variables.
*/

static int                  vgraphseparatevwfilenum = 0; /* Number of file to output */

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine outputs the mapping file.
** It returns:
** - 0   : if the file could be produced.
** - !0  : on error.
*/

int
vgraphSeparateVw (
Vgraph * restrict const             grafptr)      /*+ Separation graph +*/
{
  char                nametab[64];                /* File name */
  FILE * restrict     fileptr;
  Gnum                vertnum;                    /* Vertex number */

  sprintf (nametab, "vgraphseparatevw_output_%08d.map", vgraphseparatevwfilenum ++);
  if ((fileptr = fopen (nametab, "w+")) == NULL) {
    errorPrint ("vgraphSeparateVw: cannot open partition file");
    return     (1);
  }

  fprintf (fileptr, "%ld\n",                      /* Output size of mapping; test if failure later, in main loop */
           (long) grafptr->s.vertnbr);

  for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) {
    if (fprintf (fileptr, "%ld\t%d\n",
                 (long) ((grafptr->s.vnumtax != NULL) ? grafptr->s.vnumtax[vertnum] : vertnum),
                 (int) grafptr->parttax[vertnum]) <= 0) {
      errorPrint ("vgraphSeparateVw: bad output");
      fclose     (fileptr);
      return     (1);
    }
  }

  return (0);
}
