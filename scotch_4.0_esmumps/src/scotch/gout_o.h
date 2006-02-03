/************************************************************/
/**                                                        **/
/**   NAME       : gout_o.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a result viewer.                **/
/**                This module contains the data declara-  **/
/**                tions for the output module.            **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 08 oct 1994     **/
/**                                 to     02 nov 1994     **/
/**                # Version 3.0  : from : 14 jul 1995     **/
/**                                 to     03 oct 1995     **/
/**                # Version 3.1  : from : 05 apr 1996     **/
/**                                 to     05 apr 1996     **/
/**                # Version 3.2  : from : 03 dec 1996     **/
/**                                 to     05 jun 1998     **/
/**                # Version 3.3  : from : 02 jun 1999     **/
/**                                 to     02 jun 1999     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ Generic PostScript output definitions. +*/

#define O_PSDPI                     72            /* PostScript dots-per-inch              */
#define O_PSPAGEHEIGHT              11.6          /* PostScript page height (in inches)    */
#define O_PSPAGEWIDTH               8.2           /* PostScript page witdh (in inches)     */
#define O_PSPICTHEIGHT              10.0          /* PostScript picture height (in inches) */
#define O_PSPICTWIDTH               6.6           /* PostScript picture witdh (in inches)  */

/*+ PostScript Mesh output definitions. +*/

#define O_POSMESHPICTRESOL          10000.0       /* Picture resolution */

#define O_POSMESHISOCOS             0.866025404   /* cos(30 degrees)              */
#define O_POSMESHISOSIN             0.5           /* sin(30 degrees)              */
#define O_POSMESHISOREDUC           0.20          /* Z-axis reduction coefficient */

#define O_POSMESHCOLNBR             16            /* Number of colors */

/*
**  The type and structure definitions.
*/

/*+ The 2D point type. +*/

typedef struct O_Point_ {
  double                    c[2];                 /*+ Page coordinates +*/
} O_Point;

/*+ The output type type. +*/

typedef enum O_OutType_ {
  O_OUTTYPEINVMESH,                               /*+ Mesh SGI Open Inventor (3D) +*/
  O_OUTTYPEPOSMATR,                               /*+ Matrix PostScript (2D)      +*/
  O_OUTTYPEPOSMESH,                               /*+ Mesh PostScript (2D)        +*/
  O_OUTTYPENBR                                    /*+ Number of output types      +*/
} O_OutType;

/*+ The output parameter data structure. +*/

typedef struct O_OutParam_ {
  O_OutType                 type;                 /*+ Output type              +*/
  struct {                                        /*+ Inventor mesh structure  +*/
    char                    edge;                 /*+ 'r' : remove; 'v' : view +*/
  } InvMesh;
  struct {                                        /*+ PostScript matrix structure +*/
    char                    type;                 /*+ 'f' : page; 'e' : EPSF      +*/
  } PosMatr;
  struct {                                        /*+ PostScript mesh structure +*/
    char                    type;                 /*+ 'f' : page; 'e' : EPSF    +*/
    char                    color;                /*+ 'c' : color; 'g' : gray   +*/
    char                    edge;                 /*+ 'r' : remove; 'v' : view  +*/
    char                    disk;                 /*+ 'd' : draw; 'a' : avoid   +*/
    char                    clip;                 /*+ 'l' : large; 's' : short  +*/
    O_Point                 min;                  /*+ Clipping ratios           +*/
    O_Point                 max;
  } PosMesh;
} O_OutParam;

/*+ The Inventor path array element. +*/

typedef struct O_InvMeshPath_ {
  SCOTCH_Num                nbr;                  /*+ Number of output paths     +*/
  SCOTCH_Num                idx;                  /*+ Index from which to search +*/
} O_InvMeshPath;

/*+ The PostScript path array element. +*/

typedef struct O_PosMeshPath_ {
  SCOTCH_Num                nbr;                  /*+ Number of output paths     +*/
  SCOTCH_Num                idx;                  /*+ Index from which to search +*/
} O_PosMeshPath;

/*+ The PostScript mesh graph vertex. +*/

typedef struct O_PosMeshVertex_ {
  int                       vis;                  /*+ Visibility flag  +*/
  O_Point                   pos;                  /*+ Point position   +*/
  double                    rad;                  /*+ Disk radius      +*/
  int                       col;                  /*+ Disk color index +*/
} O_PosMeshVertex;

/*
**  The function prototypes.
*/

void                        O_outColor          (const SCOTCH_Num, double[]);
int                         O_outParse          (char * const);
void                        O_out               (const C_Graph * const, const C_Geometry * const, const C_Mapping * const, FILE * const);

int                         O_outInvMesh        (const C_Graph * const, const C_Geometry * const, const C_Mapping * const, FILE * const);
int                         O_outPosMatr        (const C_Graph * const, const C_Geometry * const, const C_Mapping * const, FILE * const);
int                         O_outPosMesh        (const C_Graph * const, const C_Geometry * const, const C_Mapping * const, FILE * const);
