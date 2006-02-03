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
/**   NAME       : vmesh_separate_gg.h                     **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the the greedy mesh growing node    **/
/**                separation method.                      **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 16 sep 2002     **/
/**                                 to     07 apr 2004     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/*+ System-defined constants. +*/

#define VMESHSEPAGGSUBBITS          4

#define VMESHSEPAGGSTATEPART0       ((GainLink *) 0) /*+ Element vertex in part 0 (initial state)  +*/
#define VMESHSEPAGGSTATEPART1       ((GainLink *) 1) /*+ Element vertex in part 1                  +*/
#define VMESHSEPAGGSTATEPART2       ((GainLink *) 2) /*+ Element vertex in part 2, chained         +*/
#define VMESHSEPAGGSTATELINK        ((GainLink *) 3) /*+ Currently in gain table if higher         +*/

/*
**  The type and structure definitions.
*/

/*+ Method parameters. +*/

typedef struct VmeshSeparateGgParam_ {
  INT                       passnbr;              /*+ Number of passes to perform +*/
} VmeshSeparateGgParam;

/*+ The complementary element vertex structure.
    For trick reasons, the gain table data structure
    must be the first field of the structure.        +*/

typedef struct VmeshSeparateGgElem_ {
  GainLink                  gainlink;             /*+ Gain link: FIRST                               +*/
  Gnum                      ncmpgain2;            /*+ Computation gain in separator: (0->2) - (2->1) +*/
  Gnum                      ncmpgaindlt;          /*+ Overall computation delta: - (0->2) - (2->1)   +*/
} VmeshSeparateGgElem;

/*+ The complementary vertex structure. Only
    partval is always valid. Other fields are
    valid only when vertex belongs to separator. +*/

typedef struct VmeshSeparateGgNode_ {
  int                       partval;              /*+ Part to which node vertex belongs                                   +*/
  Gnum                      commsize0;            /*+ Number of neighbors in part 0                                       +*/
  Gnum                      velmisum0;            /*+ Sum of all element indices in part 0; the last one is the right one +*/
} VmeshSeparateGgNode;

/*
**  The function prototypes.
*/

#ifndef VMESH_SEPARATE_GG
#define static
#endif

int                         vmeshSeparateGg     (Vmesh * restrict const, const VmeshSeparateGgParam * restrict const);

#ifdef SCOTCH_DEBUG_VMESH3
static int                  vmeshSeparateGgCheck (Vmesh * restrict const, const Gnum, const Gnum, const VmeshSeparateGgElem * restrict const, const VmeshSeparateGgNode * restrict const  vnoxtax);
#endif /* SCOTCH_DEBUG_VMESH3 */

#undef static
