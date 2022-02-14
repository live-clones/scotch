if(MPI_FOUND)
  # Save current values
  set(CMAKE_REQUIRED_INCLUDES_TMP ${CMAKE_REQUIRED_INCLUDES})
  set(CMAKE_REQUIRED_LIBRARIES_TMP ${CMAKE_REQUIRED_LIBRARIES})
  set(CMAKE_REQUIRED_FLAGS_TMP ${CMAKE_REQUIRED_FLAGS})
  # Set flags for building test program
  set(CMAKE_REQUIRED_INCLUDES ${MPI_INCLUDE_PATH})
  set(CMAKE_REQUIRED_LIBRARIES ${MPI_LIBRARIES})
  set(CMAKE_REQUIRED_FLAGS ${MPI_COMPILE_FLAGS})

  include(CheckCSourceRuns)
  set(CODE_MPI_THREAD_MULTIPLE "
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char **argv) {
  int prov;
  MPI_Init_thread (&argc, &argv, MPI_THREAD_MULTIPLE, &prov);
  MPI_Finalize ();
  return ((prov < MPI_THREAD_MULTIPLE) ? EXIT_FAILURE : EXIT_SUCCESS);
}
")
  check_c_source_runs("${CODE_MPI_THREAD_MULTIPLE}" MPI_THREAD_MULTIPLE_OK)
  # Restore previous values
  set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_TMP})
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_TMP})
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_TMP})
endif()
