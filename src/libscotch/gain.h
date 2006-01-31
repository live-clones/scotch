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
/**   NAME       : gain.h                                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the definitions of **/
/**                the generic gain tables.                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 26 oct 1996     **/
/**                                 to     17 nov 1997     **/
/**              : # Version 0.1  : from : 10 may 1999     **/
/**                                 to     18 mar 2005     **/
/**                                                        **/
/**   NOTES      : # Most of the contents of this module   **/
/**                  comes from "map_b_fm" of the SCOTCH   **/
/**                  project.                              **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

#define GAINMAX                  ((int) (((unsigned int) 1 << ((sizeof (int) << 3) - 1)) - 2))

#define GAIN_LINMAX              1024

/*
**  The type and structure definitions.
*/

/* The gain link data structure. This must be the
   first item of objects that are linked into gain
   tables.                                         */

typedef struct GainLink_ {
  struct GainLink_ *        next;                 /*+ Pointer to next element: FIRST +*/
  struct GainLink_ *        prev;                 /*+ Pointer to previous element    +*/
  struct GainEntr_ *        tabl;                 /*+ Index into the gain table      +*/
} GainLink;

/* Gain table entry structure. */

typedef struct GainEntr_ {
  GainLink *                next;                 /*+ Pointer to first element: FIRST +*/
} GainEntr;

/* The gain table structure, built from table entries.
   For trick reasons, the pointer to the first entry
   must be the first field of the structure.           */

typedef struct GainTabl_ {
  void                   (* tablAdd)  ();         /*+ Add method                                +*/
  INT                       subbits;              /*+ Number of subbits                         +*/
  INT                       submask;              /*+ Subbit mask                               +*/
  INT                       totsize;              /*+ Total table size                          +*/
  GainEntr *                tmin;                 /*+ Non-empty entry of minimum gain in table  +*/
  GainEntr *                tmax;                 /*+ Non-empty entry of maximum gain in table  +*/
  GainEntr *                tend;                 /*+ Point after last valid gain entry         +*/
  GainEntr *                tabl;                 /*+ Gain table structure is...   [SIZE - ADJ] +*/
  GainEntr                  tabk[1];              /*+ Split in two for relative access    [ADJ] +*/
} GainTabl;

/*
**  The function prototypes.
*/

#ifndef GAIN
#define static
#endif

GainTabl *                  gainTablInit        (const INT, const INT);
void                        gainTablExit        (GainTabl * const);
void                        gainTablFree        (GainTabl * const);
void                        gainTablAddLin      (GainTabl * const, GainLink * const, const INT gain);
void                        gainTablAddLog      (GainTabl * const, GainLink * const, const INT gain);
void                        gainTablDel         (GainTabl * const, GainLink * const);
GainLink *                  gainTablFrst        (GainTabl * const);
GainLink *                  gainTablNext        (GainTabl * const, const GainLink * const);
#ifdef SCOTCH_DEBUG_GAIN3
int                         gainTablCheck       (GainEntr * const);
static int                  gainTablCheck2      (GainEntr * const, GainLink * const);
#endif /* SCOTCH_DEBUG_GAIN3 */

#undef static

/*
**  The marco definitions.
*/

#define gainTablEmpty(tabl)         ((tabl)->tmin == (tabl)->tend)
#define gainTablAdd(tabl,link,gain) ((tabl)->tablAdd  ((tabl), (link), (gain)))
#if ((! defined GAIN) && (! defined SCOTCH_DEBUG_GAIN1))
#define gainTablDel(tabl,link)      (((GainLink *) (link))->next->prev = ((GainLink *) (link))->prev, \
                                     ((GainLink *) (link))->prev->next = ((GainLink *) (link))->next)
#endif /* ((! defined GAIN) && (! defined SCOTCH_DEBUG_GAIN1)) */
