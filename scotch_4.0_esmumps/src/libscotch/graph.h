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
/**   NAME       : graph.h                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the source graph functions.         **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 02 dec 1992     **/
/**                                 to     18 may 1993     **/
/**                # Version 1.3  : from : 30 apr 1994     **/
/**                                 to     18 may 1994     **/
/**                # Version 2.0  : from : 06 jun 1994     **/
/**                                 to     18 aug 1994     **/
/**                # Version 3.0  : from : 07 jul 1995     **/
/**                                 to     28 sep 1995     **/
/**                # Version 3.1  : from : 28 nov 1995     **/
/**                                 to     28 nov 1995     **/
/**                # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     15 sep 1998     **/
/**                # Version 3.3  : from : 28 sep 1998     **/
/**                                 to     23 mar 1999     **/
/**                # Version 3.4  : from : 20 mar 2000     **/
/**                                 to     20 mar 2000     **/
/**                # Version 4.0  : from : 24 nov 2001     **/
/**                                 to     31 aug 2004     **/
/**                                                        **/
/************************************************************/

#define GRAPH_H

/*
**  The defines.
*/

/*+ Graph option flags. +*/

#define GRAPHNONE                   0x0000        /* No options set */

#define GRAPHFREEEDGE               0x0001        /* Free edgetab array        */
#define GRAPHFREEVERT               0x0002        /* Free verttab array        */
#define GRAPHFREEVNUM               0x0004        /* Free vnumtab array        */
#define GRAPHFREEOTHR               0x0008        /* Free all other arrays     */
#define GRAPHFREETABS               0x000F        /* Free all graph arrays     */
#define GRAPHVERTGROUP              0x0010        /* All vertex arrays grouped */
#define GRAPHEDGEGROUP              0x0020        /* All edge arrays grouped   */

#define GRAPHBITSUSED               0x003F        /* Significant bits for plain graph routines               */
#define GRAPHBITSNOTUSED            0x0040        /* Value above which bits not used by plain graph routines */

#define GRAPHIONOLOADVERT           1             /* Remove vertex loads on loading */
#define GRAPHIONOLOADEDGE           2             /* Remove edge loads on loading   */

/*
**  The type and structure definitions.
*/

#ifndef GNUMMAX                                   /* If dgraph.h not included */
typedef INT                   Gnum;               /* Vertex and edge numbers  */

#define GNUMMAX                     (INT_MAX)     /* Maximum signed Gnum value */
#endif /* GNUMMAX */

/*+ The vertex part type, in compressed form. +*/

typedef byte GraphPart;

/*+ The vertex list structure. Since a vertex list
    always refers to a given graph, vertex indices
    contained in the vertex list array are based with
    respect to the base value of the associated graph.
    However, the array itself is not based.            +*/

typedef struct VertList_ {
  Gnum                      vnumnbr;              /*+ Number of vertices in list +*/
  Gnum * restrict           vnumtab;              /*+ Pointer to vertex array    +*/
} VertList;

/*+ The graph flag type. +*/

typedef int GraphFlag;                            /*+ Graph property flags +*/

/*+ The graph structure. +*/

typedef struct Graph_ {
  GraphFlag                 flagval;              /*+ Graph properties                          +*/
  Gnum                      baseval;              /*+ Base index for edge/vertex arrays         +*/
  Gnum                      vertnbr;              /*+ Nmber of vertices in graph                +*/
  Gnum                      vertnnd;              /*+ Number of vertices in graph, plus baseval +*/
  Gnum * restrict           verttax;              /*+ Vertex array [based]                      +*/
  Gnum * restrict           vendtax;              /*+ End vertex array [based]                  +*/
  Gnum * restrict           velotax;              /*+ Vertex load array (if present)            +*/
  Gnum                      velosum;              /*+ Overall graph vertex load                 +*/
  Gnum * restrict           vnumtax;              /*+ Vertex number in ancestor graph           +*/
  Gnum * restrict           vlbltax;              /*+ Vertex label (from file)                  +*/
  Gnum                      edgenbr;              /*+ Number of edges (arcs) in graph           +*/
  Gnum * restrict           edgetax;              /*+ Edge array [based]                        +*/
  Gnum * restrict           edlotax;              /*+ Edge load array (if present)              +*/
  Gnum                      degrmax;              /*+ Maximum degree                            +*/
} Graph;

/*
**  The function prototypes.
*/

#ifndef GRAPH
#define static
#endif

int                         listInit            (VertList *);
void                        listExit            (VertList *);
int                         listAlloc           (VertList *, Gnum);
int                         listFree            (VertList *);
int                         listLoad            (VertList *, FILE *);
int                         listSave            (VertList *, FILE *);
void                        listSort            (VertList *);
int                         listCopy            (VertList *, VertList *);

int                         graphInit           (Graph * const);
void                        graphExit           (Graph * const);
void                        graphFree           (Graph * const);
int                         graphLoad           (Graph * const, FILE * const, const Gnum, const GraphFlag);
int                         graphLoad2          (const Gnum, const Gnum, const Gnum * const, const Gnum * const, Gnum * restrict const, const Gnum, const Gnum * const);
int                         graphSave           (const Graph * const, FILE * const);
Gnum                        graphBase           (Graph * const, const Gnum);
int                         graphInduceList     (const Graph * const, const VertList * const, Graph * const);
int                         graphInducePart     (const Graph * const, const GraphPart *, const Gnum, const GraphPart, Graph * const);
int                         graphCheck          (const Graph *);

#ifdef GEOM_H
int                         graphGeomLoadChac   (Graph * restrict const, Geom * restrict const, FILE * const, FILE * const, const char * const);
int                         graphGeomSaveChac   (const Graph * restrict const, const Geom * restrict const, FILE * const, FILE * const, const char * const);
int                         graphGeomLoadHabo   (Graph * restrict const, Geom * restrict const, FILE * const, FILE * const, const char * const);
int                         graphGeomLoadScot   (Graph * restrict const, Geom * restrict const, FILE * const, FILE * const, const char * const);
int                         graphGeomSaveScot   (const Graph * restrict const, const Geom * restrict const, FILE * const, FILE * const, const char * const);

#endif /* GEOM_H */

#undef static
