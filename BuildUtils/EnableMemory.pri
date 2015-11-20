#-------------------------------------------------
#  SKIRT -- an advanced radiative transfer code
#  © Astronomical Observatory, Ghent University
#-------------------------------------------------

#-------------------------------------------------------------------------
# This qmake include file enables memory (de)allocation logging for the
# project (which is only invoked when using the "-l <limit>" flag),
# assuming that the BUILDING_MEMORY configuration flag is turned on.
#-------------------------------------------------------------------------

include(BuildOptions.pri)

BUILDING_MEMORY {
    message (compiling $$TARGET with memory (de)allocation diagnostic output enabled)
    QMAKE_CXXFLAGS += -DBUILDING_MEMORY
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS
    QMAKE_CXXFLAGS_DEBUG = $$QMAKE_CXXFLAGS
}
