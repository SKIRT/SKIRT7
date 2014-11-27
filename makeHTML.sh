#!/bin/bash
# (use "chmod +rx scriptname" to make script executable)
#
# Execute this script with "git" as default directory
#

##############################
## On the Mac OS X platform ##
##############################

if [ "$(uname)" == "Darwin" ]
then

  #####################################################
  ### Search for the most recent version of Qt first ##
  #####################################################

  if [ -f $HOME/Qt5.3.2/5.3/clang_64/bin/qhelpgenerator ]
  then
    
    # generate html documentation in a temporary folder next to the git folder
    /Applications/Doxygen.app/Contents/Resources/doxygen doc/html.doxygen     
  
    # move Qt compressed help file
    mkdir -p ../doc
    mv -f ../html/skirt.qch ../doc/SKIRT.qch
    
  ############################################################
  ## Search for the older Qt version that is also supported ##
  ############################################################
    
  elif [ -f $HOME/Qt5.2.1/5.2.1/clang_64/bin/qhelpgenerator ]
  then
      
    # generate html documentation in a temporary folder next to the git folder
    /Applications/Doxygen.app/Contents/Resources/doxygen doc/html_qt521.doxygen
    
    # move Qt compressed help file
    mkdir -p ../doc
    mv -f ../html/skirt.qch ../doc/SKIRT.qch
    
  else
    echo "Error: could not find the Qt help file generator. If Qt is installed in a custom location, change this file accordingly."	
  fi
    
##########################
# On the Linux platform ##
##########################

elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]
then
    
  #####################################################
  ### Search for the most recent version of Qt first ##
  #####################################################
    
  if [ -f $HOME/Qt5.3.2/5.3/gcc_64/bin/qhelpgenerator ]
  then
        
    # generate html documentation in a temporary folder next to the git folder
    doxygen doc/html_ubuntu.doxygen

    # move Qt compressed help file
    mkdir -p ../doc
    mv -f ../html/skirt.qch ../doc/SKIRT.qch

  ############################################################
  ## Search for the older Qt version that is also supported ##
  ############################################################

  elif [ -f $HOME/Qt5.2.1/5.2.1/gcc_64/bin/qhelpgenerator ]
  then
  
    # generate html documentation in a temporary folder next to the git folder
    doxygen doc/html_qt521_ubuntu.doxygen

    # move Qt compressed help file
    mkdir -p ../doc
    mv -f ../html/skirt.qch ../doc/SKIRT.qch
  
  else
    echo "Error: could not find the Qt help file generator. If Qt is installed in a custom location, change this file accordingly."	
  fi

else
  echo "Platforms other than Mac OS X or Ubuntu are not supported!"
fi
