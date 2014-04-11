#!/bin/bash
# (use "chmod +rx scriptname" to make script executable)
#
# For use on any Unix system for which there are no pre-built Qt binaries
#
# To build the Qt libraries:
#  - download the Qt source tarball (e.g. qt-everywhere-opensource-src-5.2.1.tar.gz) and extract it
#  - copy this script into the directory containing the Qt sources extracted from the tarball
#  - make the script excutable and execute it
#

./configure -prefix "$HOME/Qt/Desktop/5.2.1" -opensource -confirm-license \
   -no-qt3support -no-multimedia -no-audio-backend -no-phonon -no-phonon-backend -no-svg -no-webkit \
   -no-javascript-jit -no-script -no-scripttools -no-declarative -no-declarative-debug \
   -no-gif -no-libtiff -no-libpng -no-libmng -no-libjpeg -no-openssl \
   -nomake tools -nomake examples -nomake demos -nomake docs -nomake translations \
   -no-gui -no-nis -no-cups -no-dbus -no-opengl -no-openvg -no-sm -no-xshape -no-xvideo \
   -no-xsync -no-xinerama -no-xcursor -no-xfixes -no-xrandr -no-xrender -no-mitshm \
   -no-fontconfig -no-xinput -no-xkb -no-glib

make
make install
