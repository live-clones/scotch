/************************************************************/
/**                                                        **/
/**   NAME       : gout_o.c                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a result viewer.                **/
/**                This module contains output routines.   **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 07 oct 1994     **/
/**                                 to     23 dec 1994     **/
/**                # Version 3.0  : from : 14 jul 1995     **/
/**                                 to     03 oct 1995     **/
/**                # Version 3.1  : from : 28 mar 1996     **/
/**                                 to     03 jun 1996     **/
/**                # Version 3.2  : from : 02 dec 1996     **/
/**                                 to     05 jun 1998     **/
/**                # Version 3.3  : from : 29 may 1999     **/
/**                                 to   : 03 jun 1999     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to     11 dec 2001     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes
*/

#define GOUT

#include "common.h"
#include "scotch.h"
#include "gout_c.h"
#include "gout_o.h"

/*
**  The static and global variables
*/

static O_OutParam           O_outParam = {        /* Parameter structure        */
                              O_OUTTYPEINVMESH,   /* Default output type        */
                              { 'v' },            /* OpenInventor mesh defaults */
                              { 'f' },            /* PostScript matrix defaults */
                              { 'f', 'g',         /* PostScript mesh defaults   */
                                'v', 'd',
                                's',
                                { { 0.0, 0.0 } },
                                { { 1.0, 1.0 } } } };

static C_ParseCode          O_outList[] = {       /* Output code list */
                              { O_OUTTYPEINVMESH, "i"  },
                              { O_OUTTYPEPOSMATR, "m"  },
                              { O_OUTTYPEPOSMESH, "p"  },
                              { O_OUTTYPENBR,     NULL } };

static C_ParseArg           O_outArg[] = {        /* Output type argument list */
                              { "r",  O_OUTTYPEINVMESH, NULL,  &O_outParam.InvMesh.edge },
                              { "v",  O_OUTTYPEINVMESH, NULL,  &O_outParam.InvMesh.edge },
                              { "e",  O_OUTTYPEPOSMATR, NULL,  &O_outParam.PosMatr.type  },
                              { "f",  O_OUTTYPEPOSMATR, NULL,  &O_outParam.PosMatr.type  },
                              { "c",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.color },
                              { "g",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.color },
                              { "e",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.type  },
                              { "f",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.type  },
                              { "l",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.clip  },
                              { "s",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.clip  },
                              { "a",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.disk  },
                              { "d",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.disk  },
                              { "r",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.edge  },
                              { "v",  O_OUTTYPEPOSMESH, NULL,  &O_outParam.PosMesh.edge  },
                              { "x",  O_OUTTYPEPOSMESH, "%lf", &O_outParam.PosMesh.min.x },
                              { "X",  O_OUTTYPEPOSMESH, "%lf", &O_outParam.PosMesh.max.x },
                              { "y",  O_OUTTYPEPOSMESH, "%lf", &O_outParam.PosMesh.min.y },
                              { "Y",  O_OUTTYPEPOSMESH, "%lf", &O_outParam.PosMesh.max.y },
                              { NULL, O_OUTTYPENBR,     "",    NULL                      } };

static double               O_colorTab[16][3] = { /* Color list   */
                              { 1.00, 0.00, 0.00 }, /* Red        */
                              { 0.00, 1.00, 0.00 }, /* Green      */
                              { 1.00, 1.00, 0.00 }, /* Yellow     */
                              { 0.00, 0.00, 1.00 }, /* Blue       */
                              { 1.00, 0.00, 1.00 }, /* Magenta    */
                              { 0.00, 1.00, 1.00 }, /* Cyan       */
                              { 1.00, 0.50, 0.20 }, /* Orange     */
                              { 0.30, 0.55, 0.00 }, /* Olive      */
                              { 0.72, 0.47, 0.47 }, /* Dark pink  */
                              { 0.33, 0.33, 0.81 }, /* Sea blue   */
                              { 1.00, 0.63, 0.63 }, /* Pink       */
                              { 0.62, 0.44, 0.65 }, /* Violet     */
                              { 0.60, 0.80, 0.70 }, /* Pale green */
                              { 0.47, 0.20, 0.00 }, /* Brown      */
                              { 0.00, 0.68, 0.68 }, /* Turquoise  */
                              { 0.81, 0.00, 0.40 } }; /* Purple   */

/****************************************/
/*                                      */
/* This is the color selection routine. */
/*                                      */
/****************************************/

void
O_outColor (
const SCOTCH_Num            labl,
double                      color[])
{
  if (labl == (-1)) {
    color[0] =
    color[1] =
    color[2] = 1.0L;
  }
  else {
    color[0] = (double) O_colorTab[labl % 16][0];
    color[1] = (double) O_colorTab[labl % 16][1];
    color[2] = (double) O_colorTab[labl % 16][2];
  }
}

/****************************/
/*                          */
/* The main output routine. */
/*                          */
/****************************/

/* This routine parses the output
** option string.
** It returns:
** - 0  : if string successfully scanned.
** - 1  : if invalid options
** - 2  : if invalid option arguments.
** - 3  : if syntax error in string.
*/

int
O_outParse (
char * const                string)
{
  return (C_parse (O_outList, O_outArg, (uint * const) &O_outParam.type, string));
}


/* This routine is the generic output call.
** It returns:
** - VOID  : in all cases.
*/

void
O_out (
const C_Graph * const       grafptr,              /* Graph structure */
const C_Geometry * const    geomptr,              /* Graph geometry  */
const C_Mapping * const     mapptr,               /* Result mapping  */
FILE * const                stream)               /* Output stream   */
{
  switch (O_outParam.type) {
    case O_OUTTYPEINVMESH :                       /* Mesh OpenInventor output type */
      O_outInvMesh (grafptr, geomptr, mapptr, stream);
      break;
    case O_OUTTYPEPOSMATR :                       /* Matrix PostScript output type */
      O_outPosMatr (grafptr, geomptr, mapptr, stream);
      break;
    case O_OUTTYPEPOSMESH :                       /* Mesh PostScript output type */
      O_outPosMesh (grafptr, geomptr, mapptr, stream);
      break;
    default :
      errorPrint ("O_out : invalid output method (%d)", O_outParam.type);
  }
}

/******************************************/
/**                                      **/
/** This is the Inventor output routine. **/
/**                                      **/
/******************************************/

int
O_outInvMesh (
const C_Graph * const       grafptr,              /* Graph structure, sorted by vertex index */
const C_Geometry * const    geomptr,              /* Graph geometry, sorted by vertex label  */
const C_Mapping * const     mapptr,               /* Result mapping, sorted by vertex label  */
FILE * const                stream)               /* Output stream                           */
{
  O_InvMeshPath *     pattab;                     /* Array of path building data */
  uint *              idxtab;                     /* Array of indexes            */
  uint                idxnbr;                     /* Number of indexes           */
  time_t              pictime;                    /* Creation time               */
  double              color[3];                   /* Vertex color                */
  uint                i, j, k;

  if (geomptr->verttab == NULL) {
    errorPrint ("O_outInvMesh : geometry not provided");
    return      (1);
  }

  time (&pictime);                                /* Get current time */

  if (((idxtab = (unsigned int *)  memAlloc ((grafptr->edgenbr / 2) * 3 * sizeof (unsigned int))) == NULL) ||
      ((pattab = (O_InvMeshPath *) memAlloc (grafptr->vertnbr * sizeof (O_InvMeshPath)))          == NULL)) {
    errorPrint ("O_outInvMesh : out of memory");
    if (idxtab != NULL)
      memFree (idxtab);
    return (1);
  }
  idxnbr = 0;                                     /* No indexes yet */

  for (i = 0, j = 0; i < grafptr->vertnbr; i ++) { /* For all vertices                  */
    pattab[i].nbr = 0;                            /* Compute the number of output paths */
    pattab[i].idx = grafptr->verttab[i];
    for ( ; j < grafptr->vendtab[i]; j ++) {
      if ((grafptr->edgetab[j] > i) &&            /* If it can be an output edge */
          ((O_outParam.InvMesh.edge != 'r') ||    /* And this edge can be drawn  */
           (mapptr->labltab[i] == mapptr->labltab[grafptr->edgetab[j]])))
        pattab[i].nbr ++;                         /* One more path to higher vertices */
    }
  }
  for (i = 0; i < grafptr->vertnbr; ) {           /* For all vertices                  */
    if (pattab[i].nbr == 0) {                     /* If no output path for this vertex */
      i ++;                                       /* Skip to next vertex               */
      continue;
    }

    j = i;                                        /* Begin with this vertex        */
    idxtab[idxnbr ++] = j;                        /* Add it to the current segment */
    do {
      for (k = pattab[j].idx; k < grafptr->vendtab[j]; k ++) { /* Search for first output */
        if ((grafptr->edgetab[k] > j) &&          /* If it can be an output edge          */
          ((O_outParam.InvMesh.edge != 'r') ||    /* And this edge can be drawn           */
           (mapptr->labltab[j] == mapptr->labltab[grafptr->edgetab[k]])))
          break;
      }
      pattab[j].nbr --;                           /* One less output path remaining */
      pattab[j].idx = k + 1;                      /* Search from the next position  */
      j = grafptr->edgetab[k];                    /* Get the path end vertex number */
      idxtab[idxnbr ++] = j;                      /* Add it to the current segment  */
    } while (pattab[j].nbr > 0);                  /* As long as there is a path     */
    idxtab[idxnbr ++] = ~0;                       /* Mark end of path               */
  }

  fprintf (stream, "#Inventor V2.0 ascii\n");     /* Write header */
  fprintf (stream, "#Title: %s %s %s\n",
           C_filenamesrcinp, C_filenamegeoinp, C_filenamemapinp);
  fprintf (stream, "#Creator: out (F. Pellegrini, LaBRI, Bordeaux)\n");
  fprintf (stream, "#CreationDate: %s", ctime (&pictime));

  if (idxnbr == 0)                                /* If nothing to write */
    return (0);

  fprintf (stream, "Separator {\n");
  fprintf (stream, "  LightModel {\n    model\t\tBASE_COLOR\n  }\n");
  fprintf (stream, "  DrawStyle {\n    style\t\tLINES\n  }\n");
  fprintf (stream, "  MaterialBinding {\n    value\t\tPER_VERTEX\n  }\n");

  fprintf (stream, "  Coordinate3 {\n    point [\n\t%g\t%g\t%g", /* Write vertex coordinates */
           geomptr->verttab[0].x,
           geomptr->verttab[0].y,
           geomptr->verttab[0].z);
  for (i = 1; i < grafptr->vertnbr; i ++)
    fprintf (stream, ",\n\t%g\t%g\t%g",
             geomptr->verttab[i].x,
             geomptr->verttab[i].y,
             geomptr->verttab[i].z);
  fprintf (stream, " ]\n  }\n");

  fprintf (stream, "  BaseColor {\n    rgb [");   /* Write color vector */
  for (i = 0; i < idxnbr - 2; i ++) {
    if (idxtab[i] != ~0) {
      O_outColor (mapptr->labltab[idxtab[i]], color);
      fprintf (stream, "\n\t%g\t%g\t%g,",
               (double) color[0],
               (double) color[1],
               (double) color[2]);
    }
  }
  O_outColor (mapptr->labltab[idxtab[idxnbr - 2]], color);
  fprintf (stream, "\n\t%g\t%g\t%g ]\n  }\n",
           (double) color[0],
           (double) color[1],
           (double) color[2]);

  fprintf (stream, "  IndexedLineSet {\n    coordIndex ["); /* Write set of lines */
  for (i = 0; i < idxnbr - 1; i ++) {
    if ((i % 8) == 0)
      fprintf (stream, "\n");
    if (idxtab[i] == ~0)
      fprintf (stream, "\t-1,");
    else
      fprintf (stream, "\t%u,", idxtab[i]);
  }
  if (((idxnbr - 1) % 8) == 0)
    fprintf (stream, "\n");
  fprintf (stream, "\t-1 ]\n  }\n");

  fprintf (stream, "}\n");                        /* Write end of separator */


#if 0
  for (i = 0; i < grafptr->vertnbr; i ++) {       /* For all vertices */
    O_outColor (mapptr->labltab[i], color);
    fprintf (stream, "Separator { Translation { translation %lg %lg %lg } BaseColor { rgb [ %lg %lg %lg ] } Sphere { radius 0.3 } }\n",
             geomptr->verttab[i].x,
             geomptr->verttab[i].y,
             geomptr->verttab[i].z,
             (double) color[0],
             (double) color[1],
             (double) color[2]);
  }
#endif

  memFree (pattab);                               /* Free path array  */
  memFree (idxtab);                               /* Free index array */

  return (0);
}

/*************************************************/
/*                                               */
/* This is the PostScript matrix output routine. */
/*                                               */
/*************************************************/

int
O_outPosMatr (
const C_Graph * const       grafptr,              /* Graph structure, sorted by vertex index */
const C_Geometry * const    geomptr,              /* Graph geometry, sorted by vertex label  */
const C_Mapping * const     mapptr,               /* Result mapping, sorted by vertex label  */
FILE * const                stream)               /* Output stream                           */
{
  unsigned int *      nonztab;                    /* Array of non-zero entries              */
  unsigned int        nonzfrst;                   /* First non-zero entry of area           */
  unsigned int        nonzlast;                   /* Last non-zero entry of area            */
  double              pictsize;                   /* Number of distinct coordinates         */
  double              pictdisp;                   /* Size of the matrix display (in inches) */
  time_t              picttime;                   /* Creation time                          */
  SCOTCH_Num          colnum;
  SCOTCH_Num          vertnum;
  SCOTCH_Num *        edgeptr;

  if ((nonztab = (unsigned int *) memAlloc ((grafptr->vertnbr + 1) * sizeof (unsigned int))) == NULL) {
    errorPrint ("O_outPosMatr : out of memory");
    return      (1);
  }

  time (&picttime);                               /* Get current time */
  pictsize  = (double) (grafptr->vertnbr + 1);    /* Get matrix size  */
  pictdisp  = MIN (O_PSPICTWIDTH, O_PSPICTHEIGHT);

  if (O_outParam.PosMatr.type == 'e') {           /* EPSF-type output */
    fprintf (stream, "%%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf (stream, "%%%%Title: %s %s %s\n",
             C_filenamesrcinp, C_filenamegeoinp, C_filenamemapinp);
    fprintf (stream, "%%%%Creator: out (F. Pellegrini, LaBRI, Bordeaux)\n");
    fprintf (stream, "%%%%CreationDate: %s", ctime (&picttime));
    fprintf (stream, "%%%%BoundingBox: 0 0 %d %d\n",
             (int) (pictdisp * O_PSDPI), (int) (pictdisp * O_PSDPI));
    fprintf (stream, "%%%%Pages: 0\n");
    fprintf (stream, "%%%%EndComments\n");
  }
  else {                                          /* Full page output */
    fprintf (stream, "%%!PS-Adobe-2.0\n");
    fprintf (stream, "%%%%Title: %s %s %s\n",
             C_filenamesrcinp, C_filenamegeoinp, C_filenamemapinp);
    fprintf (stream, "%%%%Creator: out (F. Pellegrini, LaBRI, Bordeaux)\n");
    fprintf (stream, "%%%%CreationDate: %s", ctime (&picttime));
  }

  fprintf (stream, "/p { pop } bind def\n");
  fprintf (stream, "/h { 3 1 roll exch 2 copy moveto 2 copy 1 add 5 -3 roll 3 1 roll add exch 2 copy lineto 1 add lineto lineto fill } bind def\n");
  fprintf (stream, "/v { 3 copy pop moveto 2 copy add exch pop exch 3 copy pop pop 1 add dup 3 -1 roll lineto exch dup 3 1 roll lineto lineto fill } bind def\n");
  fprintf (stream, "/b { 3 copy v 3 copy h pop pop } bind def\n");
  fprintf (stream, "/c { 1 3 copy v 3 copy h pop pop } bind def\n");

  fprintf (stream, "gsave\n");                    /* Save the context    */
  fprintf (stream, "0 setlinecap\n");             /* Perform miter caps  */
  if (O_outParam.PosMatr.type == 'f')             /* If full page output */
    fprintf (stream, "%d %d translate\n",         /* Center the picture  */
             (int) (O_PSDPI * (O_PSPAGEWIDTH - pictdisp)) / 2,
             (int) (O_PSDPI * (O_PSPAGEWIDTH - pictdisp)) / 2);
  fprintf (stream, "%f %f scale\n",               /* Print scaling factor */
           (double) O_PSDPI * pictdisp / pictsize,
           (double) O_PSDPI * pictdisp / pictsize);
  fprintf (stream, "[ 1 0 0 -1 0 %d ] concat\n",  /* Reverse Y coordinate */
           (int) (grafptr->vertnbr + 1));
  fprintf (stream, "0 setgray newpath\n");        /* Select black color */

  for (vertnum = 0; vertnum < grafptr->vertnbr; vertnum ++) {
    colnum = (mapptr->labltab[vertnum] == ~0) ? vertnum : mapptr->labltab[vertnum];

    fprintf (stream, "%u\n", colnum);             /* Set column value */
    memset (nonztab, 0, (colnum + 2) * sizeof (unsigned int));
    for (edgeptr = grafptr->edgetab + grafptr->verttab[colnum];
         edgeptr < grafptr->edgetab + grafptr->vendtab[colnum]; edgeptr ++) {
      if (*edgeptr < colnum)
        nonztab[*edgeptr] = 1;
    }
    nonztab[colnum] = 1;                          /* Diagonal is non-zero */
    for (nonzfrst = 0; nonzfrst <= vertnum; nonzfrst ++) {
      if (nonztab[nonzfrst] != 0) {               /* A non-zero has been found */
        for (nonzlast = nonzfrst; nonztab[nonzlast] != 0; nonzlast ++) ;
        if ((nonzlast - nonzfrst) > 1)            /* Draw row block coefficient */
          fprintf (stream, "%u %u b\n", nonzfrst, nonzlast - nonzfrst);
        else
          fprintf (stream, "%u c\n", nonzfrst);
        nonzfrst = nonzlast - 1;
      }
    }
    fprintf (stream, "p ");                       /* Close the column */
  }

  fprintf (stream, "\ngrestore\n");               /* Restore context     */
  if (O_outParam.PosMatr.type == 'f')             /* If full page output */
    fprintf (stream, "showpage\n");               /* Display the page    */

  memFree (nonztab);

  return (0);
}

/***********************************************/
/*                                             */
/* This is the PostScript mesh output routine. */
/*                                             */
/***********************************************/

int
O_outPosMesh (
const C_Graph * const       grafptr,              /* Graph structure, sorted by vertex index */
const C_Geometry * const    geomptr,              /* Graph geometry, sorted by vertex label  */
const C_Mapping * const     mapptr,               /* Result mapping, sorted by vertex label  */
FILE * const                stream)               /* Output stream                           */
{
  uint                idxnbr;                     /* Number of indexes                               */
  uint *              idxtab;                     /* Array of indexes                                */
  O_PosMeshPath *     pattab;                     /* Array of path building data                     */
  O_PosMeshVertex *   pictab;                     /* Array of 2D coordinates, sorted by vertex index */
  O_Point             picmin;                     /* Picture minimum and maximum coordinates         */
  O_Point             picmax;
  O_Point             picdelt;
  double              picscale;                   /* Scaling factor          */
  double              picsrad;                    /* Square of circle radius */
  time_t              pictime;                    /* Creation time           */
  double              color[3];                   /* Color values            */
  uint                i, j, k;

  if (geomptr->verttab == NULL) {
    errorPrint ("O_outPosMesh : geometry not provided");
    return      (1);
  }

  time (&pictime);                                /* Get current time */

  if (((pictab = (O_PosMeshVertex *) memAlloc (grafptr->vertnbr * sizeof (O_PosMeshVertex)))        == NULL) ||
      ((idxtab = (unsigned int *)    memAlloc ((grafptr->edgenbr / 2) * 3 * sizeof (unsigned int))) == NULL) ||
      ((pattab = (O_PosMeshPath *)   memAlloc (grafptr->vertnbr * sizeof (O_PosMeshPath)))          == NULL)) {
    errorPrint ("O_outPosMesh : out of memory");
    if (pictab != NULL) {
      if (idxtab != NULL)
        memFree (idxtab);
      memFree (pictab);
    }
    return (1);
  }

  for (i = 0; i < grafptr->vertnbr; i ++) {       /* For all vertex indices              */
    pictab[i].pos.x = geomptr->verttab[i].x +     /* Project 3D coordinates into 2D ones */
                      geomptr->verttab[i].z * (O_POSMESHISOCOS * O_POSMESHISOREDUC);
    pictab[i].pos.y = geomptr->verttab[i].y +
                      geomptr->verttab[i].z * (O_POSMESHISOSIN * O_POSMESHISOREDUC);
  }

  picmin.x = picmin.y =  1e30;                    /* Pre-set coordinates extrema */
  picmax.x = picmax.y = -1e30;

  if (O_outParam.PosMesh.clip == 'l') {           /* If clipping encompasses disks  */
    for (i = 0, j = 0; i < grafptr->vertnbr; i ++) {
      pictab[i].rad = 1e30;                       /* Assume a huge square of radius */
      for ( ; j < grafptr->vendtab[i]; j ++) {
        k = grafptr->edgetab[j];
        picsrad = (pictab[i].pos.x - pictab[k].pos.x) *
                  (pictab[i].pos.x - pictab[k].pos.x) +
                  (pictab[i].pos.y - pictab[k].pos.y) *
                  (pictab[i].pos.y - pictab[k].pos.y);
        if (picsrad < pictab[i].rad)              /* Get the smallest square of radius */
         pictab[i].rad = picsrad;
      }
      pictab[i].rad = sqrt (pictab[i].rad) / 2.0; /* Keep the half-distance for radius */

      if ((pictab[i].pos.x - pictab[i].rad) < picmin.x) /* Update extrema if necessary */
        picmin.x = pictab[i].pos.x - pictab[i].rad;
      if ((pictab[i].pos.y - pictab[i].rad) < picmin.y)
        picmin.y = pictab[i].pos.y - pictab[i].rad;
      if ((pictab[i].pos.x + pictab[i].rad) > picmax.x)
        picmax.x = pictab[i].pos.x + pictab[i].rad;
      if ((pictab[i].pos.y + pictab[i].rad) > picmax.y)
        picmax.y = pictab[i].pos.y + pictab[i].rad;
    }
  }
  else {                                          /* Border clipping             */
    for (i = 0; i < grafptr->vertnbr; i ++) {     /* For all vertex indices      */
      if (pictab[i].pos.x < picmin.x)             /* Update extrema if necessary */
        picmin.x = pictab[i].pos.x;
      if (pictab[i].pos.y < picmin.y)
        picmin.y = pictab[i].pos.y;
      if (pictab[i].pos.x > picmax.x)
        picmax.x = pictab[i].pos.x;
      if (pictab[i].pos.y > picmax.y)
        picmax.y = pictab[i].pos.y;
    }
  }

  picdelt.x = picmax.x - picmin.x;                /* Compute picture extents */
  picdelt.y = picmax.y - picmin.y;
  picmin.x += picdelt.x * O_outParam.PosMesh.min.x; /* Resize picture (if necessary) */
  picmin.y += picdelt.y * O_outParam.PosMesh.min.y;
  picmax.x -= picdelt.x * (1.0L - O_outParam.PosMesh.max.x);
  picmax.y -= picdelt.y * (1.0L - O_outParam.PosMesh.max.y);
  picdelt.x = picmax.x - picmin.x;                /* Recompute picture extents */
  picdelt.y = picmax.y - picmin.y;

  picscale = (picdelt.x == 0.0L)                  /* Compute scaling factor */
             ? ((picdelt.y == 0.0L)
                ? 1.0L
                : (O_PSPICTHEIGHT / picdelt.y))
             : ((picdelt.y == 0.0L)
                ? (O_PSPICTWIDTH / picdelt.x)
                : MIN (O_PSPICTWIDTH  / picdelt.x, O_PSPICTHEIGHT / picdelt.y));

  picdelt.x *= picscale * O_POSMESHPICTRESOL;     /* Rescale extents */
  picdelt.y *= picscale * O_POSMESHPICTRESOL;

  for (i = 0; i < grafptr->vertnbr; i ++) {
    pictab[i].pos.x = (pictab[i].pos.x - picmin.x) * picscale * O_POSMESHPICTRESOL; /* Rescale coordinates */
    pictab[i].pos.y = (pictab[i].pos.y - picmin.y) * picscale * O_POSMESHPICTRESOL;
  }

  if (O_outParam.PosMesh.disk == 'd') {           /* If disks wanted */
    for (i = 0, j = 0; i < grafptr->vertnbr; i ++) {
      pictab[i].rad = 1e30;                       /* Assume huge square of radius */
      for ( ; j < grafptr->vendtab[i]; j ++) {
        k = grafptr->edgetab[j];
        picsrad = (pictab[i].pos.x - pictab[k].pos.x) *
                  (pictab[i].pos.x - pictab[k].pos.x) +
                  (pictab[i].pos.y - pictab[k].pos.y) *
                  (pictab[i].pos.y - pictab[k].pos.y);
        if (picsrad < pictab[i].rad)              /* Get smallest square of radius */
          pictab[i].rad = picsrad;
      }
      pictab[i].rad = sqrt (pictab[i].rad) / 2.0; /* Keep the half-distance for radius */
      if (pictab[i].rad < 1.0L)                  /* Always get a non-zero radius       */
        pictab[i].rad = 1.0L;

      pictab[i].vis = ((pictab[i].pos.x > - pictab[i].rad)           && /* Compute vertex visibility */
                       (pictab[i].pos.x < picdelt.x + pictab[i].rad) &&
                       (pictab[i].pos.y > - pictab[i].rad)           &&
                       (pictab[i].pos.y < picdelt.y + pictab[i].rad)) ? 1 : 0;
    }
  }
  else {                                          /* If disks not wanted */
    for (i = 0; i < grafptr->vertnbr; i ++)
      pictab[i].vis = ((pictab[i].pos.x > 0.0L)      && /* Compute vertex visibility */
                       (pictab[i].pos.x < picdelt.x) &&
                       (pictab[i].pos.y > 0.0L)      &&
                       (pictab[i].pos.y < picdelt.y)) ? 1 : 0;
  }
  for (i = 0; i < grafptr->vertnbr; i ++) {
    pictab[i].pos.x += 0.5L;                      /* Prepare to switch to integer coordinates */
    pictab[i].pos.y += 0.5L;
  }
  picdelt.x += 0.5L;
  picdelt.y += 0.5L;

  if (O_outParam.PosMesh.color == 'c') {          /* If color output               */
    for (i = 0; i < grafptr->vertnbr; i ++)       /* Select color for all vertices */
      pictab[i].col = mapptr->labltab[i] % O_POSMESHCOLNBR;
  }
  else {                                          /* If gray level output                  */
    for (i = 0; i < grafptr->vertnbr; i ++) {     /* Select color for all vertices         */
      for (j = mapptr->labltab[i] & 255, k = 7, pictab[i].col = 0; /* Half-tone color      */
           j > 0;                                 /* As long as there are subdivision bits */
           j >>= 1, k --)                         /* (Same as reversing the last 8 bits )  */
        pictab[i].col |= ((j & 1) << k);
    }
  }

  for (i = 0, j = 0; i < grafptr->vertnbr; i ++) { /* For all vertices                  */
    pattab[i].nbr = 0;                            /* Compute the number of output paths */
    pattab[i].idx = grafptr->verttab[i];
    for ( ; j < grafptr->vendtab[i]; j ++) {
      if ((grafptr->edgetab[j] > i) &&            /* If it can be an output edge           */
          ((pictab[i].vis | pictab[grafptr->edgetab[j]].vis) != 0) && /* And it is visible */
          ((O_outParam.PosMesh.edge != 'r') ||    /* And it can be drawn                   */
           (mapptr->labltab[i] == mapptr->labltab[grafptr->edgetab[j]])))
        pattab[i].nbr ++;                         /* One more path to higher vertices */
    }
  }
  idxnbr = 0;                                     /* No indexes yet */
  for (i = 0; i < grafptr->vertnbr; ) {
    if (pattab[i].nbr == 0) {                     /* If no output path       */
      i ++;                                       /* Skip to the next vertex */
      continue;
    }

    j = i;                                        /* Begin with this vertex        */
    idxtab[idxnbr ++] = j;                        /* Add it to the current segment */
    do {
      for (k = pattab[j].idx; k < grafptr->vendtab[j]; k ++) { /* Search for first output    */
        if ((grafptr->edgetab[k] > j) &&          /* If it can be an output edge             */
            ((pictab[j].vis | pictab[grafptr->edgetab[k]].vis) != 0) && /* And it is visible */
            ((O_outParam.InvMesh.edge != 'r') ||  /* And it can be drawn                     */
             (mapptr->labltab[j] == mapptr->labltab[grafptr->edgetab[k]])))
          break;
      }
      pattab[j].nbr --;                           /* One less output path remaining */
      pattab[j].idx = k + 1;                      /* Search from the next position  */
      j = grafptr->edgetab[k];                    /* Get the path end vertex number */
      idxtab[idxnbr ++] = j;                      /* Add it to the current segment  */
    } while (pattab[j].nbr > 0);                  /* As long as there is a path     */
    idxtab[idxnbr ++] = ~0;                       /* Mark end of path               */
  }

  if (O_outParam.PosMesh.type == 'e') {           /* EPSF-type output */
    fprintf (stream, "%%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf (stream, "%%%%Title: %s %s %s\n",
             C_filenamesrcinp, C_filenamegeoinp, C_filenamemapinp);
    fprintf (stream, "%%%%Creator: out (F. Pellegrini, LaBRI, Bordeaux)\n");
    fprintf (stream, "%%%%CreationDate: %s", ctime (&pictime));
    fprintf (stream, "%%%%BoundingBox: 0 0 %d %d\n",
             (int) ((picdelt.x * O_PSDPI) / O_POSMESHPICTRESOL),
             (int) ((picdelt.y * O_PSDPI) / O_POSMESHPICTRESOL));
    fprintf (stream, "%%%%Pages: 0\n");
    fprintf (stream, "%%%%EndComments\n");
  }
  else {                                          /* Full page output */
    fprintf (stream, "%%!PS-Adobe-2.0\n");
    fprintf (stream, "%%%%Title: %s %s %s\n",
             C_filenamesrcinp, C_filenamegeoinp, C_filenamemapinp);
    fprintf (stream, "%%%%Creator: out (F. Pellegrini, LaBRI, Bordeaux)\n");
    fprintf (stream, "%%%%CreationDate: %s", ctime (&pictime));
  }

  fprintf (stream, "/A  { 0 360 arc fill } bind def\n"); /* Macro definitions */
  if (O_outParam.PosMesh.color == 'c') {          /* If color output          */
    for (i = 0; i < O_POSMESHCOLNBR; i ++) {      /* Build color indexes      */
      O_outColor (i, color);
      fprintf (stream, "/C%c { %g %g %g setrgbcolor } bind def\n",
               ('a' + i), color[0], color[1], color[2]);
    }
  }
  fprintf (stream, "/G  { 255 div setgray } bind def\n");
  fprintf (stream, "/L  { lineto stroke } bind def\n");
  fprintf (stream, "/l  { lineto } bind def\n");
  fprintf (stream, "/m  { moveto } bind def\n");

  fprintf (stream, "gsave\n");                    /* Save the context    */
  fprintf (stream, "1 setlinecap\n");             /* Perform round caps  */
  if (O_outParam.PosMesh.type == 'f')             /* If full page output */
    fprintf (stream, "%d %d translate\n",         /* Center the picture  */
             (int) ((O_PSDPI * (O_PSPAGEWIDTH  * O_POSMESHPICTRESOL - picdelt.x)) /
                    (2 * O_POSMESHPICTRESOL)),
             (int) ((O_PSDPI * (O_PSPAGEHEIGHT * O_POSMESHPICTRESOL - picdelt.y)) /
                    (2 * O_POSMESHPICTRESOL)));
  fprintf (stream, "%f %f scale\n",               /* Print scaling factor */
           (double) O_PSDPI / O_POSMESHPICTRESOL,
           (double) O_PSDPI / O_POSMESHPICTRESOL);
  fprintf (stream, "newpath 0 0 m %d 0 l %d %d l 0 %d l closepath clip\n", /* Clip picture */
           (int) picdelt.x, (int) picdelt.x,
           (int) picdelt.y, (int) picdelt.y);

  fprintf (stream, "0 G\n");                      /* Select black color */
  for (i = 0; i < idxnbr; i ++) {
    fprintf (stream, "%d\t%d\tm\n",               /* Set initial point */
             (int) pictab[idxtab[i]].pos.x,
             (int) pictab[idxtab[i]].pos.y);
    for (i ++; idxtab[i] != ~0; i ++ ) {          /* Build path */
      fprintf (stream, "%d\t%d\t%c\n",
               (int) pictab[idxtab[i]].pos.x,
               (int) pictab[idxtab[i]].pos.y,
               (idxtab[i + 1] == ~0) ? 'L' : 'l');
    }
  }

  if (O_outParam.PosMesh.disk == 'd') {           /* If disks wanted */
    for (i = 0, j = ~0; i < grafptr->vertnbr; i ++) {
      if ((pictab[i].vis > 0) &&                  /* If disk is visible              */
          (mapptr->labltab[i] != (-1))) {         /* And is mapped                   */
        if ((j == ~0) || (pictab[i].col != pictab[j].col)) { /* Update drawing color */
          if (O_outParam.PosMesh.color == 'c')
            fprintf (stream, "C%c\n", 'a' + pictab[i].col);
          else
            fprintf (stream, "%u G\n", pictab[i].col);
          j = i;                                  /* Record the new current color */
        }

        fprintf (stream, "%d %d %d A\n",          /* Draw the disk */
                (int) pictab[i].pos.x,
                (int) pictab[i].pos.y,
                (int) pictab[i].rad);
      }
    }
  }

  fprintf (stream, "grestore\n");                 /* Restore the context */
  if (O_outParam.PosMesh.type == 'f')             /* If full page output */
    fprintf (stream, "showpage\n");               /* Display the page    */

  memFree (pattab);
  memFree (idxtab);
  memFree (pictab);

  return (0);
}
