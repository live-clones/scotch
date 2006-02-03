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
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the data declarations   **/
/**                for the graph structure.                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 27 jul 1998     **/
/**                                 to     24 jan 2004     **/
/**                # Version 2.0  : from : 28 feb 2004     **/
/**                                 to     23 apr 2004     **/
/**                                                        **/
/************************************************************/

#define GRAPH_H

/*
**  The defines.
*/

#define Graph                       SCOTCH_Graph

#define graphInit                   SCOTCH_graphInit
#define graphExit                   SCOTCH_graphExit
#define graphLoad                   SCOTCH_graphLoad
#define graphSave                   SCOTCH_graphSave
#define graphBase                   SCOTCH_graphBase
#define graphData                   SCOTCH_graphData
#define graphCheck                  SCOTCH_graphCheck

/*
**  The function prototypes.
*/

#ifndef GRAPH
#define static
#endif

int                         graphBuild          (Graph * const grafptr, const INT baseval, const INT vertnbr, const INT edgenbr, void * const ngbdptr, INT nghbfrstfunc (void * const, const INT), INT nghbnextfunc (void * const));
int                         graphBuildGraph     (Graph * const grafptr, const INT baseval, const INT vertnbr, const INT edgenbr, INT * restrict verttab, INT * restrict velotab, INT * restrict edgetab);
int                         graphBuildGraph2    (Graph * const grafptr, const INT baseval, const INT vertnbr, const INT edgenbr, INT * restrict verttab, INT * restrict vendtab, INT * restrict velotab, INT * restrict vlbltab, INT * restrict edgetab, INT * restrict edlotab);

#undef static
