/************************************************************/
/**                                                        **/
/**   NAME       : esmumps_f.c                             **/
/**                                                        **/
/**   AUTHOR     : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : This module contains Fortran MUMPS      **/
/**                stubs for the ordering routines of the  **/
/**                libSCOTCH + Emilio libfax libraries.    **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 16 may 2001     **/
/**                                 to     17 may 2001     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#include "common.h"
#include "esmumps.h"

/**************************************/
/*                                    */
/* These routines are the Fortran API */
/* for the MUMPS ordering routine.    */
/*                                    */
/**************************************/

FORTRAN (                                       \
ESMUMPSF, esmumpsf, (                           \
const INT * const           n,                  \
const INT * const           iwlen,              \
INT * const                 petab,              \
const INT * const           pfree,              \
INT * const                 lentab,             \
INT * const                 iwtab,              \
INT * const                 nvtab,              \
INT * const                 elentab,            \
INT * const                 lasttab,            \
INT * const                 ncmpa),             \
(n, iwlen, petab, pfree, lentab, iwtab, nvtab, elentab, lasttab, ncmpa))
{
  *ncmpa = esmumps (*n, *iwlen, petab, *pfree, lentab, iwtab, nvtab, elentab, lasttab);
}
