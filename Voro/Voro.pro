#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This library provides support for building Voronoi meshes.
# The source code is adapted from the Voro++ library written by
# Chris H. Rycroft (LBL / UC Berkeley)
# which can be downloaded at http://math.lbl.gov/voro++/about.html
#---------------------------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = voro
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    c_loops.hh \
    cell.hh \
    common.hh \
    config.hh \
    container.hh \
    container_prd.hh \
    rad_option.hh \
    unitcell.hh \
    v_base.hh \
    v_compute.hh \
    wall.hh \
    worklist.hh

SOURCES += \
    c_loops.cc \
    cell.cc \
    common.cc \
    container.cc \
    container_prd.cc \
    unitcell.cc \
    v_base.cc \
    v_base_wl.cc \
    v_compute.cc \
    wall.cc
