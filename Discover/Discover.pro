#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This library offers the discovery functionality that handles
# parameter files for SKIRT and FitSKIRT.
#---------------------------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = discover
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# include libraries internal to the project
INCLUDEPATH += $$PWD/../Fundamentals $$PWD/../SKIRTcore
DEPENDPATH += $$PWD/../Fundamentals $$PWD/../SKIRTcore
unix: LIBS += -L$$OUT_PWD/../Fundamentals/ -lfundamentals -L$$OUT_PWD/../SKIRTcore/ -lskirtcore
unix: PRE_TARGETDEPS += $$OUT_PWD/../Fundamentals/libfundamentals.a $$OUT_PWD/../SKIRTcore/libskirtcore.a

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    BoolPropertyHandler.hpp \
    ConsoleHierarchyCreator.hpp \
    DoubleListPropertyHandler.hpp \
    DoublePropertyHandler.hpp \
    EnumPropertyHandler.hpp \
    IntPropertyHandler.hpp \
    ItemListPropertyHandler.hpp \
    ItemPropertyHandler.hpp \
    LatexHierarchyWriter.hpp \
    PropertyHandler.hpp \
    PropertyHandlerVisitor.hpp \
    RegisterSimulationItems.hpp \
    SimulationItemDiscovery.hpp \
    SimulationItemRegistry.hpp \
    StringPropertyHandler.hpp \
    XmlHierarchyCreator.hpp \
    XmlHierarchyWriter.hpp

SOURCES += \
    BoolPropertyHandler.cpp \
    ConsoleHierarchyCreator.cpp \
    DoubleListPropertyHandler.cpp \
    DoublePropertyHandler.cpp \
    EnumPropertyHandler.cpp \
    IntPropertyHandler.cpp \
    ItemListPropertyHandler.cpp \
    ItemPropertyHandler.cpp \
    LatexHierarchyWriter.cpp \
    PropertyHandler.cpp \
    PropertyHandlerVisitor.cpp \
    RegisterSimulationItems.cpp \
    SimulationItemDiscovery.cpp \
    SimulationItemRegistry.cpp \
    SmileSchemaWriter.cpp \
    StringPropertyHandler.cpp \
    XmlHierarchyCreator.cpp \
    XmlHierarchyWriter.cpp
