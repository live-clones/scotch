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

set_source_files_properties(${CMAKE_BINARY_DIR}/libscotch/scotch.h PROPERTIES GENERATED 1)

################
#  libEsMumps  #
################

# esmumps.h
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/esmumps.h
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/library.h ${CMAKE_CURRENT_BINARY_DIR}/esmumps.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/library.h
  COMMENT "Copy esmumps.h to bin directory")

add_custom_target(esmumps_h
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/esmumps.h")

# libEsMumps
set(esmumps_src
  dof.c
  esmumps.c
  graph_graph.c
  library_esmumps.c
  library_esmumps_f.c
  library_esmumps_strats.c
  order.c
  order_check.c
  order_scotch_graph.c
  symbol.c
  symbol_check.c
  symbol_cost.c
  symbol_fax_graph.c
  symbol_fax.c)

set(esmumps_headers
  module.h
  library.h
  ${CMAKE_BINARY_DIR}/libscotch/scotch.h
  ${CMAKE_SOURCE_DIR}/libscotch/common.h
  graph.h
  dof.h
  symbol.h
  order.h
  fax.h
  esmumps.h)

add_library(esmumps ${esmumps_src} ${esmumps_headers})

target_include_directories(esmumps PRIVATE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libscotch>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libscotch>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(esmumps PRIVATE scotch)
add_dependencies(esmumps scotch_h)
add_executable(main_esmumps main_esmumps.c ${esmumps_header})
target_include_directories(main_esmumps PRIVATE ${CMAKE_BINARY_DIR}/libscotch)
target_link_libraries(main_esmumps PRIVATE esmumps scotch scotcherrexit)
add_dependencies(main_esmumps scotch_h)

# libEsMumps targets install
install(EXPORT esmumpsTargets
  FILE esmumpsTargets.cmake
  NAMESPACE SCOTCH::
  DESTINATION lib/cmake/scotch)

install(TARGETS esmumps
  EXPORT esmumpsTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include)
