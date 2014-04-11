/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DoxStyleMain.hpp"
#include "Chunk.hpp"
#include <QCoreApplication>
#include <QTextCodec>

////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    // construct application object for argument parsing and such,
    // but don't run the event loop because we don't need it
    QCoreApplication app(argc, argv);

    // set the locale encoding to UTF-8 since we expect UTF-8 source code as input and output
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // perform a streamline operation from stdin to stdout
    Chunk styler;
    styler.readFromConsole();
    styler.streamline();
    styler.writeToConsole();
}

////////////////////////////////////////////////////////////////////
