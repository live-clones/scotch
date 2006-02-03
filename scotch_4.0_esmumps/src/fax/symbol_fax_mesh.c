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
/**   NAME       : symbol_fax_mesh.c                       **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This is the block symbolic factoriza-   **/
/**                tion routine for meshes.                **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 21 mar 2003     **/
/**                                 to     20 nov 2003     **/
/**                # Version 3.0  : from : 02 mar 2004     **/
/**                                 to     02 mar 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_FAX_MESH

#include "common.h"
#include "scotch.h"
#include "mesh.h"
#include "symbol.h"
#include "order.h"
#include "fax.h"
#include "symbol_fax_mesh.h"

/* These routines define the adjacency
** of the given mesh.
*/

static
INT
symbolFaxMeshDegr (
SymbolFaxMeshNghb * const   nghbptr,              /* Mesh adjacency structure */
const INT                   vertnum)
{
  return (nghbptr->degrmax * (nghbptr->verttax[vertnum + nghbptr->vnodadj + 1] - nghbptr->verttax[vertnum + nghbptr->vnodadj]) - 1);
}

static
INT
symbolFaxMeshFrst (
SymbolFaxMeshNghb * const   nghbptr,              /* Mesh adjacency structure */
INT                         vertnum)
{
  INT                 vnodnum;
  INT                 enodnum;
  INT                 enodend;
  INT                 hnodnum;
  INT                 velmnum;

  vnodnum = vertnum + nghbptr->vnodadj;

  enodnum = nghbptr->verttax[vnodnum];
  enodend = nghbptr->verttax[vnodnum + 1];
  if (enodnum >= enodend)
    return (nghbptr->baseval - 1);

  nghbptr->vnodnum = vnodnum;
  nghbptr->enodnum = enodnum + 1;
  nghbptr->enodend = enodend;

  hnodnum = (vnodnum * SYMBOL_FAX_MESH_HASHPRIME) & nghbptr->hashmsk;
  nghbptr->hashtab[hnodnum].vnodnum = vnodnum;    /* Avoid loop when enumerating neighbors */
  nghbptr->hashtab[hnodnum].vnodend = vnodnum;

  velmnum = nghbptr->edgetax[enodnum];
  nghbptr->eelmnum = nghbptr->verttax[velmnum];
  nghbptr->eelmend = nghbptr->verttax[velmnum + 1];

  return (symbolFaxMeshNext (nghbptr));
}

static
INT
symbolFaxMeshNext (
SymbolFaxMeshNghb * const   nghbptr)              /* Mesh adjacency structure */
{
  for ( ; ; ) {
    INT                 enodnum;
    INT                 velmnum;

    while (nghbptr->eelmnum < nghbptr->eelmend) {
      INT                 vnodend;
      INT                 hnodend;

      vnodend = nghbptr->edgetax[nghbptr->eelmnum ++];

      for (hnodend = (vnodend * SYMBOL_FAX_MESH_HASHPRIME) & nghbptr->hashmsk; ; hnodend = (hnodend + 1) & nghbptr->hashmsk) {
        if (nghbptr->hashtab[hnodend].vnodnum != nghbptr->vnodnum) { /* If node not yet explored */
          nghbptr->hashtab[hnodend].vnodnum = nghbptr->vnodnum;
          nghbptr->hashtab[hnodend].vnodend = vnodend;
          return (vnodend - nghbptr->vnodadj);    /* Return found node */
        }
        if (nghbptr->hashtab[hnodend].vnodend == vnodend) /* If node already explored */
          break;                                  /* Skip to next neighbor node       */
      }
    }

    enodnum = nghbptr->enodnum;
    if (enodnum >= nghbptr->enodend)
      return (-1);

    velmnum = nghbptr->edgetax[enodnum];
    nghbptr->enodnum = enodnum + 1;
    nghbptr->eelmnum = nghbptr->verttax[velmnum];
    nghbptr->eelmend = nghbptr->verttax[velmnum + 1];
  }

  return (nghbptr->baseval - 1);                  /* Not reached */
}

/*+ This routine computes the block symbolic
*** factorization of the given mesh.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolFaxMesh (
SymbolMatrix * const        symbptr,              /*+ Symbolic block matrix [based] +*/
const Mesh * const          meshptr,              /*+ Mesh structure [based]        +*/
const Order * const         ordeptr)              /*+ Matrix ordering               +*/
{
  INT                 velmbas;
  INT                 vnodbas;
  INT                 vnodnbr;
  INT *               verttab;
  INT *               edgetab;
  INT                 degrmax;
  SymbolFaxMeshNghb   nghbdat;                    /* Mesh neighbor adjacency structure   */
  INT                 hashnbr;                    /* Maximum number of neighbors in hash */
  INT                 hashsiz;                    /* Size of adjacency hash table        */
  int                 o;

  SCOTCH_meshData (meshptr, &velmbas, &vnodbas, NULL, &vnodnbr,
                   &verttab, NULL, NULL, NULL, NULL, NULL, &edgetab, &degrmax);

  hashnbr = (degrmax * degrmax - 1) * 2;          /* Fill at twice the maximum capacity */
  for (hashsiz = 32; hashsiz < hashnbr; hashsiz <<= 1) ; /* Compute upper power of two  */

  if ((nghbdat.hashtab = memAlloc (hashsiz * sizeof (SymbolFaxMeshHash))) == NULL) {
    errorPrint ("symbolFaxMesh: out of memory");
    return     (1);
  }
  memSet (nghbdat.hashtab, ~0, hashsiz * sizeof (SymbolFaxMeshHash)); /* Initialize hash table */

  nghbdat.hashmsk = hashsiz - 1;                  /* Initialize neighbor definition structure */
  nghbdat.baseval = MIN (velmbas, vnodbas);
  nghbdat.degrmax = degrmax;
  nghbdat.verttax = verttab - nghbdat.baseval;
  nghbdat.edgetax = edgetab - nghbdat.baseval;
  nghbdat.vnodadj = vnodbas - nghbdat.baseval;

  o = symbolFax (symbptr, vnodnbr, ((hashnbr >> 1) + 1) * vnodnbr, nghbdat.baseval, &nghbdat,
                 (INT (*) (void * const, const INT)) symbolFaxMeshFrst,
                 (INT (*) (void * const)) symbolFaxMeshNext,
                 (INT (*) (void * const, const INT)) symbolFaxMeshDegr, ordeptr);

  memFree (nghbdat.hashtab);

#ifdef FAX_DEBUG
  if (symbolCheck (symbptr) != 0) {
    errorPrint ("symbolFaxMesh: internal error");
    return     (1);
  }
#endif /* FAX_DEBUG */

  return (o);
}
