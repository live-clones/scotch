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
/**   NAME       : library.h                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Declaration file for the LibScotch      **/
/**                static mapping and sparse matrix block  **/
/**                ordering sequential library.            **/
/**                                                        **/
/**   DATES      : # Version 3.2  : from : 07 sep 1996     **/
/**                                 to     22 aug 1998     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     31 may 1999     **/
/**                # Version 3.4  : from : 10 oct 1999     **/
/**                                 to     15 nov 2001     **/
/**                # Version 4.0  : from : 11 dec 2001     **/
/**                                 to     20 dec 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Integer type. +*/

typedef DUMMYINT SCOTCH_Num;

#define SCOTCH_NUMMAX               DUMMYMAXINT

/*+ Opaque objects. The dummy sizes of these objects,
computed at compile-time by program "dummysizes",
are given as double values for proper padding +*/

typedef struct {
  double                    dummy[DUMMYSIZESTRAT];
} SCOTCH_Strat;

typedef struct {
  double                    dummy[DUMMYSIZEARCH];
} SCOTCH_Arch;

typedef struct {
  double                    dummy[DUMMYSIZEGEOM];
} SCOTCH_Geom;

typedef struct {
  double                    dummy[DUMMYSIZEGRAPH];
} SCOTCH_Graph;

typedef struct {
  double                    dummy[DUMMYSIZEMESH];
} SCOTCH_Mesh;

typedef struct {
  double                    dummy[DUMMYSIZEMAP];
} SCOTCH_Mapping;

typedef struct {
  double                    dummy[DUMMYSIZEORDER];
} SCOTCH_Ordering;

/*
**  The function prototypes.
*/

#ifdef X_ARCHalpha_compaq_osf1
#ifndef USE_CXX
extern "C" {
#endif /* USE_CXX */
#endif /* X_ARCHalpha_compaq_osf1 */

void                        SCOTCH_errorProg    (const char * const);
void                        SCOTCH_errorPrint   (const char * const, ...);
void                        SCOTCH_errorPrintW  (const char * const, ...);

int                         SCOTCH_stratInit    (SCOTCH_Strat * const);
void                        SCOTCH_stratExit    (SCOTCH_Strat * const);
int                         SCOTCH_stratSave    (const SCOTCH_Strat * const, FILE * const);
int                         SCOTCH_stratGraphBipart (SCOTCH_Strat * const, const char * const);
int                         SCOTCH_stratGraphMap (SCOTCH_Strat * const, const char * const);
int                         SCOTCH_stratGraphOrder (SCOTCH_Strat * const, const char * const);
int                         SCOTCH_stratMeshOrder (SCOTCH_Strat * const, const char * const);

int                         SCOTCH_archInit     (SCOTCH_Arch * const);
void                        SCOTCH_archExit     (SCOTCH_Arch * const);
int                         SCOTCH_archLoad     (SCOTCH_Arch * const, FILE * const);
int                         SCOTCH_archSave     (const SCOTCH_Arch * const, FILE * const);
int                         SCOTCH_archBuild    (SCOTCH_Arch * const, const SCOTCH_Graph * const, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Strat * const);
int                         SCOTCH_archCmplt    (SCOTCH_Arch * const, const SCOTCH_Num);
char *                      SCOTCH_archName     (const SCOTCH_Arch * const);
SCOTCH_Num                  SCOTCH_archSize     (const SCOTCH_Arch * const);

int                         SCOTCH_geomInit     (SCOTCH_Geom * const);
void                        SCOTCH_geomExit     (SCOTCH_Geom * const);
void                        SCOTCH_geomData     (const SCOTCH_Geom * const, SCOTCH_Num * const, double ** const);

int                         SCOTCH_graphInit    (SCOTCH_Graph * const);
void                        SCOTCH_graphExit    (SCOTCH_Graph * const);
int                         SCOTCH_graphLoad    (SCOTCH_Graph * const, FILE * const, const SCOTCH_Num, const SCOTCH_Num);
int                         SCOTCH_graphSave    (const SCOTCH_Graph * const, FILE * const);
int                         SCOTCH_graphBuild   (SCOTCH_Graph * const, const SCOTCH_Num, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Num * const, const SCOTCH_Num * const, const SCOTCH_Num * const, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Num * const);
SCOTCH_Num                  SCOTCH_graphBase    (SCOTCH_Graph * const, const SCOTCH_Num baseval);
int                         SCOTCH_graphCheck   (const SCOTCH_Graph * const);
void                        SCOTCH_graphSize    (const SCOTCH_Graph * const, SCOTCH_Num * const, SCOTCH_Num * const);
void                        SCOTCH_graphData    (const SCOTCH_Graph * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num ** const, SCOTCH_Num ** const, SCOTCH_Num ** const, SCOTCH_Num ** const, SCOTCH_Num * const, SCOTCH_Num ** const, SCOTCH_Num ** const);
void                        SCOTCH_graphStat    (const SCOTCH_Graph * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, double * const, double * const, SCOTCH_Num * const, SCOTCH_Num * const, double * const, double * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, double * const, double * const);
int                         SCOTCH_graphGeomLoadChac (SCOTCH_Graph * const, SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);
int                         SCOTCH_graphGeomSaveChac (const SCOTCH_Graph * const, const SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);
int                         SCOTCH_graphGeomLoadHabo (SCOTCH_Graph * const, SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);
int                         SCOTCH_graphGeomLoadScot (SCOTCH_Graph * const, SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);
int                         SCOTCH_graphGeomSaveScot (const SCOTCH_Graph * const, const SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);

int                         SCOTCH_graphMapInit (const SCOTCH_Graph * const, SCOTCH_Mapping * const, const SCOTCH_Arch * const, SCOTCH_Num * const);
void                        SCOTCH_graphMapExit (const SCOTCH_Graph * const, SCOTCH_Mapping * const);
int                         SCOTCH_graphMapLoad (const SCOTCH_Graph * const, const SCOTCH_Mapping * const, FILE * const);
int                         SCOTCH_graphMapSave (const SCOTCH_Graph * const, const SCOTCH_Mapping * const, FILE * const);
int                         SCOTCH_graphMapView (const SCOTCH_Graph * const, const SCOTCH_Mapping * const, FILE * const);
int                         SCOTCH_graphMapCompute (const SCOTCH_Graph * const, SCOTCH_Mapping * const, const SCOTCH_Strat * const);
int                         SCOTCH_graphMap     (const SCOTCH_Graph * const, const SCOTCH_Arch * const, const SCOTCH_Strat * const, SCOTCH_Num * const);
int                         SCOTCH_graphPart    (const SCOTCH_Graph * const, const SCOTCH_Num, const SCOTCH_Strat * const, SCOTCH_Num * const);

int                         SCOTCH_graphOrderInit (const SCOTCH_Graph * const, SCOTCH_Ordering * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const);
void                        SCOTCH_graphOrderExit (const SCOTCH_Graph * const, SCOTCH_Ordering * const);
int                         SCOTCH_graphOrderLoad (const SCOTCH_Graph * const, SCOTCH_Ordering * const, FILE * const);
int                         SCOTCH_graphOrderSave (const SCOTCH_Graph * const, const SCOTCH_Ordering * const, FILE * const);
int                         SCOTCH_graphOrderSaveMap (const SCOTCH_Graph * const, const SCOTCH_Ordering * const, FILE * const);
int                         SCOTCH_graphOrderSaveTree (const SCOTCH_Graph * const, const SCOTCH_Ordering * const, FILE * const);
int                         SCOTCH_graphOrderCompute (const SCOTCH_Graph * const, SCOTCH_Ordering * const, const SCOTCH_Strat * const);
int                         SCOTCH_graphOrderComputeList (const SCOTCH_Graph * const, SCOTCH_Ordering * const, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Strat * const);
int                         SCOTCH_graphOrderFactor (const SCOTCH_Graph * const, const SCOTCH_Ordering * const, SCOTCH_Graph * const);
int                         SCOTCH_graphOrderView (const SCOTCH_Graph * const, const SCOTCH_Ordering * const, FILE * const);
int                         SCOTCH_graphOrder   (const SCOTCH_Graph * const, const SCOTCH_Strat * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const);
int                         SCOTCH_graphOrderList (const SCOTCH_Graph * const, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Strat * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const);
int                         SCOTCH_graphOrderCheck (const SCOTCH_Graph * const, const SCOTCH_Ordering * const);

int                         SCOTCH_meshInit     (SCOTCH_Mesh * const);
void                        SCOTCH_meshExit     (SCOTCH_Mesh * const);
int                         SCOTCH_meshLoad     (SCOTCH_Mesh * const, FILE * const, const SCOTCH_Num);
int                         SCOTCH_meshSave     (const SCOTCH_Mesh * const, FILE * const);
int                         SCOTCH_meshBuild    (SCOTCH_Mesh * const, const SCOTCH_Num, const SCOTCH_Num, const SCOTCH_Num, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Num * const, const SCOTCH_Num * const, const SCOTCH_Num * const, const SCOTCH_Num * const, const SCOTCH_Num, const SCOTCH_Num * const);
int                         SCOTCH_meshCheck    (const SCOTCH_Mesh * const);
void                        SCOTCH_meshSize     (const SCOTCH_Mesh * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const);
void                        SCOTCH_meshData     (const SCOTCH_Mesh * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num ** const, SCOTCH_Num ** const, SCOTCH_Num ** const, SCOTCH_Num ** const, SCOTCH_Num ** const, SCOTCH_Num * const, SCOTCH_Num ** const, SCOTCH_Num * const);
void                        SCOTCH_meshStat     (const SCOTCH_Mesh * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, double * const, double * const, SCOTCH_Num * const, SCOTCH_Num * const, double * const, double * const, SCOTCH_Num * const, SCOTCH_Num * const, double * const, double * const);
int                         SCOTCH_meshGraph    (const SCOTCH_Mesh * restrict const, SCOTCH_Graph * restrict const);
int                         SCOTCH_meshGeomLoadHabo (SCOTCH_Mesh * const, SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);
int                         SCOTCH_meshGeomLoadScot (SCOTCH_Mesh * const, SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);
int                         SCOTCH_meshGeomSaveScot (const SCOTCH_Mesh * const, const SCOTCH_Geom * const, FILE * const, FILE * const, const char * const);

int                         SCOTCH_meshOrderInit (const SCOTCH_Mesh * const, SCOTCH_Ordering * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const);
void                        SCOTCH_meshOrderExit (const SCOTCH_Mesh * const, SCOTCH_Ordering * const);
int                         SCOTCH_meshOrderSave (const SCOTCH_Mesh * const, const SCOTCH_Ordering * const, FILE * const);
int                         SCOTCH_meshOrderSaveMap (const SCOTCH_Mesh * const, const SCOTCH_Ordering * const, FILE * const);
int                         SCOTCH_meshOrderCompute (const SCOTCH_Mesh * const, SCOTCH_Ordering * const, const SCOTCH_Strat * const);
int                         SCOTCH_meshOrderComputeList (const SCOTCH_Mesh * const, SCOTCH_Ordering * const, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Strat * const);
int                         SCOTCH_meshOrder    (const SCOTCH_Mesh * const, const SCOTCH_Strat * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const);
int                         SCOTCH_meshOrderList (const SCOTCH_Mesh * const, const SCOTCH_Num, const SCOTCH_Num * const, const SCOTCH_Strat * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const, SCOTCH_Num * const);
int                         SCOTCH_meshOrderCheck (const SCOTCH_Mesh * const, const SCOTCH_Ordering * const);

void                        SCOTCH_randomReset  (void);

#ifdef X_ARCHalpha_compaq_osf1
#ifndef USE_CXX
}
#endif /* USE_CXX */
#endif /* X_ARCHalpha_compaq_osf1 */
