#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#---------------------------------------------------------------------
# This "subdirs" project builds all library and application projects
# needed for SKIRT, FitSKIRT and DoxStyle (documentation streamliner)
#---------------------------------------------------------------------

TEMPLATE = subdirs

# add subdirectories in order of dependency,
# i.e. a project must be listed after any projects on which it depends
SUBDIRS += \
    DoxStyle \
    Cfitsio \
    GAlib \
    Voro \
    Fundamentals \
    MPIsupport \
    SKIRTcore \
    Discover \
    SKIRTmain \
    FitSKIRTcore \
    FitSKIRTmain
