#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This library encapsulates any and all MPI-related functionality used
# by SKIRT and FitSKIRT. Concentrating all MPI calls in this library
# allows all other code to be compiled without the MPI extra's.
#---------------------------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = mpisupport
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# Enable MPI compilation if required
include(../BuildUtils/EnableMPI.pri)

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    ProcessManager.hpp

SOURCES += \
    ProcessManager.cpp
