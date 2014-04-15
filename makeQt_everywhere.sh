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
    -c++11 -no-javascript-jit -no-qml-debug \
    -no-gif -no-libpng  -no-libjpeg -no-freetype -no-harfbuzz -no-openssl -no-xinput2 -no-xcb-xlib -no-glib \
    -no-gui -no-widgets -no-nis -no-cups -no-fontconfig -no-dbus -no-xcb -no-eglfs -no-directfb -no-linuxfb \
    -no-kms -no-opengl -nomake tools -nomake examples

make
make install
