/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FILEWAVELENGTHGRID_HPP
#define FILEWAVELENGTHGRID_HPP

#include "PanWavelengthGrid.hpp"

////////////////////////////////////////////////////////////////////

/** FileWavelengthGrid is a subclass of the PanWavelengthGrid class representing wavelength grids
    read in from a file. */
class FileWavelengthGrid : public PanWavelengthGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a wavelength grid read from a file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the file with the wavelength grid points")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE FileWavelengthGrid();

protected:
    /** This function reads the wavelength grid points from the specified file. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file containing the wavelength grid points, optionally including an
        absolute or relative path. The text file must contain on the first line the number of
        wavelength grid points and subsequently all grid points in micron. The order of the grid
        points is irrelevant. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file containing the wavelength grid points. */
    Q_INVOKABLE QString filename() const;

    //======================== Data Members ========================

private:
    QString _filename;
};

////////////////////////////////////////////////////////////////////

#endif // FILEWAVELENGTHGRID_HPP
