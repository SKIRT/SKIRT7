#!/bin/bash
# (use "chmod +rx scriptname" to make script executable)
#
# For use on any Unix system
#
# Execute this script with "git" as default directory to
# build a release version of skirt in the "release" directory
# using your current local copy of the code
#

# determine the qmake install path
#QMAKEPATH=`which qmake`
if [ "$QMAKEPATH" == "" ]
then
    # try the path targeted by the one-click Qt installer
    if [ -e "$HOME/Qt5.2.1/5.2.1/clang_64/bin/qmake" ]
    then
        QMAKEPATH=$HOME/Qt5.2.1/5.2.1/clang_64/bin/qmake
    fi

    # try the path targeted by ./makeQt_everywhere.sh
    if [ -e "$HOME/Qt/Desktop/5.2.1/bin/qmake" ]
    then
        QMAKEPATH=$HOME/Qt/Desktop/5.2.1/bin/qmake
    fi
fi

# exit with an error if we don't find qmake
if [ "$QMAKEPATH" == "" ]
then
    echo error: qmake not found
    exit
fi

# create the make file and perform the build
$QMAKEPATH buildSKIRT.pro -o ../release/Makefile CONFIG+=release
make -w -C ../release
