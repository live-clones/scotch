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
/**   NAME       : vmesh.h                                 **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data declara-  **/
/**                tions for mesh vertex separation        **/
/**                routines.                               **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 10 sep 2002     **/
/**                                 to   : 10 sep 2002     **/
/**                                                        **/
/************************************************************/

/*
**  The type and structure definitions.
*/

/*+ Active graph structure. +*/

typedef struct Vmesh_ {
  Mesh                      m;                    /*+ Source mesh                                        +*/
  GraphPart * restrict      parttax;              /*+ Based part array: 0,1: part; 2: separator          +*/
  Gnum                      ecmpsize[2];          /*+ Number of elements in each part (not in separator) +*/
  Gnum                      ncmpload[3];          /*+ Loads of nodes in both parts and separator         +*/
  Gnum                      ncmploaddlt;          /*+ Node load difference between both parts            +*/
  Gnum                      ncmpsize[2];          /*+ Number of nodes in parts (separator is fronnbr)    +*/
  Gnum                      fronnbr;              /*+ Number of frontier nodes; TRICK: ncmpsize[2]       +*/
  Gnum * restrict           frontab;              /*+ Array of frontier node numbers                     +*/
  Gnum                      levlnum;              /*+ Nested dissection or coarsening level              +*/
} Vmesh;

/*+ The graph separator storing structure. +*/

typedef struct VmeshStore_ {
  Gnum                      ecmpsize[2];          /*+ Number of elements in each part                 +*/
  Gnum                      ncmpload[3];          /*+ Loads of nodes in both parts and separator      +*/
  Gnum                      ncmploaddlt;          /*+ Node load difference between both parts         +*/
  Gnum                      ncmpsize[2];          /*+ Number of nodes in parts (separator is fronnbr) +*/
  Gnum                      fronnbr;              /*+ Number of frontier nodes; TRICK: ncmpsize[2]    +*/
  byte *                    datatab;              /*+ Variable-sized data array                       +*/
} VmeshStore;

/*
**  The function prototypes.
*/

#ifndef VMESH
#define static
#endif

void                        vmeshExit           (Vmesh * const);
void                        vmeshZero           (Vmesh * const);
int                         vmeshCheck          (const Vmesh * const);

int                         vmeshStoreInit      (const Vmesh * const, VmeshStore * const);
void                        vmeshStoreExit      (VmeshStore * const);
void                        vmeshStoreSave      (const Vmesh * const , VmeshStore * const);
void                        vmeshStoreUpdt      (Vmesh * const, const VmeshStore * const);

#undef static
