/* Copyright 2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : dgraph_build_grid3d.c                   **/
/**                                                        **/
/**   AUTHOR     : Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : These lines are the distributed source  **/
/**                graph building routines for 3D grid     **/
/**                graphs.                                 **/
/**                                                        **/
/**   DATES      : # Version 5.0  : from : 21 jul 2005     **/
/**                                 to   : 10 sep 2007     **/
/**                                                        **/
/************************************************************/

#define DGRAPH

#include "module.h"
#include "common.h"
#include "dgraph.h"


typedef struct DgraphBuildGrid3DPoint_ {
  Gnum x, y, z;
} DgraphBuildGrid3DPoint;


/* #define INTERIOR(x) (((x) < 2)?(0):((x)-2)) */
#define ORDINAL(x,y,z,height,width) ((x)+(y)*(height)+(z)*(height)*(width))

#ifndef inline
# define inline static __inline__
#endif /* inline */

inline Gnum
dgraphBuildGrid3DNeighbors (DgraphBuildGrid3DPoint * restrict edgecase,
			    Gnum x, Gnum y, Gnum z, Gnum height, Gnum width, Gnum depth)
{
  Gnum edgenbrcase;
  for (edgenbrcase = 0; edgenbrcase < 6 ; edgenbrcase ++)
    {
      edgecase [edgenbrcase].x = x;
      edgecase [edgenbrcase].y = y;
      edgecase [edgenbrcase].z = z;
    }
  edgenbrcase = 0;
  /* Fill edge array */
  if (x > 0)
    edgecase [edgenbrcase++].x = x-1;
  if (x < height - 1)
    edgecase [edgenbrcase++].x = x+1;
  if (y > 0)
    edgecase [edgenbrcase++].y = y-1;
  if (y < width - 1)
    edgecase [edgenbrcase++].y = y+1;
  if (z > 0)
    edgecase [edgenbrcase++].z = z-1;
  if (z < depth - 1)
    edgecase [edgenbrcase++].z = z+1;

  return (edgenbrcase);
}


/**
   @brief
   This routine builds a distributed grid of the given dimensions.

   @param grafptr  Pointer to the distributed graph to be completed
   @param height   First dimension of grid
   @param width    Second dimension of grid
   @param depth    Third dimension of grid
   @param baseval  Base value to access different tabs

   @return
     - 0  : if successfull
     - !0 : if an error occurs
*/

int
dgraphBuildGrid3D (
Dgraph * restrict const     grafptr,              /* Graph               */
const Gnum                  baseval,              /* Base value          */
const Gnum                  rheight,              /* First dimension     */
const Gnum                  rwidth,               /* Second dimension    */
const Gnum                  rdepth,               /* Third dimension     */
const int                   distribute)           /* By slices or random */
{
  Gnum                procngbnum;
  Gnum                vertglbnbr;                 /* Total number of vertices */
  Gnum                vertlocnbr;                 /* Number of local vertices */
  Gnum                vertlocnnd;
  Gnum                vertlocnum;                 /* Index of current local vertex (based) */
  Gnum *              vertloctnd;                 /* End of local vertex array             */
  Gnum *              vertloctax;                 /* Array of vertices                     */
  Gnum *              vertlocptr;                 /* Current vertices                      */
  Gnum *              vlblloctax;                 /* Array of labels                       */
  Gnum *              vlbllocptr;                 /* Pointer to the current label          */
  Gnum                edgeglbnbr;
  Gnum                edgelocnbr;
  Gnum *              edgeloctax;
  Gnum *              edgelocptr;
  Gnum                edgelocnum;
  Gnum *              edloloctax;
  Gnum *              edlolocptr;
  Gnum                slicewidth;
  Gnum                sliceoffset;                /* Beginning of the slice                */
  Gnum                height, width, depth;       /* We must have height >= width>= depth  */
  Gnum                swap;

  if ((rwidth < 1) || (rheight < 1) || (rdepth < 1)) /* At least one vertex */
    return (1);

  width  = rwidth;
  height = rheight;
  depth  = rdepth;

  if (height < width) {
    swap  = height;
    height = width;
    width  = swap;
  }
  if (height < depth) {
    swap   = height;
    height = depth;
    depth  = swap;
  }
  if (width < depth) {
    swap  = width;
    width = depth;
    depth = swap;
  }

  vertglbnbr  = width * height * depth;
  edgeglbnbr  = width * height * depth * 6;       /* Vertices which have 6 neighbors, thus they are in the box                          */
  edgeglbnbr -= 2 * (width * height + width * depth + height * depth);  /* Vertices which have only 5 neighbors, on the side of the box */

  if (edgeglbnbr > 6 * vertglbnbr) {
    errorPrint ("dgraphBuildGrid3D: internal error (1)");
    return     (1);
  }

  vertlocnum  =
  vertlocnbr  =
  edgelocnbr  =
  slicewidth  =
  sliceoffset = 0;
  if ((distribute == DGRAPHBUILDRANDOM) || (height < grafptr->procglbnbr)) {

    vertlocnbr = DATASIZE (vertglbnbr, grafptr->procglbnbr, grafptr->proclocnum);
    if ((grafptr->procglbnbr & 1) == 0) {           /* If even number of processes */
      /* Add ghost vertices to number of local vertices */
      if (grafptr->proclocnum < (grafptr->procglbnbr / 2))
        vertlocnbr += (grafptr->proclocnum - (grafptr->procglbnbr / 2));
      else
        vertlocnbr += (grafptr->proclocnum - (grafptr->procglbnbr / 2) + 1);

      /* The following loop computes indice of the first local vertex */
      for (procngbnum = 0, vertlocnum = 0;
           procngbnum < grafptr->proclocnum;
           procngbnum ++) {
        vertlocnum += DATASIZE (vertglbnbr, grafptr->procglbnbr, procngbnum);
        if (procngbnum < (grafptr->procglbnbr / 2))
          vertlocnum += (procngbnum - (grafptr->procglbnbr / 2));
        else
          vertlocnum += (procngbnum - (grafptr->procglbnbr / 2) + 1);
      }
    }
    else {                                          /* Odd number of processes */
      /* Add ghost vertices to number of local vertices */
      vertlocnbr += (grafptr->proclocnum - (int) (grafptr->procglbnbr / 2));

      /* The following loop computes indice of the first local vertex */
      for (procngbnum = 0, vertlocnum = 0; procngbnum < grafptr->proclocnum; procngbnum ++)
        vertlocnum += DATASIZE (vertglbnbr, grafptr->procglbnbr, procngbnum) +
          (procngbnum - (int) (grafptr->procglbnbr / 2));
    }

    edgelocnbr = vertlocnbr * 6; /* The true number is smaller */
    fprintf (stderr, "(I) vertlocnbr=%ld\t edgelocnbr=%ld\n", (long)vertlocnbr, (long)edgelocnbr);
  }
  else if (distribute == DGRAPHBUILDSLICES) {
    slicewidth  = height / grafptr->procglbnbr;
    sliceoffset = slicewidth * (grafptr->proclocnum);

    if (grafptr->proclocnum == grafptr->procglbnbr - 1) /* If the last proc */
      slicewidth += height - grafptr->procglbnbr * slicewidth;

    vertlocnbr  = slicewidth * width * depth;     /* Vertices which have 6 neighbors, thus they are in the box */
    edgelocnbr  = width * slicewidth * depth * 6;
    edgelocnbr -= 2 * (width * slicewidth + depth * slicewidth);

    if (grafptr->proclocnum == 0)
      edgelocnbr -= width * depth;
    if (grafptr->proclocnum == grafptr->procglbnbr - 1)
      edgelocnbr -= width * depth;
  }
#ifdef SCOTCH_DEBUG_DGRAPH2
  else {
    errorPrint ("dgraphBuildGrid3D: param error");
    return     (1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  fprintf (stderr, "vertlocnbr=%ld\t edgelocnbr=%ld\n", (long)vertlocnbr, (long)edgelocnbr);

  vertlocnnd = vertlocnbr + baseval;

  if (memAllocGroup ((void **) (void *)
                     &vertloctax, (size_t) ((vertlocnbr + 1) * sizeof (Gnum)), /* +1 to indicate end of array */
                     &vlblloctax, (size_t) (vertlocnbr       * sizeof (Gnum)), NULL) == NULL) {
    errorPrint ("dgraphBuildGrid3D: out of memory (1)");
    return     (1);
  }
  if (memAllocGroup ((void **) (void *)
                     &edgeloctax, (size_t) (edgelocnbr * sizeof (Gnum)),
                     &edloloctax, (size_t) (edgelocnbr * sizeof (Gnum)), NULL) == NULL) {
    memFree (vertloctax);
    errorPrint ("dgraphBuildGrid3D: out of memory (2)");
    return     (1);
  }
/*   if (memAllocGroup ((void **) (void *) */
/* 		     &vlblloctax, (size_t) (vertlocnbr * sizeof (Gnum)), */
/* 		     NULL) == NULL) { */
/*     memFree (edgeloctax); */
/*     memFree (vertloctax); */
/*     errorPrint ("dgraphBuildGrid3D: out of memory (3)"); */
/*     return (1); */
/*   } */

  vertloctax -= baseval;
  vlblloctax -= baseval;
  edgeloctax -= baseval;
  edloloctax -= baseval;

  if ((distribute == DGRAPHBUILDRANDOM) || (height < grafptr->procglbnbr)) {
    Gnum x,y,z;

    fprintf (stderr, "starting construction\n");

    for (vertlocptr = vertloctax + baseval,              /* pointer to current vertex */
         vertloctnd = vertloctax + baseval + vertlocnbr, /* pointer limit */
         vlbllocptr = vlblloctax + baseval,            /* Pointer to current label  */
         edgelocptr = edgeloctax + baseval,            /* Pointer to current edge   */
         edgelocnum = baseval,
         edlolocptr = edloloctax + baseval;            /* Pointer to weight of current edge */
         vertlocptr < vertloctnd;
         vertlocptr ++, vertlocnum ++, vlbllocptr ++) {  /* Take the next element */
      DgraphBuildGrid3DPoint edgecase[6];
      Gnum edgenbrcase;

      *vlbllocptr = ((vertlocnum * COARHASHPRIME) % vertglbnbr) + baseval; /* Hash vertices */
      /* To have an homogeneous distribution of label */
      *vertlocptr = edgelocnum;

      /* Fill edge array */

      z = *vlbllocptr / (height*width);
      y = (*vlbllocptr % (height*width)) / height;
      x = (*vlbllocptr % (height*width)) % height;

      edgenbrcase = dgraphBuildGrid3DNeighbors (edgecase, x, y, z, height, width, depth);
      edgelocnum += edgenbrcase;
      edgenbrcase --;

      for (; edgenbrcase >= 0; edgenbrcase --, edgelocptr ++, edlolocptr ++) {
        *edgelocptr = ORDINAL (edgecase [edgenbrcase].x, edgecase [edgenbrcase].y,
                               edgecase [edgenbrcase].z, height, width ) + baseval;
        *edlolocptr = ((*vlbllocptr + *edgelocptr) % 16) + 1; /* Pseudo random weight (1 to 16) */
      }
    }
    *vertlocptr = edgelocnum;                       /* Mark end of local vertex array */
    edgelocnbr  = edgelocnum - baseval;

  }
  else if (distribute == DGRAPHBUILDSLICES) {
    /* In this case, we make slices */
    Gnum x,y,z;

#ifdef SCOTCH_DEBUG_DGRAPH2
    fprintf (stderr, "x from %ld to %ld\n", (long)sliceoffset, (long)slicewidth + sliceoffset);
#endif /* SCOTCH_DEBUG_DGRAPH2 */

    for (z = 0, vertlocptr = vertloctax + baseval,              /* pointer to current vertex */
           *vertlocptr = baseval,
           vertloctnd = vertloctax + baseval + vertlocnbr, /* pointer limit */
           vlbllocptr = vlblloctax + baseval,            /* Pointer to current label  */
           edgelocptr = edgeloctax + baseval,            /* Pointer to current edge   */
           edgelocnum = baseval,
           edlolocptr = edloloctax + baseval;            /* Pointer to weight of current edge */
         z < depth ;
         ++ z)                                      /* Take the next element */
      for ( y = 0 ; y < width ; ++ y)
        for (x = sliceoffset ; x < sliceoffset + slicewidth  ; ++ x,
               vertlocptr ++, vertlocnum ++, vlbllocptr ++) {
          DgraphBuildGrid3DPoint edgecase[6];
          Gnum edgenbrcase;

          *vlbllocptr = ORDINAL (x, y, z, height, width) + baseval;
          *vertlocptr = edgelocnum;

#ifdef SCOTCH_DEBUG_DGRAPH2
          if (vertlocptr > vertloctnd) {
            fprintf (stderr, "vertlocnbr = %ld\tverlocnum = %ld\n", (long)vertlocnbr, (long)*vlbllocptr);
            errorPrint ("dgraphBuildGrid3D: internal error (2)");
            memFree    (vertloctax + baseval);           /* Free memory group leader */
            return     (1);
          }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

          edgenbrcase = dgraphBuildGrid3DNeighbors (edgecase, x, y, z, height, width, depth);

          edgelocnum += edgenbrcase;
          edgenbrcase --;

          for (; edgenbrcase >= 0; edgenbrcase --, edgelocptr ++, edlolocptr ++) {
            *edgelocptr = ORDINAL (edgecase [edgenbrcase].x, edgecase [edgenbrcase].y,
                                   edgecase [edgenbrcase].z, height, width ) + baseval;
            *edlolocptr = ((*vlbllocptr + *edgelocptr) % 16) + 1; /* Pseudo random weight (1 to 16) */
          }
        }
    *vertlocptr = edgelocnum;                       /* Mark end of local vertex array */
  }

#ifdef SCOTCH_DEBUG_DGRAPH2
  if (edgelocnum != edgelocnbr + baseval) {
    fprintf (stderr, "edgelocnbr = %ld\tedglocmax= %ld\tedgelocnum = %ld\n", (long)edgelocnbr, (long)vertlocnbr * 6, (long)edgelocnum);
    errorPrint ("dgraphBuildGrid3D: internal error (3)");
    memFree    (vertloctax + baseval);           /* Free memory group leader */
    return     (1);
  }
#endif /* SCOTCH_DEBUG_DGRAPH2 */

  if (dgraphBuild2 (grafptr, baseval,             /* Build the distributed graph */
                    vertlocnbr, vertlocnbr, vertloctax, vertloctax + 1, NULL, vertlocnbr, NULL, vlblloctax,
                    edgelocnbr, edgelocnbr, edgeloctax, NULL, edloloctax, 6) != 0) {
    memFree (edgeloctax + baseval);               /* Free memory group leaders */
    memFree (vertloctax + baseval);
    return  (1);
  }
  grafptr->vnumloctax = grafptr->vlblloctax;      /* No labels, but vertex indices after redistribution */
  grafptr->vlblloctax = NULL;

  grafptr->flagval |= (DGRAPHFREETABS | DGRAPHVERTGROUP | DGRAPHEDGEGROUP); /* Arrays created by the routine itself */

  return (0);
}
