/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef READFITSGEOMETRY_HPP
#define READFITSGEOMETRY_HPP

#include "Array.hpp"
#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The ReadFitsGeometry class is a subclass of the GenGeometry class, and describes a geometry
    characterized by observations. A 2D observed image FITS file is read into <tt>SKIRT</tt>, and
    deprojected assuming a certain position angle, inclination and azimuth. The density is assumed
    to follow a exponential profile in the vertical directions, \f[ \rho(z) = \rho_0\,
    \exp\left(-\frac{|z|}{h_z}\right). \f] By running a <tt>SKIRT</tt> simulation with inclination
    of 0 degrees and position angle of the simulated galaxy, the <tt>SKIRT</tt> model images can be
    compared with the observations. The model geometry is set by nine parameters: the input
    filename, the pixel scale \f$pix\f$, the position angle \f$pa\f$, the inclination \f$incl\f$,
    the number of pixels in x and y direction \f$n_x\f$ and \f$n_y\f$, the center of galaxy in
    (x,y) image coordinates \f$x_c\f$ and \f$y_c\f$ and the vertical scale height \f$h_z\f$. */
class ReadFitsGeometry: public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry read from a FITS file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the input image file")

    Q_CLASSINFO("Property", "pixelScale")
    Q_CLASSINFO("Title", "the physical scale of the image (length per pixel)")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "positionAngle")
    Q_CLASSINFO("Title", "the position angle ω of the system")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "-360 deg")
    Q_CLASSINFO("MaxValue", "360 deg")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "inclination")
    Q_CLASSINFO("Title", "the inclination angle θ of the system")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "45 deg")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "xelements")
    Q_CLASSINFO("Title", "number of pixels in the x direction")
    Q_CLASSINFO("MinValue", "1")

    Q_CLASSINFO("Property", "yelements")
    Q_CLASSINFO("Title", "number of pixels in the y direction")
    Q_CLASSINFO("MinValue", "1")

    Q_CLASSINFO("Property", "xcenter")
    Q_CLASSINFO("Title", "x coordinate of the center (in pixels)")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "ycenter")
    Q_CLASSINFO("Title", "y coordinate of the center (in pixels)")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "axialScale")
    Q_CLASSINFO("Title", "the axial scale height")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ReadFitsGeometry();

protected:
    /** This function verifies the validity of the pixel scale, the inclination angle, the number of
        pixels in the x and y direction, the center of the image in x and y coordinates and the
        vertical scale height \f$h_z\f$. A vector of normalized cumulative pixel luminosities is
        computed, satisfying the condition that the total mass equals 1. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file with the image parameters. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file with the image parameters. */
    Q_INVOKABLE QString filename() const;

    /** Sets the physical scale of the image (length per pixel). */
    Q_INVOKABLE void setPixelScale(double value);

    /** Returns the physical scale of the image (length per pixel). */
    Q_INVOKABLE double pixelScale() const;

    /** Sets the position angle. */
    Q_INVOKABLE void setPositionAngle(double value);

    /** Returns the position angle. */
    Q_INVOKABLE double positionAngle() const;

    /** Sets the inclination. */
    Q_INVOKABLE void setInclination(double value);

    /** Returns the inclination. */
    Q_INVOKABLE double inclination() const;

    /** Sets the number of pixels in the x direction. */
    Q_INVOKABLE void setXelements(int value);

    /** Returns the number of pixels in the x direction. */
    Q_INVOKABLE int xelements() const;

    /** Sets the number of pixels in the y direction. */
    Q_INVOKABLE void setYelements(int value);

    /** Returns the number of pixels in the y direction. */
    Q_INVOKABLE int yelements() const;

    /** Sets the x coordinate of the center. */
    Q_INVOKABLE void setXcenter(double value);

    /** Returns the x coordinate of the center. */
    Q_INVOKABLE double xcenter() const;

    /** Sets the y coordinate of the center. */
    Q_INVOKABLE void setYcenter(double value);

    /** Returns the y coordinate of the center. */
    Q_INVOKABLE double ycenter() const;

    /** Sets the axial scale height. */
    Q_INVOKABLE void setAxialScale(double value);

    /** Returns the axial scale height. */
    Q_INVOKABLE double axialScale() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(x,y,z)\f$ at the position (x,y,z). */
    double density(Position bfr) const;

    /** This function generates a random position (x,y,z) from the geometry, by drawing a random point
        from the appropriate probability density distribution function. The (x,y) coordinates are
        derived from the normalized cumulative luminosity vector of the observed 2D projection. The z
        coordinate is derived from the vertical exponential probability distribution function. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along the
        entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\, {\text{d}}x. \f] */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along the
        entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\, {\text{d}}y. \f] */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z. \f] */
    double SigmaZ() const;

private:
    /** This function ... */
    void rotate(double &x, double &y) const;

    /** This function ... */
    void derotate(double &x, double &y) const;

    /** This function ... */
    void project(double &x) const;

    /** This function ... */
    void deproject(double &x) const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    QString _filename;
    double _pix;
    double _positionangle;
    double _inclination;
    int _Nx;
    int _Ny;
    double _xc;
    double _yc;
    double _hz;

    // data members initialized during setup
    double _xmax, _ymax, _xmin, _ymin;
    double _cospa, _sinpa, _cosi, _sini;
    double _C1x, _C1y, _C2x, _C2y, _C3x, _C3y, _C4x, _C4y;
    Array _Lv, _Xv;
};

////////////////////////////////////////////////////////////////////

#endif // READFITSGEOMETRY_HPP
