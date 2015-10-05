#!/bin/bash
# (use "chmod +rx scriptname" to make script executable)
#
# For use on any Unix system
#
# Execute this script with "git" as default directory to
# build a release version of skirt in the "release" directory
# using your current local copy of the code
#
# By default the build uses five parallel threads; you can
# specify another number as the first command line argument
#

# --------------------------------------------------------------------

# Search for qmake in the home directory
PATHLIST="$(find $HOME/Qt* -name qmake -type f | tr '\n' ' ')"

# Search for qmake in the $PATH
PATHLIST="$(which qmake) $PATHLIST"

# Set the QMAKEPATH to an empty string initially
QMAKEPATH=""

# This function returns the Qt version associated with a certain qmake executable
function qt_version {
    
    local VERSION_OUTPUT="$($1 -v | tr '\n' ' ')"
    local VERSION_OUTPUT="$(tr -d ' ' <<< $VERSION_OUTPUT)"
    local SPLITTED="$(sed s/'Qtversion'/' '/g <<< $VERSION_OUTPUT)"
    local LIST=($SPLITTED)
    local SECOND_PART=${LIST[1]}
    local SPLITTED="$(sed s/'in'/' '/g <<< $SECOND_PART)"
    local LIST=($SPLITTED)
    local VERSION=${LIST[0]}
    echo $VERSION
}

# Loop over all the qmake paths
for path in $PATHLIST; do
    
    # Get the associated Qt version
    VERSION="$(qt_version $path)"
    
    # Check whether the Qt version is supported
    if [[ $VERSION > '5.2.0' ]]
    then
        
        # If another supported qmake was found, check whether this qmake corresponds to
        # a more recent Qt installation
        if [[ ! "$QMAKEPATH" == "" ]]
        then
          CURRENT_VERSION="$(qt_version $path)"
          if [[ $VERSION > $CURRENT_VERSION ]]
          then
              QMAKEPATH=$path
          fi
          
        # Else, just use this qmake
        else
          QMAKEPATH=$path  
        fi
    fi 

done

# Exit with an error if we don't find qmake
if [ "$QMAKEPATH" == "" ]
then
echo error: There was no supported version of qmake found on this system
exit
else
echo Using qmake in $QMAKEPATH
fi

# Create the make file and perform the build
$QMAKEPATH BuildSKIRT.pro -o ../release/Makefile CONFIG+=release
make -j ${1:-5} -w -C ../release
