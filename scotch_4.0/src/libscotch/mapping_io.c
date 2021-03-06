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
/**   NAME       : mapping_io.c                            **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles (partial) mappings. **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 31 mar 1993     **/
/**                                 to     31 mar 1993     **/
/**                # Version 1.0  : from : 04 oct 1993     **/
/**                                 to     06 oct 1993     **/
/**                # Version 1.1  : from : 15 oct 1993     **/
/**                                 to     15 oct 1993     **/
/**                # Version 1.3  : from : 09 apr 1994     **/
/**                                 to     11 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     17 nov 1994     **/
/**                # Version 2.1  : from : 07 apr 1995     **/
/**                                 to     18 jun 1995     **/
/**                # Version 3.0  : from : 01 jul 1995     **/
/**                                 to     19 oct 1995     **/
/**                # Version 3.1  : from : 30 oct 1995     **/
/**                                 to     14 jun 1996     **/
/**                # Version 3.2  : from : 23 aug 1996     **/
/**                                 to     07 sep 1998     **/
/**                # Version 3.3  : from : 19 oct 1998     **/
/**                                 to     30 mar 1999     **/
/**                # Version 3.4  : from : 11 sep 2001     **/
/**                                 to     08 nov 2001     **/
/**                # Version 4.0  : from : 16 jan 2004     **/
/**                                 to     14 nov 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define MAPPING_IO

#include "module.h"
#include "common.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "mapping_io.h"

/***********************************/
/*                                 */
/* These routines handle mappings. */
/*                                 */
/***********************************/

/* This routine reads the contents of the given mapping
** file to the given mapping, reordering vertices
** according to the given vertex label table if necessary.
** It returns:
** - 0   : if mapping successfully written.
** - 1   : on error.
** - 2   : variable-sized architectures cannot be loaded.
*/

static
int
mapLoadSortMap (
MappingLoadMap * restrict   sort1ptr,
MappingLoadMap * restrict   sort2ptr)
{
  return ((sort1ptr->slblnum > sort2ptr->slblnum) ? 1 : -1);
}

static
int
mapLoadSortPerm (
MappingLoadPerm * restrict  sort1ptr,
MappingLoadPerm * restrict  sort2ptr)
{
  return ((sort1ptr->vlblnum > sort2ptr->vlblnum) ? 1 : -1);
}

int
mapLoad (
Mapping * restrict const        mappptr,
const Gnum * restrict const     vlbltab,
FILE * restrict const           stream)
{
  Gnum                  vertnum;
  Gnum                  mappnum;
  Gnum                  mappnbr;
  MappingLoadMap *      mapptab;                  /* Mapping array                     */
  MappingLoadPerm *     permtab;                  /* Array of sorted label/index pairs */
  Anum                  archnbr;                  /* Size of the target architecture   */
  ArchDom               fdomdat;                  /* First domain of architecture      */

  if (strcmp (archName (&mappptr->archdat), "term") == 0) /* If target architecture is variable-sized */
    return (2);

  archDomFrst (&mappptr->archdat, &fdomdat);      /* Get architecture size */
  archnbr = archDomSize (&mappptr->archdat, &fdomdat);
  if (mappptr->domnmax < (archnbr + 1)) {         /* If mapping array too small to store mapping data */
    ArchDom * restrict    domntab;

    if ((domntab = (ArchDom *) memRealloc (mappptr->domntab, (archnbr + 1) * sizeof (ArchDom))) == NULL) { /* If cannot resize domain array */
      errorPrint ("mapLoad: out of memory (1)");
      return     (1);
    }

    mappptr->domnmax = archnbr + 1;               /* Point to new array */
    mappptr->domntab = domntab;
  }
  mappptr->domnnbr = archnbr + 1;                 /* One more for first domain, for unmapped vertices                 */
  archDomFrst (&mappptr->archdat, &mappptr->domntab[0]); /* Set first domain with root domain data                    */
  for (mappnum = 0; mappnum < archnbr; mappnum ++) /* For all terminal domain numbers                                 */
    archDomTerm (&mappptr->archdat, &mappptr->domntab[mappnum + 1], mappnum); /* Set domain with terminal domain data */

  if ((intLoad (stream, &mappnbr) != 1) ||        /* Read number of mapping entries */
      (mappnbr < 1)) {
    errorPrint ("mapLoad: bad input (1)");
    return     (1);
  }

  if (memAllocGroup ((void **)
                     &mapptab, (size_t) (mappnbr          * sizeof (MappingLoadMap)),
                     &permtab, (size_t) (mappptr->vertnbr * sizeof (MappingLoadPerm)), NULL) == NULL) {
    errorPrint ("mapLoad: out of memory (2)");
    return     (1);
  }

  for (mappnum = 0; mappnum < mappnbr; mappnum ++) { /* Load mapping array */
    if ((intLoad (stream, &mapptab[mappnum].slblnum) +
         intLoad (stream, &mapptab[mappnum].tlblnum)) != 2) {
      errorPrint ("mapLoad: bad input (2)");
      return     (1);
    }
  }
  qsort ((char *) mapptab, mappnbr,               /* Sort mapping array by increasing source labels */
         sizeof (MappingLoadMap), (int (*) (const void *, const void *)) mapLoadSortMap);

  if (vlbltab != NULL) {                          /* If graph has vertex labels */
    Gnum                vertnum;

    for (vertnum = 0; vertnum < mappptr->vertnbr; vertnum ++) { /* Build inverse permutation */
      permtab[vertnum].vertnum = vertnum + mappptr->baseval;
      permtab[vertnum].vlblnum = vlbltab[vertnum];
    }
    qsort ((char *) permtab, mappptr->vertnbr,    /* Sort vertex array by increasing labels */
           sizeof (MappingLoadPerm), (int (*) (const void *, const void *)) mapLoadSortPerm);
  }
  else {
    Gnum                vertnum;

    for (vertnum = 0; vertnum < mappptr->vertnbr; vertnum ++) { /* Build identity permutation */
      permtab[vertnum].vertnum = vertnum + mappptr->baseval;
      permtab[vertnum].vlblnum = vertnum;
    }
  }

  for (vertnum = 0, mappnum = 0;                  /* For all graph vertices */
       vertnum < mappptr->vertnbr; vertnum ++) {
    while ((mappnum < mappnbr) &&                 /* Skip useless mapping data (if graph is subgraph of originally mapped graph) */
           (permtab[vertnum].vlblnum > mapptab[mappnum].slblnum))
      mappnum ++;
    if (mappnum >= mappnbr)                       /* If all mapping data exhausted */
      break;                                      /* Exit the matching loop        */

    if (permtab[vertnum].vlblnum == mapptab[mappnum].slblnum) { /* If matching mapping data found */
      if ((mapptab[mappnum].tlblnum >= 0) &&      /* If mapping valid                             */
          (mapptab[mappnum].tlblnum < archnbr))
        mappptr->parttax[permtab[vertnum].vertnum] = mapptab[mappnum].tlblnum + 1; /* Set mapping to terminal domain */
      mappnum ++;                                 /* Mapping pair has been used                               */
    }
  }

  memFree (mapptab);                              /* Free group leader */

  return (0);
}

/* This routine writes the contents of the
** given mapping to the given string.
** It returns:
** - 0   : if mapping successfully written.
** - !0  : on error.
*/

int
mapSave (
const Mapping * restrict const  mappptr,
const Gnum * restrict const     vlbltab,
FILE * restrict const           stream)
{
  const Gnum * restrict vlbltax;
  Gnum                  vertnum;

  vlbltax = (vlbltab != NULL) ? (vlbltab - mappptr->baseval) : NULL;

  if (fprintf (stream, "%ld\n", (long) mappptr->vertnbr) == EOF) {
    errorPrint ("mapSave: bad output (1)");
    return     (1);
  }

  for (vertnum = mappptr->baseval; vertnum < (mappptr->vertnbr + mappptr->baseval); vertnum ++) {
    if (fprintf (stream, "%ld\t%ld\n",
                 (long) ((vlbltax != NULL) ? vlbltax[vertnum] : vertnum),
                 (long) archDomNum (&mappptr->archdat, &mappptr->domntab[mappptr->parttax[vertnum]])) == EOF) {
      errorPrint ("mapSave: bad output (2)");
      return     (1);
    }
  }

  return (0);
}
