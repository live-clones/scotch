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
/**   NAME       : bipart_fm.c                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module bipartitions an active      **/
/**                graph using our improvements of the     **/
/**                Fiduccia-Mattheyses heuristics.         **/
/**                                                        **/
/**   DATES      : # Version 1.0  : from : 30 sep 1993     **/
/**                                 to     09 oct 1993     **/
/**                # Version 1.1  : from : 15 oct 1993     **/
/**                                 to     15 oct 1993     **/
/**                # Version 1.2  : from : 07 feb 1994     **/
/**                                 to     15 feb 1994     **/
/**                # Version 1.3  : from : 06 apr 1994     **/
/**                                 to     30 apr 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     03 nov 1994     **/
/**                # Version 3.1  : from : 06 nov 1995     **/
/**                                 to     07 jun 1996     **/
/**                # Version 3.2  : from : 21 sep 1996     **/
/**                                 to     13 sep 1998     **/
/**                # Version 3.3  : from : 01 oct 1998     **/
/**                                 to     12 mar 1999     **/
/**                # Version 3.4  : from : 01 jun 2001     **/
/**                                 to     01 jun 2001     **/
/**                # Version 4.0  : from : 20 dec 2003     **/
/**                                 to     05 may 2006     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define BGRAPH_BIPART_FM

#include "module.h"
#include "common.h"
#include "gain.h"
#include "graph.h"
#include "arch.h"
#include "mapping.h"
#include "bgraph.h"
#include "bgraph_bipart_fm.h"
#include "bgraph_bipart_gg.h"

/*********************************/
/*                               */
/* Gain table handling routines. */
/*                               */
/*********************************/

/* This routine returns the vertex of best gain
** whose swap will keep the balance correct.
** It returns:
** - !NULL  : pointer to the vertex.
** - NULL   : if no more vertices available.
*/

static
BgraphBipartFmVertex *
bgraphBipartFmTablGet (
GainTabl * restrict const   tablptr,              /*+ Gain table        +*/
const Gnum                  deltcur,              /*+ Current imbalance +*/
const Gnum                  deltmax)              /*+ Maximum imbalance +*/
{
  BgraphBipartFmVertex * restrict vexxptr;
  BgraphBipartFmVertex * restrict vertbest;
  Gnum                            gainbest;
  const GainEntr * restrict       tablbest;
  Gnum                            deltbest;

  tablbest = tablptr->tend;                       /* Assume no candidate vertex found yet */
  gainbest = GAINMAX;
  vertbest = NULL;
  deltbest = deltmax;

  for (vexxptr = (BgraphBipartFmVertex *) gainTablFrst (tablptr); /* Select candidate vertices */
       (vexxptr != NULL) && (vexxptr->gainlink.tabl < tablbest);
       vexxptr = (BgraphBipartFmVertex *) gainTablNext (tablptr, &vexxptr->gainlink)) {
    Gnum                deltnew;

    deltnew = abs (deltcur + vexxptr->compgain);
    if (deltnew <= deltmax) {                     /* If vertex enforces balance  */
      if ((vexxptr->commgain < gainbest) ||       /* And if it gives better gain */
          ((vexxptr->commgain == gainbest) &&     /* Or if it gives better load  */
           (deltnew < deltbest))) {
        tablbest = vexxptr->gainlink.tabl;        /* Select it */
        gainbest = vexxptr->commgain;
        vertbest = vexxptr;
        deltbest = deltnew;
      }
    }
  }

  return (vertbest);
}

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

/* This routine performs the bipartitioning.
** It returns:
** - 0 : if bipartitioning could be computed.
** - 1 : on error.
*/

int
bgraphBipartFm (
Bgraph * restrict const           grafptr,        /*+ Active graph      +*/
const BgraphBipartFmParam * const paraptr)        /*+ Method parameters +*/
{
  GainTabl * restrict             tablptr;        /* Pointer to gain table                 */
  INT                             passnbr;        /* Maximum number of passes to go        */
  BgraphBipartFmSave * restrict   savetab;        /* Pointer to move array                 */
  Gnum                            movenbr;        /* Number of uneffective moves done      */
  Gnum                            savenbr;        /* Number of recorded backtrack moves    */
  Gnum                            mswpnum;        /* Current number of recording sweep     */
  int                             moveflag;       /* Flag set if useful moves made         */
  int                             swapval;        /* Flag set if global swap performed     */
  int                             swapvalbst;     /* Recorded swap value for best position */
  Gnum                            hashsiz;        /* Size of hash table                    */
  Gnum                            hashmsk;        /* Mask for access to hash table         */
  Gnum                            hashnum;        /* Hash value                            */
  BgraphBipartFmVertex *          lockptr;        /* Linked list of locked vertices        */
  BgraphBipartFmVertex * restrict hashtab;        /* Extended vertex array                 */
  Gnum                            hashmax;
  Gnum                            hashnbr;

  Gnum               fronnbr;
  Gnum               compload0dltmat;            /* Theoretical latgest unbalance allowed   */
  Gnum               compload0dltmax;            /* Largest unbalance allowed               */
  Gnum               compload0dltbst;            /* Best unbalance value found to date      */
  Gnum               compload0dlt;               /* Current imbalance                       */
  Gnum               compsize0dlt;               /* Update of size of part 0                */
  Gnum               commgainextn;               /* Current external communication gain     */
  Gnum               commgainextnbst;            /* External gain of best recorded position */
  Gnum               commload;                   /* Communication load of current position  */
  Gnum               commloadbst;                /* Best communication load to date         */
  Gnum               domdist;                    /* Distance between the two subdomains     */
  Gnum               fronnum;

  compload0dltmat = (paraptr->deltrat > 0.0L) ? ((Gnum) (paraptr->deltrat * (double) grafptr->s.velosum) + 1) : 0;
  compload0dltmax = MAX (compload0dltmat, abs (grafptr->compload0dlt)); /* Set current maximum distance */

  if (grafptr->fronnbr == 0) {                    /* If no current frontier      */
    if (abs (grafptr->compload0dlt) <= compload0dltmat) /* If balance is correct */
      return (0);                                 /* Nothing to do               */
    else {                                        /* Imbalance must be fought    */
      BgraphBipartGgParam   paradat;

      paradat.passnbr = 4;                        /* Use a standard algorithm */
      bgraphBipartGg (grafptr, &paradat);
      if (grafptr->fronnbr == 0)                  /* If new partition has no frontier */
        return (0);                               /* This algorithm is still useless  */
      compload0dltmax = MAX (compload0dltmat, abs (grafptr->compload0dlt));
    }
  }

#ifdef SCOTCH_DEBUG_BGRAPH2
  hashnbr = 2 * grafptr->fronnbr + 1;             /* Ensure resizing will be performed, for maximum code coverage */
#else /* SCOTCH_DEBUG_BGRAPH2 */
  hashnbr = 4 * (grafptr->fronnbr + paraptr->movenbr + grafptr->s.degrmax);
#endif /* SCOTCH_DEBUG_BGRAPH2 */
  if (hashnbr > grafptr->s.vertnbr)
    hashnbr = grafptr->s.vertnbr;

  for (hashsiz = 256; hashsiz < hashnbr; hashsiz <<= 1) ; /* Get upper power of two */
  hashmsk = hashsiz - 1;
  hashmax = hashsiz >> 2;

  if ((tablptr = gainTablInit (GAIN_LINMAX, BGRAPHBIPARTFMSUBBITS)) != NULL) {
    void *                          hashtmp;      /* Temporary variable for vertex array to avoid problems wirh "restrict" */

    if (memAllocGroup ((void **)
                       &hashtmp, (size_t) (hashsiz * sizeof (BgraphBipartFmVertex)),
                       &savetab, (size_t) (hashsiz * sizeof (BgraphBipartFmSave)), NULL) == NULL) {
      errorPrint   ("bgraphBipartFm: out of memory (1)");
      gainTablExit (tablptr);
      return       (1);
    }
    hashtab = hashtmp;
  }
  else
    return (1);  
  memset (hashtab, ~0, hashsiz * sizeof (BgraphBipartFmVertex)); /* Set all vertex numbers to ~0 */

  domdist = grafptr->domdist;

  for (fronnum = 0, hashnbr = grafptr->fronnbr;   /* Set initial gains */
       fronnum < hashnbr; fronnum ++) {
    Gnum                vertnum;
    Gnum                veloval;
    Gnum                hashnum;
    Gnum                edgenum;
    Gnum                edloval;
    Gnum                commcut;
    Gnum                commgain;
    int                 partval;
    int                 partdlt;

    vertnum = grafptr->frontab[fronnum];
    partval = grafptr->parttax[vertnum];

    for (edgenum = grafptr->s.verttax[vertnum], commcut = commgain = 0, edloval = 1;
         edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
      Gnum                vertend;
      int                 partend;
      int                 partdlt;

      vertend = grafptr->s.edgetax[edgenum];
      partend = grafptr->parttax[vertend];
      if (grafptr->s.edlotax != NULL)
        edloval = grafptr->s.edlotax[edgenum];

      partdlt   = partval ^ partend;
      commcut  += partdlt;
      commgain += (1 - 2 * partdlt) * edloval;
    }
    commgain *= domdist;                          /* Adjust internal gains with respect to external gains */
    partdlt   = 2 * partval - 1;
    veloval   = (grafptr->s.velotax != NULL) ? grafptr->s.velotax[vertnum] : 1;

    for (hashnum = (vertnum * BGRAPHBIPARTFMHASHPRIME) & hashmsk; hashtab[hashnum].vertnum != ~0; hashnum = (hashnum + 1) & hashmsk) ;

    hashtab[hashnum].vertnum  = vertnum;          /* Implicitely set slot as used */
    hashtab[hashnum].partval  = partval;
    hashtab[hashnum].compgain = partdlt * veloval;
    hashtab[hashnum].commgain = (grafptr->veextax == NULL) ? commgain : (commgain - partdlt * grafptr->veextax[vertnum]);
    hashtab[hashnum].commcut  = commcut;
    hashtab[hashnum].mswpnum  = 0;                /* Implicitely set slot as used */
    gainTablAdd (tablptr, &hashtab[hashnum], hashtab[hashnum].commgain);
  }

  compload0dltbst = grafptr->compload0dlt;
  commloadbst     = grafptr->commload;
  commgainextnbst = grafptr->commgainextn;
  swapvalbst      = 0;                            /* No global swap performed yet */

#ifdef SCOTCH_DEBUG_BGRAPH2
#ifdef SCOTCH_DEBUG_BGRAPH3
  if (bgraphBipartFmCheck (grafptr, hashtab, hashmsk, 0, compload0dltbst, commloadbst, commgainextnbst) != 0) {
    errorPrint ("bgraphBipartFm: internal error (1)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH3 */
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  passnbr = paraptr->passnbr;                     /* Set remaining number of passes    */
  savenbr = 0;                                    /* For empty backtrack of first pass */
  mswpnum = 0;                                    /* Will be incremented afterwards    */
  lockptr = NULL;                                 /* Locked list is empty              */

  do {                                            /* As long as there are improvements */
    BgraphBipartFmVertex *    vexxptr;

    while (savenbr -- > 0) {                      /* Delete exceeding moves */
      Gnum                hashnum;
      int                 partval;

      hashnum = savetab[savenbr].hashnum;
      partval = savetab[savenbr].partval;
      hashtab[hashnum].partval  = partval;        /* Restore vertex data */
      hashtab[hashnum].compgain = savetab[savenbr].compgain;
      hashtab[hashnum].commgain = savetab[savenbr].commgain;
      hashtab[hashnum].commcut  = savetab[savenbr].commcut;

      if (hashtab[hashnum].gainlink.next >= BGRAPHBIPARTFMSTATELINK) { /* If vertex is linked */
        gainTablDel (tablptr, &hashtab[hashnum].gainlink); /* Unlink it                       */
        hashtab[hashnum].gainlink.next = BGRAPHBIPARTFMSTATEFREE; /* Set it as free           */
      }
      if ((hashtab[hashnum].gainlink.next == BGRAPHBIPARTFMSTATEFREE) && (partval == 2)) /* If vertex not locked and in separator */
        gainTablAdd (tablptr, &hashtab[hashnum].gainlink, hashtab[hashnum].commgain); /* Re-link it                               */
    }
    compload0dlt = compload0dltbst;               /* Restore best separator parameters */
    commload     = commloadbst;
    commgainextn = commgainextnbst;
    swapval      = swapvalbst;
    mswpnum ++;                                   /* Forget all recorded moves */

#ifdef SCOTCH_DEBUG_BGRAPH2
#ifdef SCOTCH_DEBUG_BGRAPH3
    if (bgraphBipartFmCheck (grafptr, hashtab, hashmsk, swapval, compload0dlt, commload, commgainextn) != 0) {
      errorPrint ("bgraphBipartFm: internal error (2)");
      return     (1);
    }
#endif /* SCOTCH_DEBUG_BGRAPH3 */
#endif /* SCOTCH_DEBUG_BGRAPH2 */

    while (lockptr != NULL) {                     /* For all vertices in locked list */
      BgraphBipartFmVertex *    vexxptr;

      vexxptr = lockptr;                          /* Unlink vertex from list */
      lockptr = (BgraphBipartFmVertex *) vexxptr->gainlink.prev;

      if (vexxptr->commcut > 0)                   /* If vertex has cut edges  */
        gainTablAdd (tablptr, vexxptr, vexxptr->commgain); /* Put it in table */
      else
        vexxptr->gainlink.next = BGRAPHBIPARTFMSTATEFREE; /* Set it free anyway */
    }

    moveflag = 0;                                 /* No useful moves made              */
    movenbr  = 0;                                 /* No uneffective moves recorded yet */
    savenbr  = 0;                                 /* Back up to beginning of table     */

    while ((movenbr < paraptr->movenbr) &&        /* As long as we can find effective vertices */
           ((vexxptr = (BgraphBipartFmVertex *) bgraphBipartFmTablGet (tablptr, compload0dlt, compload0dltmax)) != NULL)) {
      Gnum               vertnum;                 /* Number of current vertex */
      int                partval;                 /* Part of current vertex   */
      Gnum               edgenum;
      Gnum               edloval;

      gainTablDel (tablptr, &vexxptr->gainlink);  /* Remove it from table  */
      vexxptr->gainlink.next = BGRAPHBIPARTFMSTATEUSED; /* Mark it as used */
      vexxptr->gainlink.prev = (GainLink *) lockptr; /* Lock it            */
      lockptr                = vexxptr;

      vertnum = vexxptr->vertnum;
      partval = vexxptr->partval;

      if (vexxptr->mswpnum != mswpnum) {          /* If vertex data not yet recorded */
        vexxptr->mswpnum = mswpnum;
        savetab[savenbr].hashnum  = vexxptr - hashtab;
        savetab[savenbr].partval  = partval;
        savetab[savenbr].compgain = vexxptr->compgain;
        savetab[savenbr].commgain = vexxptr->commgain;
        savetab[savenbr].commcut  = vexxptr->commcut;
        savenbr ++;                               /* One more move recorded */
      }
      movenbr ++;                                 /* One more move done */

      commload     += vexxptr->commgain;
      compload0dlt += vexxptr->compgain;
      if (grafptr->veextax != NULL)
        commgainextn += 2 * (2 * partval - 1) * grafptr->veextax[vertnum];

      vexxptr->partval  = partval ^ 1;            /* Swap vertex first in case neighbors are added */
      vexxptr->compgain = - vexxptr->compgain;
      vexxptr->commgain = - vexxptr->commgain;
      vexxptr->commcut  = grafptr->s.vendtax[vertnum] - grafptr->s.verttax[vertnum] - vexxptr->commcut;

      edloval = 1;
      for (edgenum = grafptr->s.verttax[vertnum]; /* (Re-)link neighbors */
           edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
        Gnum                vertend;              /* Number of current end neighbor vertex */
        Gnum                hashnum;

        vertend = grafptr->s.edgetax[edgenum];
        if (grafptr->s.edlotax != NULL)
          edloval = grafptr->s.edlotax[edgenum];

        for (hashnum = (vertend * BGRAPHBIPARTFMHASHPRIME) & hashmsk; ; hashnum = (hashnum + 1) & hashmsk) {
          if (hashtab[hashnum].vertnum == vertend) { /* If hash slot found */
            int                 partdlt;

            if (hashtab[hashnum].mswpnum != mswpnum) { /* If vertex data not yet recorded */
              savetab[savenbr].hashnum  = hashnum; /* Record them                         */
              savetab[savenbr].partval  = hashtab[hashnum].partval;
              savetab[savenbr].compgain = hashtab[hashnum].compgain;
              savetab[savenbr].commgain = hashtab[hashnum].commgain;
              savetab[savenbr].commcut  = hashtab[hashnum].commcut;
              hashtab[hashnum].mswpnum  = mswpnum;
              savenbr ++;
            }

            partdlt = 2 * (partval ^ hashtab[hashnum].partval) - 1;
            hashtab[hashnum].commgain += (domdist * 2) * edloval * partdlt;
            hashtab[hashnum].commcut  -= partdlt;

            if (hashtab[hashnum].gainlink.next != BGRAPHBIPARTFMSTATEUSED) { /* If vertex is of use   */
              if (hashtab[hashnum].gainlink.next >= BGRAPHBIPARTFMSTATELINK) { /* If vertex is linked */
                gainTablDel (tablptr, &hashtab[hashnum].gainlink); /* Remove it from table            */
                hashtab[hashnum].gainlink.next = BGRAPHBIPARTFMSTATEFREE; /* Mark it as free anyway   */
              }
              if (hashtab[hashnum].commcut > 0)   /* If vertex belongs to the frontier */
                gainTablAdd (tablptr, &hashtab[hashnum].gainlink, hashtab[hashnum].commgain); /* Re-link it */
            }
            break;
          }
          if (hashtab[hashnum].vertnum == ~0) {   /* If hash slot empty */
            Gnum                commgain;         /* Communication gain of current vertex     */
            Gnum                commgainold;      /* Old communication gain of current vertex */
            Gnum                veloval;
            Gnum                veexval;
            int                 partold;
            int                 partdlt;

            if (hashnbr >= hashmax) {             /* If extended vertex table is already full */
              if (bgraphBipartFmResize (&hashtab, &hashmax, &hashmsk, &savetab, savenbr, tablptr, &lockptr) != 0) {
                errorPrint   ("bgraphBipartFm: out of memory (2)");
                memFree      (hashtab);           /* Free group leader */
                gainTablExit (tablptr);
              }
              for (hashnum = (vertend * BGRAPHBIPARTFMHASHPRIME) & hashmsk; hashtab[hashnum].vertnum != ~0; hashnum = (hashnum + 1) & hashmsk) ; /* Search for new first free slot */
            }

            if (grafptr->s.edlotax != NULL) {     /* If graph edges are weighted */
              Gnum               edgeend;

              for (edgeend = grafptr->s.verttax[vertend], commgainold = 0; /* Compute neighbor edge load sum */
                   edgeend < grafptr->s.vendtax[vertend]; edgeend ++)
                commgainold += grafptr->s.edlotax[edgeend];
              commgain = commgainold - 2 * edloval;
            }
            else {                                /* Graph edges are not weighted */
              commgainold = grafptr->s.vendtax[vertend] - grafptr->s.verttax[vertend];
              commgain    = commgainold - 2;
            }

            veloval = 1;
            if (grafptr->s.velotax != NULL)
              veloval = grafptr->s.velotax[vertend];
            veexval = 0;
            if (grafptr->veextax != NULL)
              veexval = grafptr->veextax[vertend];

#ifdef SCOTCH_DEBUG_BGRAPH2
            if (grafptr->parttax[vertend] != (partval ^ swapval)) {
              errorPrint ("bgraphBipartFm: internal error (3)");
              return     (1);
            }
#endif /* SCOTCH_DEBUG_BGRAPH2 */
            partold = partval ^ swapval ^ swapvalbst; /* Get part of vertex as in latest accepted configuration               */
            partdlt = 2 * partold - 1;            /* Compute values to fill save table according to last stable configuration */
            savetab[savenbr].hashnum  = hashnum;  /* Record initial state of new vertex                                       */
            savetab[savenbr].partval  = partold;
            savetab[savenbr].compgain = partdlt * veloval;
            savetab[savenbr].commgain = commgainold * domdist - (partdlt * veexval);
            savetab[savenbr].commcut  = 0;
            savenbr ++;

            partdlt = 2 * partval - 1;            /* Compute values to fill hash table according to current configuration */
            hashtab[hashnum].vertnum  = vertend;
            hashtab[hashnum].partval  = partval;  /* It was a neighbor of the moved vertex, in current global swap state */
            hashtab[hashnum].compgain = partdlt * veloval;
            hashtab[hashnum].commgain = commgain * domdist - (partdlt * veexval);
            hashtab[hashnum].commcut  = 1;
            hashtab[hashnum].mswpnum  = mswpnum;  /* Vertex has just been saved    */
            hashnbr ++;                           /* One more vertex in hash table */

            gainTablAdd (tablptr, &hashtab[hashnum], hashtab[hashnum].commgain);
            break;
          }
        }
      }

      if (commload < commloadbst) {               /* If move improves the cost */
        compload0dltbst = compload0dlt;           /* This move was effective   */
        commloadbst     = commload;
        commgainextnbst = commgainextn;
        swapvalbst      = swapval;
        moveflag        = 1;
        movenbr         =
        savenbr         = 0;
        mswpnum ++;
      } else if (commload == commloadbst) {
        if (abs (compload0dlt) < abs (compload0dltbst)) {
          compload0dltbst = compload0dlt;         /* This move was effective */
          commgainextnbst = commgainextn;
          swapvalbst      = swapval;
          moveflag        = 1;
          movenbr         =
          savenbr         = 0;
          mswpnum ++;
        }
        else if (abs (compload0dlt) == abs (compload0dltbst)) {
          compload0dltbst = compload0dlt;         /* Forget backtracking */
          commgainextnbst = commgainextn;
          swapvalbst      = swapval;
          savenbr         = 0;                   
          mswpnum ++;
        }
      }
      if (compload0dltmax > compload0dltmat) {    /* If must restrict distance bounds */
        Gnum                compload0dlttmp;

        compload0dlttmp = compload0dltmax;        /* Save old working compdeltmax value */
        compload0dltmax = MAX (compload0dltmat,   /* Restrict at most to maximum        */
                               abs (compload0dlt));
        if (compload0dltmax < compload0dlttmp) {  /* If we have done something useful */
          compload0dltbst = compload0dlt;         /* Then record best move done       */
          commloadbst     = commload;
          commgainextnbst = commgainextn;
          swapvalbst      = swapval;
          moveflag        = 1;
          movenbr         =
          savenbr         = 0;
          mswpnum ++;
        }
      }
#ifdef SCOTCH_DEBUG_BGRAPH2
#ifdef SCOTCH_DEBUG_BGRAPH3
      if (bgraphBipartFmCheck (grafptr, hashtab, hashmsk, swapval, compload0dlt, commload, commgainextn) != 0) {
        errorPrint ("bgraphBipartFm: internal error (4)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_BGRAPH3 */
#endif /* SCOTCH_DEBUG_BGRAPH2 */

      if (commgainextn < 0) {                     /* If global swap improves gain */
        Gnum                compload0dlttmp;

#ifdef SCOTCH_DEBUG_BGRAPH2
        if (grafptr->veextax == NULL) {           /* commgainextn should always be 0 if (veextab == NULL) */
          errorPrint ("bgraphBipartFm: internal error (5)");
          return     (1);
        }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

        compload0dlttmp = grafptr->s.velosum - compload0dlt - 2 * grafptr->compload0avg;
        if (abs (compload0dlttmp) <= compload0dltmax) { /* If still within bounds, perform actual swapping */
          Gnum                    hashnum;

          commload    +=   commgainextn;          /* Perform global swap */
          commgainextn = - commgainextn;
          compload0dlt =   compload0dlttmp;
          swapval     ^= 1;

          for (hashnum = 0; hashnum <= hashmsk; hashnum ++) { /* hashsiz no longer valid after resizing, so use hashmsk */
            Gnum                commgain;

            if (hashtab[hashnum].mswpnum == ~0)   /* If hash slot not used, skip it */
              continue;

            if (hashtab[hashnum].mswpnum != mswpnum) { /* And vertex data not yet recorded */
              hashtab[hashnum].mswpnum  = mswpnum; /* Record them                          */
              savetab[savenbr].hashnum  = hashnum;
              savetab[savenbr].partval  = hashtab[hashnum].partval;
              savetab[savenbr].compgain = hashtab[hashnum].compgain;
              savetab[savenbr].commgain = hashtab[hashnum].commgain;
              savetab[savenbr].commcut  = hashtab[hashnum].commcut;
              savenbr ++;
            }

            hashtab[hashnum].partval ^= 1;        /* Swap the vertex */
            hashtab[hashnum].compgain = - hashtab[hashnum].compgain;

            commgain = grafptr->veextax[hashtab[hashnum].vertnum];
            if (commgain != 0) {                  /* If vertex has external cocycle edges                         */
              hashtab[hashnum].commgain += 2 * (1 - 2 * hashtab[hashnum].partval) * commgain; /* Compute new gain */
              if (hashtab[hashnum].gainlink.next >= BGRAPHBIPARTFMSTATELINK) { /* If vertex is linked             */
                gainTablDel (tablptr, &hashtab[hashnum].gainlink); /* Remove it from table                        */
                gainTablAdd (tablptr, &hashtab[hashnum].gainlink, hashtab[hashnum].commgain); /* Re-link it       */
              }
            }
          }

          if ((commload < commloadbst) ||         /* If move improves cost */
              ((commload == commloadbst) &&
               (abs (compload0dlt) < abs (compload0dltbst)))) {
            compload0dltbst = compload0dlt;       /* Then record best move done */
            commloadbst     = commload;
            commgainextnbst = commgainextn;
            swapvalbst      = swapval;
            moveflag        = 1;
            movenbr         =
            savenbr         = 0;
            mswpnum ++;
          }

#ifdef SCOTCH_DEBUG_BGRAPH2
#ifdef SCOTCH_DEBUG_BGRAPH3
          if (bgraphBipartFmCheck (grafptr, hashtab, hashmsk, swapval, compload0dlt, commload, commgainextn) != 0) {
            errorPrint ("bgraphBipartFm: internal error (6)");
            return     (1);
          }
#endif /* SCOTCH_DEBUG_BGRAPH3 */
#endif /* SCOTCH_DEBUG_BGRAPH2 */
        }
      }
    }
  } while ((moveflag != 0) &&                     /* As long as vertices are moved */
           (-- passnbr > 0));                     /* And we are allowed to loop    */

#ifdef SCOTCH_DEBUG_BGRAPH2
#ifdef SCOTCH_DEBUG_BGRAPH3
  if (bgraphBipartFmCheck (grafptr, hashtab, hashmsk, swapval, compload0dlt, commload, commgainextn) != 0) {
    errorPrint ("bgraphBipartFm: internal error (7)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH3 */
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  compsize0dlt = 0;                               /* No difference to number of vertices yet */
  if (swapvalbst != 0) {                          /* If global swap needed                   */
    Gnum                vertnum;

    compsize0dlt = grafptr->s.vertnbr - 2 * grafptr->compsize0; /* Set difference so as to swap all vertices        */
    for (vertnum = grafptr->s.baseval; vertnum < grafptr->s.vertnnd; vertnum ++) /* Swap all vertices in part array */
      grafptr->parttax[vertnum] ^= 1;
  }

  while (savenbr -- > 0) {                        /* Delete exceeding moves */
    Gnum                hashnum;

    hashnum = savetab[savenbr].hashnum;
    hashtab[hashnum].partval = savetab[savenbr].partval; /* Restore vertex data */
    hashtab[hashnum].commcut = savetab[savenbr].commcut;
  }
  compload0dlt = compload0dltbst;                 /* Restore best separator parameters */
  commload     = commloadbst;
  commgainextn = commgainextnbst;

  for (hashnum = fronnbr = 0;                     /* Build new frontier                                     */
       hashnum <= hashmsk; hashnum ++) {          /* hashsiz no longer valid after resizing, so use hashmsk */
    Gnum                vertnum;
    int                 partval;

    vertnum = hashtab[hashnum].vertnum;           /* Get vertex data from slot */
    if (vertnum == ~0)
      continue;

    partval = hashtab[hashnum].partval;

    if (grafptr->parttax[vertnum] != partval) {   /* If vertex part changed            */
      grafptr->parttax[vertnum] = partval;        /* Set new part value                */
      compsize0dlt += (1 - 2 * partval);          /* Adjust size of part 0 accordingly */
    }
    if (hashtab[hashnum].commcut > 0)             /* If vertex belongs to cut */
      grafptr->frontab[fronnbr ++] = vertnum;     /* Add vertex to frontier   */
  }
  grafptr->fronnbr      = fronnbr;
  grafptr->compload0    = compload0dlt + grafptr->compload0avg;
  grafptr->compload0dlt = compload0dlt;
  grafptr->compsize0   += compsize0dlt;
  grafptr->commload     = commload;
  grafptr->commgainextn = commgainextn;

#ifdef SCOTCH_DEBUG_BGRAPH2
  if (bgraphCheck (grafptr) != 0) {
    errorPrint ("bgraphBipartFm: inconsistent graph data");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  memFree      (hashtab);                         /* Free group leader */
  gainTablExit (tablptr);

  return (0);
}

/* This routine doubles the size all of the arrays
** involved in handling the hash table and hash
** vertex arrays.
** It returns:
** - 0   : if resizing succeeded.
** - !0  : if out of memory.
*/

static
int
bgraphBipartFmResize (
BgraphBipartFmVertex * restrict * hashtabptr,     /*+ Extended vertex array      +*/
Gnum * restrict const             hashmaxptr,     /*+ Size of vertex array       +*/
Gnum * const                      hashmskptr,     /*+ Pointer to hash table mask +*/
BgraphBipartFmSave * restrict *   savetabptr,     /*+ Move array                 +*/
const Gnum                        savenbr,        /*+ Number of moves recorded   +*/
GainTabl * const                  tablptr,        /*+ Gain table                 +*/
BgraphBipartFmVertex ** const     lockptr)        /*+ Pointer to locked list     +*/
{
  BgraphBipartFmVertex * restrict hashtab;        /* Extended vertex array                */
  BgraphBipartFmSave * restrict   savetab;        /* Move backtracking array              */
  BgraphBipartFmSave * restrict   saveold;        /* Pointer to translated old save array */
  Gnum                            savenum;
  Gnum                            hashold;        /* Size of old hash table (half of new) */
  Gnum                            hashsiz;
  Gnum                            hashmax;
  Gnum                            hashmsk;
  Gnum                            hashnum;

  hashmax = *hashmaxptr << 1;                     /* Compute new sizes */
  hashold = *hashmaxptr << 2;
  hashsiz = *hashmaxptr << 3;
  hashmsk = hashsiz - 1;

#ifdef SCOTCH_DEBUG_BGRAPH2
  if (sizeof (BgraphBipartFmVertex) < sizeof (BgraphBipartFmSave)) { /* Should always be true */
    errorPrint ("bgraphBipartFmResize: internal error (1)");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

  if (memReallocGroup ((void *) *hashtabptr,
                       &hashtab, (size_t) (hashsiz * sizeof (BgraphBipartFmVertex)),
                       &savetab, (size_t) (hashsiz * sizeof (BgraphBipartFmSave)), NULL) == NULL) {
    errorPrint ("bgraphBipartFmResize: out of memory");
    return (1);
  }

  saveold = (BgraphBipartFmSave *) ((byte *) hashtab + ((byte *) *savetabptr - (byte *) *hashtabptr));
  for (savenum = savenbr - 1; savenum >= 0; savenum --) { /* Move save array, in reverse order */
    savetab[savenum].commcut  = saveold[savenum].commcut;
    savetab[savenum].commgain = saveold[savenum].commgain;
    savetab[savenum].compgain = saveold[savenum].compgain;
    savetab[savenum].partval  = saveold[savenum].partval;
    savetab[savenum].hashnum  = hashtab[saveold[savenum].hashnum].vertnum; /* Temporarily translate from hash index to number */
  }

  *hashtabptr = hashtab;
  *hashmaxptr = hashmax;
  *hashmskptr = hashmsk;
  *savetabptr = savetab;

  memSet (hashtab + hashold, ~0, hashold * sizeof (BgraphBipartFmVertex));

  gainTablFree (tablptr);                         /* Reset gain table  */
  *lockptr = NULL;                                /* Rebuild lock list */

  if (hashtab[0].vertnum != ~0) {                 /* If vertex overflowing may have occured in old hash table */
    Gnum                        hashtmp;
    Gnum                        hashnew;

    for (hashtmp = hashold - 1, hashnew = 0;      /* Temporarily move vertices away from end of old table to prevent overflowing */
         hashtab[hashtmp].vertnum != ~0; hashtmp --) {
      while (hashtab[++ hashnew].vertnum != ~0) ; /* Find an empty slot to receive moved vertex */
#ifdef SCOTCH_DEBUG_BGRAPH2
      if (hashnew >= hashtmp) {
        errorPrint ("bgraphBipartFmResize: internal error (2)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_BGRAPH2 */

      hashtab[hashnew] = hashtab[hashtmp];        /* Move vertex from end of table */
      hashtab[hashtmp].mswpnum = ~0;              /* TRICK: not tested at creation */
      hashtab[hashtmp].vertnum = ~0;              /* Set old slot as free          */
    }
  }

  for (hashnum = 0; hashnum < hashold; hashnum ++) { /* Re-compute position of vertices in new table */
    Gnum                        vertnum;

    vertnum = hashtab[hashnum].vertnum;
    if (vertnum != ~0) {                          /* If hash slot used */
      Gnum                        hashnew;

      for (hashnew = (vertnum * BGRAPHBIPARTFMHASHPRIME) & hashmsk; ; hashnew = (hashnew + 1) & hashmsk) {
        if (hashnew == hashnum)                   /* If hash slot is the same      */
          break;                                  /* There is nothing to do        */
        if (hashtab[hashnew].vertnum == ~0) {     /* If new slot is empty          */
          hashtab[hashnew] = hashtab[hashnum];    /* Copy data to new slot         */
          hashtab[hashnum].mswpnum = ~0;          /* TRICK: not tested at creation */
          hashtab[hashnum].vertnum = ~0;          /* Make old slot empty           */
          break;
        }
      }
      if ((hashnew > hashnum) && (hashnew < hashold)) /* If vertex was an overflowed vertex which will be replaced at end of old table */
        continue;                                 /* It will be re-processed again and re-linked once for good at the end of the loop  */

      if (hashtab[hashnew].gainlink.next >= BGRAPHBIPARTFMSTATELINK) /* If vertex was linked, re-link it */
        gainTablAdd (tablptr, &hashtab[hashnew].gainlink, hashtab[hashnew].compgain);
      else if (hashtab[hashnew].gainlink.next == BGRAPHBIPARTFMSTATEUSED) { /* Re-lock used vertices */
        hashtab[hashnew].gainlink.prev = (GainLink *) *lockptr; /* Lock it */
        *lockptr = &hashtab[hashnew];
      }
    }
  }

  for (savenum = 0; savenum < savenbr; savenum ++) {
    Gnum                  vertnum;
    Gnum                  hashnum;

    vertnum = savetab[savenum].hashnum;           /* Get vertex number temporarily saved */
    for (hashnum = (vertnum * BGRAPHBIPARTFMHASHPRIME) & hashmsk; hashtab[hashnum].vertnum != vertnum; hashnum = (hashnum + 1) & hashmsk) {
#ifdef SCOTCH_DEBUG_BGRAPH2
      if (hashtab[hashnum].vertnum == ~0) {
        errorPrint ("bgraphBipartFmResize: internal error (3)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_BGRAPH2 */
    }
    savetab[savenum].hashnum = hashnum;           /* Set new hash table index */
  }

  return (0);
}

/* This routine checks the consistency of
** the hash structures.
** It returns:
** - 0   : in case of success.
** - !0  : in case of error.
*/

#ifdef SCOTCH_DEBUG_BGRAPH2
#ifdef SCOTCH_DEBUG_BGRAPH3
static
int
bgraphBipartFmCheck (
const Bgraph * restrict const               grafptr,
const BgraphBipartFmVertex * restrict const hashtab,
const Gnum                                  hashmsk,
const int                                   swapval,
const Gnum                                  compload0dlt,
const Gnum                                  commload,
const Gnum                                  commgainextn)
{
  Gnum                  domdist;
  Gnum                  hashnum;
  Gnum                  compload0tmp;
  Gnum                  commloaddlttmp;           /* Difference between old and current communication load */
  Gnum                  commloadextndlttmp;
  Gnum                  commgainextntmp;

  domdist            = grafptr->domdist;
  compload0tmp       = (swapval == 0) ? grafptr->compload0 : (grafptr->s.velosum - grafptr->compload0);
  commloaddlttmp     = 0;                         /* No difference yet */
  commloadextndlttmp = swapval * grafptr->commgainextn;
  commgainextntmp    = (1 - 2 * swapval) * grafptr->commgainextn;
  for (hashnum = 0; hashnum <= hashmsk; hashnum ++) { /* For all vertex slots */
    Gnum                vertnum;
    Gnum                veloval;
    Gnum                veexval;
    Gnum                edgenum;
    int                 partval;
    int                 partold;
    Gnum                commcut;
    Gnum                commgain;
    Gnum                commgainextn;

    vertnum = hashtab[hashnum].vertnum;
    if (vertnum == ~0)                            /* If unallocated slot */
      continue;                                   /* Skip to next slot   */

    veloval = (grafptr->s.velotax != NULL) ? grafptr->s.velotax[vertnum] : 1;
    partval = hashtab[hashnum].partval;
    if ((partval < 0) || (partval > 1)) {
      errorPrint ("bgraphBipartFmCheck: invalid vertex part value");
      return     (1);
    }
    if (hashtab[hashnum].compgain != (2 * partval - 1) * veloval) {
      errorPrint ("bgraphBipartFmCheck: invalid vertex computation gain");
      return     (1);
    }
    partold = grafptr->parttax[vertnum] ^ swapval;
    veexval = (grafptr->veextax != NULL) ? grafptr->veextax[vertnum] : 0;

    compload0tmp    += (partval ^ partold) * (1 - 2 * partval) * veloval;
    commgainextn     = (1 - 2 * partval) * veexval;
    commgainextntmp += (partval ^ partold) * commgainextn * 2;
    commloadextndlttmp -= (partval ^ partold) * commgainextn;

    commcut  =
    commgain = 0;
    for (edgenum = grafptr->s.verttax[vertnum];   /* For all neighbors */
         edgenum < grafptr->s.vendtax[vertnum]; edgenum ++) {
      Gnum                edloval;
      Gnum                vertend;
      Gnum                hashend;
      int                 partend;
      int                 partond;
      int                 partdlt;

      vertend = grafptr->s.edgetax[edgenum];
      partond = grafptr->parttax[vertend] ^ swapval;
      edloval = (grafptr->s.edlotax != NULL) ? grafptr->s.edlotax[edgenum] : 1;

      for (hashend = (vertend * BGRAPHBIPARTFMHASHPRIME) & hashmsk; ; hashend = (hashend + 1) & hashmsk) {
        if (hashtab[hashend].vertnum == vertend) { /* If end vertex found */
          partend = hashtab[hashend].partval;
          break;
        }
        if (hashtab[hashend].vertnum == ~0) {     /* If end vertex not present */
          partend = partond;                      /* Keep old end part         */
          break;
        }
      }
      partdlt         = partval ^ partend;
      commcut        += partdlt;
      commgain       += (1 - 2 * partdlt) * edloval;
      commloaddlttmp += (partdlt - (partold ^ partond)) * edloval; /* Will account twice for difference of edge loads */
    }
    if (commcut != hashtab[hashnum].commcut) {
      errorPrint ("bgraphBipartFmCheck: invalid vertex cut value");
      return     (1);
    }
    if ((commgain * domdist + commgainextn) != hashtab[hashnum].commgain) {
      errorPrint ("bgraphBipartFmCheck: invalid vertex communication gain value");
      return     (1);
    }
  }
  if ((compload0tmp - grafptr->compload0avg) != compload0dlt) {
    errorPrint ("bgraphBipartFmCheck: invalid computation load");
    return     (1);
  }
  if ((grafptr->commload + (commloaddlttmp / 2) * domdist) != (commload - commloadextndlttmp)) {
    errorPrint ("bgraphBipartFmCheck: invalid communication load");
    return     (1);
  }
  if (commgainextntmp != commgainextn) {
    errorPrint ("bgraphBipartFmCheck: invalid external communication gain");
    return     (1);
  }

  return (0);
}
#endif /* SCOTCH_DEBUG_BGRAPH3 */
#endif /* SCOTCH_DEBUG_BGRAPH2 */
