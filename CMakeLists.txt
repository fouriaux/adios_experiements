cmake_minimum_required(VERSION 2.8)
project (adios_exp)
include (BlueGenePortability.cmake)
### configure packages variables
set(ADIOS_ROOT $ENV{ADIOS_INSTALL} CACHE PATH "path to ADIOS library")
list(APPEND CMAKE_MODULE_PATH ${ADIOS_ROOT}/etc)
message (STATUS "CMAKE_MOUDLE_PATH=" ${CMAKE_MODULE_PATH})
find_package(ADIOS REQUIRED)
find_package(MPI REQUIRED)
  
### Compilation options
set(ADIOS_PROGS adios_buffer_size)
foreach(PROG ${ADIOS_PROGS})
  include_directories(${MPI_INCLUDE_PATH} ${ADIOS_INCLUDE_DIRS})
  add_executable(${PROG} ${PROG}/${PROG}.cpp ./common/time.cpp)
  set_target_properties(${PROG} PROPERTIES COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")
  target_link_libraries(${PROG} ${MPI_CXX_LIBRARIES} ${ADIOS_LIBRARIES})
endforeach()

