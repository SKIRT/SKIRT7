#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This library offers generic functionality that is essentially
# unrelated to SKIRT and FitSKIRT. Most items are fully defined
# in their respective header files to optimize performance.
#---------------------------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = fundamentals
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# Enable memory (de)allocation compilation if required
include(../BuildUtils/EnableMemory.pri)

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    Array.hpp \
    ArrayTable.hpp \
    Box.hpp \
    CommandLineArguments.hpp \
    NR.hpp \
    Table.hpp \
    Vec.hpp \
    LockFree.hpp \
    MemoryStatistics.hpp \
    MemoryLogger.hpp

SOURCES += \
    CommandLineArguments.cpp \
    MemoryStatistics.cpp \
    Array.cpp
