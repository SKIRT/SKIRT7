#!/bin/bash
# (use "chmod +rx scriptname" to make script executable)
#
# Execute this script with "git" as default directory
#

#####################################################
### Search for the most recent version of Qt first ##
#####################################################

if [ -f $HOME/Qt5.3.2/5.3/clang_64/bin/qhelpgenerator ]
then
  # generate the html documentation in a temporary folder next to the git folder
  /Applications/Doxygen.app/Contents/Resources/doxygen doc/html.doxygen
  # move Qt compressed help file
  mkdir -p ../doc
  mv -f ../html/skirt.qch ../doc/SKIRT.qch

#############################################################################################
## Search for the older Qt version that is also supported (and a lot of SKIRT users have)  ##
#############################################################################################

elif [ -f $HOME/Qt5.2.1/5.2.1/clang_64/bin/qhelpgenerator ]
then
  # generate the html documentation in a temporary folder next to the git folder
  /Applications/Doxygen.app/Contents/Resources/doxygen doc/html_qt521.doxygen
  # move Qt compressed help file
  mkdir -p ../doc
  mv -f ../html/skirt.qch ../doc/SKIRT.qch

else
  echo "Error: could not find the Qt help file generator. If Qt is installed in a custom location, change this file accordingly."	
fi
