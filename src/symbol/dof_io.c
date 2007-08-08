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
/**   NAME       : dof_io.c                                **/
/**                                                        **/
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the input/output        **/
/**                routines for the DOF structure.         **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 07 oct 1998     **/
/**                                 to     07 oct 1998     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define DOF_IO

#include "common.h"
#include "scotch.h"
#include "graph.h"
#include "dof.h"

/****************************************/
/*                                      */
/* The DOF structure handling routines. */
/*                                      */
/****************************************/

/*+ This routine saves the given DOF
*** structure to the given stream.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
dofSave (
const Dof * const           deofptr,
FILE * const                stream)
{
  const INT *         noddtnd;
  const INT *         noddptr;
  INT                 noddnum;
  int                 o;

  o = (fprintf (stream, "0\n%ld\t%ld\t%ld\n\n",   /* Write file header */
                (long) deofptr->nodenbr,
                (long) deofptr->noddval,
                (long) deofptr->baseval) == EOF);
  if (deofptr->noddtab != NULL) {
    for (noddptr = deofptr->noddtab, noddtnd = noddptr + deofptr->nodenbr, noddnum = 1;
         (noddptr < noddtnd) && (o == 0); noddptr ++, noddnum ++) {
      o = (fprintf (stream, "%ld%c",
                    (long) *noddptr,
                    ((noddnum % 8) == 0) ? '\n' : '\t') == EOF);
    }
    o |= (fprintf (stream, "%ld\n",
                   (long) *noddptr) == EOF);
  }

  return (o);
}
