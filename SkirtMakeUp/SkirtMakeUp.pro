#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This is the graphical user interface for SKIRT. It allows creating
# and editing ski files through a wizard-like GUI.
#---------------------------------------------------------------------

# overall setup
TEMPLATE = app
TARGET   = SkirtMakeUp
QT      *= gui widgets
CONFIG  *= link_prl thread c++11

# include application icon
macx {
  ICON = Mirror.icns
}

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
# touch SkirtMakeUp.cpp so it always gets recompiled to update the version number and time stamp
A_QUOTE = "\'\"\'"
A_SEMICOLON = "\';\'"
build_version.commands = cd ../../git && \
                         echo const char* git_version = $$A_QUOTE`git rev-list HEAD | wc -l`-`git describe --dirty --always` $$A_QUOTE $$A_SEMICOLON > SkirtMakeUp/git_version.h && \
                         touch SkirtMakeUp/SkirtMakeUp.cpp && \
                         cd $$OUT_PWD
QMAKE_EXTRA_TARGETS += build_version
PRE_TARGETDEPS += build_version

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    SkirtMakeUp.hpp \
    MainWindow.hpp

SOURCES += \
    SkirtMakeUp.cpp \
    MainWindow.cpp
