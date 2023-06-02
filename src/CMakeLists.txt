# (C) Copyright 2022 UCAR.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

list( APPEND genint_src_files
Constants.h
Constants.cc
Covariance.h
Fields.cc
Fields.h
Geometry.cc
Geometry.h
Increment.cc
Increment.h
LinearVariableChange.h
LinearVariableChange.cc
State.cc
State.h
VariableChange.h
VariableChange.cc
)

ecbuild_add_library( TARGET   genint
                     SOURCES  ${genint_src_files}
                     PUBLIC_LIBS  oops saber ufo
                     HEADER_DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
                     INSTALL_HEADERS LISTED
                     LINKER_LANGUAGE CXX )

 target_link_libraries( genint PUBLIC NetCDF::NetCDF_Fortran NetCDF::NetCDF_C )

#Configure include directory layout for build-tree to match install-tree
set(genint_BUILD_DIR_INCLUDE_PATH ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/include)
add_custom_target(genint_headers ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${genint_BUILD_DIR_INCLUDE_PATH}"
                                     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR} ${genint_BUILD_DIR_INCLUDE_PATH}/genint)
add_dependencies(genint genint_headers)
target_include_directories(genint PUBLIC $<BUILD_INTERFACE:${genint_BUILD_DIR_INCLUDE_PATH}>
                                         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
                                         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../..>)
