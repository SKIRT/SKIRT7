#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = fundamentals
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

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
    Vec.hpp

SOURCES += \
    CommandLineArguments.cpp
