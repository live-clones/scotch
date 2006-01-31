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
/**   NAME       : amk_ccc.h                               **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Creates the distance map for CCC        **/
/**                graphs, to be used to build the         **/
/**                architecture description files for      **/
/**                these graphs.                           **/
/**                                                        **/
/**   DATES      : # Version 1.3  : from : 24 apr 1994     **/
/**                                 to   : 24 apr 1994     **/
/**                # Version 2.0  : from : 13 jul 1994     **/
/**                                 to   : 18 jul 1994     **/
/**                # Version 3.0  : from : 18 sep 1995     **/
/**                                 to   : 18 sep 1995     **/
/**                # Version 3.2  : from : 07 may 1997     **/
/**                                 to   : 07 may 1997     **/
/**                # Version 3.3  : from : 02 oct 1998     **/
/**                                 to   : 02 oct 1998     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ File name aliases. +*/

#define C_FILENBR                   1             /* Number of files in list */

#define C_filenamearcout            C_fileTab[0].name /* Architecture output file name */

#define C_filepntrarcout            C_fileTab[0].pntr /* Architecture output file */

/*
**  The type and structure definitions.
*/

/*+ This structure defines a CCC vertex. +*/

typedef struct C_Vertex_ {
  SCOTCH_Num                lvl;                  /*+ Vertex level    +*/
  SCOTCH_Num                pos;                  /*+ Vertex position +*/
} C_Vertex;

/*+ This structure defines a vertex distance information. +*/

typedef struct C_VertDist_ {
  int                       queued;               /*+ Flag set if vertex queued  +*/
  SCOTCH_Num                dist;                 /*+ Distance to initial vertex +*/
} C_VertDist;

/*+ This is a neighbor queue element. +*/

typedef struct C_QueueElem_ {
  C_Vertex                  vert;                 /*+ Vertex number    +*/
  SCOTCH_Num                dist;                 /*+ Distance reached +*/
} C_QueueElem;

/*+ This is the distance queue. +*/

typedef struct C_Queue_ {
  C_QueueElem *             tab;                  /*+ Pointer to queue data    +*/
  SCOTCH_Num                min;                  /*+ Pointer to first element +*/
  SCOTCH_Num                max;                  /*+ Pointer to last element  +*/
} C_Queue;

/*
**  The macro definitions.
*/

#define C_vertLabl(v)               (((v)->lvl << ccdim) | (v)->pos)

#define C_queueInit(q,n)            ((((q)->tab = (C_QueueElem *) memAlloc ((n) * sizeof (C_QueueElem))) == NULL) ? 1 : 0)
#define C_queueExit(q)              memFree ((q)->tab)
#define C_queueFlush(q)             (q)->min = \
                                    (q)->max = 0
#define C_queuePut(q,v,d)           ((q)->tab[(q)->max].vert    = *(v),  \
                                     (q)->tab[(q)->max ++].dist = (d))
#define C_queueGet(q,v,d)           (((q)->min < (q)->max) ? (*(v) = (q)->tab[(q)->min].vert,    \
                                                              *(d) = (q)->tab[(q)->min ++].dist, \
                                                              1)                                 \
                                                           : 0)

#define C_distaRoot(v)              (C_queueFlush (&C_distaQueue),         \
                                     C_queuePut   (&C_distaQueue, (v), 0), \
                                     C_distaTab[C_vertLabl (v)].queued = 1)
#define C_distaGet(v,d)             (C_queueGet (&C_distaQueue, (v), (d)))
#define C_distaPut(v,d)             ((C_distaTab[C_vertLabl (v)].queued == 0) \
                                     ? C_queuePut (&C_distaQueue, (v), d),    \
                                       C_distaTab[C_vertLabl (v)].queued = 1  \
                                     : 0)
