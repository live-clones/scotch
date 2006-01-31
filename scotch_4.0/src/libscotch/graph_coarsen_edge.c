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
/**   NAME       : graph_coarsen_edge.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This commodity file contains the edge   **/
/**                arrays building subroutine which is     **/
/**                duplicated, with minor modifications,   **/
/**                into graph_coarsen.c                    **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 17 dec 2001     **/
/**                                 to     25 feb 2004     **/
/**                                                        **/
/************************************************************/

static
void
GRAPHCOARSENEDGENAME (
const Graph * restrict const              finegrafptr, /*+ Graph to coarsen      +*/
const Gnum * restrict const               finecoartax, /*+ Fine to coarse array  +*/
const GraphCoarsenMulti * restrict const  coarmulttax, /*+ Multinode array       +*/
Graph * restrict const                    coargrafptr, /*+ Coarse graph to build +*/
GraphCoarsenHash * restrict const         coarhashtab, /*+ End vertex hash table +*/
const Gnum                                coarhashmsk) /*+ Hash table mask       +*/
{
  Gnum                coarvertnum;
  Gnum                coaredgenum;
  Gnum                coardegrmax;

  for (coarvertnum = coaredgenum = coargrafptr->baseval, coardegrmax = 0;
       coarvertnum < coargrafptr->vertnnd; coarvertnum ++) {
    Gnum                finevertnum;
    int                 i;

    GRAPHCOARSENEDGEVERTINIT;                     /* Set vertex edge index */
    i = 0;
    do {                                          /* For all fine edges of multinode vertices */
      Gnum                fineedgenum;

      finevertnum = coarmulttax[coarvertnum].vertnum[i];
      for (fineedgenum = finegrafptr->verttax[finevertnum];
           fineedgenum < finegrafptr->vendtax[finevertnum]; fineedgenum ++) {
        Gnum                coarvertend;          /* Number of coarse vertex which is end of fine edge */
        Gnum                h;

        coarvertend = finecoartax[finegrafptr->edgetax[fineedgenum]];
        if (coarvertend != coarvertnum) {         /* If not end of collapsed edge */
          for (h = (coarvertend * GRAPHCOARHASHPRIME) & coarhashmsk; ; h = (h + 1) & coarhashmsk) {
            if (coarhashtab[h].vertorgnum != coarvertnum) { /* If old slot           */
              coarhashtab[h].vertorgnum = coarvertnum; /* Mark it in reference array */
              coarhashtab[h].vertendnum = coarvertend;
              coarhashtab[h].edgenum    = coaredgenum;
              GRAPHCOARSENEDGEEDGEINIT;           /* One more edge created      */
              GRAPHCOARSENEDGEEDLOINIT;           /* Initialize edge load entry */
              coaredgenum ++;
              break;                              /* Give up hashing */
            }
            if (coarhashtab[h].vertendnum == coarvertend) { /* If coarse edge already exists */
              GRAPHCOARSENEDGEEDLOADD;            /* Accumulate edge load                    */
              break;                              /* Give up hashing                         */
            }
          }
        }
      }
    } while (i ++, finevertnum != coarmulttax[coarvertnum].vertnum[1]);
    if (coardegrmax < (coaredgenum - coargrafptr->verttax[coarvertnum]))
      coardegrmax = coaredgenum - coargrafptr->verttax[coarvertnum];
  }
  GRAPHCOARSENEDGEVERTEXIT;                       /* Mark end of edge array */

  coargrafptr->degrmax = coardegrmax;
}
