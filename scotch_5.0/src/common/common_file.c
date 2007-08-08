/* Copyright 2007 INRIA
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
/**   NAME       : common_file.c                           **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module handles files and file      **/
/**                names.                                  **/
/**                                                        **/
/**   DATES      : # Version P0.5 : from : 21 may 2007     **/
/**                                 to   : 08 jun 2007     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_FILE

#include "common.h"
#include "common_file.h"

/*********************************/
/*                               */
/* Basic routines for filenames. */
/*                               */
/*********************************/

/* This routine expands distributed filenames
** according to process numbers and the root
** process number.
** It returns:
** - 1  : on success.
** - 0  : on error.
*/

int
fileNameDistExpand (
char ** const               nameptr,              /*+ Pointer to name string pointer +*/
const int                   procnbr,              /*+ Number of processes            +*/
const int                   procnum,              /*+ Number of current process      +*/
const int                   protnum)              /*+ Root process number            +*/
{
  int                 namemax;
  int                 namenum;
  char *              naexptr;
  int                 naexmax;
  int                 naexnum;
  int                 flagval;                    /* Flag set if expansion took place */

  namemax = strlen (*nameptr);
  naexmax = namemax + FILENAMEDISTEXPANDNBR * 2;

  if ((naexptr = memAlloc ((naexmax + 1) * sizeof (char))) == NULL) /* "+ 1" for terminating character */
    return (1);

#ifdef COMMON_DEBUG
  sprintf (naexptr, FILENAMEDISTEXPANDSTR, procnbr); /* TRICK: Test if FILENAMEDISTEXPANDNBR is a size large enough */
  if (atoi (naexptr) != procnbr) {
    errorPrint ("fileNameDistExpand: undersized integer string size");
    return     (1);
  }
#endif /* COMMON_DEBUG */

  for (namenum = naexnum = flagval = 0; namenum < namemax; ) {
    char                charval;
    int                 dataval;
    int                 datasiz;

    charval = (*nameptr)[namenum ++];             /* Get current characted                */
    datasiz = 1;                                  /* Assume individual expanded character */
    if (charval == '%') {
      char                chnxval;                /* Value of next char */

      switch (chnxval = (*nameptr)[namenum ++]) {
        case 'p' :                                /* "%p" translates into number of processes */
          flagval = 1;
          datasiz = FILENAMEDISTEXPANDNBR;
          dataval = procnbr;
          break;
        case 'r' :                                /* "%r" translates into process rank */
          flagval = 1;
          datasiz = FILENAMEDISTEXPANDNBR;
          dataval = procnum;
          break;
        case '-' :                                /* "%-" translates into nothing but indicates distributed file */
          datasiz = 0;
          flagval = 1;
          break;
        case '%' :                                /* "%%" translates into '%' */
          break;
        default :
          charval = chnxval;                      /* Unrecognized character */
      }
    }
    if (datasiz > 0) {
      if ((naexnum + datasiz) > naexmax) {
        char *              nanwptr;

        naexmax += datasiz + FILENAMEDISTEXPANDNBR;
        if ((nanwptr = memRealloc (naexptr, (naexmax + 1) * sizeof (char))) == NULL) { /* "+ 1" for terminating character */
          memFree (naexptr);
          return  (1);
        }
        naexptr = nanwptr;
      }
      if (datasiz == 1)
        naexptr[naexnum ++] = charval;
      else {
        sprintf (&naexptr[naexnum], FILENAMEDISTEXPANDSTR, dataval); /* TRICK: Change format string if FILENAMEDISTEXPANDNBR changes */
        naexptr[naexnum + FILENAMEDISTEXPANDNBR] = ' ';
        naexnum = strchr (&naexptr[naexnum], ' ') - naexptr;
      }
    }
  }
  naexptr[naexnum] = '\0';                        /* Set terminating character as there is always room for it */

  if ((flagval != 0) || (procnum == protnum))     /* If file name is a distributed one or we are the root process */
    *nameptr = naexptr;                           /* Replace new string by old one                                */
  else {
    memFree (naexptr);                            /* No need for the expanded string */
    *nameptr = NULL;
  }

  return (0);
}
