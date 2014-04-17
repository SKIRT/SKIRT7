#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This console application can be invoked from the Qt Creator code
# editor to help streamline Doxygen documentation blocks.
# It is independent from SKIRT and FitSKIRT.
#---------------------------------------------------------------------

# overall setup
TEMPLATE = app
TARGET   = doxstyle
QT      -= gui
CONFIG  -= app_bundle
CONFIG  *= link_prl thread console c++11

# compile C++ with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    Block.hpp \
    Chunk.hpp \
    DoxStyleMain.hpp

SOURCES += \
    Block.cpp \
    Chunk.cpp \
    DoxStyleMain.cpp
