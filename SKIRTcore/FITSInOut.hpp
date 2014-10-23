/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FITINSOUT_HPP
#define FITINSOUT_HPP

#include <QString>
#include "Array.hpp"

////////////////////////////////////////////////////////////////////

/** This namespace supports writing a 2D or 3D data stream to a standard FITS file, including a
    basic set of metadata in the header. */
namespace FITSInOut
{
    /** This function writes a FITS file containing one or more data planes (i.e. a 2D or 3D data
        cube). The first argument specifies a relative or absolute file path; if a file with that
        name already exists, it is overwritten. The subsequent arguments specify the contents of
        the file: \em data contains the actual values in the data cube; \em nx and \em ny specify
        the number of values in each direction, \em nz specifies the number of planes (set \em nz
        to 1 for 2D data), \em incx and \em incy specify the increment between subsequent grid
        points in each direction, \em dataUnits describes the units of the data values, and \em
        xyUnits describes the units of the xy-grid increments. The values in the \em data array
        must be ordered such that the index along the x-axis varies most rapidly, the index along
        the y-axis varies less rapidly, and the index along the z-axis (if present) varies least
        rapidly. */
    void write(QString filepath, const Array& data, int nx, int ny, int nz,
               double incx, double incy, QString dataUnits, QString xyUnits);

    /** This function reads from a FITS file containing one or more data planes (i.e. a 2D or 3D
        data cube). The first argument specifies a relative or absolute file path; a file with that
        name should exist. The subsequent arguments serve to store data read from the file: \em
        data receives the actual values in the data cube; \em nx and \em ny receive the number of
        values in each direction, \em nz specifies the number of planes (which is equal to 1 for 2D
        data). The values in the \em data array are ordered such that the index along the x-axis
        varies most rapidly, the index along the y-axis varies less rapidly, and the index along
        the z-axis (if present) varies least rapidly. */
    void read(QString filepath, Array& data, int& nx, int& ny, int& nz);
}

////////////////////////////////////////////////////////////////////

#endif // FITINSOUT_HPP
