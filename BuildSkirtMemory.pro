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
    FFTConvolution \
    Fundamentals \
    MPIsupport \
    SKIRTcore \
    SkirtMemory \
    Voro

# set the appropriate flag for the other SKIRT subprojects
CONFIG += BUILDING_MEMORY

# define dependencies between subprojects
Fundamentals.depends   = MPIsupport
FFTConvolution.depends = Fundamentals MPIsupport
SKIRTcore.depends      = Cfitsio Voro Fundamentals MPIsupport
Discover.depends       = Cfitsio Voro Fundamentals MPIsupport SKIRTcore
SkirtMemory.depends    = Cfitsio Voro Fundamentals MPIsupport SKIRTcore Discover
