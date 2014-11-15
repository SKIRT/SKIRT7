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
macx:CONFIG *= BUILDING_GUI

# to build without MPI support even on systems that offer MPI, precede this line with a # comment character
CONFIG *= BUILDING_WITH_MPI

#-------------------------------------------------------------------------
# Do not change the lines below unless you're an expert...
#-------------------------------------------------------------------------

# try to locate the MPI compiler (invoke 'which' via bash in login script mode to honor PATHS on Mac OS X)
# if we don't find it, disable MPI support even if it was requested
BUILDING_WITH_MPI {
    isEmpty(MPI_COMPILER) {
        MPI_COMPILER = $$system(bash -lc "'which mpiicpc'")
    }
    isEmpty(MPI_COMPILER) {
        MPI_COMPILER = $$system(bash -lc "'which mpicxx'")
    }
    isEmpty(MPI_COMPILER) {
        CONFIG -= BUILDING_WITH_MPI
    }
}

# inform the user
BUILDING_WITH_MPI {
    message (using MPI compiler $$MPI_COMPILER)
}
