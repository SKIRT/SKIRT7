#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#-------------------------------------------------------------------------
# Do not change the lines below unless you're an expert...
#-------------------------------------------------------------------------

include(BuildOptions.pri)

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

# use MPI compiler/linker if available
!isEmpty(MPI_COMPILER) {
    QMAKE_CXXFLAGS += -DBUILDING_WITH_MPI
    QMAKE_CXX = $$MPI_COMPILER
    QMAKE_LINK = $$MPI_COMPILER
    QMAKE_LFLAGS += $$system($$MPI_COMPILER --showme:link)
    QMAKE_CXXFLAGS += $$system($$MPI_COMPILER --showme:compile)
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS
}
