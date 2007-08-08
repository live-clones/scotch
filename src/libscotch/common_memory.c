/* Copyright 2004,2007 INRIA
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
/**   NAME       : common_memory.c                         **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module handles errors.             **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 07 sep 2001     **/
/**                                 to     07 sep 2001     **/
/**                # Version 0.1  : from : 14 apr 2001     **/
/**                                 to     24 mar 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_MEMORY

#include "common.h"

/*********************************/
/*                               */
/* The memory handling routines. */
/*                               */
/*********************************/

/* This routine allocates a set of arrays in
** a single memAlloc()'ed array, the address
** of which is placed in the first argument.
** Arrays to be allocated are described as
** a duplet of ..., &ptr, size, ...,
** terminated by a NULL pointer.
** It returns:
** - !NULL  : pointer to block, all arrays allocated.
** - NULL   : no array allocated.
*/

void *
memAllocGroup (
void **                     memptr,               /*+ Pointer to first argument to allocate +*/
...)
{
  va_list             memlist;                    /* Argument list of the call              */
  byte **             memloc;                     /* Pointer to pointer of current argument */
  size_t              memoff;                     /* Offset value of argument               */
  byte *              blkptr;                     /* Pointer to memory chunk                */

  memoff = 0;
  memloc = (byte **) memptr;                      /* Point to first memory argument */
  va_start (memlist, memptr);                     /* Start argument parsing         */
  while (memloc != NULL) {                        /* As long as not NULL pointer    */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1));
    memoff += va_arg (memlist, size_t);
    memloc  = va_arg (memlist, byte **);
  }

  if ((blkptr = (byte *) memAlloc (memoff)) == NULL) { /* If cannot allocate   */
    *memptr = NULL;                               /* Set first pointer to NULL */
    return (NULL);
  }

  memoff = 0;
  memloc = (byte **) memptr;                      /* Point to first memory argument */
  va_start (memlist, memptr);                     /* Restart argument parsing       */
  while (memloc != NULL) {                        /* As long as not NULL pointer    */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1)); /* Pad  */
    *memloc = blkptr + memoff;                    /* Set argument address           */
    memoff += va_arg (memlist, size_t);           /* Accumulate padded sizes        */
    memloc  = va_arg (memlist, void *);           /* Get next argument pointer      */
  }

  return ((void *) blkptr);
}

/* This routine reallocates a set of arrays in
** a single memRealloc()'ed array passed as
** first argument, and the address of which
** is placed in the second argument.
** Arrays to be allocated are described as
** a duplet of ..., &ptr, size, ...,
** terminated by a NULL pointer.
** It returns:
** - !NULL  : pointer to block, all arrays allocated.
** - NULL   : no array allocated.
*/

void *
memReallocGroup (
void *                      oldptr,               /*+ Pointer to block to reallocate +*/
...)
{
  va_list             memlist;                    /* Argument list of the call              */
  byte **             memloc;                     /* Pointer to pointer of current argument */
  size_t              memoff;                     /* Offset value of argument               */
  byte *              blkptr;                     /* Pointer to memory chunk                */

  memoff = 0;
  va_start (memlist, oldptr);                     /* Start argument parsing */

  while ((memloc = va_arg (memlist, byte **)) != NULL) { /* As long as not NULL pointer */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1)); /* Pad      */
    memoff += va_arg (memlist, size_t);           /* Accumulate padded sizes            */
  }

  if ((blkptr = (byte *) memRealloc (oldptr, memoff)) == NULL) /* If cannot allocate block */
    return (NULL);

  memoff = 0;
  va_start (memlist, oldptr);                     /* Restart argument parsing           */
  while ((memloc = va_arg (memlist, byte **)) != NULL) { /* As long as not NULL pointer */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1)); /* Pad      */
    *memloc = blkptr + memoff;                    /* Set argument address               */
    memoff += va_arg (memlist, size_t);           /* Accumulate padded sizes            */
  }

  return ((void *) blkptr);
}

/* This routine computes the offsets of arrays
** of given sizes and types with respect to a
** given base address passed as first argument.
** Arrays the offsets of which are to be computed
** are described as a duplet of ..., &ptr, size, ...,
** terminated by a NULL pointer.
** It returns:
** - !NULL  : in all cases, pointer to the end of
**            the memory area.
*/

void *
memOffset (
void *                      memptr,               /*+ Pointer to base address of memory area +*/
...)
{
  va_list             memlist;                    /* Argument list of the call              */
  byte **             memloc;                     /* Pointer to pointer of current argument */
  size_t              memoff;                     /* Offset value of argument               */

  memoff = 0;
  va_start (memlist, memptr);                     /* Start argument parsing */

  while ((memloc = va_arg (memlist, byte **)) != NULL) { /* As long as not NULL pointer */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1));
    *memloc = (byte *) memptr + memoff;           /* Set argument address    */
    memoff += va_arg (memlist, size_t);           /* Accumulate padded sizes */
  }

  return ((void *) ((byte *) memptr + memoff));
}
