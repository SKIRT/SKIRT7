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

# determine the qmake install path

#####################################################
### Search for the most recent version of Qt first ##
#####################################################

# try the path targeted by the one-click Qt installer
if [ -e "$HOME/Qt5.3.2/5.3/clang_64/bin/qmake" ]
then
    QMAKEPATH=$HOME/Qt5.3.2/5.3/clang_64/bin/qmake
fi

# try the path targeted by ./makeQt_everywhere.sh
if [ -e "$HOME/Qt/Desktop/5.3.2/bin/qmake" ]
then
    QMAKEPATH=$HOME/Qt/Desktop/5.3.2/bin/qmake
fi

# try the path in /usr/local used on Ghent research team computers
if [ -e "/usr/local/Qt/5.3.2/bin/qmake" ]
then
    QMAKEPATH=/usr/local/Qt/5.3.2/bin/qmake
fi

#############################################################################################
## Search for the older Qt version that is also supported (and a lot of SKIRT users have)  ##
#############################################################################################

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

# try the path in /usr/local used on Ghent research team computers
if [ -e "/usr/local/Qt/5.2.1/bin/qmake" ]
then
    QMAKEPATH=/usr/local/Qt/5.2.1/bin/qmake
fi

# exit with an error if we don't find qmake
if [ "$QMAKEPATH" == "" ]
then
    echo error: qmake not found
    exit
fi

# create the make file and perform the build
$QMAKEPATH BuildSKIRT.pro -o ../release/Makefile CONFIG+=release
make -j ${1:-5} -w -C ../release
