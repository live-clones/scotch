## Copyright 2014-2016,2021-2024 IPB, Universite de Bordeaux, INRIA & CNRS
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
##   AUTHORS    : Marc FUENTES                            ##
##                Florent PRUVOST                         ##
##                Cedric LACHAT (v6.0)                    ##
##                Amaury JACQUES (v6.0)                   ##
##                Clement BARTHELEMY                      ##
##                                                        ##
##   FUNCTION   : Secondary configuration file for CMake  ##
##                                                        ##
##   DATES      : # Version 6.0  : from : 01 sep 2014     ##
##                                 to     01 sep 2021     ##
##                # Version 7.0  : from : 01 sep 2021     ##
##                                 to     30 jul 2024     ##
##                                                        ##
############################################################

# Add general compiler options
add_definitions(-Drestrict=__restrict -DCOMMON_RANDOM_FIXED_SEED -DSCOTCH_RENAME)

# Add OS-specific compiler options
if(APPLE)
  add_definitions(-DCOMMON_OS_MACOS -D_DARWIN_C_SOURCE)
endif(APPLE)
if(MINGW OR MSYS OR WIN32)
  add_definitions(-DCOMMON_OS_WINDOWS)
endif(MINGW OR MSYS OR WIN32)
if(MSVC)
  add_definitions(-D_CRT_NONSTDC_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

# Detect FreeBSD
if(${CMAKE_HOST_SYSTEM_NAME}} MATCHES "^.*FreeBSD.*$")
  set(OS_IS_FREEBSD ON)
  message(STATUS "OS is FreeBSD")
endif()

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

# Set integer size
if(NOT INTSIZE STREQUAL "")
  if(INTSIZE STREQUAL "32")
    message(STATUS "Integer size is 32 bits")
    add_definitions(-DINTSIZE32)
  elseif(INTSIZE STREQUAL "64")
    message(STATUS "Integer size is 64 bits")
    add_definitions(-DINTSIZE64)
  else()
    message(FATAL_ERROR "Invalid integer size value")
  endif()
endif()

# Thread support in Scotch
if(THREADS)
  find_package(Threads)
  if(Threads_FOUND)
    add_definitions(-DCOMMON_PTHREAD -DSCOTCH_PTHREAD)
  endif()
  if(CMAKE_USE_WIN32_THREADS_INIT)
    add_definitions(-DCOMMON_THREAD_WIN32)
  endif()
  include(CheckPthreadAffinity)
  if(PTHREAD_AFFINITY_LINUX_OK)
    add_definitions(-DCOMMON_PTHREAD_AFFINITY_LINUX)
  endif()
endif()

# decompression libs
if(USE_ZLIB)
  find_package(ZLIB)
endif(USE_ZLIB)

if (USE_BZ2)
  find_package(BZip2)
endif (USE_BZ2)

if (USE_LZMA)
  find_package(LibLZMA)
endif(USE_LZMA)

# MPI
if(BUILD_PTSCOTCH)
  set(MPI_DETERMINE_LIBRARY_VERSION ON)
  find_package(MPI COMPONENTS C)
  if(NOT MPI_C_FOUND)
    message(FATAL_ERROR "MPI required to compile PT-Scotch\n Rerun cmake with -DBUILD_PTSCOTCH=OFF")
  else()
    string(REGEX MATCHALL "\( Open MPI \)+" found_suite ${MPI_C_LIBRARY_VERSION_STRING})
    if(NOT ${found_suite} STREQUAL "")
      message(STATUS "Found OpenMPI: using '--oversubscribe' policy for testing")
      set(MPIEXEC_PREFLAGS ${MPIEXEC_PREFLAGS} --oversubscribe)
    endif()
    string(REGEX MATCHALL "\(Intel\)+" found_suite ${MPI_C_LIBRARY_VERSION_STRING})
    if(NOT ${found_suite} STREQUAL "")
      message(STATUS "Found Intel MPI: using '-genv=I_MPI_PIN_DOMAIN=core' for testing")
      set(MPIEXEC_PREFLAGS ${MPIEXEC_PREFLAGS} -genv=I_MPI_PIN_DOMAIN=core)
    endif()
  endif()

  if(MPI_THREAD_MULTIPLE)
    if(Threads_FOUND)
      include(CheckMpiThreadMultiple)
      if(MPI_THREAD_MULTIPLE_OK)
        message(STATUS "MPI implementation DOES support MPI_THREAD_MULTIPLE")
        add_definitions(-DSCOTCH_PTHREAD_MPI)
      else()
        message(STATUS "MPI implementation does NOT support MPI_THREAD_MULTIPLE")
      endif()
    endif()
  endif()
endif(BUILD_PTSCOTCH)

# Put libraries in the lib subdirectory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Put executables in the bin subdirectory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set variables for include directories
set(GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(LIBSCOTCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libscotch)
file(MAKE_DIRECTORY ${GENERATED_INCLUDE_DIR})

# Build Scotch and PT-Scotch libraries
add_subdirectory(libscotch)

# Build Scotch executables
add_subdirectory(scotch)

# Build EsMUMPS library
if(BUILD_LIBESMUMPS)
  add_subdirectory(esmumps)
endif(BUILD_LIBESMUMPS)

# Build ScotchMeTiS library
if(BUILD_LIBSCOTCHMETIS)
  add_subdirectory(libscotchmetis)
endif(BUILD_LIBSCOTCHMETIS)

# Testing
if(ENABLE_TESTS)
  add_subdirectory(check)
endif(ENABLE_TESTS)

####################
#  Export targets  #
####################

# See https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html
include(CMakePackageConfigHelpers)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/SCOTCHConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/SCOTCHConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scotch
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR)

write_basic_package_version_file(SCOTCHConfigVersion.cmake
  VERSION ${SCOTCH_VERSION_LONG}
  COMPATIBILITY AnyNewerVersion)

# Install config files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SCOTCHConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/SCOTCHConfigVersion.cmake
  COMPONENT libscotch
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scotch)
