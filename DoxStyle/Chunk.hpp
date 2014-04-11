/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <QStringList>

////////////////////////////////////////////////////////////////////

/** This class streamlines doxygen documentation blocks in a chunk of source code. The read/write
    methods are separated from the actual streamlining because future versions of this class may be
    extended with other input/output mechanisms in addition to stdin/stdout. */
class Chunk
{
public:
    /** Constructs a streamliner with an empty source code chunk. */
    Chunk();

    /** Reads lines from stdin until EOF is reached, adding them to the current source code chunk.
        */
    void readFromConsole();

    /** Writes the current source code chunk to stdout. */
    void writeToConsole();

    /** Streamlines the current source code chunk, adjusting its contents as needed. */
    void streamline();

private:
    /** The current source code chunk. */
    QStringList _chunk;
};

////////////////////////////////////////////////////////////////////

#endif // CHUNK_HPP
