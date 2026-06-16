!* Copyright 2026 IPB, Universite de Bordeaux, INRIA & CNRS
!*
!* This file is part of the Scotch software package for static mapping,
!* graph partitioning and sparse matrix ordering.
!*
!* This software is governed by the CeCILL-C license under French law
!* and abiding by the rules of distribution of free software. You can
!* use, modify and/or redistribute the software under the terms of the
!* CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
!* URL: "http://www.cecill.info".
!*
!* As a counterpart to the access to the source code and rights to copy,
!* modify and redistribute granted by the license, users are provided
!* only with a limited warranty and the software's author, the holder of
!* the economic rights, and the successive licensors have only limited
!* liability.
!*
!* In this respect, the user's attention is drawn to the risks associated
!* with loading, using, modifying and/or developing or reproducing the
!* software by the user in light of its specific status of free software,
!* that may mean that it is complicated to manipulate, and that also
!* therefore means that it is reserved for developers and experienced
!* professionals having in-depth computer knowledge. Users are therefore
!* encouraged to load and test the software's suitability as regards
!* their requirements in conditions enabling the security of their
!* systems and/or data to be ensured and, more generally, to use and
!* operate it in the same conditions as regards security.
!*
!* The fact that you are presently reading this means that you have had
!* knowledge of the CeCILL-C license and that you accept its terms.
!
!***********************************************************
!*                                                        **
!*   NAME       : test_scotch_mesh_build.f90              **
!*                                                        **
!*   AUTHOR     : Francois PELLEGRINI                     **
!*                                                        **
!*   FUNCTION   : This module tests te operation of       **
!*                the mesh building routine.              **
!*                                                        **
!*   DATES      : # Version 7.0  : from : 24 jan 2026     **
!*                                 to     24 jan 2026     **
!*                                                        **
!234567*****************************************************

program test_scotch_mesh_build
  use iso_c_binding
  implicit none
  interface
    subroutine exit_c(val) bind(c, name='exit')
      use, intrinsic :: iso_c_binding, only: c_int
      integer(c_int), value, intent(in) :: val
    end subroutine exit_c
  end interface

  include 'scotchf.h'

  doubleprecision         :: meshdat(SCOTCH_MESHDIM)
  integer(SCOTCH_NUMSIZE) :: velmbas
  integer(SCOTCH_NUMSIZE) :: vnodbas
  integer(SCOTCH_NUMSIZE) :: velmnbr
  integer(SCOTCH_NUMSIZE) :: vnodnbr
  integer(SCOTCH_NUMSIZE) :: edgenbr
  integer                 :: ierr

  integer(SCOTCH_NUMSIZE), allocatable :: verttab(:)
  integer(SCOTCH_NUMSIZE), allocatable :: edgetab(:)

  ierr = 0

  call scotchfmeshinit(meshdat, ierr)

  velmbas=1
  vnodbas=4
  velmnbr=3
  vnodnbr=8
  edgenbr=24
  allocate(verttab(1+velmnbr+vnodnbr))
  allocate(edgetab(24))
  verttab = [1,5,9,13,14,16,18,20,21,22,23,25]
  edgetab = [5,11,7,6,10,5,11,4,8,9,6,7,2,2,1,1,3,1,3,3,3,2,2,1]

  call scotchfmeshbuild(meshdat, & ! doubleprecision (*) meshdat
  velmbas, & ! integer*num velmbas,
  vnodbas, & ! integer*num vnodbas,
  velmnbr, & ! integer*num velmnbr,
  vnodnbr, & ! integer*num vnodnbr,
  verttab, & ! integer*num (*) verttab,
  verttab, & ! integer*num (*) vendtab,
  verttab, & ! integer*num (*) velotab,
  verttab, & ! integer*num (*) vnlotab,
  verttab, & ! integer*num (*) vlbltab,
  edgenbr, & ! integer*num edgenbr,
  edgetab, & ! integer*num (*) edgetab,
  ierr) ! integer*num ierr)

  if (ierr .ne. 0) then
    print *, "ERROR: main: error in scotchfmeshbuild"
    call exit_c (1)
  end if

end program test_scotch_mesh_build
