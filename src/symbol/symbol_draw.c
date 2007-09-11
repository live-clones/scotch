/* Copyright 2004,2007 ENSEIRB, INRIA & CNRS
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
/**   NAME       : symbol_draw.c                           **/
/**                                                        **/
/**   AUTHORS    : Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                This module draws symbolic matrices in  **/
/**                PostScript (tm) format .                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 29 sep 1998     **/
/**                                 to     29 sep 1998     **/
/**                # Version 1.0  : from : 26 jun 2002     **/
/**                                 to     26 jun 2002     **/
/**                # Version 1.3  : from : 10 apr 2003     **/
/**                                 to     10 jun 2003     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define SYMBOL_DRAW

#include "common.h"
#include "symbol.h"
#include "symbol_draw.h"

/*
**  The static and global variables
*/

static float                symbolDrawColorTab[16][3] = {
                              { 1.00, 0.00, 0.00 }, /* Red          */
                              { 0.00, 1.00, 0.00 }, /* Green        */
                              { 1.00, 1.00, 0.00 }, /* Yellow       */
                              { 0.00, 0.00, 1.00 }, /* Blue         */
                              { 1.00, 0.00, 1.00 }, /* Magenta      */
                              { 0.00, 1.00, 1.00 }, /* Cyan         */
                              { 1.00, 0.50, 0.20 }, /* Orange       */
                              { 0.30, 0.55, 0.00 }, /* Olive        */
                              { 0.72, 0.47, 0.47 }, /* Dark pink    */
                              { 0.33, 0.33, 0.81 }, /* Sea blue     */
                              { 1.00, 0.63, 0.63 }, /* Pink         */
                              { 0.62, 0.44, 0.65 }, /* Violet       */
                              { 0.60, 0.80, 0.70 }, /* Pale green   */
                              { 0.47, 0.20, 0.00 }, /* Brown        */
                              { 0.00, 0.68, 0.68 }, /* Turquoise    */
                              { 0.81, 0.00, 0.40 } }; /* Purple     */

/******************************************/
/*                                        */
/* The symbolic matrix handling routines. */
/*                                        */
/******************************************/

/*+ This routine returns one of 16 predefined,
*** visually distinct distinct colors.
+*/

void
symbolDrawColor (
const INT                   labl,
float                       color[])
{
  color[0] = (float) symbolDrawColorTab[(labl - 1) % 16][0];
  color[1] = (float) symbolDrawColorTab[(labl - 1) % 16][1];
  color[2] = (float) symbolDrawColorTab[(labl - 1) % 16][2];
}

/*+ This routine writes to the given stream
*** a PostScript (tm) picture of the symbolic
*** block matrix.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolDrawFunc (
const SymbolMatrix * const  symbptr,
int                         (* diagfunc) (const SymbolMatrix * const, const SymbolBlok * const, void * const, float * const),
int                         (* offdfunc) (const SymbolMatrix * const, const SymbolBlok * const, void * const, float * const),
void * const                dataptr,              /* Data structure for block coloring */
FILE * const                stream)
{
  INT                 cblknum;                    /* Number of current column block */
  INT                 bloknum;                    /* Number of current block        */
  time_t              picttime;                   /* Creation time                  */
  double              pictsize;                   /* Number of distinct coordinates */
  int                 o;

  time (&picttime);                               /* Get current time */
  pictsize = (double) (symbptr->nodenbr + 1);     /* Get matrix size  */

  fprintf (stream, "%%!PS-Adobe-2.0 EPSF-2.0\n"); /* Write header */
  fprintf (stream, "%%%%Title: symbolmatrix (%ld,%ld,%ld)\n",
           (long) symbptr->cblknbr, (long) symbptr->bloknbr, (long) symbptr->nodenbr);
  fprintf (stream, "%%%%Creator: symbolDraw (LaBRI, Universite Bordeaux I)\n");
  fprintf (stream, "%%%%CreationDate: %s", ctime (&picttime));
  fprintf (stream, "%%%%BoundingBox: 0 0 %ld %ld\n",
           (long) (SYMBOL_PSPICTSIZE * SYMBOL_PSDPI),
           (long) (SYMBOL_PSPICTSIZE * SYMBOL_PSDPI));
  fprintf (stream, "%%%%Pages: 0\n");
  fprintf (stream, "%%%%EndComments\n");          /* Write shortcuts */
  fprintf (stream, "/c { 4 2 roll pop pop newpath 2 copy 2 copy moveto dup lineto dup lineto closepath fill } bind def\n");
  fprintf (stream, "/b { 4 copy 2 index exch moveto lineto dup 3 index lineto exch lineto closepath fill pop } bind def\n");
  fprintf (stream, "/r { setrgbcolor } bind def\n");
  fprintf (stream, "/g { setgray } bind def\n");

  fprintf (stream, "gsave\n");                    /* Save context         */
  fprintf (stream, "0 setlinecap\n");             /* Use miter caps       */
  fprintf (stream, "%f dup scale\n",              /* Print scaling factor */
           (double) SYMBOL_PSDPI * SYMBOL_PSPICTSIZE / pictsize);
  fprintf (stream, "[ 1 0 0 -1 0 %d ] concat\n",  /* Reverse Y coordinate */
           (int) (symbptr->nodenbr + 1));

  fprintf (stream, "0 0\n");                      /* Output fake column block */
  for (cblknum = 0, bloknum = 0; cblknum < symbptr->cblknbr; cblknum ++) {
    float               coloval[3];               /* Color of diagonal block and previous color */
    INT                 blokend;                  /* Number of end block for column             */

    coloval[0] =
    coloval[1] =
    coloval[2] = 0.5;
    if (diagfunc != NULL)                         /* Always display diagonal blocks */
      diagfunc (symbptr, &symbptr->bloktab[bloknum], dataptr, coloval);
    if ((coloval[0] == coloval[1]) &&
        (coloval[1] == coloval[2]))
      fprintf (stream, "%.2g g ",
               (float) coloval[0]);
    else
      fprintf (stream, "%.2g %.2g %.2g r \n",
               (float) coloval[0], (float) coloval[1], (float) coloval[2]);

    fprintf (stream, "%ld\t%ld\tc\n",             /* Begin new column block */
             (long) (symbptr->cblktab[cblknum].fcolnum - symbptr->baseval),
             (long) (symbptr->cblktab[cblknum].lcolnum - symbptr->baseval + 1));

    for (bloknum ++, blokend = symbptr->cblktab[cblknum + 1].bloknum; /* Skip diagonal block */
         bloknum < blokend; bloknum ++) {
      float               colbval[3];             /* Color of off-diagonal block */

      colbval[0] =
      colbval[1] =
      colbval[2] = 0.0;
      if ((offdfunc == NULL) || (offdfunc (symbptr, &symbptr->bloktab[bloknum], dataptr, colbval) != 0)) { /* If block is kept */
        if ((coloval[0] != colbval[0]) ||         /* If change with respect to previous  color */
            (coloval[1] != colbval[1]) ||
            (coloval[2] != colbval[2])) {
          coloval[0] = colbval[0];                /* Save new color data */
          coloval[1] = colbval[1];
          coloval[2] = colbval[2];

          if ((coloval[0] == coloval[1]) &&
              (coloval[1] == coloval[2]))
            fprintf (stream, "%.2g g ",
                     (float) coloval[0]);
          else
            fprintf (stream, "%.2g %.2g %.2g r \n",
                     (float) coloval[0], (float) coloval[1], (float) coloval[2]);
        }

        fprintf (stream, "%ld\t%ld\tb\n",         /* Write block in column block */
                 (long) (symbptr->bloktab[bloknum].frownum - symbptr->baseval),
                 (long) (symbptr->bloktab[bloknum].lrownum - symbptr->baseval + 1));
      }
    }
  }
  fprintf (stream, "pop pop\n");                  /* Purge last column block indices */
  o = fprintf (stream, "grestore\nshowpage\n");   /* Restore context                 */

  return ((o != EOF) ? 0 : 1);
}

/*+ This routine writes to the given stream
*** a PostScript (tm) picture of the symbolic
*** block matrix, with diagonal blocks in
*** black and off-diagonal blocks in dark gray.
*** It returns:
*** - 0   : on success.
*** - !0  : on error.
+*/

int
symbolDraw (
const SymbolMatrix * const  symbptr,
FILE * const                stream)
{
  return (symbolDrawFunc (symbptr, NULL, NULL, NULL, stream));
}
