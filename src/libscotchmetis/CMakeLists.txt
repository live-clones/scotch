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

####################
#  libScotchMeTiS  #
####################

set_source_files_properties(${CMAKE_BINARY_DIR}/libscotch/scotch.h PROPERTIES GENERATED 1)
set_source_files_properties(${CMAKE_BINARY_DIR}/libscotchmetis/metis.h PROPERTIES GENERATED 1)

# metis.h
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/metis.h
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/library_metis.h ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND $<TARGET_FILE:dummysizes> library_metis.h metis.h DEPENDS dummysizes
  DEPENDS library_metis.h
  COMMENT "Generating metis.h")

add_custom_target(metis_h
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/metis.h")

# libScotchMeTiS
foreach(version 3 5)
  add_library(scotchmetisv${version}
    metis_graph_order.c
    metis_graph_order_f.c
    metis_graph_part.c
    metis_graph_part_f.c
    metis_graph_dual.c
    metis_graph_dual_f.c
    metis_options.c
    metis_options_f.c
    ${CMAKE_SOURCE_DIR}/libscotch/module.h
    ${CMAKE_SOURCE_DIR}/libscotch/common.h
    ${CMAKE_BINARY_DIR}/libscotch/scotch.h
    metis.h)

  target_compile_definitions(scotchmetisv${version} PUBLIC SCOTCH_METIS_VERSION=${version})

  target_include_directories(scotchmetisv${version}  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libscotch>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libscotch>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>)
  target_link_libraries(scotchmetisv${version} PRIVATE scotch)

  add_dependencies(scotchmetisv${version} scotch_h)

  set_target_properties(scotchmetisv${version} PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/metis.h")

  # libScotchMeTiS targets install
  install(EXPORT scotchmetisTargets
    FILE scotchmetisTargets.cmake
    NAMESPACE SCOTCH::
    DESTINATION lib/cmake/scotch)

  install(TARGETS scotchmetisv${version}
    EXPORT scotchmetisTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
endforeach(version 3 5)

#########################
#  libPTScotchParMeTiS  #
#########################

if (PTSCOTCH)
  set_source_files_properties(${CMAKE_BINARY_DIR}/libscotch/ptscotch.h PROPERTIES GENERATED 1)

  # parmetis.h
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parmetis.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/library_parmetis.h ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND $<TARGET_FILE:dummysizes> library_parmetis.h parmetis.h DEPENDS dummysizes
    DEPENDS library_parmetis.h)

  add_custom_target(parmetis_h
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/parmetis.h")

  # libPTScotchParMeTiS
  add_library(ptscotchparmetis
    parmetis_dgraph_order.c
    parmetis_dgraph_order_f.c
    parmetis_dgraph_part.c
    parmetis_dgraph_part_f.c
    ${CMAKE_SOURCE_DIR}/libscotch/module.h
    ${CMAKE_SOURCE_DIR}/libscotch/common.h
    ${CMAKE_BINARY_DIR}/libscotch/ptscotch.h
    parmetis.h)

  target_link_libraries(ptscotchparmetis PUBLIC MPI::MPI_C)

  target_compile_definitions(ptscotchparmetis PUBLIC SCOTCH_PTSCOTCH)

  target_include_directories(ptscotchparmetis PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libscotch>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libscotch>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>)

  target_link_libraries(ptscotchparmetis PRIVATE scotch)

  add_dependencies(ptscotchparmetis ptscotch_h)
  set_target_properties(ptscotchparmetis PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/parmetis.h")

  # libPTScotchParMeTiS targets install
  install(EXPORT ptscotchparmetisTargets
    FILE ptscotchparmetisTargets.cmake
    NAMESPACE SCOTCH::
    DESTINATION lib/cmake/scotch)

  install(TARGETS ptscotchparmetis
    EXPORT ptscotchparmetisTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
endif(PTSCOTCH)
