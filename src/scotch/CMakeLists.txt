## Copyright 2014-2016,2021-2023 IPB, Universite de Bordeaux, INRIA & CNRS
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
##                Cedric LACHAT                           ##
##                Amaury JACQUES                          ##
##                                                        ##
##   FUNCTION   : Secondary configuration file for CMake  ##
##                                                        ##
##   DATES      : # Version 6.0  : from : 01 sep 2014     ##
##                                 to     01 sep 2021     ##
##                # Version 7.0  : from : 01 sep 2021     ##
##                                 to     01 nov 2023     ##
##                                                        ##
############################################################

set_source_files_properties(${GENERATED_INCLUDE_DIR}/scotch.h PROPERTIES GENERATED 1)

# Macro to add quickly simple exectubles linked against the libScotch library
function(add_scotch_exe)
  if(ARGC LESS_EQUAL 0)
    message(FATAL_ERROR "Macro add_scotch_exe requires arguments")
  endif()
  set(file_we ${ARGV0})
  if(ARGC GREATER_EQUAL 2)
    set(src_exe ${ARGV})
    list(REMOVE_AT src_exe 0)
  elseif(ARGC EQUAL 1)
    set(src_exe ${file_we}.c ${file_we}.h)
  endif()
  add_executable(${file_we}
    ${src_exe}
    ${LIBSCOTCH_INCLUDE_DIR}/module.h
    ${GENERATED_INCLUDE_DIR}/scotch.h
    ${LIBSCOTCH_INCLUDE_DIR}/common.h)
  add_dependencies(${file_we} scotch_h)
  target_include_directories(${file_we} PRIVATE ${GENERATED_INCLUDE_DIR})
  target_compile_definitions(${file_we} PUBLIC "SCOTCH_CODENAME=\"${SCOTCH_CODENAME}\"")
  if(Threads_FOUND)
    target_compile_definitions(${file_we} PUBLIC SCOTCH_PTHREAD COMMON_PTHREAD)
    target_link_libraries(${file_we} PUBLIC Threads::Threads ${THREADS_PTHREADS_WIN32_LIBRARY})
  endif(Threads_FOUND)
  target_link_libraries(${file_we} PUBLIC scotch scotcherrexit)
endfunction(add_scotch_exe)

# Scotch executables
set(scotch_exe
  acpl
  amk_ccc
  amk_fft2
  amk_grf
  amk_hy
  amk_m2
  amk_p2
  atst
  gbase
  gcv
  gmap
  gmk_hy
  gmk_m2
  gmk_m3
  gmk_msh
  gmk_ub2
  gmtst
  gord
  gotst
  gscat
  gtst
  mcv
  mmk_m2
  mmk_m3
  mord
  mtst)

# Generic target
foreach(target ${scotch_exe})
  add_scotch_exe(${target})
endforeach()

# Targets which need special source lists
add_scotch_exe(gout gout_c.c gout_o.c)
if(NOT WIN32)
  target_link_libraries(gout PRIVATE m)
endif(NOT WIN32)

# gpart is a special target (same source as gmap)
add_scotch_exe(gpart gmap.c gmap.h)
target_compile_definitions(gpart PRIVATE SCOTCH_COMPILE_PART)

# Install all Scotch executables
foreach(target ${scotch_exe})
  install(TARGETS ${target} COMPONENT scotch RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()

# Executables linked against the libPTScotch library
if(BUILD_PTSCOTCH)
  set_source_files_properties(${GENERATED_INCLUDE_DIR}/ptscotch.h PROPERTIES GENERATED 1)
  function(add_ptscotch_exe)
    if(ARGC LESS_EQUAL 0)
      message(FATAL_ERROR "Macro add_ptscotch_exe requires arguments")
    endif()
    set(file_we ${ARGV0})
    if(ARGC GREATER_EQUAL 2)
      set(src_exe ${ARGV})
      list(REMOVE_AT src_exe 0)
    elseif(ARGC EQUAL 1)
      set(src_exe ${file_we}.c ${file_we}.h)
    endif()
    add_executable(${file_we}
      ${src_exe}
      ${LIBSCOTCH_INCLUDE_DIR}/module.h
      ${LIBSCOTCH_INCLUDE_DIR}/common.h
      ${GENERATED_INCLUDE_DIR}/ptscotch.h)
    add_dependencies(${file_we} ptscotch_h)
    target_compile_definitions(${file_we} PUBLIC "SCOTCH_CODENAME=\"${SCOTCH_CODENAME}\"")
    if(Threads_FOUND)
      target_compile_definitions(${file_we} PUBLIC SCOTCH_PTHREAD COMMON_PTHREAD)
      target_link_libraries(${file_we} PUBLIC Threads::Threads ${THREADS_PTHREADS_WIN32_LIBRARY})
    endif(Threads_FOUND)
    target_include_directories(${file_we} PRIVATE ${GENERATED_INCLUDE_DIR})
    target_link_libraries(${file_we} PUBLIC scotch ptscotch ptscotcherrexit)
  endfunction(add_ptscotch_exe)

  set(ptscotch_exe
    dggath
    dgmap
    dgord
    dgscat
    dgtst)

  # Generic PT-Scotch targets
  foreach(target ${ptscotch_exe})
    add_ptscotch_exe(${target})
  endforeach()
  add_ptscotch_exe(dgpart dgmap.c dgmap.h)
  target_compile_definitions(dgpart PRIVATE SCOTCH_COMPILE_PART)
  # Install all PT-Scotch executables
  foreach(target ${ptscotch_exe})
    install(TARGETS ${target} COMPONENT ptscotch RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  endforeach()
 endif(BUILD_PTSCOTCH)
