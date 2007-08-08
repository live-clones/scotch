/* Copyright 2004,2007 INRIA
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
/**   NAME       : symbol_draw.h                           **/
/**                                                        **/
/**   AUTHORS    : Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the data declarations   **/
/**                for the symbolic matrix drawing         **/
/**                routine.                                **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 29 sep 1998     **/
/**                                 to     30 sep 1998     **/
/**                # Version 1.0  : from : 26 jun 2002     **/
/**                                 to     26 jun 2002     **/
/**                # Version 1.3  : from : 10 jun 2003     **/
/**                                 to     10 jun 2003     **/
/**                # Version 3.0  : from : 29 feb 2004     **/
/**                                 to     29 feb 2004     **/
/**                                                        **/
/************************************************************/

#define SYMBOL_DRAW_H

#ifdef CXREF_DOC
#ifndef COMMON_H
#include "common.h"
#endif /* COMMON_H */
#ifndef SYMBOL_H
#include "symbol.h"
#endif /* SYMBOL_H */
#endif /* CXREF_DOC */

/*
**  The definitions.
*/

/*+ Generic PostScript (tm) output definitions. +*/

#define SYMBOL_PSDPI                72            /*+ PostScript dots-per-inch            +*/
#define SYMBOL_PSPICTSIZE           6.6           /*+ PostScript picture size (in inches) +*/
/*
**  The function prototypes.
*/

#ifndef SYMBOL_DRAW
#define static
#endif

#undef static
