#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This "subdirs" project builds all library and application projects
# needed for SKIRT, FitSKIRT and DoxStyle (documentation streamliner)
#---------------------------------------------------------------------

TEMPLATE = subdirs

# add subproject subdirectories in arbitrary (preferably alphabetical) order
SUBDIRS += \
    Cfitsio \
    Discover \
    DoxStyle \
    FFTConvolution \
    FitSKIRTcore \
    FitSKIRTmain \
    Fundamentals \
    GAlib \
    MPIsupport \
    SKIRTcore \
    SKIRTmain \
    Voro

# conditionally add GUI subproject subdirectories
include(BuildUtils/BuildOptions.pri)
BUILDING_GUI:SUBDIRS += SkirtMakeUp

# define dependencies between subprojects
FFTConvolution.depends = Fundamentals
SKIRTcore.depends      = Cfitsio Voro Fundamentals MPIsupport
Discover.depends       = Cfitsio Voro Fundamentals MPIsupport SKIRTcore
SKIRTmain.depends      = Cfitsio Voro Fundamentals MPIsupport SKIRTcore Discover
FitSKIRTcore.depends   = FFTConvolution GAlib Cfitsio Voro Fundamentals MPIsupport SKIRTcore Discover
FitSKIRTmain.depends   = FFTConvolution GAlib Cfitsio Voro Fundamentals MPIsupport SKIRTcore Discover FitSKIRTcore
BUILDING_GUI:SkirtMakeUp.depends = FFTConvolution GAlib Cfitsio Voro Fundamentals MPIsupport SKIRTcore Discover FitSKIRTcore
