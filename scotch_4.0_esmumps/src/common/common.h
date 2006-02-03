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
/**                                                        **/
/************************************************************/

#define COMMON_H

/*
** External symbols
**
** Allowed are :
**
** - OS selection (Mutually exclusive) :
**    X_OSDOS        : DOS.
**    X_OSUNIX       : Generic UNIX :
**                     X_OSUNIX_BSD : Berkeley UNIX.
**                     X_OSUNIX_SV  : ATT Sys5 UNIX.
*/

/*
** Machine configuration values.
** The end of the X_ARCH variable is built with parts of the
** `uname -m`, `uname -r`, and `uname -s` commands.
*/

#define X_OSUNIX
#define X_OSUNIX_SV

#define X_C_RESTRICT

#if (defined X_ARCHpower3_ibm_aix5) || (defined X_ARCHpower3_ibm_aix4) || (defined X_ARCHpower2_ibm_aix4) || (defined X_ARCHpowerPC_ibm_aix5) || (defined X_ARCHpowerPC_ibm_aix4)
#define X_INCLUDE_ESSL
#undef  X_C_RESTRICT
#endif /* (defined X_ARCHpower3_ibm_aix5) || (defined X_ARCHpower3_ibm_aix4) || (defined X_ARCHpower2_ibm_aix4) || (defined X_ARCHpowerPC_ibm_aix5) || (defined X_ARCHpowerPC_ibm_aix4) */

#if (defined X_ARCHsparc_sun_sunos3) || (defined X_ARCHsparc_sun_sunos4)
#undef X_OSUNIX_SV
#define X_OSUNIX_BSD
#endif /* (defined X_ARCHsparc_sun_sunos3) || (defined X_ARCHsparc_sun_sunos4) */

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
** Debug values.
*/

#ifdef DEBUG_ALL
#define DEBUG_COMMON
#define DEBUG_GRAPH
#define DEBUG_DOF
#define DEBUG_MESH
#define DEBUG_SYMBOL
#define DEBUG_SCOTCH
#define DEBUG_ORDER
#define DEBUG_FAX
#define DEBUG_BLEND
#endif /* DEBUG_ALL */

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

#define FLAG_ASSERT /*+ not define for final version +*/

#ifdef FLAG_ASSERT
#include <assert.h>
#define MPI_PRINT_ERR(x,y) {char s[MPI_MAX_ERROR_STRING];int l;MPI_Error_string(y,s,&l);printf("error in %s (%s)(line=%d,file=%s)\n",x,s,__LINE__,__FILE__); exit(1);}
#define CALL_MPI err_mpi=
#define TEST_MPI(x) if (err_mpi!=MPI_SUCCESS) MPI_PRINT_ERR(x,err_mpi)
#else
#define CALL_MPI
#define TEST_MPI(x)
#endif

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)<(y))?(y):(x))

#define MALLOC_ERROR(x) {printf("error in %s allocation (line=%d,file=%s)\n",x,__LINE__,__FILE__); exit(1);}

#define FALSE 0
#define TRUE 1

/*
**  The handling of generic types.
*/

#define byte unsigned char                        /*+ Byte type +*/

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

#ifndef INT                                       /*+ If type not overriden    +*/
#define INT int                                   /*+ Generic integer type     +*/
#define COMM_INT MPI_INT                          /*+ Generic MPI integer type +*/
#endif /* INT */
#ifndef INT_MAX
#define INT_MAX MAXINT
#endif /* INT_MAX */
#ifndef INT_BITS
#define INT_BITS (sizeof (INT) * 8)
#endif /* INT_BITS */

/*
**  The handling of random numbers.
*/

#define RANDOMMAX                   65535        /* To comply with SYSV pseudo-random generators */

/*
**  The handling of timers.
*/

/** The clock type. **/

typedef struct Clock_ {
  double                    time[2];              /*+ The start and accumulated times +*/
} Clock;

/*
**  The handling of files.
*/

/** The file structure. **/

typedef struct File_ {
  char *                    name;                 /*+ File name    +*/
  FILE *                    pntr;                 /*+ File pointer +*/
  char *                    mode;                 /*+ Open mode    +*/
} File;

/*
**  The handling of generic BLAS.
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

void                        dgeam               (char * transa, char * transb, int m, int n, double alpha, double * a, int lda, double * b, int ldb);
void                        sgeam               (char * transa, char * transb, int m, int n, float alpha, float * a, int lda, float * b, int ldb);
void                        dgecp               (int m, int n, double * a, int lda, double * b, int ldb);
void                        sgecp               (int m, int n, float * a, int lda, float * b, int ldb);

/*
**  The macro definitions.
*/

#define clockInit(clk)              ((clk)->time[0]  = (clk)->time[1] = 0)
#define clockStart(clk)             ((clk)->time[0]  = clockGet ())
#define clockStop(clk)              ((clk)->time[1] += (clockGet () - (clk)->time[0]))
#define clockVal(clk)               ((clk)->time[1])

#define intRandVal(ival)            ((((ival) <= (RANDOMMAX + 1)) ? (INT) random () : ((INT) (((double) random () * (double) (ival)) / (double) RANDOMMAX))) % (ival))

#define FORTRAN(nu,nl,pl,pc)                     \
void nu pl;                                      \
void nl pl                                       \
{ nu pc; }                                       \
void nl##_ pl                                    \
{ nu pc; }                                       \
void nl##__ pl                                   \
{ nu pc; }                                       \
void nu pl
