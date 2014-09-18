/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Chunk.hpp"
#include "Block.hpp"
#include <QTextStream>

////////////////////////////////////////////////////////////////////

Chunk::Chunk()
{
}

////////////////////////////////////////////////////////////////////

void Chunk::readFromConsole()
{
     QTextStream in(stdin);
     while (!in.atEnd())
     {
         _chunk << in.readLine();
     }
}

////////////////////////////////////////////////////////////////////

void Chunk::writeToConsole()
{
    QTextStream out(stdout);
    foreach (QString line, _chunk)
    {
        out << line << endl;
    }
}

////////////////////////////////////////////////////////////////////

void Chunk::streamline()
{
    // some regular expressions
    QRegExp startDox("\\s*/\\*\\*.*");  //   \s* (white space) /** (literal text) .* (anything)
    QRegExp endDox(".*\\*/\\s*");       //    .* (anything) */ (literal text) \s* (white space)

    // loop over the lines of the source code
    int index = 0;
    while (index < _chunk.size())
    {
        // look for the start of a documentation block
        index = _chunk.indexOf(startDox, index);
        if (index < 0) break;

        // look for the end of the documentation block (which could be on the same line)
        int end = _chunk.indexOf(endDox, index);
        if (end < 0) break;

        // streamline the block
        Block styler(_chunk, index, end);
        QStringList block = styler.streamlined();

        // replace the block in the source code
        for (int j = 0; j < end-index+1; j++) _chunk.removeAt(index);
        foreach (QString line, block) _chunk.insert(index++, line);

        // remove any empty lines following the block in the source code
        while (index < _chunk.size() && _chunk[index].simplified().isEmpty()) _chunk.removeAt(index);
    }
}

////////////////////////////////////////////////////////////////////
