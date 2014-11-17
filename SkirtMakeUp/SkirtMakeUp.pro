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
INCLUDEPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../FitSKIRTcore
DEPENDPATH += $$PWD/../Fundamentals $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../FitSKIRTcore
unix: LIBS += -L$$OUT_PWD/../Fundamentals/ -lfundamentals \
              -L$$OUT_PWD/../Discover/ -ldiscover \
              -L$$OUT_PWD/../SKIRTcore/ -lskirtcore \
              -L$$OUT_PWD/../FitSKIRTcore/ -lfitskirtcore
unix: PRE_TARGETDEPS += $$OUT_PWD/../Fundamentals/libfundamentals.a \
                        $$OUT_PWD/../Discover/libdiscover.a \
                        $$OUT_PWD/../SKIRTcore/libskirtcore.a \
                        $$OUT_PWD/../FitSKIRTcore/libfitskirtcore.a

# Enable MPI compilation if required
include(../BuildUtils/EnableMPI.pri)

# create a header file containing a reasonably unique description of the git version and
# ensure that SkirtMakeUp.cpp gets recompiled to update the version number and time stamp
A_QUOTE = "\'\"\'"
A_SEMICOLON = "\';\'"
versionTarget.target = ../../git/SkirtMakeUp/git_version.h
versionTarget.depends = FORCE
versionTarget.commands = cd ../../git ; \
                         echo const char* git_version = $$A_QUOTE`git rev-list HEAD | wc -l`-`git describe --dirty --always` $$A_QUOTE $$A_SEMICOLON > SkirtMakeUp/git_version.h ; \
                         cd $$OUT_PWD
PRE_TARGETDEPS += ../../git/SkirtMakeUp/git_version.h
QMAKE_EXTRA_TARGETS += versionTarget
HEADERS += git_version.h

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    SkirtMakeUp.hpp \
    MainWindow.hpp \
    WizardEngine.hpp \
    BasicChoiceWizardPane.hpp \
    CreateRootWizardPane.hpp \
    PropertyWizardPane.hpp \
    ItemPropertyWizardPane.hpp \
    DoublePropertyWizardPane.hpp \
    IntPropertyWizardPane.hpp \
    EnumPropertyWizardPane.hpp \
    ItemListPropertyWizardPane.hpp \
    DoubleListPropertyWizardPane.hpp \
    BoolPropertyWizardPane.hpp \
    StringPropertyWizardPane.hpp \
    SubItemPropertyWizardPane.hpp \
    SaveWizardPane.hpp \
    OpenWizardPane.hpp \
    SimulationItemTools.hpp

SOURCES += \
    SkirtMakeUp.cpp \
    MainWindow.cpp \
    WizardEngine.cpp \
    BasicChoiceWizardPane.cpp \
    CreateRootWizardPane.cpp \
    PropertyWizardPane.cpp \
    ItemPropertyWizardPane.cpp \
    DoublePropertyWizardPane.cpp \
    IntPropertyWizardPane.cpp \
    EnumPropertyWizardPane.cpp \
    ItemListPropertyWizardPane.cpp \
    DoubleListPropertyWizardPane.cpp \
    BoolPropertyWizardPane.cpp \
    StringPropertyWizardPane.cpp \
    SubItemPropertyWizardPane.cpp \
    SaveWizardPane.cpp \
    OpenWizardPane.cpp \
    SimulationItemTools.cpp
