/************************************************************/
/**                                                        **/
/**   NAME       : esmumps.h                               **/
/**                                                        **/
/**   AUTHORS    : Francois PELLEGRINI                     **/
/**                                                        **/
/**   FUNCTION   : These lines are the data declarations   **/
/**                for the symbolic factorization routine. **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 22 jul 1998     **/
/**                                 to     24 sep 1998     **/
/**                # Version 0.1  : from : 04 apr 1999     **/
/**                                 to     01 may 1999     **/
/**                                                        **/
/************************************************************/

#define ESMUMPS_H

/*
**  The function prototypes.
*/

#ifndef ESMUMPS
#define static
#endif

int                         esmumps             (const INT n, const INT iwlen, INT * const pe, const INT pfree, INT * const len, INT * const iw, INT * const nv, INT * const elen, INT * const last);

int                         esmumps_strat1      (const INT procnbr, const INT leafsiz, const int leorval, const INT cminval, const INT cmaxval, const double fratval, const int verbval, FILE * const stream, char * const straptr);

#undef static
