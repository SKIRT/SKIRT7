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

# List of possible paths of qmake (separate paths with a space)
PATHLIST=( /Applications/Qt/5.3/clang_64/bin/qmake $HOME/Qt5.4.1/5.4/clang_64/bin/qmake $HOME/Qt5.4.1/5.4/gcc_64/bin/qmake $HOME/Qt/Desktop/5.4.1/bin/qmake /usr/local/Qt/5.4.1/bin/qmake
           $HOME/Qt5.4.0/5.4/clang_64/bin/qmake $HOME/Qt5.4.0/5.4/gcc_64/bin/qmake $HOME/Qt/Desktop/5.4.0/bin/qmake /usr/local/Qt/5.4.0/bin/qmake
           $HOME/Qt5.3.2/5.3/clang_64/bin/qmake $HOME/Qt5.3.2/5.3/gcc_64/bin/qmake $HOME/Qt/Desktop/5.3.2/bin/qmake /usr/local/Qt/5.3.2/bin/qmake
           $HOME/Qt5.2.1/5.2.1/clang_64/bin/qmake $HOME/Qt5.2.1/5.2.1/gcc_64/bin/qmake $HOME/Qt/Desktop/5.2.1/bin/qmake /usr/local/Qt/5.2.1/bin/qmake )

QMAKEPATH=""

# Determine the qmake install path on this system
count=$((${#PATHLIST[@]} - 1))
for i in $(eval echo {0..$count})
do
    POSSIBLEPATH=${PATHLIST[$i]}
    if [ -e "$POSSIBLEPATH" ]
    then
        QMAKEPATH=$POSSIBLEPATH
        echo "Found qmake in $QMAKEPATH"
        break  # Break the for loop
    fi
done

# If qmake is not found above, try qmake installed in the standard system path
if [ "$QMAKEPATH" == "" ] && which qmake >/dev/null
then
    QMAKEPATH=qmake
fi

# Exit with an error if we don't find qmake
if [ "$QMAKEPATH" == "" ]
then
    echo error: qmake not found
    exit
fi

# Create the make file and perform the build
$QMAKEPATH BuildSKIRT.pro -o ../release/Makefile CONFIG+=release
make -j ${1:-5} -w -C ../release
