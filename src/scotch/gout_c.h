/************************************************************/
/**                                                        **/
/**   NAME       : gout_c.h                                **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a result viewer.                **/
/**                This module contains the data declara-  **/
/**                tions for the main module.              **/
/**                                                        **/
/**   DATES      : # Version 2.0  : from : 06 oct 1994     **/
/**                                 to     01 nov 1994     **/
/**                # Version 3.0  : from : 14 jul 1995     **/
/**                                 to     02 oct 1995     **/
/**                # Version 3.2  : from : 02 dec 1996     **/
/**                                 to     05 jun 1998     **/
/**                # Version 3.3  : from : 01 jun 1999     **/
/**                                 to     01 jun 1999     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to     11 dec 2001     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   4             /* Number of files in list                */
#define C_FILEARGNBR                4             /* Number of files which can be arguments */

#define C_filenamesrcinp            C_fileTab[0].name /* Source graph file name          */
#define C_filenamegeoinp            C_fileTab[1].name /* Source graph geometry file name */
#define C_filenamemapinp            C_fileTab[2].name /* Mapping result file name        */
#define C_filenamedatout            C_fileTab[3].name /* Output data file name           */

#define C_filepntrsrcinp            C_fileTab[0].pntr /* Source graph input file    */
#define C_filepntrgeoinp            C_fileTab[1].pntr /* Source graph geometry file */
#define C_filepntrmapinp            C_fileTab[2].pntr /* Mapping result input file  */
#define C_filepntrdatout            C_fileTab[3].pntr /* Data output file           */

/*+ Dimension definitions. +*/

#define x                           c[0]
#define y                           c[1]
#define z                           c[2]

/*+ Geometry flags. +*/

#define C_GEOFLAGDEFAULT            0x0001        /* Default geometry flag            */
#define C_GEOFLAGUSE                0x0001        /* Use geometry                     */
#define C_GEOFLAGROTATE             0x0002        /* Rotate the picture by 90 degrees */
#define C_GEOFLAGPERMUT             0x0004        /* Permute Y and Z dimensions       */

/*
**  The type and structure definitions.
*/

/*+ This structure defines a source graph. +*/

typedef struct C_Graph_ {
  SCOTCH_Graph            grafdat;                /*+ Source graph data  +*/
  SCOTCH_Num              baseval;                /*+ Base value         +*/
  SCOTCH_Num              vertnbr;                /*+ Number of vertices +*/
  SCOTCH_Num *            verttab;                /*+ Vertex array       +*/
  SCOTCH_Num *            vendtab;                /*+ Vertex end array   +*/
  SCOTCH_Num *            vlbltab;                /*+ Vertex label array +*/
  SCOTCH_Num              edgenbr;                /*+ Number of edges    +*/
  SCOTCH_Num *            edgetab;                /*+ Edge array         +*/
} C_Graph;

/*+ This structure defines a geometrical vertex. +*/

typedef struct C_GeoVert_ {
  double                    c[3];                 /*+ Vertex coordinates (x,y,z) +*/
} C_GeoVert;

/*+ This structure defines a geometrical
    mapping which contains the positions
    of the graph vertices.               +*/

typedef struct C_Geometry_ {
  const C_Graph *         grafptr;                /*+ Pointer to source graph      +*/
  C_GeoVert *             verttab;                /*+ Pointer to coordinates array +*/
} C_Geometry;

/*+ This structure defines a domain label
    mapping, which contains the reference
    to the mapping source graph.          +*/

typedef struct C_Mapping_ {
  const C_Graph *         grafptr;                /*+ Pointer to source graph +*/
  SCOTCH_Num *            labltab;                /*+ Pointer to label array  +*/
} C_Mapping;

/*+ The sort structure, used to sort graph vertices by label. +*/

typedef struct C_VertSort_ {
  SCOTCH_Num                labl;                 /*+ Vertex label  +*/
  SCOTCH_Num                num;                  /*+ Vertex number +*/
} C_VertSort;

/*+ This structure is the code
    name array entries.        +*/

typedef struct C_ParseCode_ {
  uint                      code;                 /*+ Code value +*/
  char *                    name;                 /*+ Code name  +*/
} C_ParseCode;

/* This structure defines the
   code argument array entries. */

typedef struct C_ParseArg_ {
  const char *              name;                 /*+ Name of the argument                         +*/
  uint                      code;                 /*+ Code value                                   +*/
  const char *              format;               /*+ scanf-like format; NULL means char, no value +*/
  const void *              ptr;                  /*+ Pointer to the argument location             +*/
  int                    (* func) ();             /*+ Pointer to the argument test function        +*/
} C_ParseArg;

/*
**  The global data declarations.
*/

extern File                 C_fileTab[C_FILENBR]; /*+ File array +*/

/*
**  The function prototypes.
*/

int                         C_geoParse          (const char * const);
void                        C_geoInit           (C_Geometry * const, const C_Graph * const);
void                        C_geoExit           (C_Geometry * const);
int                         C_geoLoad           (C_Geometry * const, FILE * const);

void                        C_mapInit           (C_Mapping * const, const C_Graph * const);
void                        C_mapExit           (C_Mapping * const);
int                         C_mapLoad           (C_Mapping * const, FILE * const);

int                         C_parse             (const C_ParseCode * const, const C_ParseArg * const, uint * const, char * const);
