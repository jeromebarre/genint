# (C) Copyright 2017-2019 UCAR
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

# VADER libraries
if( vader_FOUND )
    set( vader_LIBRARIES "vader" )
else()
    set( vader_LIBRARIES "" )
endif()

# Executables

ecbuild_add_executable( TARGET  genint_convertstate.x
                        SOURCES genintConvertState.cc
                        LIBS    genint )

ecbuild_add_executable( TARGET  genint_hofx3d.x
                        SOURCES genintHofX3D.cc
                        LIBS    genint
                                ufo )
