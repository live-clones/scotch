/* Copyright 2004,2007,2018,2024 IPB, Universite de Bordeaux, INRIA & CNRS
**
** This file is part of the Scotch software package for static mapping,
** graph partitioning and sparse matrix ordering.
**
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software. You can
** use, modify and/or redistribute the software under the terms of the
** CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
** URL: "http://www.cecill.info".
**
** As a counterpart to the access to the source code and rights to copy,
** modify and redistribute granted by the license, users are provided
** only with a limited warranty and the software's author, the holder of
** the economic rights, and the successive licensors have only limited
** liability.
**
** In this respect, the user's attention is drawn to the risks associated
** with loading, using, modifying and/or developing or reproducing the
** software by the user in light of its specific status of free software,
** that may mean that it is complicated to manipulate, and that also
** therefore means that it is reserved for developers and experienced
** professionals having in-depth computer knowledge. Users are therefore
** encouraged to load and test the software's suitability as regards
** their requirements in conditions enabling the security of their
** systems and/or data to be ensured and, more generally, to use and
** operate it in the same conditions as regards security.
**
** The fact that you are presently reading this means that you have had
** knowledge of the CeCILL-C license and that you accept its terms.
*/
/************************************************************/
/**                                                        **/
/**   NAME       : hmesh_order_st.h                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains the data           **/
/**                declarations for the main halo mesh     **/
/**                ordering routine.                       **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 28 sep 2002     **/
/**                                 to   : 08 feb 2004     **/
/**                # Version 6.0  : from : 07 jun 2018     **/
/**                                 to   : 07 jun 2018     **/
/**                # Version 7.0  : from : 11 sep 2024     **/
/**                                 to   : 07 nov 2024     **/
/**                                                        **/
/************************************************************/

/*
**  The type definitions.
*/

/*+ Method types. +*/

typedef enum HmeshOrderStMethodType_ {
  HMESHORDERSTMETHBL = 0,                         /*+ Block splitting post-processing       +*/
  HMESHORDERSTMETHCP,                             /*+ Mesh compression                      +*/
  HMESHORDERSTMETHGP,                             /*+ Gibbs-Poole-Stockmeyer                +*/
  HMESHORDERSTMETHGR,                             /*+ Graph-based ordering                  +*/
  HMESHORDERSTMETHHD,                             /*+ Block Halo Approximate Minimum Degree +*/
  HMESHORDERSTMETHHF,                             /*+ Block Halo Approximate Minimum Fill   +*/
  HMESHORDERSTMETHND,                             /*+ Nested Dissection                     +*/
  HMESHORDERSTMETHSI,                             /*+ Simple                                +*/
  HMESHORDERSTMETHNBR                             /*+ Number of methods                     +*/
} HmeshOrderStMethodType;

/*+ Method function pointer type. +*/

typedef int (* HmeshOrderFunc) (Hmesh * restrict const, Order * restrict const, const Gnum, OrderCblk * restrict const, const void * const);

/*
**  The external declarations.
*/

extern StratTab             hmeshorderststratab;

/*
**  The function prototypes.
*/

int                         hmeshOrderSt       (Hmesh * restrict const, Order * restrict const, const Gnum, OrderCblk * restrict const, const Strat * const);
