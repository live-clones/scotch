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
** $Id:
*/
/************************************************************/
/**                                                        **/
/**   NAME       : common.h                                **/
/**                                                        **/
/**   AUTHORS    : David GOUDIN                            **/
/**                Pascal HENON                            **/
/**                Francois PELLEGRINI                     **/
/**                Pierre RAMET                            **/
/**                Cedric CHEVALIER                        **/
/**                                                        **/
/**   FUNCTION   : Part of a parallel direct block solver. **/
/**                These lines are the common data         **/
/**                declarations for all modules.           **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 08 may 1998     **/
/**                                 to   : 08 jan 2001     **/
/**                # Version 1.0  : from : 06 jun 2002     **/
/**                                 to   : 06 jun 2002     **/
/**                # Version 2.0  : from : 13 jun 2005     **/
/**                                 to   : 27 apr 2006     **/
/**                                                        **/
/************************************************************/

#define COMMON_H

/*
** Machine configuration values.
** The end of the X_ARCH variable is built with parts of the
** `uname -m`, `uname -r`, and `uname -s` commands.
*/

#define X_C_RESTRICT

#if (defined X_ARCHpower_ibm_aix)
#define X_INCLUDE_ESSL
#undef  X_C_RESTRICT
#endif /* (defined X_ARCHpower_ibm_aix) */

/*
** Compiler optimizations.
*/

#ifdef __GNUC__
#define restrict                    __restrict
#endif /* __GNUC__ */
#ifndef /* X_C_RESTRICT */
#define restrict
#endif /* X_C_RESTRICT */

/*
** The includes.
*/

#include            <ctype.h>
#include            <math.h>
#include            <memory.h>
#include            <stdio.h>
#include            <stdarg.h>
#include            <stdlib.h>
#ifdef HAVE_MALLOC_H
#include            <malloc.h>                    /* Deprecated, but required on some old systems */
#endif /* HAVE_MALLOC_H */
#include            <string.h>
#include            <time.h>                      /* For the effective calls to clock () */
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

#define memAlloc(size)              malloc((size) | 8) /* For platforms which return NULL for malloc(0) */
#define memAlloca(size)             alloca(size)
#define memRealloc(ptr,size)        realloc((ptr),((size) | 8))
#define memFree(ptr)                (free ((char *) (ptr)), 0)
#define memFreea(ptr)               (0)           /* Freeing function if memAlloca implemented as malloc */
#define memSet(ptr,val,siz)         memset((ptr),(val),(siz))
#define memCpy(dst,src,siz)         memcpy((dst),(src),(siz))
#define memMov(dst,src,siz)         memmove((dst),(src),(siz))

#define MALLOC_ERROR(x)             {printf("error in %s allocation (line=%d,file=%s)\n",x,__LINE__,__FILE__); exit(1);}

#define MIN(x,y)                    (((x) < (y)) ? (x) : (y))
#define MAX(x,y)                    (((x) < (y)) ? (y) : (x))
#define ABS(x)                      MAX ((x), -(x))
#define SIGN(x)                     (((x) < 0) ? -1 : 1)

/*
**  Handling of generic types.
*/

#ifdef DOUBLE                                     /*+ If double data type wanted      +*/
#ifndef FLOAT                                     /*+ If type not overriden           +*/
#define FLOAT double                              /*+ Generic floating-point type     +*/
#define FLOAT_MAX MAXDOUBLE
#define BLAS_DOUBLE                               /*+ Sets double BLAS calls          +*/
#define COMM_FLOAT MPI_DOUBLE                     /*+ Generic MPI floating-point type +*/
#endif /* FLOAT */
#else /* DOUBLE */
#ifndef FLOAT                                     /*+ If type not overriden           +*/
#define FLOAT float                               /*+ Generic floating-point type     +*/
#define FLOAT_MAX MAXFLOAT
#define BLAS_FLOAT                                /*+ Sets float BLAS calls           +*/
#define COMM_FLOAT MPI_FLOAT                      /*+ Generic MPI floating-point type +*/
#endif /* FLOAT */
#endif /* DOUBLE */

#ifndef INT                                       /* If type not overriden        */
#ifdef LONG                                       /* If long ints not wanted      */
#define INT long                                  /* Generic integer type to long */
#else /* LONG */
#define INT int                                   /* Generic integer type to int */
#endif /* LONG */
#endif /* INT */
#define INT_TYPE_VAL_int            1
#define INT_TYPE_VAL_long           2
#define INT_TYPE_EXPAND(t)          (INT_TYPE_EXPAND_TWO(t))
#define INT_TYPE_EXPAND_TWO(t)      (INT_TYPE_VAL_##t)
#define INT_TYPE                    (INT_TYPE_EXPAND (INT))
#ifndef COMM_INT
#if (INT_TYPE == INT_TYPE_VAL_long)
#define COMM_INT MPI_LONG                         /* Generic MPI integer type */
#ifndef INT_MAX
#define INT_MAX MAXLONG                           /* Hmm, it is an existing macro */
#endif /* INT_MAX */
#else /* (INT_TYPE == INT_TYPE_VAL_long) */
#define COMM_INT MPI_INT                          /*+ Generic MPI integer type +*/
#ifndef INT_MAX
#define INT_MAX MAXINT
#endif /* INT_MAX */
#endif /* (INT_TYPE == INT_TYPE_VAL_long) */
#endif /* COMM_INT */
#ifndef INT_BITS
#define INT_BITS (sizeof (INT) * 8)
#endif /* INT_BITS */

#define byte unsigned char                        /* Byte type */
#ifndef BYTE
#define BYTE byte
#endif /* BYTE */
#ifndef COMM_BYTE
#define COMM_BYTE MPI_BYTE
#endif /* COMM_BYTE */
#define COMM_PART COMM_BYTE

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
**  Handling of generic BLAS.
*/

#ifdef BLAS_DOUBLE
#define AXPY                        daxpy
#define GEMM                        dgemm
#define GEMV                        dgemv
#define GEAM                        dgeam
#define GECP                        dgecp
#define SYMM                        gsymm
#define TRSM                        dtrsm
#define GEF                         dgef
#define POF                         dpof
#define PPF                         dppf
#define GER                         dger
#define GETMO                       dgetmo
#define SYR                         dsyr
#define SPR                         dspr
#define SYRK                        dsyrk
#define COPY                        dcopy
#define SCAL                        dscal
#define V_DIV                       dv_div
#define POTRF                       dpotrf
#define POTF2                       dpotf2
#else
#define AXPY                        saxpy
#define GEMM                        sgemm
#define GEMV                        sgemv
#define GEAM                        sgeam
#define GECP                        sgecp
#define SYMM                        ssymm
#define TRSM                        strsm
#define GEF                         sgef
#define POF                         spof
#define PPF                         sppf
#define GER                         sger
#define GETMO                       sgetmo
#define SYR                         ssyr
#define SPR                         sspr
#define SYRK                        ssyrk
#define COPY                        scopy
#define SCAL                        sscal
#define V_DIV                       sv_div
#define POTRF                       spotrf
#define POTF2                       spotf2
#endif

/*
**  Function prototypes.
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
INT                         intSearchDicho      (const INT * const, const INT, const INT, const INT);

void                        clockInit           (Clock * const);
void                        clockStart          (Clock * const);
void                        clockStop           (Clock * const);
double                      clockVal            (Clock * const);
double                      clockGet            (void);

/*
**  Macro definitions.
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
