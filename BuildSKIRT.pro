#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

TEMPLATE = subdirs

# add subdirectories in order of dependency,
# i.e. a project must be listed after any projects on which it depends
SUBDIRS += \
    DoxStyle \
    Cfitsio \
    GAlib \
    Voro \
    Fundamentals \
    SKIRTcore \
    MPIsupport \
    Discover \
    SKIRTmain \
    FitSKIRTcore \
    FitSKIRTmain
