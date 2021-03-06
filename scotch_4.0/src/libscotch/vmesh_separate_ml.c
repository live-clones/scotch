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
/**   NAME       : vmesh_separate_ml.c                     **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module separates an active         **/
/**                mesh using a multi-level scheme.        **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 19 feb 2003     **/
/**                                 to     31 aug 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define VMESH_SEPARATE_ML

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "mesh.h"
#include "mesh_coarsen.h"
#include "vmesh.h"
#include "vmesh_separate_ml.h"
#include "vmesh_separate_st.h"

/*********************************************/
/*                                           */
/* The coarsening and uncoarsening routines. */
/*                                           */
/*********************************************/

/* This routine builds a coarser mesh from the
** mesh that is given on input. The coarser
** meshes differ at this stage from classical
** active meshes as their internal gains are not
** yet computed.
** It returns:
** - 0  : if the coarse mesh has been built.
** - 1  : if threshold achieved.
** - 2  : on error.
*/

static
int
vmeshSeparateMlCoarsen (
const Vmesh * restrict const        finemeshptr,  /*+ Finer mesh                          +*/
Vmesh * restrict const              coarmeshptr,  /*+ Coarser mesh to build               +*/
Gnum * restrict * const             finecoarptr,  /*+ Pointer to multinode table to build +*/
const VmeshSeparateMlParam * const  paraptr)      /*+ Method parameters                   +*/
{
  int                 o;

  if (finemeshptr->m.vnodnbr <= (Gnum) paraptr->vnodnbr)
    return (1);

  if ((o = meshCoarsen (&finemeshptr->m, &coarmeshptr->m, finecoarptr, (Gnum) paraptr->vnodnbr, paraptr->coarrat, paraptr->coartype)) != 0)
    return (o);                                   /* Return if coarsening failed */

  coarmeshptr->parttax = NULL;                    /* Do not allocate partition data yet     */
  coarmeshptr->frontab = finemeshptr->frontab;    /* Re-use frontier array for coarser mesh */
  coarmeshptr->levlnum = finemeshptr->levlnum + 1; /* Mesh level is coarsening level        */

  return (0);
}

/* This routine propagates the separation of the
** coarser mesh back to the finer mesh, according
** to the multinode table of collapsed elements.
** After the separation is propagated, it finishes
** to compute the parameters of the finer mesh that
** were not computed at the coarsening stage.
** It returns:
** - 0   : if coarse mesh data has been propagated to fine mesh.
** - !0  : on error.
*/

static
int
vmeshSeparateMlUncoarsen (
Vmesh * restrict const        finemeshptr,        /*+ Finer mesh      +*/
const Vmesh * restrict const  coarmeshptr,        /*+ Coarser mesh    +*/
const Gnum * restrict const   finecoartax)        /*+ Multinode array +*/
{
  if (finemeshptr->parttax == NULL) {             /* If partition array not yet allocated */
    if ((finemeshptr->parttax = (GraphPart *) memAlloc ((finemeshptr->m.velmnbr + finemeshptr->m.vnodnbr) * sizeof (GraphPart))) == NULL) {
      errorPrint ("vmeshSeparateMlUncoarsen: out of memory");
      return     (1);                             /* Allocated data will be freed along with mesh structure */
    }
    finemeshptr->parttax -= finemeshptr->m.baseval;
  }

  if (coarmeshptr != NULL) {                      /* If coarser mesh provided */
    Gnum                finevelmnum;
    Gnum                fineecmpsize1;            /* Number of fine elements */
    Gnum                fineecmpsize2;
    Gnum                finevnodnum;
    Gnum                finencmpsize1;            /* Number of fine nodes                     */
    Gnum                finefronnbr;              /* Number of frontier vertices in fine mesh */

    for (finevelmnum = finemeshptr->m.velmbas, fineecmpsize1 = fineecmpsize2 = 0;
         finevelmnum < finemeshptr->m.velmnnd; finevelmnum ++) {
      Gnum                partval;

#ifdef SCOTCH_DEBUG_VMESH2
      if ((finecoartax[finevelmnum] < coarmeshptr->m.baseval) ||
          (finecoartax[finevelmnum] >= (coarmeshptr->m.velmnbr + coarmeshptr->m.vnodnbr + coarmeshptr->m.baseval))) {
        errorPrint ("vmeshSeparateMlUncoarsen: internal error (1)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_VMESH2 */
      partval = (Gnum) coarmeshptr->parttax[finecoartax[finevelmnum]];
      finemeshptr->parttax[finevelmnum] = partval;

      fineecmpsize1 += (partval & 1);             /* Superscalar update of counters */
      fineecmpsize2 += (partval & 2);
    }
    finemeshptr->ecmpsize[0] = finemeshptr->m.velmnbr - fineecmpsize1 - (fineecmpsize2 >> 1);
    finemeshptr->ecmpsize[1] = fineecmpsize1;

    for (finevnodnum = finemeshptr->m.vnodbas, finencmpsize1 = finefronnbr = 0;
         finevnodnum < finemeshptr->m.vnodnnd; finevnodnum ++) {
      Gnum                partval;

#ifdef SCOTCH_DEBUG_VMESH2
      if ((finecoartax[finevnodnum] <  coarmeshptr->m.vnodbas) || /* Sons of nodes are always nodes */
          (finecoartax[finevnodnum] >= coarmeshptr->m.vnodnnd)) {
        errorPrint ("vmeshSeparateMlUncoarsen: internal error (2)");
        return     (1);
      }
#endif /* SCOTCH_DEBUG_VMESH2 */
      partval = coarmeshptr->parttax[finecoartax[finevnodnum]];
      finemeshptr->parttax[finevnodnum] = partval;

      if ((partval & 2) != 0)                     /* If node is in separator   */
        finemeshptr->frontab[finefronnbr ++] = finevnodnum; /* Add to frontier */

      finencmpsize1 += (partval & 1);
    }

    finemeshptr->ncmpload[0] = coarmeshptr->ncmpload[0];
    finemeshptr->ncmpload[1] = coarmeshptr->ncmpload[1];
    finemeshptr->ncmpload[2] = coarmeshptr->ncmpload[2];
    finemeshptr->ncmploaddlt = coarmeshptr->ncmploaddlt;
    finemeshptr->ncmpsize[0] = finemeshptr->m.vnodnbr - finencmpsize1 - finefronnbr;
    finemeshptr->ncmpsize[1] = finencmpsize1;
    finemeshptr->fronnbr     = finefronnbr;
  }
  else                                            /* No coarse mesh provided       */
    vmeshZero (finemeshptr);                      /* Assign all vertices to part 0 */

#ifdef SCOTCH_DEBUG_VMESH2
  if (vmeshCheck (finemeshptr) != 0) {
    errorPrint ("vmeshSeparateMlUncoarsen: internal error (3)");
    return (1);
  }
#endif /* SCOTCH_DEBUG_VMESH2 */

  return (0);
}

/* This routine recursively performs the
** coarsening recursion.
** It returns:
** - 0   : if recursion proceeded well.
** - !0  : on error.
*/

static
int
vmeshSeparateMl2 (
Vmesh * restrict const                      finemeshptr, /* Vertex-separation mesh */
const VmeshSeparateMlParam * restrict const paraptr) /* Method parameters          */
{
  Vmesh               coarmeshdat;
  Gnum * restrict     finecoartax;
  int                 o;

  switch (vmeshSeparateMlCoarsen (finemeshptr, &coarmeshdat, &finecoartax, paraptr)) {
    case 0 :
      if (((o = vmeshSeparateMl2         (&coarmeshdat, paraptr))                  == 0) &&
          ((o = vmeshSeparateMlUncoarsen (finemeshptr, &coarmeshdat, finecoartax)) == 0) &&
          ((o = vmeshSeparateSt          (finemeshptr, paraptr->stratasc))         != 0)) /* Apply ascending strategy */
        errorPrint ("vmeshSeparateMl2: cannot apply ascending strategy");
      coarmeshdat.frontab = NULL;                 /* Prevent frontab of fine mesh from being freed */
      vmeshExit (&coarmeshdat);
      memFree (finecoartax + finemeshptr->m.baseval); /* Free finecoartab as not part of coarse mesh vertex group (unlike for graphCoarsen) */
      break;
    case 1 :
    case 2 :                                      /* Cannot coarsen due to lack of memory */
#ifdef SCOTCH_DEBUG_VMESH2
      finecoartax = NULL;                         /* Prevent Purify from yelling */
#endif /* SCOTCH_DEBUG_VMESH2 */
      if (((o = vmeshSeparateMlUncoarsen (finemeshptr, NULL, finecoartax)) == 0) && /* Finalize mesh    */
          ((o = vmeshSeparateSt          (finemeshptr, paraptr->stratlow)) != 0)) /* Apply low strategy */
        errorPrint ("vmeshSeparateMl2: cannot apply low strategy");
      break;
#ifdef SCOTCH_DEBUG_VMESH2
    default :
      o = 1;
      break;
#endif /* SCOTCH_DEBUG_VMESH2 */
  }

  return (o);
}

/*****************************/
/*                           */
/* This is the main routine. */
/*                           */
/*****************************/

int
vmeshSeparateMl (
Vmesh * restrict const              meshptr,      /*+ Node-separation mesh +*/
const VmeshSeparateMlParam * const  paraptr)      /*+ Method parameters    +*/
{
  Gnum                levlnum;                    /* Save value for mesh level */
  int                 o;

  levlnum = meshptr->levlnum;                     /* Save mesh level                */
  meshptr->levlnum = 0;                           /* Initialize coarsening level    */
  o = vmeshSeparateMl2 (meshptr, paraptr);        /* Perform multi-level separation */
  meshptr->levlnum = levlnum;                     /* Restore mesh level             */

  return (o);
}
