# (C) Copyright 2017-2020 UCAR.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

################################################################################
# Quobs
################################################################################

cmake_minimum_required( VERSION 3.12 )

project( genint VERSION 1.5.0 LANGUAGES CXX Fortran )

option(OPENMP "Build genint with OpenMP support" ON)

## Ecbuild integration
find_package( ecbuild QUIET )
include( ecbuild_system NO_POLICY_SCOPE )
ecbuild_declare_project()
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
set( CMAKE_DIRECTORY_LABELS ${PROJECT_NAME} )

## Configuration options
option( ENABLE_GENINT_INSTRUMENTATION "Enable GENINT instrumentation" OFF )
option( ENABLE_OFFLINE_CODECOV "Enable GENINT offline code coverage" OFF )
option( ENABLE_GENINT_DOC "Enable GENINT documentation" OFF )
option( ENABLE_MKL "Use MKL for LAPACK implementation (if available)" ON )
include( ${PROJECT_NAME}_compiler_flags )

## Dependencies
# Required
find_package( jedicmake REQUIRED )  # Prefer find modules from jedi-cmake
if(OPENMP)
  find_package( OpenMP REQUIRED COMPONENTS Fortran )
endif()
find_package( MPI REQUIRED COMPONENTS Fortran )
find_package( NetCDF REQUIRED COMPONENTS C Fortran )
find_package( eckit 1.11.6 REQUIRED COMPONENTS MPI )
find_package( fckit 0.7.0  REQUIRED )
find_package( atlas 0.30.0 REQUIRED COMPONENTS OMP_Fortran )

# oops
find_package( oops 1.3.0 REQUIRED )

# saber
find_package( saber 1.3.0 REQUIRED )

# crtm
find_package( crtm 2.2.3 REQUIRED )

# ufo
find_package( ufo 1.3.0 REQUIRED )

if( ENABLE_MKL )
    find_package( MKL )
endif()
if( MKL_FOUND )
    set( LAPACK_LIBRARIES ${MKL_LIBRARIES} )
else()
    find_package( LAPACK REQUIRED )
endif()

find_package( gsibec 1.0.6 QUIET )
find_package( vader QUIET )


## SABER instrumentation
if( ENABLE_GENINT_INSTRUMENTATION )
    message( STATUS "GENINT instrumentation is enabled" )
    add_definitions(-DENABLE_GENINT_INSTRUMENTATION=1)
else()
    message( STATUS "GENINT instrumentation is not enabled" )
    add_definitions(-DENABLE_GENINT_INSTRUMENTATION=0)
endif()


## Code coverage
if ( ENABLE_OFFLINE_CODECOV )
    message( STATUS "Offline code coverage is enabled" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set( CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fprofile-arcs -ftest-coverage")
    set( CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fprofile-arcs -ftest-coverage")
    set( CMAKE_Fortran_LINK_FLAGS "${CMAKE_Fortran_LINK_FLAGS} -fprofile-arcs -ftest-coverage")
else()
    message( STATUS "Offline code coverage is not enabled" )
endif()

## Sources
add_subdirectory( src )
add_subdirectory( mains )
add_subdirectory( test )

## Global tests
ecbuild_add_test( TARGET genint_coding_norms_src
                  TYPE SCRIPT
                  COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}_cpplint.py
                  ARGS --quiet --recursive ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/src )

ecbuild_add_test( TARGET genint_coding_norms_test
                  TYPE SCRIPT
                  COMMAND ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}_cpplint.py
                  ARGS --quiet --recursive ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/test )

# Setup GENINT data links
message( STATUS "Setup GENINT data links" )
file( REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/data )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data )
execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
                 ${CMAKE_CURRENT_SOURCE_DIR}/data/landsea.nc
                 ${CMAKE_CURRENT_BINARY_DIR}/data/landsea.nc )

 # Link scripts
 list( APPEND test_files
     genint_cpplint.py
 )

 file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/saber_plot )
 foreach( FILENAME ${test_files} )
     execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
                              ${PROJECT_SOURCE_DIR}/tools/${FILENAME}
                              ${CMAKE_BINARY_DIR}/bin/${FILENAME} )
 endforeach()

## Package Config
ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
