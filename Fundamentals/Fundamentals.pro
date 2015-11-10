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

# include the MPI library if we are building the SkirtMemory application
# and set the appropriate compiler flag for enabling memory diagnostic output
BUILDING_MEMORY
{
    # Include libraries internal to the project
    INCLUDEPATH += $$PWD/../MPIsupport
    DEPENDPATH += $$PWD/../MPIsupport
    unix: LIBS += -L$$OUT_PWD/../MPIsupport/ -lmpisupport
    unix: PRE_TARGETDEPS += $$OUT_PWD/../MPIsupport/libmpisupport.a

    # Enable 'Memory-enabled' compilation of this SKIRT subproject
    include(../BuildUtils/EnableMemory.pri)
}

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
    MemoryStatistics.hpp

SOURCES += \
    CommandLineArguments.cpp \
    MemoryStatistics.cpp
