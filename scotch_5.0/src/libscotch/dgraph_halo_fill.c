/* Copyright 2007,2008 ENSEIRB, INRIA & CNRS
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
/**********************************************************/
/*                                                        */
/*   NAME       : dgraph_halo_fill.c                      */
/*                                                        */
/*   AUTHOR     : Francois PELLEGRINI                     */
/*                                                        */
/*   FUNCTION   : Part of a parallel static mapper.       */
/*                This module contains the halo update    */
/*                routines.                               */
/*                                                        */
/*                # Version 5.0  : from : 31 dec 2006     */
/*                                 to     05 feb 2008     */
/*                                                        */
/**********************************************************/

/* This function fills the send array used by
** all of the halo routines.
** It returns:
** - void  : in all cases.
*/

static
void
DGRAPHHALOFILLNAME (
const Dgraph * restrict const grafptr,
const byte * restrict const   attrgsttab,         /* Attribute array to diffuse     */
int                           attrglbsiz,         /* Type extent of attribute       */
byte * restrict const         attrsndtab,         /* Array for packing data to send */
int * restrict const          senddsptab)         /* Temporary displacement array   */
{
  Gnum                  vertlocnum;
  const int * restrict  procsidtab;
  int                   procsidnum;

  procsidtab = grafptr->procsidtab;
  for (procsidnum = 0, vertlocnum = 0;            /* vertlocnum starts from 0 because attrgsttab pointer is not based */
       procsidnum < grafptr->procsidnbr; procsidnum ++) {
    int                 procsidval;

    procsidval = procsidtab[procsidnum];
    if (procsidval < 0)
      vertlocnum -= (Gnum) procsidval;
    else {
      DGRAPHHALOFILLCOPY (attrsndtab + attrglbsiz * senddsptab[procsidval],
                          attrgsttab + attrglbsiz * vertlocnum, attrglbsiz);
      senddsptab[procsidval] ++;                  /* Skip to next position in send buffer */
    }
  }
}
