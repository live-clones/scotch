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
/**   NAME       : vmesh_separate_st.h                     **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for the global mesh separation    **/
/**                strategy and method tables.             **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 20 sep 2002     **/
/**                                 to     31 oct 2003     **/
/**                                                        **/
/************************************************************/

/*
**  The type definitions.
*/

/*+ Method types. +*/

typedef enum VmeshSeparateStMethodType_ {
  VMESHSEPASTMETHFM = 0,                          /*+ Fiduccia-Mattheyses    +*/
  VMESHSEPASTMETHGG,                              /*+ Greedy Mesh Growing    +*/
  VMESHSEPASTMETHGR,                              /*+ Graph partitioning     +*/
  VMESHSEPASTMETHML,                              /*+ Multi-level separation +*/
  VMESHSEPASTMETHZR,                              /*+ Zero method            +*/
  VMESHSEPASTMETHNBR                              /*+ Number of methods      +*/
} VmeshSeparateStMethodType;

/*
**  The external declarations.
*/

extern StratTab             vmeshseparateststratab;

/*
**  The function prototypes.
*/

#ifndef VMESH_SEPARATE_ST
#define static
#endif

int                         vmeshSeparateSt     (Vmesh * restrict const, const Strat * restrict const);

#undef static
