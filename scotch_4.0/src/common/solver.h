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
/**   NAME       : solver.h                                **/
/**                                                        **/
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the data declarations   **/
/**                for the solver matrix.                  **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 22 jul 1998     **/
/**                                 to     28 oct 1998     **/
/**                # Version 1.0  : from : 06 jun 2002     **/
/**                                 to     06 jun 2002     **/
/**                                                        **/
/************************************************************/

#define SOLVER_H

#ifdef CXREF_DOC
#ifndef COMMON_H
#include "common.h"
#endif /* COMMON_H */
#ifndef GRAPH_H
#include "graph.h"
#endif /* GRAPH_H */
#ifndef DOF_H
#include "dof.h"
#endif /* DOF_H */
#ifndef SYMBOL_H
#include "symbol.h"
#endif /* SYMBOL_H */
#ifndef FTGT_H
#include "ftgt.h"
#endif /* FTGT_H */
#endif /* CXREF_DOC */

#include "solvooc.h"

#define DISTRIB_1D
/* #define DISTRIB_2D */
#ifndef DISTRIB_2D
#define COMPUTE_3T
/* #define COMPUTE_2T */
#endif

/*
**  The type and structure definitions.
*/

#define COMP_1D                     0
#define DIAG                        1
#define E1                          2
#define E2                          3
#define DRUNK                       4

typedef struct Task_ {
  INT                       taskid;               /*+ COMP_1D DIAG E1 E2                                     +*/
  INT                       prionum;              /*+ Priority value                                         +*/
  INT                       cblknum;              /*+ Attached column block                                  +*/
  INT                       bloknum;              /*+ Attached block                                         +*/
  INT                       ctrbcnt;              /*+ Number of fan-in targets                               +*/
  BlockTarget *             btagptr;              /*+ si non local, pointeur sur la structure (NB reception) +*/
  INT                       indnum;               /*+ For E2 (COMP_1D), index of ftgt (>0) else if local = -taskdest  
                                                      For DIAG and E1 , index of btag (>0) if there is a local one it must be
                                                      the first of the chain of local E1                     +*/
  INT                       tasknext;             /*+ chainage des E1 ou E2, si fin = -1 => liberer les btagptr +*/
} Task;

/*+ Solver column block structure. +*/

typedef struct SolverCblk_  {
/*  INT                       ctrbnbr; */             /*+ Number of contributions            +*/
/*  INT                       ctrbcnt; */             /*+ Number of remaining contributions  +*/
/*  INT                       prionum; */             /*+ Column block priority              +*/
  INT                       stride;               /*+ Column block stride                +*/
#ifdef SOLVER_DEBUG
  INT			    color;		  /*+ Color of column block (PICL trace) +*/
#endif /* SOLVER_DEBUG */
} SolverCblk; 

/*+ Solver block structure. +*/

typedef struct SolverBlok_ {
  INT                       coefind;              /*+ Index in coeftab +*/
} SolverBlok;

/*+ Solver matrix structure. +*/

typedef struct SolverMatrix_ {
  SymbolMatrix              symbmtx;              /*+ Symbolic matrix                           +*/
  SolverCblk * restrict     cblktab;              /*+ Array of solver column blocks             +*/
  SolverBlok * restrict     bloktab;              /*+ Array of solver blocks                    +*/
  INT                       coefnbr;              /*+ Number of coefficients                    +*/
  INT                       ftgtnbr;              /*+ Number of fanintargets                    +*/
  FLOAT * restrict          coeftab;              /*+ Coefficients access vector                +*/
  FanInTarget * restrict    ftgttab;              /*+ Fanintarget access vector                 +*/
  INT                       coefmax;              /*+ Working block max size (cblk coeff 1D)    +*/
  INT                       bpftmax;              /*+ Maximum of block size for btag to receive +*/
  INT                       cpftmax;              /*+ Maximum of block size for ftgt to receive +*/
  int                       procnum;              /*+ current processor number                  +*/
  int                       procnbr;              /*+ number of processors                      +*/
  BlockTarget * restrict    btagtab;              /*+ Blocktarget access vector                 +*/
  INT                       btagnbr;              /*+ Number of Blocktargets                    +*/
  BlockCoeff  * restrict    bcoftab;              /*+ BlockCoeff access vector                  +*/
  INT                       bcofnbr;
  INT                       indnbr;
  INT * restrict            indtab;
  Task * restrict           tasktab;              /*+ Task access vector                        +*/
  INT                       tasknbr;              /*+ Number of Tasks                           +*/
  int                       gridldim,             /*+ Dimensions of the virtual processors      +*/
  int                       gridcdim;             /*+ grid if dense end block                   +*/
  Ooc * restrict            oocstr;               /*+ Structure used by out of core             +*/
} SolverMatrix;
