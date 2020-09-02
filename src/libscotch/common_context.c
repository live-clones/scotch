/* Copyright 2019 IPB, Universite de Bordeaux, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
**
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
**
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
**
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
*/
/************************************************************/
/**                                                        **/
/**   NAME       : common_context.c                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles the execution       **/
/**                context management routines.            **/
/**                                                        **/
/**   DATES      : # Version 7.0  : from : 07 may 2019     **/
/**                                 to   : 18 sep 2019     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_CONTEXT

#ifndef COMMON_NOMODULE
#include "module.h"
#endif /* COMMON_NOMODULE */
#include "common.h"
#include "common_thread.h"
#include "common_thread_system.h"

/***********************************/
/*                                 */
/* These routines handle contexts. */
/*                                 */
/***********************************/

/* This routine initializes the given context.
** It returns:
** - void  : in all cases.
*/

void
contextInit (
Context * const             contptr)
{
  contptr->thrdptr = NULL;                        /* Thread context not initialized yet     */
  contptr->randptr = &intranddat;                 /* Use global random generator by default */

  intRandInit (&intranddat);                      /* Make sure random context is initialized before cloning */
}

/* This routine frees a context structure.
** It returns:
** - VOID  : in all cases.
*/

void
contextExit (
Context * const             contptr)
{
  if (contptr->thrdptr != NULL) {                 /* If context has been commited */
    threadContextExit (contptr->thrdptr);
    memFree (contptr->thrdptr);
  }
  if (contptr->randptr != &intranddat)            /* If not global random generator */
    memFree (contptr->randptr);

#ifdef SCOTCH_DEBUG_CONTEXT1
  contptr->thrdptr = NULL;
  contptr->randptr = NULL;
#endif /* SCOTCH_DEBUG_CONTEXT1 */
}

/* This routine allocates the features of the
** given context if they have not been already.
** It returns:
** - 0   : if the initialization succeeded.
** - !0  : on error.
*/

int
contextCommit (
Context * const             contptr)
{
  int                 o;

  o = 0;
  if (contptr->thrdptr == NULL)                   /* If thread context not already initialized */
    o = contextThreadInit (contptr);

  return (o);
}

/************************************/
/*                                  */
/* These routines handle the random */
/* generator features of contexts.  */
/*                                  */
/************************************/

/*+ This routine creates a clone of the default
*** pseudo-random in its current state and places
*** it in the given context.
*** It returns:
*** - 0   : if the cloning succeeded.
*** - !0  : on error.
+*/

int
contextRandomClone (
Context * const             contptr)
{
  IntRandContext *    randptr;

  if (contptr->randptr == &intranddat) {          /* If no clone yet, allocate space for it */
    if ((randptr = memAlloc (sizeof (IntRandContext))) == NULL) {
      errorPrint ("contextRandomClone: out of memory");
      return (1);
    }
    contptr->randptr = randptr;                   /* Context now uses its private clone */
  }
  else                                            /* Else re-use old clone */
    randptr = contptr->randptr;

  *randptr = intranddat;                          /* Clone default generator (always already initialized) */

  return (0);
}

/************************************/
/*                                  */
/* These routines handle the thread */
/* features of contexts.            */
/*                                  */
/************************************/

/* This routine initializes the thread context
** of an execution context.
** It returns:
** - 0   : if the thread context has been initialized.
** - !0  : on error.
*/

int
contextThreadInit2 (
Context * const             contptr,
const int                   thrdnbr,
const int * const           coretab)
{
  if (contptr->thrdptr != NULL) {
    errorPrint ("contextThreadInit2: thread context already allocated");
    return (1);
  }

  if ((contptr->thrdptr = memAlloc (sizeof (ThreadContext))) == NULL) {
    errorPrint ("contextThreadInit2: out of memory");
    return (1);
  }

  if (threadContextInit (contptr->thrdptr, thrdnbr, coretab) != 0) {
    memFree (contptr->thrdptr);
    contptr->thrdptr = NULL;
    return (1);
  }

  return (0);
}

int
contextThreadInit (
Context * const             contptr)
{
  int                 thrdnbr;

#ifdef SCOTCH_PTHREAD
#ifdef SCOTCH_PTHREAD_NUMBER
  thrdnbr = SCOTCH_PTHREAD_NUMBER;                /* If prescribed number defined at compile time, use it as default */
#else /* SCOTCH_PTHREAD_NUMBER */
  thrdnbr = -1;                                   /* Else take the number of cores at run time */
#endif /* SCOTCH_PTHREAD_NUMBER */
  thrdnbr = envGetInt ("SCOTCH_PTHREAD_NUMBER", thrdnbr);
  if (thrdnbr < 1)
    thrdnbr = threadSystemCoreNbr ();
#else /* SCOTCH_PTHREAD */
  thrdnbr = 1;                                    /* No threads allowed */
#endif /* SCOTCH_PTHREAD */

  return (contextThreadInit2 (contptr, thrdnbr, NULL));
}
