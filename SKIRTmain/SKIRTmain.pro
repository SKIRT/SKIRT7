#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This is the SKIRT console application. It handles the command
# line arguments and links in everything else from other libraries.
#---------------------------------------------------------------------

# overall setup
TEMPLATE = app
TARGET   = skirt
QT      -= gui
CONFIG  -= app_bundle
CONFIG  *= link_prl thread console c++11

# compile C++ with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# include libraries internal to the project
INCLUDEPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../MPIsupport
DEPENDPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../MPIsupport
unix: LIBS += -L$$OUT_PWD/../Fundamentals/ -lfundamentals \
              -L$$OUT_PWD/../Discover/ -ldiscover \
              -L$$OUT_PWD/../SKIRTcore/ -lskirtcore \
              -L$$OUT_PWD/../MPIsupport/ -lmpisupport
unix: PRE_TARGETDEPS += $$OUT_PWD/../Fundamentals/libfundamentals.a \
                        $$OUT_PWD/../Discover/libdiscover.a \
                        $$OUT_PWD/../SKIRTcore/libskirtcore.a \
                        $$OUT_PWD/../MPIsupport/libmpisupport.a

# use MPI compiler/linker if available
include(../BuildOptions.pri)
!isEmpty(MPI_COMPILER) {
    QMAKE_CXXFLAGS += -DBUILDING_WITH_MPI
    QMAKE_CXX = $$MPI_COMPILER
    QMAKE_LINK = $$MPI_COMPILER
}

# create a header file containing a reasonably unique description of the git version and
# touch SkirtMain.cpp so it always gets recompiled to update the version number and time stamp
A_QUOTE = "\'\"\'"
A_SEMICOLON = "\';\'"
build_version.commands = cd ../../git && \
                         echo const char* git_version = $$A_QUOTE`git rev-list HEAD | wc -l`-`git describe --dirty --always` $$A_QUOTE $$A_SEMICOLON > SKIRTmain/git_version.h && \
                         touch SKIRTmain/SkirtMain.cpp && \
                         cd $$OUT_PWD
QMAKE_EXTRA_TARGETS += build_version
PRE_TARGETDEPS += build_version

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    SkirtMain.hpp \
    SkirtCommandLineHandler.hpp

SOURCES += \
    SkirtMain.cpp \
    SkirtCommandLineHandler.cpp
