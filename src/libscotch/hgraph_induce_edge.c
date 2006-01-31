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
/**   NAME       : hgraph_induce_edge.c                    **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This commodity file contains the edge   **/
/**                arrays building subroutine which is     **/
/**                duplicated, with minor modifications,   **/
/**                into hgraph_induce.c                    **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 10 jan 2002     **/
/**                                 to     17 jan 2003     **/
/**                                                        **/
/************************************************************/

static
void
HGRAPHINDUCE2NAME (
const Hgraph * restrict const orggrafptr,         /* Pointer to original halo graph        */
Gnum * restrict const         orgindxtax,         /* Array of numbers of selected vertices */
Hgraph * restrict const       indgrafptr)         /* Pointer to induced halo graph         */
{
  Gnum                indvertnum;                 /* Number of current induced vertex                */
  Gnum                indvertnnd;                 /* Number of after-last induced (halo) vertex      */
  Gnum                indvelosum;                 /* Overall induced vertex load                     */
  Gnum                indedgenum;                 /* Number of current edge in induced halo subgraph */
  Gnum                indenohnbr;                 /* Number of non-halo edges in halo subgraph       */
  Gnum                inddegrmax;                 /* Maximum degree                                  */
#ifdef SCOTCH_DEBUG_HGRAPH2
  Gnum                indedgenbs;                 /* Revised number of edges in halo subgraph        */
#endif /* SCOTCH_DEBUG_HGRAPH2 */

  inddegrmax = 0;
  for (indvertnum = indedgenum = indgrafptr->s.baseval, indvelosum = indenohnbr = 0, indvertnnd = indgrafptr->vnohnnd; /* For all non-halo vertices */
       indvertnum < indgrafptr->vnohnnd; indvertnum ++) {
    Gnum                orgvertnum;               /* Number of current vertex in original halo graph       */
    Gnum                orgedgenum;               /* Number of current edge in original halo graph         */
    Gnum                indedgennd;               /* Index of after-last edge position in edge array       */
    Gnum                indedhdnum;               /* Index of after-last edge linking to non-halo vertices */
    Gnum                inddegrval;

    orgvertnum = indgrafptr->s.vnumtax[indvertnum];
    indgrafptr->s.verttax[indvertnum] = indedgenum;
    indenohnbr -= indedgenum;                     /* Subtract base of non-halo edges */
    if (indgrafptr->s.velotax != NULL) {          /* If graph has vertex weights     */
      indvelosum +=                               /* Accumulate vertex loads         */
      indgrafptr->s.velotax[indvertnum] = orggrafptr->s.velotax[orgvertnum];
    }

    inddegrval = orggrafptr->s.vendtax[orgvertnum] - orggrafptr->s.verttax[orgvertnum]; /* Get degree of non-halo node */
    if (inddegrmax < inddegrval)                  /* Keep maximum degree */
      inddegrmax = inddegrval;

    for (orgedgenum = orggrafptr->s.verttax[orgvertnum], indedhdnum = indedgennd = indedgenum + inddegrval;
         orgedgenum < orggrafptr->s.vendtax[orgvertnum]; orgedgenum ++) {
      Gnum                orgvertend;             /* Number of current end vertex in original halo graph   */
      Gnum                indvertend;             /* Number of current end vertex in induced halo subgraph */

      orgvertend = orggrafptr->s.edgetax[orgedgenum];
      indvertend = orgindxtax[orgvertend];
      if (indvertend == ~0) {                     /* If neighbor is yet undeclared halo vertex */
        indgrafptr->s.vnumtax[indvertnnd] = orgvertend; /* Add number of halo vertex to array  */
        indvertend = orgindxtax[orgvertend] = indvertnnd ++; /* Get induced number of vertex   */
      }
      if (indvertend >= indgrafptr->vnohnnd) {    /* If neighbor is halo vertex            */
        indedhdnum --;                            /* Add neighbor at end of edge sub-array */
        indgrafptr->s.edgetax[indedhdnum] = indvertend;
        HGRAPHINDUCE2EDLOINIT (indedhdnum);
      }
      else {                                      /* If heighbor is non-halo vertex                    */
        indgrafptr->s.edgetax[indedgenum] = indvertend; /* Add neighbor at beginning of edge sub-array */
        HGRAPHINDUCE2EDLOINIT (indedgenum);
        indedgenum ++;
      }
    }
#ifdef SCOTCH_DEBUG_HGRAPH2
    if (indedgenum != indedhdnum) {
      errorPrint ("HGRAPHINDUCE2NAME: internal error (1)");
      return;
    }
#endif /* SCOTCH_DEBUG_HGRAPH2 */
    indenohnbr += indedhdnum;                     /* Add position to number of non-halo edges */
    indgrafptr->vnhdtax[indvertnum] = indedhdnum; /* Set end of non-halo sub-array            */
    indedgenum = indedgennd;                      /* Point to next free space in edge array   */
  }
  indgrafptr->vnlosum = (indgrafptr->s.velotax != NULL) ? indvelosum : indgrafptr->vnohnbr;
  indgrafptr->enohnbr = indenohnbr;

#ifdef SCOTCH_DEBUG_HGRAPH2
  indedgenbs = 2 * (indedgenum - indgrafptr->s.baseval) - indenohnbr; /* Compute total number of edges */
#endif /* SCOTCH_DEBUG_HGRAPH2 */
#ifdef HGRAPHINDUCE2L                             /* If edge loads present */
  {
    Gnum *              indedgetab;               /* Dummy area to recieve un-based edgetab */
    Gnum *              indedlotab;               /* Save of old position of edgetab array  */
#ifndef SCOTCH_DEBUG_HGRAPH2
    Gnum                indedgenbs;               /* Revised number of edges in halo subgraph */

    indedgenbs = 2 * (indedgenum - indgrafptr->s.baseval) - indenohnbr; /* Compute total number of edges */
#endif /* SCOTCH_DEBUG_HGRAPH2 */

    indedlotab = indgrafptr->s.edlotax + indgrafptr->s.baseval; /* Save old offset of move area */
    memOffset (indgrafptr->s.edgetax + indgrafptr->s.baseval, /* Compute new offsets            */
               &indedgetab,            (size_t) indedgenbs,
               &indgrafptr->s.edlotax, (size_t) indedgenbs, NULL);
    memMov (indgrafptr->s.edlotax, indedlotab, (indedgenum - indgrafptr->s.baseval) * sizeof (Gnum)); /* Move already existing edge load array */
    indgrafptr->s.edlotax -= indgrafptr->s.baseval;
  }
#endif /* HGRAPHINDUCE2L */

  for ( ; indvertnum < indvertnnd; indvertnum ++) { /* For all halo vertices found during first pass */
    Gnum                orgvertnum;               /* Number of current vertex in original halo graph */
    Gnum                orgedgenum;               /* Number of current edge in original halo graph   */

    orgvertnum = indgrafptr->s.vnumtax[indvertnum];
    indgrafptr->s.verttax[indvertnum] = indedgenum;
    if (indgrafptr->s.velotax != NULL) {          /* If graph has vertex weights */
      indvelosum +=                               /* Accumulate vertex loads     */
      indgrafptr->s.velotax[indvertnum] = orggrafptr->s.velotax[orgvertnum];
    }

    for (orgedgenum = orggrafptr->s.verttax[orgvertnum];
         orgedgenum < orggrafptr->s.vendtax[orgvertnum]; orgedgenum ++) {
      Gnum                orgvertend;             /* Number of current end vertex in original halo graph   */
      Gnum                indvertend;             /* Number of current end vertex in induced halo subgraph */

      orgvertend = orggrafptr->s.edgetax[orgedgenum];
      indvertend = orgindxtax[orgvertend];
      if ((indvertend != ~0) &&                   /* If end vertex in induced halo subgraph */
          (indvertend < indgrafptr->vnohnnd)) {   /* And in its non-halo part only          */
        indgrafptr->s.edgetax[indedgenum] = indvertend;
        HGRAPHINDUCE2EDLOINIT(indedgenum);
        indedgenum ++;
      }
    }
    if (inddegrmax < (indedgenum - indgrafptr->s.verttax[indvertnum]))
      inddegrmax = (indedgenum - indgrafptr->s.verttax[indvertnum]);
  }
#ifdef SCOTCH_DEBUG_HGRAPH2
  if ((indedgenum - indgrafptr->s.baseval) != indedgenbs) {
    errorPrint ("HGRAPHINDUCE2NAME: internal error (2)");
    return;
  }
#endif /* SCOTCH_DEBUG_HGRAPH2 */
  indgrafptr->s.verttax[indvertnnd] = indedgenum; /* Set end of compact vertex array */
  indgrafptr->s.vertnbr = indvertnnd - indgrafptr->s.baseval;
  indgrafptr->s.vertnnd = indvertnnd;
  indgrafptr->s.edgenbr = indedgenum - indgrafptr->s.baseval; /* Set actual number of edges */
  indgrafptr->s.degrmax = inddegrmax;
  indgrafptr->s.velosum = (indgrafptr->s.velotax != NULL) ? indvelosum : indgrafptr->s.vertnbr;
}
