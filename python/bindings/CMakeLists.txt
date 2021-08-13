# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT ofdmradar_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT ofdmradar_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND ofdmradar_python_files
    ofdmradar_python.cc
    ofdmradar_tx_python.cc
    ofdmradar_rx_python.cc
    ofdmradar_gui_python.cc
    array_corr_python.cc
    array_music_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(ofdmradar 
   ../..
   gr::ofdmradar
   "${ofdmradar_python_files}")

install(TARGETS ofdmradar_python DESTINATION ${GR_PYTHON_DIR}/ofdmradar COMPONENT pythonapi)
