#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#-------------------------------------------------------------------------
# This qmake include file defines the build options for the SKIRT projects.
# Customize the lines below to configure the appropriate build options.
#-------------------------------------------------------------------------

# to skip building the GUI, precede this line with a # comment character
# to build the GUI on systems other than Mac OS X, remove the "macx:" prefix from this line
CONFIG *= BUILDING_GUI

# to build without MPI support even on systems that offer MPI, precede this line with a # comment character
CONFIG *= BUILDING_WITH_MPI
