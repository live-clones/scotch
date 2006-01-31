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
/**   NAME       : ftgt.h                                  **/
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
/**   DATES      : # Version 0.0  : from : 02 may 1998     **/
/**                                 to     16 oct 1998     **/
/**                                                        **/
/************************************************************/

#define FTGT_H

#ifdef CXREF_DOC
#ifndef COMMON_H
#include "common.h"
#endif /* COMMON_H */
#endif /* CXREF_DOC */

/*
**  The type and structure definitions.
*/

/*+ Fanintarget info type +*/

typedef enum {
  FTGT_CTRBNBR = 0,                               /*+ Number of contributions           +*/
  FTGT_CTRBCNT,                                   /*+ Number of contributions remaining +*/
  FTGT_PROCDST,                                   /*+ Destination for fanintarget       +*/
  FTGT_CBLKDST,                                   /*+ Column block destination          +*/
  FTGT_PRIONUM,                                   /*+ Fanintarget priority              +*/
  FTGT_FCOLNUM,                                   /*+ Fanintarget first column          +*/
  FTGT_LCOLNUM,                                   /*+ Fanintarget last column           +*/
  FTGT_BLOKNBR,                                   /*+ Number of blocks in fanintarget   +*/
  MAXINFO
} FanInInfo;

/*+ Fanintarget structure +*/

typedef struct FanInTarget_ {
  INT *                     infotab;              /*+ Fanintarget descriptor (size MAXINFO) +*/
  INT *                     indtab;               /*+ Block index in fanintarget vector     +*/
  INT *                     rowtab;               /*+ Block first row                       +*/
  FLOAT *                   coeftab;              /*+ Fanintarget vector access             +*/
} FanInTarget;
