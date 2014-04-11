/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <QStringList>

////////////////////////////////////////////////////////////////////

/** This class streamlines a single doxygen documentation block. */
class Block
{
public:
    /** Constructs a streamliner for the specified documentation block. */
    Block(QStringList block);

    /** Constructs a streamliner for the documentation block specified as a portion of a larger
        source code chunk. */
    Block(QStringList chunk, int first, int last);

    /** Returns the streamlined documentation block. */
    QStringList streamlined();

private:
    /** The documentation block. */
    QStringList _block;
};

////////////////////////////////////////////////////////////////////

#endif // BLOCK_HPP
