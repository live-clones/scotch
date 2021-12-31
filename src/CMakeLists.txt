## Copyright 2014-2016,2021 IPB, Universite de Bordeaux, INRIA & CNRS
##
## This file is part of the Scotch software package for static mapping,
## graph partitioning and sparse matrix ordering.
##
## This software is governed by the CeCILL-C license under French law
## and abiding by the rules of distribution of free software. You can
## use, modify and/or redistribute the software under the terms of the
## CeCILL-C license as circulated by CEA, CNRS and INRIA at the following
## URL: "http://www.cecill.info".
##
## As a counterpart to the access to the source code and rights to copy,
## modify and redistribute granted by the license, users are provided
## only with a limited warranty and the software's author, the holder of
## the economic rights, and the successive licensors have only limited
## liability.
##
## In this respect, the user's attention is drawn to the risks associated
## with loading, using, modifying and/or developing or reproducing the
## software by the user in light of its specific status of free software,
## that may mean that it is complicated to manipulate, and that also
## therefore means that it is reserved for developers and experienced
## professionals having in-depth computer knowledge. Users are therefore
## encouraged to load and test the software's suitability as regards
## their requirements in conditions enabling the security of their
## systems and/or data to be ensured and, more generally, to use and
## operate it in the same conditions as regards security.
##
## The fact that you are presently reading this means that you have had
## knowledge of the CeCILL-C license and that you accept its terms.
##
############################################################
##                                                        ##
##   AUTHORS    : Cedric LACHAT                           ##
##                Amaury JACQUES                          ##
##                Florent PRUVOST                         ##
##                Marc FUENTES                            ##
##                                                        ##
##   FUNCTION   : Secondary configuration file for CMake  ##
##                                                        ##
##   DATES      : # Version 6.0  : from : 01 sep 2014     ##
##                                 to     01 sep 2021     ##
##                # Version 7.0  : from : 01 sep 2021     ##
##                                 to     30 dec 2021     ##
##                                                        ##
############################################################

project(Scotch)
cmake_minimum_required(VERSION 3.10)
enable_language(C Fortran)

set(SCOTCH_VERSION 7)
set(SCOTCH_RELEASE 0)
set(SCOTCH_PATCHLEVEL 0)
add_definitions(-DSCOTCH_VERSION_NUM=${SCOTCH_VERSION}
  -DSCOTCH_RELEASE_NUM=${SCOTCH_RELEASE}
  -DSCOTCH_PATCHLEVEL_NUM=${SCOTCH_PATCHLEVEL})

set(SCOTCH_VERSION_LONG "${SCOTCH_VERSION}.${SCOTCH_RELEASE}.${SCOTCH_PATCHLEVEL}")

# Old style cmake: TODO: use some preload.cmake for old configs
add_definitions(-Drestrict=__restrict -DCOMMON_RANDOM_FIXED_SEED -DSCOTCH_RENAME)

###################
#  Prerequisites  #
###################

# Bison
find_package(BISON)
if(NOT BISON_FOUND)
  message(FATAL_ERROR "Bison required to compile Scotch and PT-Scotch")
else()
  message(STATUS "Bison found here: ${BISON_EXECUTABLE}")
endif()

# Flex
find_package(FLEX)
if(NOT FLEX_FOUND)
  message(FATAL_ERROR "Flex required to compile Scotch and PT-Scotch")
else()
  message(STATUS "Flex found here: ${FLEX_EXECUTABLE}")
endif()

#############
#  Options  #
#############

# Thread support in scotch
option(THREADS "Activate multi-threading for Scotch and PT-Scotch" ON)
if(THREADS)
  find_package(Threads)
  if (Threads_FOUND)
    add_definitions(-DSCOTCH_PTHREAD -DCOMMON_PTHREAD)
  endif()
endif(THREADS)

add_subdirectory(libscotch)
add_subdirectory(scotch)

# MPI
option(PTSCOTCH "Activate building of PT-Scotch" ON)
option(THREADS_MPI "Activate multi-threading with MPI for PT-Scotch" ON)
if (PTSCOTCH)
  find_package(MPI COMPONENTS C)
  if (NOT MPI_C_FOUND)
    message(FATAL_ERROR "MPI required to compile PT-Scotch")
  endif()
endif(PTSCOTCH)
if(THREADS_MPI)
  if (Threads_FOUND)
    add_definitions(-DSCOTCH_PTHREAD_MPI) # TODO: write a cmake function to check that MPI_THREAD_MULTIPLE is ON
  endif()
endif(THREADS_MPI)

# ScotchMetis
option(SCOTCHMETIS "Activate building of ScotchMeTiS library" ON)
if(SCOTCHMETIS)
  add_subdirectory(libscotchmetis)
endif(SCOTCHMETIS)

# Esmumps
option(ESMUMPS "Activate building of EsMumps library" ON)
if(ESMUMPS)
  add_subdirectory(esmumps)
endif(ESMUMPS)

# Testing
enable_testing()
add_subdirectory(check)

####################
#  Export targets  #
####################

# see https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html
include(CMakePackageConfigHelpers)

set(INC_INSTALL_DIR "include/" CACHE STRING "Where to install headers relative to prefix")
set(LIB_INSTALL_DIR "lib/" CACHE STRING "Where to install libraries relative to prefix")

configure_package_config_file(cmake/SCOTCHConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/SCOTCHConfig.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/scotch
  PATH_VARS INC_INSTALL_DIR LIB_INSTALL_DIR)

write_basic_package_version_file(SCOTCHConfigVersion.cmake
  VERSION ${SCOTCH_VERSION_LONG}
  COMPATIBILITY AnyNewerVersion)

# Install config files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SCOTCHConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/SCOTCHConfigVersion.cmake
  DESTINATION ${LIB_INSTALL_DIR}/cmake/scotch)
