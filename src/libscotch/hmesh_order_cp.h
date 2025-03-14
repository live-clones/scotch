/* Copyright 2004,2007,2010,2018,2023,2024 IPB, Universite de Bordeaux, INRIA & CNRS
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
/**   NAME       : hmesh_order_cp.h                        **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declaration    **/
/**                for the mesh compression algorithm.     **/
/**                                                        **/
/**   DATES      : # Version 4.0  : from : 08 feb 2004     **/
/**                                 to   : 08 feb 2004     **/
/**                # Version 5.1  : from : 04 nov 2010     **/
/**                                 to   : 04 nov 2010     **/
/**                # Version 6.0  : from : 07 jun 2018     **/
/**                                 to   : 07 jun 2018     **/
/**                # Version 7.0  : from : 20 jan 2023     **/
/**                                 to   : 11 sep 2024     **/
/**                                                        **/
/************************************************************/

/*
**  The defines.
*/

/** Prime number for hashing vertex numbers. **/

#define HMESHORDERCPHASHPRIME       17            /* Prime number */

/*
**  The type and structure definitions.
*/

/*+ This structure holds the method parameters. +*/

typedef struct HmeshOrderCpParam_ {
  double                    comprat;              /*+ Compression ratio threshold            +*/
  Strat *                   stratcpr;             /*+ Compressed submesh ordering strategy   +*/
  Strat *                   stratunc;             /*+ Uncompressed submesh ordering strategy +*/
} HmeshOrderCpParam;

/*+ This structure holds fine neighbor hashing data. +*/

typedef struct HmeshOrderCpHash_ {
  Gnum                      vnodnum;              /*+ Origin node vertex (i.e. pass) number +*/
  Gnum                      velmnum;              /*+ Adjacent end element vertex number    +*/
} HmeshOrderCpHash;

/*+ This structure holds coarse neighbor mate data. +*/

typedef struct HgraphOrderCpMate_ {
  Gnum                      coarvertend;          /*+ Adjacent coarse end vertex number +*/
  Gnum                      finevertend;          /*+ Adjacent end vertex number        +*/
} HgraphOrderCpMate;

/*
**  The function prototypes.
*/

#ifdef SCOTCH_HMESH_ORDER_CP
static Gnum                 hmeshOrderCpTree    (const Gnum * const, const Gnum * const, OrderCblk * const, Gnum);
#endif /* SCOTCH_HMESH_ORDER_CP */

int                         hmeshOrderCp        (Hmesh * const, Order * const, const Gnum, OrderCblk * const, const HmeshOrderCpParam * const);
