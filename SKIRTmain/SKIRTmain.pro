#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

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
INCLUDEPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore
DEPENDPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore
unix: LIBS += -L$$OUT_PWD/../Fundamentals/ -lfundamentals \
              -L$$OUT_PWD/../Discover/ -ldiscover \
              -L$$OUT_PWD/../SKIRTcore/ -lskirtcore
unix: PRE_TARGETDEPS += $$OUT_PWD/../Fundamentals/libfundamentals.a \
                        $$OUT_PWD/../Discover/libdiscover.a \
                        $$OUT_PWD/../SKIRTcore/libskirtcore.a

# create a header file containing a reasonably unique description of the git version and
# touch SkirtMain.cpp so it always gets recompiled to update the version number and time stamp
A_QUOTE = "\'\"\'"
A_SEMICOLON = "\';\'"
build_version.commands = echo const char* git_version = $$A_QUOTE `git -C ../../git rev-list HEAD | wc -l`.`git -C ../../git describe --dirty --always` $$A_QUOTE $$A_SEMICOLON > ../../git/SKIRTmain/git_version.h && \
                         touch ../../git/SKIRTmain/SkirtMain.cpp
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
