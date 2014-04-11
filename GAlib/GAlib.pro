#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = galib
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    GA1DArrayGenome.h \
    GA1DBinStrGenome.h \
    GA2DArrayGenome.h \
    GA2DBinStrGenome.h \
    GA3DArrayGenome.h \
    GA3DBinStrGenome.h \
    GAAllele.h \
    GAArray.h \
    GABaseGA.h \
    GABin2DecGenome.h \
    GABinStr.h \
    GADCrowdingGA.h \
    GADemeGA.h \
    GAEvalData.h \
    GAGenome.h \
    GAIncGA.h \
    GAList.h \
    GAListBASE.h \
    GAListGenome.h \
    GAMask.h \
    GANode.h \
    GAParameter.h \
    GAPopulation.h \
    GARealGenome.h \
    GASStateGA.h \
    GAScaling.h \
    GASelector.h \
    GASimpleGA.h \
    GAStatistics.h \
    GAStringGenome.h \
    GATree.h \
    GATreeBASE.h \
    GATreeGenome.h \
    gabincvt.h \
    gaconfig.h \
    gaerror.h \
    gaid.h \
    garandom.h \
    gatypes.h \
    gaversion.h \
    std_stream.h

SOURCES += \
    GA1DBinStrGenome.cpp \
    GA2DBinStrGenome.cpp \
    GA3DBinStrGenome.cpp \
    GABaseGA.cpp \
    GABin2DecGenome.cpp \
    GABinStr.cpp \
    GADCrowdingGA.cpp \
    GADemeGA.cpp \
    GAGenome.cpp \
    GAIncGA.cpp \
    GAListBASE.cpp \
    GAParameter.cpp \
    GAPopulation.cpp \
    GASStateGA.cpp \
    GAScaling.cpp \
    GASelector.cpp \
    GASimpleGA.cpp \
    GAStatistics.cpp \
    GATreeBASE.cpp \
    gabincvt.cpp \
    gaerror.cpp \
    garandom.cpp
