# (C) Copyright 2017-2022 UCAR
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

## cmake functions

#link_files(src_dir dest_dir files..) -Link selected files from source dir to build dir
# Args:
#  src_dir: Full path to source directory
#  dest_dir: Full path to target directory
#  files: A list of file names to link relative to this source dir
macro(link_files src_dir dest_dir)
    foreach(_f IN ITEMS ${ARGN})
        execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${src_dir}/${_f} ${dest_dir}/${_f})
    endforeach()
endmacro()


# Use external jedi-cmake or bundle build
if(DEFINED ENV{jedi_cmake_ROOT})
  include( $ENV{jedi_cmake_ROOT}/share/jedicmake/Functions/git_functions.cmake )
else()
  include( ${CMAKE_SOURCE_DIR}/jedicmake/cmake/Functions/git_functions.cmake )
endif()


# Create the Data/ directory
# --------------------------
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Data)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Data/background)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Data/observation)

# config files (to be filled later)
# ----------------
#list( APPEND config_test_files
#)

# Test input scripts
# ------------------
list( APPEND genint_testinput
testinput/geometry_regular_lonlat.yaml
testinput/geometry_lambertCC.yaml
testinput/convertstate_regular_lonlat.yaml
testinput/convertstate_nonuni_lonlat.yaml
testinput/convertstate_lambertCC.yaml
testinput/convertstate_varcha_regular_lonlat.yaml
testinput/convertstate_varcha_nonuni_lonlat.yaml
testinput/hofx3d_regular_lonlat.yaml
testinput/hofx3d_nonuni_lonlat.yaml
testinput/hofx3d_lambertCC.yaml
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testinput)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testoutput)
#LINK_FILES( "${genint_testinput}" ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
link_files(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${genint_testinput})

# Test background data
# --------------------
list( APPEND genint_testbkg
Data/background/geos_cf_N10_subset.20210801_0000z.nc
Data/background/camchem_N30x20_subset.nc
Data/background/wrfout_d01_subset_20230519_000000.nc
)

link_files(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${genint_testbkg})

# Test observation data
# ---------------------
list( APPEND genint_testobs
Data/observation/tropomi_no2_tropo_2020090318_m.nc4
Data/observation/tropomi_no2_tropo_2020090318_m_singleobs.nc4
)
link_files(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${genint_testobs})


# Create tests
# ------------

# Executables
# add test specific execs later

# Interface tests
# ---------------
ecbuild_add_test( TARGET   genint_test_geometry_regular_lonlat
                  MPI      6
                  ARGS     testinput/geometry_regular_lonlat.yaml
                  COMMAND  genint_geometry.x )

ecbuild_add_test( TARGET   genint_test_geometry_lambertCC
                  MPI      6
                  ARGS     testinput/geometry_lambertCC.yaml
                  COMMAND  genint_geometry.x )

ecbuild_add_test( TARGET   genint_test_convertstate_regular_lonlat
                  MPI      6
                  ARGS     testinput/convertstate_regular_lonlat.yaml
                  COMMAND  genint_convertstate.x )

ecbuild_add_test( TARGET   genint_test_convertstate_nonuni_lonlat
                  MPI      6
                  ARGS     testinput/convertstate_nonuni_lonlat.yaml
                  COMMAND  genint_convertstate.x )

ecbuild_add_test( TARGET   genint_test_convertstate_varcha_regular_lonlat
                  MPI      6
                  ARGS     testinput/convertstate_varcha_regular_lonlat.yaml
                  COMMAND  genint_convertstate.x )

ecbuild_add_test( TARGET   genint_test_convertstate_varcha_nonuni_lonlat
                  MPI      6
                  ARGS     testinput/convertstate_varcha_nonuni_lonlat.yaml
                  COMMAND  genint_convertstate.x )

ecbuild_add_test( TARGET   genint_test_hofx3d_regular_lonlat
                  MPI      6
                  ARGS     testinput/hofx3d_regular_lonlat.yaml
                  COMMAND  genint_hofx3d.x )

ecbuild_add_test( TARGET   genint_test_hofx3d_nonuni_lonlat
                  MPI      6
                  ARGS     testinput/hofx3d_nonuni_lonlat.yaml
                  COMMAND  genint_hofx3d.x )

ecbuild_add_test( TARGET   genint_test_hofx3d_lambertCC
                  MPI      6
                  ARGS     testinput/hofx3d_lambertCC.yaml
                  COMMAND  genint_hofx3d.x )
