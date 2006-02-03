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
/**   NAME       : library_target_build.c                  **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module is the API for the target   **/
/**                architecture building routine of the    **/
/**                libSCOTCH library.                      **/
/**                                                        **/
/**   DATES      : # Version 3.3  : from : 02 oct 1998     **/
/**                                 to     29 mar 1999     **/
/**                # Version 3.4  : from : 01 nov 2001     **/
/**                                 to     01 nov 2001     **/
/**                # Version 4.0  : from : 08 mar 2005     **/
/**                                 to     17 mar 2005     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define LIBRARY

#include "module.h"
#include "common.h"
#include "parser.h"
#include "graph.h"
#include "arch.h"
#include "arch_build.h"
#include "mapping.h"
#include "bgraph.h"
#include "bgraph_bipart_st.h"
#include "scotch.h"

/**************************************/
/*                                    */
/* These routines are the C API for   */
/* the architecture building routine. */
/*                                    */
/**************************************/

/*+ This routine parses the given
*** bipartitioning strategy.
*** It returns:
*** - 0   : if string successfully scanned.
*** - !0  : on error.
+*/

int
SCOTCH_stratGraphBipart (
SCOTCH_Strat * const        stratptr,
const char * const          string)
{
  if (*((Strat **) stratptr) != NULL)
    stratExit (*((Strat **) stratptr));

  if ((*((Strat **) stratptr) = stratInit (&bgraphbipartststratab, string)) == NULL) {
    errorPrint ("SCOTCH_stratBipart: error in bipartitioning strategy");
    return     (1);
  }

  return (0);
}

/*+ This routine fills the contents of the given
*** opaque target structure with the data provided
*** by the user. The source graph provided on input
*** is turned into a decomposition-defined target
*** architecture.
*** It returns:
*** - 0   : if the computation succeeded.
*** - !0  : on error.
+*/

int
SCOTCH_archBuild (
SCOTCH_Arch * const         archptr,              /*+ Target architecture to build    +*/
const SCOTCH_Graph * const  grafptr,              /*+ Graph to turn into architecture +*/
const SCOTCH_Num            listnbr,              /*+ Number of elements in sublist   +*/
const SCOTCH_Num * const    listptr,              /*+ Pointer to sublist              +*/
const SCOTCH_Strat * const  stratptr)             /*+ Bipartitoning strategy          +*/
{
  Strat *             bipstratptr;
  VertList            graflistdat;
  VertList *          graflistptr;
  int                 o;

  if ((sizeof (SCOTCH_Num) != sizeof (Gnum)) ||
      (sizeof (SCOTCH_Num) != sizeof (Anum))) {
    errorPrint ("SCOTCH_archBuild: internal error (1)");
    return     (1);
  }

  if (*((Strat **) stratptr) == NULL)             /* Set default mapping strategy if necessary */
    *((Strat **) stratptr) = stratInit (&bgraphbipartststratab, "(m{vert=50,low=h{pass=10},asc=f{move=100,bal=0.1}}f{move=100,bal=0.05})(/((load0=load)|(load0=0))?x;)");
  bipstratptr = *((Strat **) stratptr);
  if (bipstratptr->tabl != &bgraphbipartststratab) {
    errorPrint ("SCOTCH_archBuild: not a bipartitioning strategy");
    return     (1);
  }

  if ((listnbr == ((Graph *) grafptr)->vertnbr) || (listnbr == 0) || (listptr == NULL))
    graflistptr = NULL;
  else {
    graflistptr = &graflistdat;
    graflistdat.vnumnbr = (Gnum)   listnbr;
    graflistdat.vnumtab = (Gnum *) listptr;
  }

  o = archBuild ((Arch * const) archptr, (const Graph * const) grafptr, graflistptr, bipstratptr);

  return (o);
}
