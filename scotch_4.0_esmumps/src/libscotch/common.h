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
/**   NAME       : common.h                                **/
/**                                                        **/
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the common data         **/
/**                declarations for all modules.           **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 08 may 1998     **/
/**                                 to     08 jan 2001     **/
/**                # Version 1.0  : from : 06 jun 2002     **/
/**                                 to     06 jun 2002     **/
/**                # Version 0.2  : from : 13 jun 2005     **/
/**                                 to     13 jun 2005     **/
/**                                                        **/
/************************************************************/

#define COMMON_H

/*
** Machine configuration values.
** The end of the X_ARCH variable is built with parts of the
** `uname -m`, `uname -r`, and `uname -s` commands.
*/

#ifndef X_C_NORESTRICT
#define X_C_RESTRICT
#endif /* X_C_NORESTRICT */

#if (defined X_ARCHpower_ibm_aix)
#define X_INCLUDE_ESSL
#undef  X_C_RESTRICT
#endif /* (defined X_ARCHpower_ibm_aix) */

/*
** Compiler optimizations.
*/

#ifdef X_C_RESTRICT
#ifdef __GNUC__
#define restrict                    __restrict
#endif /* __GNUC__ */
#else /* X_C_RESTRICT */
#define restrict
#endif /* X_C_RESTRICT */

/*
** The includes.
*/

#include            <ctype.h>
#include            <math.h>
#include            <malloc.h>
#include            <memory.h>
#include            <stdio.h>
#include            <stdarg.h>
#include            <stdlib.h>
#include            <string.h>
#include            <time.h>                     /* For the effective calls to clock () */
#include            <limits.h>
#include            <float.h>
#include            <sys/types.h>
#include            <sys/time.h>
#include            <sys/resource.h>
#include            <unistd.h>

#ifdef X_INCLUDE_ESSL
#include            <essl.h>
#endif /* X_INCLUDE_ESSL */

#ifdef X_ASSERT
#include <assert.h>
#endif /* X_ASSERT */

/*
**  Working definitions.
*/

#define memAlloc(size)              malloc(size)
#define memAlloca(size)             alloca(size)
#define memRealloc(ptr,size)        realloc((ptr),(size))
#define memFree(ptr)                (free ((char *) (ptr)), 0)
#define memFreea(ptr)               (0)           /* Freeing function in case memAlloca implemented as malloc */
#define memSet(ptr,val,siz)         memset((ptr),(val),(siz))
#define memCpy(dst,src,siz)         memcpy((dst),(src),(siz))
#define memMov(dst,src,siz)         memmove((dst),(src),(siz))

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)<(y))?(y):(x))

#define MALLOC_ERROR(x) {printf("error in %s allocation (line=%d,file=%s)\n",x,__LINE__,__FILE__); exit(1);}

#define FALSE 0
#define TRUE 1

/*
**  Handling of generic types.
*/

#define byte unsigned char                        /*+ Byte type +*/

#ifndef FLOAT
#define DOUBLE
#endif

#ifdef DOUBLE
#define BLAS_DOUBLE
#else
#define BLAS_FLOAT
#endif

#ifdef CPLX
#include <complex.h>
#define COMM_FLOAT GetMpiType()
#define COMM_SUM GetMpiSum()
#ifdef DOUBLE
#define FLOAT double complex
#define ABS_FLOAT(x) cabs(x)
#define BLAS_FLOAT dcmplx
#else /* DOUBLE */
#define FLOAT float complex
#define ABS_FLOAT(x) cabsf(x)
#define BLAS_FLOAT cmplx
#endif /* DOUBLE */
#else /* CPLX */
#define COMM_SUM MPI_SUM
#ifdef DOUBLE
#define FLOAT double
#define ABS_FLOAT(x) fabs(x)
#define COMM_FLOAT MPI_DOUBLE
#define FLOAT_MAX MAXDOUBLE
#else /* DOUBLE */
#define FLOAT float
#define FLOAT_MAX MAXFLOAT
#define ABS_FLOAT(x) fabsf(x)
#endif /* DOUBLE */
#endif /* CPLX */

#ifdef LONG
#define INT long
#define COMM_INT MPI_LONG
#ifndef INT_MAX
#define INT_MAX MAXLONG
#endif /* INT_MAX */
#else /* LONG */
#define INT int
#define COMM_INT MPI_INT
#ifndef INT_MAX
#define INT_MAX MAXINT
#endif /* INT_MAX */
#endif /* LONG */
#ifndef INT_BITS
#define INT_BITS (sizeof (INT) * 8)
#endif /* INT_BITS */

/*
**  Handling of timers.
*/

/** The clock type. **/

typedef struct Clock_ {
  double                    time[2];              /*+ The start and accumulated times +*/
} Clock;

/*
**  Handling of files.
*/

/** The file structure. **/

typedef struct File_ {
  char *                    name;                 /*+ File name    +*/
  FILE *                    pntr;                 /*+ File pointer +*/
  char *                    mode;                 /*+ Opening mode +*/
} File;

/*
**  The function prototypes.
*/

void *                      memAllocGroup       (void **, ...);
void *                      memReallocGroup     (void *, ...);
void *                      memOffset           (void *, ...);

void                        usagePrint          (FILE * const, const char (* []));

void                        errorProg           (const char * const);
void                        errorPrint          (const char * const, ...);
void                        errorPrintW         (const char * const, ...);

int                         intLoad             (FILE * const, INT * const);
int                         intSave             (FILE * const, const INT);
void                        intAscn             (INT * restrict const, const INT, const INT);
void                        intPerm             (INT * restrict const, const INT);
void                        intRandReset        (void);
void                        intRandInit         (void);
INT                         intRandVal          (INT);
void                        intSort1asc1        (void * const, const INT);
void                        intSort2asc1        (void * const, const INT);
void                        intSort2asc2        (void * const, const INT);

void                        clockInit           (Clock * const);
void                        clockStart          (Clock * const);
void                        clockStop           (Clock * const);
double                      clockVal            (Clock * const);
double                      clockGet            (void);

/*
**  The macro definitions.
*/

#define clockInit(clk)              ((clk)->time[0]  = (clk)->time[1] = 0)
#define clockStart(clk)             ((clk)->time[0]  = clockGet ())
#define clockStop(clk)              ((clk)->time[1] += (clockGet () - (clk)->time[0]))
#define clockVal(clk)               ((clk)->time[1])

#define intRandVal(ival)            ((INT) (((unsigned INT) random ()) % ((unsigned INT) (ival))))

#define FORTRAN(nu,nl,pl,pc)                     \
void nu pl;                                      \
void nl pl                                       \
{ nu pc; }                                       \
void nl##_ pl                                    \
{ nu pc; }                                       \
void nl##__ pl                                   \
{ nu pc; }                                       \
void nu pl
