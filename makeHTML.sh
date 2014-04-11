#!/bin/bash
# (use "chmod +rx scriptname" to make script executable)
#
# Execute this script with "git" as default directory
#

# generate the html documentation in a temporary folder next to the git folder
/Applications/Doxygen.app/Contents/Resources/doxygen doc/html.doxygen

# move Qt compressed help file
mkdir -p ../doc
mv -f ../html/skirt.qch ../doc/SKIRT.qch
