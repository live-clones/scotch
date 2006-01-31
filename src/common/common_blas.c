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
