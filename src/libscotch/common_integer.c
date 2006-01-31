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
** $Id: common_integer.c,v 1.1.1.1 2004/06/02 14:05:05 ramet Exp $
*/
/************************************************************/
/**                                                        **/
/**   NAME       : common_integer.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the generic integer **/
/**                type.                                   **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 07 sep 1998     **/
/**                                 to     22 sep 1998     **/
/**                # Version 0.1  : from : 07 jan 2002     **/
/**                                 to     17 jan 2003     **/
/**                # Version 0.2  : from : 13 jun 2005     **/
/**                                 to     15 jun 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_INTEGER

#include "common.h"
#include "common_integer.h"

/********************************/
/*                              */
/* Basic routines for fast I/O. */
/*                              */
/********************************/

/* Fast read for INT values.
** It returns:
** - 1  : on success.
** - 0  : on error.
*/

int
intLoad (
FILE * const                stream,               /*+ Stream to read from     +*/
INT * const                 valptr)               /*+ Area where to put value +*/
{
  int                 sign;                       /* Sign flag      */
  int                 car;                        /* Character read */
  INT                 val;                        /* Value          */

  sign = 0;                                       /* Assume positive constant     */
  for ( ; ; ) {                                   /* Consume whitespaces and sign */
    car = getc (stream);
    if (isspace (car))
      continue;
    if ((car >= '0') && (car <= '9'))
      break;
    if (car == '-') {
      sign = 1;
      car  = getc (stream);
      break;
    }
    if (car == '+') {
      car = getc (stream);
      break;
    }
    return (0);
  }
  if ((car < '0') || (car > '9'))                 /* If first char is non numeric */
    return (0);                                   /* Then it is an error          */
  val = car - '0';                                /* Get first digit              */  
  for ( ; ; ) {
    car = getc (stream);
    if ((car < '0') || (car > '9')) {
      ungetc (car, stream);
      break;
    }
    val = val * 10 + (car - '0');                 /* Accumulate digits */
  }
  *valptr = (sign != 0) ? (- val) : val;          /* Set result */

  return (1);
}

/* Write routine for INT values.
** It returns:
** - 1  : on success.
** - 0  : on error.
*/

int
intSave (
FILE * const                stream,               /*+ Stream to write to +*/
const INT                   val)                  /*+ Value to write     +*/
{
  return ((fprintf (stream, "%ld", (long) val) == EOF) ? 0 : 1);
}

/**********************************/
/*                                */
/* Permutation building routines. */
/*                                */
/**********************************/

/* This routine fills an array with
** consecutive INT values, in
** ascending order.
** It returns:
** - VOID  : in all cases.
*/

void
intAscn (
INT * restrict const        permtab,              /*+ Permutation array to build +*/
const INT                   permnbr,              /*+ Number of entries in array +*/
const INT                   baseval)              /*+ Base value                 +*/
{
  INT * restrict      permtax;
  INT                 permnum;
  INT                 permnnd;

  for (permnum = baseval, permnnd = baseval + permnbr, permtax = permtab - baseval;
       permnum < permnnd; permnum ++)
    permtax[permnum] = permnum;
}

/* This routine computes a random permutation
** of an array of INT values.
** It returns:
** - VOID  : in all cases.
*/

void
intPerm (
INT * restrict const        permtab,              /*+ Permutation array to build +*/
const INT                   permnbr)              /*+ Number of entries in array +*/
{
  INT * restrict      permptr;
  INT                 permrmn;

  for (permptr = permtab, permrmn = permnbr;      /* Perform random permutation */
       permrmn > 1; permptr ++, permrmn --) {
    INT                 permnum;
    INT                 permtmp;

    permnum          = intRandVal (permrmn);      /* Select index to swap       */
    permtmp          = permptr[0];                /* Swap it with current index */
    permptr[0]       = permptr[permnum];
    permptr[permnum] = permtmp;
  }
}

/********************/
/*                  */
/* Random routines. */
/*                  */
/********************/

static int          intRandFlag = 0;              /* Flag set if generator already initialized */

/* This routine enables resetting
** of the pseudo-random generator
** to simulate another run from
** scratch.
** It returns:
** - VOID  : in all cases.
*/

void
intRandReset (void)
{
  intRandFlag = 0;                                /* Next call to intRandInit() will reset random generator */
}

/* This routine initializes
** the pseudo-random generator
** if necessary.
** It returns:
** - VOID  : in all cases.
*/

void
intRandInit (void)
{
  if (intRandFlag == 0) {                         /* If generator not yet initialized */
#if ((defined COMMON_DEBUG) || (defined COMMON_RANDOM_FIXED_SEED))
    srandom (1);                                  /* Non-random seed */
#else
    srandom (time (NULL));                        /* Random seed */
#endif /* ((defined COMMON_DEBUG) || (defined COMMON_RANDOM_FIXED_SEED)) */
    intRandFlag = 1;                              /* Generator has been initialized */
  }
}

/*********************/
/*                   */
/* Sorting routines. */
/*                   */
/*********************/

/* This routine sorts an array of
** INT values by ascending order
** on their first value, used as key.
** It returns:
** - VOID  : in all cases.
*/

void
intSort1asc1 (
void * const                sorttab,              /*+ Array to sort             +*/
const INT                   sortnbr)              /*+ Number of entries to sort +*/
{
  qsort ((char *) sorttab, sortnbr, sizeof (INT),
         (int (*) (const void *, const void *)) intSort1asc1_2);
}

static
int
intSort1asc1_2 (
const INT * restrict const  sort0,
const INT * restrict const  sort1)
{
  return ((sort0[0] > sort1[0]) ? 1 : -1);
}

/* This routine sorts an array of pairs of
** INT values by ascending order on their
** first value, used as key.
** It returns:
** - VOID  : in all cases.
*/

void
intSort2asc1 (
void * const                sorttab,              /*+ Array to sort             +*/
const INT                   sortnbr)              /*+ Number of entries to sort +*/
{
  qsort ((char *) sorttab, sortnbr, 2 * sizeof (INT),
         (int (*) (const void *, const void *)) intSort1asc1_2);
}

/* This routine sorts an array of pairs of
** INT values by ascending order on both
** of their values, used as primary and
** secondary keys.
** It returns:
** - VOID  : in all cases.
*/

void
intSort2asc2 (
void * const                sorttab,              /*+ Array to sort             +*/
const INT                   sortnbr)              /*+ Number of entries to sort +*/
{
  qsort ((char *) sorttab, sortnbr, 2 * sizeof (INT),
         (int (*) (const void *, const void *)) intSort2asc2_2);
}

static
int
intSort2asc2_2 (
const INT * restrict const  sort0,
const INT * restrict const  sort1)
{
  return (((sort0[0] > sort1[0]) || ((sort0[0] == sort1[0]) && (sort0[1] > sort1[1]))) ? 1 : -1);
}
