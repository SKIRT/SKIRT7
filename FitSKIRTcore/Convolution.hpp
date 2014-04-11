/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef CONVOLUTION_HPP
#define CONVOLUTION_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The Convolution class contains all information to convolve a given frame*/
class Convolution : public SimulationItem
{
    Q_OBJECT

    Q_CLASSINFO("Title", "Convolution setup")

    Q_CLASSINFO("Property", "fwhm")
    Q_CLASSINFO("Title", "the full width at half max in pixels")
    Q_CLASSINFO("Default", "3.00")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000")

    Q_CLASSINFO("Property", "dimension")
    Q_CLASSINFO("Title", "the dimension in pixels of the psf frame")
    Q_CLASSINFO("Default", "6")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Convolution();

protected:
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the full width at half max. */
    Q_INVOKABLE void setFwhm(double value);

    /** Returns the full width at half max.*/
    Q_INVOKABLE double fwhm() const;

    /** Sets the convolution frame dimension. */
    Q_INVOKABLE void setDimension(int value);

    /** Returns the convolution frame dimension. */
    Q_INVOKABLE int dimension() const;

    //======================== Other Functions =======================
public:
    /** This function convolves the specified frame, with dimensions \em xdim x \em ydim, using the Convolution properties.
        The kernel frame is created in such a way so no intensity is lost.*/
    void convolve(Array *frame, int xdim, int ydim) const;

    //======================== Data Members ========================

private:
    double _fwhm;
    int _dimension;
};

////////////////////////////////////////////////////////////////////

#endif // CONVOLUTION_HPP
