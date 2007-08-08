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
/**   NAME       : common_blas.c                           **/
/**                                                        **/
/**   AUTHOR     : Pierre RAMET                            **/
/**                                                        **/
/**   FUNCTION   : This module handles the generic BLAS    **/
/**                calls.                                  **/
/**                                                        **/
/**   DATES      : # Version 0.0  : from : 07 sep 1998     **/
/**                                 to     14 sep 1998     **/
/**                                                        **/
/************************************************************/

/*
**  The defines and includes.
*/

#define COMMON_BLAS

#include "common.h"

/*++++++++++++++++++++++++++++++++++++++
  Matrix AXPY : B=alpha.A+B (double precision)  

  char *transa (transpose A or not)

  char *transb (transpose B or not)

  int m (horizontal dimension)

  int n (vertical dimension)

  float alpha (scale factor)

  float *a (matrix A)

  int lda (leading dimension for A)

  float *b (matrix B)

  int ldb (leading dimension for B)
  ++++++++++++++++++++++++++++++++++++++*/

void dgeam(char *transa,char *transb,int m,int n,double alpha,double *a,
	   int lda,double *b,int ldb)
{
  int i;
  
  if (*transa=='N')
    {
      if (*transb=='N')
	for (i=0;i<n;i++)
	  daxpy(m,alpha,&(a[i*lda]),1,&(b[i*ldb]),1);
      else
	for (i=0;i<n;i++)
	  daxpy(m,alpha,&(a[i*lda]),1,&(b[i]),ldb);
    }
  else
    {
      if (*transb=='N')
	for (i=0;i<n;i++)
	  daxpy(m,alpha,&(a[i]),lda,&(b[i*ldb]),1);
      else
	for (i=0;i<n;i++)
	  daxpy(m,alpha,&(a[i]),lda,&(b[i]),ldb);
    }
}


/*++++++++++++++++++++++++++++++++++++++
  Matrix AXPY : B=alpha.A+B (simple precision)  

  char *transa (transpose A or not)

  char *transb (transpose B or not)

  int m (horizontal dimension)

  int n (vertical dimension)

  float alpha (scale factor)

  float *a (matrix A)

  int lda (leading dimension for A)

  float *b (matrix B)

  int ldb (leadind dimension for B)
  ++++++++++++++++++++++++++++++++++++++*/

void sgeam(char *transa,char *transb,int m,int n,float alpha,float *a,
	   int lda,float *b,int ldb)
{
  int i;
  
  if (*transa=='N')
    {
      if (*transb=='N')
	for (i=0;i<n;i++)
	  saxpy(m,alpha,&(a[i*lda]),1,&(b[i*ldb]),1);
      else
	for (i=0;i<n;i++)
	  saxpy(m,alpha,&(a[i*lda]),1,&(b[i]),ldb);
    }
  else
    {
      if (*transb=='N')
	for (i=0;i<n;i++)
	  saxpy(m,alpha,&(a[i]),lda,&(b[i*ldb]),1);
      else
	for (i=0;i<n;i++)
	  saxpy(m,alpha,&(a[i]),lda,&(b[i]),ldb);
    }
}


/*++++++++++++++++++++++++++++++++++++++
  Matrix COPY : B <- A (double dimension)

  int m (horizontal dimension)

  int n (vertical dimension)

  double *a (matrix A)

  int lda (leading dimension for A)

  double *b (matrix B)

  int ldb (leading dimension for B)
  ++++++++++++++++++++++++++++++++++++++*/

void dgecp(int m,int n,double *a,int lda,double *b,int ldb)
{
  int i;
  
  for(i=0;i<n;i++)
    dcopy(m,&(a[i*lda]),1,&(b[i*ldb]),1);
}


/*++++++++++++++++++++++++++++++++++++++
  Matrix COPY : B <- A (simple dimension)

  int m (horizontal dimension)

  int n (vertical dimension)

  double *a (matrix A)

  int lda (leading dimension for A)

  double *b (matrix B)

  int ldb (leading dimension for B)
  ++++++++++++++++++++++++++++++++++++++*/

void sgecp(int m,int n,float *a,int lda,float *b,int ldb)
{
  int i;
  
  for(i=0;i<n;i++)
    scopy(m,&(a[i*lda]),1,&(b[i*ldb]),1);
}
