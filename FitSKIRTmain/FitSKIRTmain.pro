#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

# overall setup
TEMPLATE = app
TARGET   = fitskirt
QT      -= gui
CONFIG  -= app_bundle
CONFIG  *= link_prl thread console c++11

# compile C++ with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# include libraries internal to the project
INCLUDEPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../FitSKIRTcore $$PWD/../MPIsupport
DEPENDPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../FitSKIRTcore $$PWD/../MPIsupport
unix: LIBS += -L$$OUT_PWD/../Fundamentals/ -lfundamentals \
              -L$$OUT_PWD/../Discover/ -ldiscover \
              -L$$OUT_PWD/../SKIRTcore/ -lskirtcore \
              -L$$OUT_PWD/../FitSKIRTcore/ -lfitskirtcore \
              -L$$OUT_PWD/../MPIsupport/ -lmpisupport
unix: PRE_TARGETDEPS += $$OUT_PWD/../Fundamentals/libfundamentals.a \
                        $$OUT_PWD/../Discover/libdiscover.a \
                        $$OUT_PWD/../SKIRTcore/libskirtcore.a \
                        $$OUT_PWD/../FitSKIRTcore/libfitskirtcore.a \
                        $$OUT_PWD/../MPIsupport/libmpisupport.a

# use MPI linker if available (invoke 'which' via bash in login script mode to honor PATHS on Mac OS X)
MPI_COMPILER = $$system(bash -lc "'which mpiicpc'")
isEmpty(MPI_COMPILER) {
    MPI_COMPILER = $$system(bash -lc "'which mpicxx'")
}
!isEmpty(MPI_COMPILER) {
    message (using MPI linker $$MPI_COMPILER)
    QMAKE_LINK = $$MPI_COMPILER
}

# touch FitSkirtMain.cpp so it always gets recompiled to update the version number and time stamp
build_version.commands = touch ../../git/FitSKIRTmain/FitSkirtMain.cpp
QMAKE_EXTRA_TARGETS += build_version
PRE_TARGETDEPS += build_version

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    FitSkirtMain.hpp \
    FitSkirtCommandLineHandler.hpp

SOURCES += \
    FitSkirtMain.cpp \
    FitSkirtCommandLineHandler.cpp
