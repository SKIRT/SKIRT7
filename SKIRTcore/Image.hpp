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
        of that FITS file (with or without the extension). Additionally, it requires a pointer to an item in
        the simulation hierarchy. */
    Image(const SimulationItem* item, QString filename);

    /** This constructor creates an image from a specified FITS file. As arguments, it takes the name
        of that FITS file (with or without the extension) and the path to the directory that contains
        this file. */
    Image(const SimulationItem* item, QString filename, QString directory);

    /** This constructor creates an image from a specified FITS file and from header information passed
        to this function. */
    Image(const SimulationItem* item, QString filename, double xres, double yres, QString quantity,
          QString xyqty = "length");

    /** This constructor creates an image from a specified FITS file in a specified directory, and from
        header information passed to this function. */
    Image(const SimulationItem* item, QString filename, QString directory, double xres, double yres,
          QString quantity, QString xyqty = "length");

    /** This constructor creates an image based on the header of another image and the data, passed as
        an Array object. */
    Image(const Image& header, const Array& data);

    /** This constructor creates an image based on an Array of double values and the header information
        for the image. */
    Image(const SimulationItem* item, const Array& data, int xsize, int ysize, int nframes,
          double xres, double yres, QString quantity, QString xyqty = "length");

    /** This constructor creates an image with only a header. */
    Image(const SimulationItem* item, int xsize, int ysize, int nframes,
          double xres, double yres, QString quantity, QString xyqty = "length");

    //====================== Initialization ======================

public:
    /** This function implements the procedure where a FITS file is imported. It is used by the
        constructor that takes the same arguments, but can also be called from the derived classes
        (such as ConvolutionKernel). */
    void import(const SimulationItem* item, QString filename, QString directory = QString());

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

    /** This function returns the total number of pixels in this image. */
    int numpixels() const;

    /** This function returns the resolution in the x direction. */
    double xres() const;

    /** This function returns the resolution in the y direction. */
    double yres() const;

    /** This function returns the sum of all the pixel values in the image. */
    double sum() const;

    /** This function returns a reference to the internal data array. */
    const Array& data() const;

    //========================= Exporting ==========================

    /** This function can be used to export the current image to a FITS file. */
    void saveto(const SimulationItem* item, QString filename, QString description);

    /** This function is temporary; it is used in classes where image frames are still handled as
        Arrays, but where eventually objects of the Image class could be used (but this redesign takes
        a lot of work). This function can save an external data container (of type const Array and
        passed by reference), along with the header information contained in this Image instance, as a
        FITS file. */
    void saveto(const SimulationItem* item, const Array& data, QString filename, QString description);

    //=================== Numerical operations =====================

public:
    /** This function performs a convolution on this image with a given kernel.*/
    void convolve(const Image& kernel);

    //========================= Operators ==========================

public:
    /** This operator can be used to access (read and change) the value at a certain index of the
        underlying Array object. */
    double& operator[](int i)
    {
        return _data[i];
    }

    /** This operator can be used to read the value at a certain index of the underlying Array object. */
    const double& operator[](int i) const
    {
        return _data[i];
    }

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

    /** This operator can be used to multiply each pixel by a given value. */
    Image& operator*= (const double& value)
    {
        _data *= value;
        return (*this);
    }

    /** This operator can be used to divide each pixel by a given value. */
    Image& operator/= (const double& value)
    {
        _data /= value;
        return (*this);
    }

    /** This operator returns a new image, created by multiplying this image by a given value. */
    Image operator*(double value) const
    {
        Image result = *this;
        return (result *= value);
    }

    /** This operator returns a new image, created by adding another image element-wise to this image. */
    Image operator+(const Image& image2)
    {
        Image result = Image(*this, _data + image2.data());
        return result;
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
