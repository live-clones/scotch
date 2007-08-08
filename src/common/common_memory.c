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
** $Id: common_memory.c 176 2004-10-12 13:53:26Z goureman $
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
/**                # Version 1.3  : from : 25 feb 2004     **/
/**                                 to     25 feb 2004     **/
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
    memoff += (size_t)  va_arg (memlist, size_t);
    memloc  = (byte **) va_arg (memlist, byte **);
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
    memoff += (size_t)  va_arg (memlist, size_t); /* Accumulate padded sizes        */
    memloc  = (byte **) va_arg (memlist, void *); /* Get next argument pointer      */
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
** WARNING: Because of memory alignment issues
** between int and double values, when arrays
** are not reallocated in place, offsets of
** arrays may vary, so that one should rather
** compute differences with respect to original
** offsets than rely on offsets returned by the
** routine. This routine should be used with
** extreme caution!
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

  while ((memloc = (byte **) va_arg (memlist, byte **)) != NULL) { /* As long as not NULL pointer */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1)); /* Pad                */
    memoff += (size_t) va_arg (memlist, size_t);  /* Accumulate padded sizes                      */
  }
  memoff += sizeof (double);                      /* Ensure reallocation of a padded area into a non-padded area will never result in data loss */


  if ((blkptr = (byte *) memRealloc (oldptr, memoff)) == NULL) /* If cannot allocate block */
    return (NULL);

  memoff = 0;
  va_start (memlist, oldptr);                     /* Restart argument parsing                     */
  while ((memloc = (byte **) va_arg (memlist, byte **)) != NULL) { /* As long as not NULL pointer */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1)); /* Pad                */
    *memloc = blkptr + memoff;                    /* Set argument address                         */
    memoff += (size_t) va_arg (memlist, size_t);  /* Accumulate padded sizes                      */
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

  while ((memloc = (byte **) va_arg (memlist, byte **)) != NULL) { /* As long as not NULL pointer */
    memoff  = (memoff + (sizeof (double) - 1)) & (~ (sizeof (double) - 1));
    *memloc = (byte *) memptr + memoff;           /* Set argument address    */
    memoff += (size_t) va_arg (memlist, size_t);  /* Accumulate padded sizes */
  }

  return ((void *) ((byte *) memptr + memoff));
}
