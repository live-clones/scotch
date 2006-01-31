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
/**   NAME       : vmesh_separate_ml.h                     **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the multi-level node separation     **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 20 feb 2003     **/
/**                                 to     31 aug 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct VmeshSeparateMlParam_ {
  INT                       vnodnbr;             /*+ Minimum number of node vertices +*/
  double                    coarrat;             /*+ Coarsening ratio                +*/
  MeshCoarsenType           coartype;            /*+ Element matching function type  +*/
  Strat *                   stratlow;            /*+ Strategy at lowest level        +*/
  Strat *                   stratasc;            /*+ Strategy at ascending levels    +*/
} VmeshSeparateMlParam;

/*
**  The function prototypes.
*/

#ifndef VMESH_SEPARATE_ML
#define static
#endif

static int                  vmeshSeparateMlCoarsen (const Vmesh * restrict const, Vmesh * restrict const, Gnum * restrict * const, const VmeshSeparateMlParam * const);
static int                  vmeshSeparateMlUncoarsen (Vmesh * const, const Vmesh * const, const Gnum * restrict const);

int                         vmeshSeparateMl     (Vmesh * const, const VmeshSeparateMlParam * const);
static int                  vmeshSeparateMl2    (Vmesh * const, const VmeshSeparateMlParam * const);

#undef static
