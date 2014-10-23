/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Block.hpp"

////////////////////////////////////////////////////////////////////

// lines will be wrapped before reaching the margin
const static int MARGIN = 100;

////////////////////////////////////////////////////////////////////

Block::Block(QStringList block)
{
    _block = block;
}

////////////////////////////////////////////////////////////////////

Block::Block(QStringList chunk, int first, int last)
{
    for (int index = first; index <= last; index++)
    {
        _block << chunk[index];
    }
}

////////////////////////////////////////////////////////////////////

// this function assumes that _block contains at least one line,
// that the first line starts with /** and that the last line ends with */
QStringList Block::streamlined()
{
    // get the portion of the first line before the slash
    QString prefix = _block[0].section('/', 0, 0);

    // put the complete block contents in a single string, taking care
    // to remove leading /** and trailing */ on the first & last lines
    // and any optional leading * on any lines other than the first
    QString single;
    int numLines = _block.size();
    for (int index = 0; index < numLines; index++)
    {
        QString trimmed = _block[index].trimmed();
        if (index == 0) trimmed.remove(0,2);  // remove the /* leaving the second * alone
        if (index == numLines-1) trimmed.remove(trimmed.length()-2,2);  // remove the */
        if (trimmed.startsWith("*")) trimmed.remove(0,1);  // remove the optional *
        single += " " + trimmed;  // ensure whitespace between lines
    }

    // replace consecutive whitespace, including tabs, by a single space
    // and split the result into words (i.e. entities separated by whitespace)
    QStringList words = single.simplified().split(' ');

    // construct a streamlined documentation block by adding words until lines are full
    QStringList result;
    QString line = prefix + "/**";   // initialize the first line
    words << "*/";                   // add the terminator as a regular word
    foreach (QString word, words)
    {
        if (line.length() + word.length() > MARGIN-2)  // line full with one space to spare
        {
            result << line;          // flush the line
            line = prefix + "   ";   // start a new line
        }
        line += " " + word;          // add a word
    }
    if (!line.simplified().isEmpty()) result << line;  // flush the line if nonempty

    // we're done
    return result;
}

////////////////////////////////////////////////////////////////////
