#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This library provides the core FitSKIRT functionality, connecting
# the genetic algorithm mechanism with the SKIRT capabilities.
#---------------------------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = fitskirtcore
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# include libraries internal to the project
INCLUDEPATH += $$PWD/../Fundamentals $$PWD/../GAlib $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../MPIsupport $$PWD/../FFTConvolution
DEPENDPATH += $$PWD/../Fundamentals $$PWD/../GAlib $$PWD/../Discover $$PWD/../SKIRTcore $$PWD/../MPIsupport $$PWD/../FFTConvolution
unix: LIBS += -L$$OUT_PWD/../Fundamentals/ -lfundamentals \
              -L$$OUT_PWD/../GAlib/ -lgalib \
              -L$$OUT_PWD/../Discover/ -ldiscover \
              -L$$OUT_PWD/../SKIRTcore/ -lskirtcore \
              -L$$OUT_PWD/../MPIsupport/ -lmpisupport \
              -L$$OUT_PWD/../FFTConvolution/ -lfftconvolution
unix: PRE_TARGETDEPS += $$OUT_PWD/../Fundamentals/libfundamentals.a \
                        $$OUT_PWD/../GAlib/libgalib.a \
                        $$OUT_PWD/../Discover/libdiscover.a \
                        $$OUT_PWD/../SKIRTcore/libskirtcore.a \
                        $$OUT_PWD/../MPIsupport/libmpisupport.a \
                        $$OUT_PWD/../FFTConvolution/libfftconvolution.a

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    AdjustableSkirtSimulation.hpp \
    FitScheme.hpp \
    LumSimplex.hpp \
    OligoFitScheme.hpp \
    Optimization.hpp \
    ParameterRange.hpp \
    ParameterRanges.hpp \
    ReferenceImage.hpp \
    ReferenceImages.hpp \
    RegisterFitSchemeItems.hpp \
    GoldenSection.hpp \
    GALumfit.hpp \
    ConvolutionKernel.hpp \
    GaussianKernel.hpp \
    FitsKernel.hpp \
    Convolution.hpp

SOURCES += \
    AdjustableSkirtSimulation.cpp \
    FitScheme.cpp \
    LumSimplex.cpp \
    OligoFitScheme.cpp \
    Optimization.cpp \
    ParameterRange.cpp \
    ParameterRanges.cpp \
    ReferenceImage.cpp \
    ReferenceImages.cpp \
    RegisterFitSchemeItems.cpp \
    GoldenSection.cpp \
    GALumfit.cpp \
    ConvolutionKernel.cpp \
    GaussianKernel.cpp \
    FitsKernel.cpp \
    Convolution.cpp
