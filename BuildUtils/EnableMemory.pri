#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#-------------------------------------------------------------------------
# This qmake include file enables the MPI compiler/linker for the project,
# assuming that the BUILDING_WITH_MPI configuration flag is turned on,
# and that the MPI compiler/linker can be located on the host system.
#-------------------------------------------------------------------------

include(BuildOptions.pri)

# Add BUILDING_MEMORY flag
BUILDING_MEMORY {
    message (compiling with BUILDING_MEMORY flag for the SKIRT memory application)
    QMAKE_CXXFLAGS += -DBUILDING_MEMORY
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS
    QMAKE_CXXFLAGS_DEBUG = $$QMAKE_CXXFLAGS
}
