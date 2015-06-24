/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <QString>
#include "Array.hpp"
class SimulationItem;
class Units;

////////////////////////////////////////////////////////////////////

/** This class represents a 2D image. */
class Image
{
    //=============== Construction - Destruction  ==================
public:
    /** The default constructor. Creates an empty image. */
    Image();

    /** This constructor creates a blank image with a specified size. */
    Image(int xsize, int ysize, int nframes = 1);

    /** This constructor creates an image from a specified FITS file. As an argument, it takes the name
        of that FITS file (without the extension). Additionally, it requires a pointer to an item in
        the simulation hierarchy. */
    Image(const SimulationItem* item, QString filename);

    /** This constructor creates an image based on an Array of double values and the header information
        for the image. */
    Image(const SimulationItem* item, Array data, int xsize, int ysize, int nframes, double xres, double yres);

    /** This constructor creates an image with only a header. */
    Image(const SimulationItem* item, int xsize, int ysize, int nframes, double xres, double yres);

    //====================== Inititialization ======================

public:
    /** This function implements the procedure where a FITS file is imported. It is used by the
        constructor that takes the same arguments, but can also be called from the derived classes
        (such as ConvolutionKernel). */
    void import(const SimulationItem* item, QString filename);

    /** This function resizes the image to a specified width and height, erasing the current data. */
    void resize(int xsize, int ysize, int nframes = 1);

    //===================== Basic properties =======================

public:
    /** This function returns the number of pixels in the x direction. */
    int xsize() const;

    /** This function returns the number of pixels in the y direction. */
    int ysize() const;

    /** This function returns the number of frames in the image. */
    int numframes() const;

    /** This function returns a reference to the internal data array. */
    const Array& data() const;

    //========================= Exporting ==========================

    /** This function can be used to export the current image to a FITS file. */
    void saveto(const SimulationItem* item, QString filename, QString description);

    /** This function ... */
    void saveto(const SimulationItem* item, Array& data, QString filename, QString description);

    //=================== Numerical operations =====================

public:
    /** This function returns the sum of all the pixel values in the image. */
    double sum() const;

    //========================= Operators ==========================

public:
    /** This operator can be used to access (read and change) the value of a certain pixel in the
        image, defined by an x and y coordinate. */
    double& operator()(int x, int y)
    {
        return _data[x + _xsize*y];
    }

    /** This operator can be used to read the value of a certain pixel in the image, defined by an x
        and y coordinate. */
    const double& operator()(int x, int y) const
    {
        return _data[x + _xsize*y];
    }

    /** This operator can be used to divide each pixel by a given value. */
    Image& operator/= (const double& value)
    {
        _data /= value;
        return (*this);
    }

    //======================== Data Members ========================

protected:
    // the internal data array
    Array _data;

    // cached pointer to the units system of the simulation
    Units* _units;

    // the dimensions of the image
    int _xsize;
    int _ysize;
    int _nframes;

    // the physical properties of the image
    double _incx;
    double _incy;
    QString _dataunits;
    QString _xyunits;
};

////////////////////////////////////////////////////////////////////

#endif // IMAGE_HPP
