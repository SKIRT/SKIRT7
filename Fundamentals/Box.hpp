/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BOX_HPP
#define BOX_HPP

#include <algorithm>
#include "Vec.hpp"

//////////////////////////////////////////////////////////////////////

/** Box is a low-level class for working with three-dimensional "boxes": each instance represents a
    cuboid that is lined up with the cartesian coordinate axes. A box is represented by means of
    its six cartesian coordinates \f$(x_\text{min},y_\text{min},z_\text{min},
    \,x_\text{max},y_\text{max},z_\text{max})\f$ which are stored in data members with similar
    names. The class offers functions to retrieve various (mostly trivial) properties of the box,
    including its coordinates and widths in each direction and its volume. A Box instance is
    immutable: once created it can no longer be changed (although a derived class can possibly
    change this behavior). The Box class is fully implemented inline (in this header file). Most
    compilers optimize away all overhead so that using this class is just as efficient as directly
    writing the code in terms of the box components. */
class Box
{
protected:
    /** These data members represent the cartesian vector components */
    double _xmin, _ymin, _zmin, _xmax, _ymax, _zmax;

public:
    /** The default constructor creates an empty box at the origin, i.e. it initializes all box
        coordinates to zero. */
    inline Box()
        : _xmin(0), _ymin(0), _zmin(0), _xmax(0), _ymax(0), _zmax(0) { }

    /** This constructor initializes the box coordinates to the values provided as arguments. */
    inline Box(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
        : _xmin(xmin), _ymin(ymin), _zmin(zmin), _xmax(xmax), _ymax(ymax), _zmax(zmax) { }

    /** This constructor initializes the box coordinates to the components of the provided Vec
        objects, respectively specifying the minimum and maximum position. */
    inline Box(Vec rmin, Vec rmax)
        : _xmin(rmin.x()), _ymin(rmin.y()), _zmin(rmin.z()), _xmax(rmax.x()), _ymax(rmax.y()), _zmax(rmax.z()) { }

    /** This function returns a reference to the receiving box object. It is useful for explicitly
        retrieving the box object from instances of classes based on Box. */
    inline const Box& extent() const { return *this; }

    /** This function stores the \f$(x_\text{min},y_\text{min},z_\text{min},
        \,x_\text{max},y_\text{max},z_\text{max})\f$ coordinates of the box in the provided
        arguments. */
    inline void extent(double& xmin, double& ymin, double& zmin, double& xmax, double& ymax, double& zmax) const
    { xmin = _xmin; ymin = _ymin; zmin = _zmin; xmax = _xmax; ymax = _ymax; zmax = _zmax; }

    /** This function returns the minimum position \f${\bf{r}}_\text{min}\f$ of the box. */
    inline Vec rmin() const { return Vec(_xmin,_ymin,_zmin); }

    /** This function returns the maximum position \f${\bf{r}}_\text{max}\f$ of the box. */
    inline Vec rmax() const { return Vec(_xmax,_ymax,_zmax); }

    /** This function returns the \f$x_\text{min}\f$ coordinate of the box. */
    inline double xmin() const { return _xmin; }

    /** This function returns the \f$y_\text{min}\f$ coordinate of the box. */
    inline double ymin() const { return _ymin; }

    /** This function returns the \f$z_\text{min}\f$ coordinate of the box. */
    inline double zmin() const { return _zmin; }

    /** This function returns the \f$x_\text{max}\f$ coordinate of the box. */
    inline double xmax() const { return _xmax; }

    /** This function returns the \f$y_\text{max}\f$ coordinate of the box. */
    inline double ymax() const { return _ymax; }

    /** This function returns the \f$z_\text{max}\f$ coordinate of the box. */
    inline double zmax() const { return _zmax; }

    /** This function returns the widths \f$(x_\text{max}-x_\text{min},
        y_\text{max}-y_\text{min}, z_\text{max}-z_\text{min})\f$ of the box as a Vec object. */
    inline Vec widths() const { return Vec(_xmax-_xmin,_ymax-_ymin,_zmax-_zmin); }

    /** This function returns the width \f$x_\text{max}-x_\text{min}\f$ of the box. */
    inline double xwidth() const { return _xmax-_xmin; }

    /** This function returns the width \f$y_\text{max}-y_\text{min}\f$ of the box. */
    inline double ywidth() const { return _ymax-_ymin; }

    /** This function returns the width \f$z_\text{max}-z_\text{min}\f$ of the box. */
    inline double zwidth() const { return _zmax-_zmin; }

    /** This function returns true if the position \f${\bf{r}}\f$ is inside the box, false
        otherwise. */
    inline bool contains(Vec r) const
    { return r.x() >= _xmin && r.x() <= _xmax && r.y() >= _ymin && r.y() <= _ymax && r.z() >= _zmin && r.z() <= _zmax; }

    /** This function returns true if the position \f$(x,y,z)\f$ is inside the box, false
        otherwise. */
    inline bool contains(double x, double y, double z) const
    { return x >= _xmin && x <= _xmax && y >= _ymin && y <= _ymax && z >= _zmin && z <= _zmax; }

    /** This function returns the volume \f$(x_\text{max}-x_\text{min}) \times
        (y_\text{max}-y_\text{min}) \times (z_\text{max}-z_\text{min})\f$ of the box. */
    inline double volume() const
    { return (_xmax-_xmin) * (_ymax-_ymin) * (_zmax-_zmin); }

    /** This function returns the position corresponding to the center of the box
        \f[ x=\tfrac12 (x_\text{min}+x_\text{max}) \f]
        \f[ y=\tfrac12 (y_\text{min}+y_\text{max}) \f]
        \f[ z=\tfrac12 (x_\text{min}+z_\text{max}) \f] */
    inline Vec center() const
    { return Vec( 0.5*(_xmin + _xmax), 0.5*(_ymin + _ymax), 0.5*(_zmin + _zmax) ); }

    /** This function returns a position in the box determined by a given fraction in each spatial direction
        \f[ x=x_\text{min}+x_\text{frac}\times(x_\text{max}-x_\text{min}) \f]
        \f[ y=y_\text{min}+y_\text{frac}\times(y_\text{max}-y_\text{min}) \f]
        \f[ z=z_\text{min}+z_\text{frac}\times(z_\text{max}-z_\text{min}) \f]
        The specified fractions must be between zero and one; this is \em not checked by the function. */
    inline Vec fracpos(double xfrac, double yfrac, double zfrac) const
    { return Vec( _xmin + xfrac*(_xmax-_xmin), _ymin + yfrac*(_ymax-_ymin), _zmin + zfrac*(_zmax-_zmin) ); }

    /** This function returns a position in the box determined by a given fraction in each spatial direction.
        \f[ x=x_\text{min}+\frac{x_\text{d}}{x_\text{n}}\times(x_\text{max}-x_\text{min}) \f]
        \f[ y=y_\text{min}+\frac{y_\text{d}}{y_\text{n}}\times(y_\text{max}-y_\text{min}) \f]
        \f[ z=z_\text{min}+\frac{z_\text{d}}{z_\text{n}}\times(z_\text{max}-z_\text{min}) \f]
        Each of the fractions is specified as the quotient of two integers; the integers are
        converted to floating point before the division is made. The quotients must be between zero
        and one; this is \em not checked by the function. */
    inline Vec fracpos(int xd, int yd, int zd, int xn, int yn, int zn) const
    { return Vec( _xmin + xd*(_xmax-_xmin)/xn, _ymin + yd*(_ymax-_ymin)/yn, _zmin + zd*(_zmax-_zmin)/zn ); }

    /** This function calculates the cell indices for a given position, assuming that the box would
        be partitioned in a given number of cells in each spatial direction. */
    inline void cellindices(int& i, int& j, int& k, Vec r, int nx, int ny, int nz) const
    {
        i = std::max(0, std::min(nx-1, static_cast<int>(nx*(r.x()-_xmin)/(_xmax-_xmin)) ));
        j = std::max(0, std::min(ny-1, static_cast<int>(ny*(r.y()-_ymin)/(_ymax-_ymin)) ));
        k = std::max(0, std::min(nz-1, static_cast<int>(nz*(r.z()-_zmin)/(_zmax-_zmin)) ));
    }

};

//////////////////////////////////////////////////////////////////////

#endif // BOX_HPP
