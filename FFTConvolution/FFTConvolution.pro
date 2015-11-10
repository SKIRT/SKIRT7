#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This library encapsulates the FFTW library for Fast Fourier
# Transforms, specifically providing an interface for convolving an
# array of data with a given kernel. The code in this library is
# inspired by a package called FFTConvolution developed by Jeremy Fix
# (see https://github.com/jeremyfix/FFTConvolution).
#---------------------------------------------------------------------

# overall setup
TEMPLATE = lib
TARGET = fftconvolution
QT -= gui
CONFIG *= staticlib create_prl thread c++11

# compile with maximum optimization
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# include libraries internal to the project
INCLUDEPATH += $$PWD/../Fundamentals
DEPENDPATH += $$PWD/../Fundamentals
unix: LIBS += -L$$OUT_PWD/../Fundamentals/ -lfundamentals
unix: PRE_TARGETDEPS += $$OUT_PWD/../Fundamentals/libfundamentals.a

# include the necessary paths for FFTW3 if present in the usr/local/include directory
exists ('/usr/local/include/fftw3.h'){
    message (using FFTW3 library for $$TARGET project)
    QMAKE_CXXFLAGS += -DUSING_FFTW3
    CONFIG += USING_FFTW3
    LIBS += -L/usr/local/lib -lfftw3
    INCLUDEPATH += /usr/local/include
}

# include the necessary paths for FFTW3 if present in the home directory
exists ('$$(HOME)/FFTW/include/fftw3.h'){
    message (using FFTW3 library for $$TARGET project)
    QMAKE_CXXFLAGS += -DUSING_FFTW3
    CONFIG += USING_FFTW3
    LIBS += -L$$(HOME)/FFTW/lib -lfftw3
    INCLUDEPATH += $$(HOME)/FFTW/include
}

# include the MPI library if we are building the SkirtMemory application
# and set the appropriate compiler flag for enabling memory diagnostic output
BUILDING_MEMORY
{
    # Include libraries internal to the project
    INCLUDEPATH += $$PWD/../MPIsupport
    DEPENDPATH += $$PWD/../MPIsupport
    unix: LIBS += -L$$OUT_PWD/../MPIsupport/ -lmpisupport
    unix: PRE_TARGETDEPS += $$OUT_PWD/../MPIsupport/libmpisupport.a

    # Enable 'Memory-enabled' compilation of this SKIRT subproject
    include(../BuildUtils/EnableMemory.pri)
}

#--------------------------------------------------
# source and header files: maintained by Qt creator
#--------------------------------------------------

HEADERS += \
    FftConvolution.hpp \
    WorkSpace.hpp \
    Factorize.hpp

SOURCES += \
    FftConvolution.cpp \
    WorkSpace.cpp \
    Factorize.cpp
