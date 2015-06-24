/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GAUSSIANKERNEL_HPP
#define GAUSSIANKERNEL_HPP

#include "ConvolutionKernel.hpp"

////////////////////////////////////////////////////////////////////

/** The GaussianKernel class represents a convolution kernel that is described by a 2D (symmetric)
    Gaussian function. */
class GaussianKernel : public ConvolutionKernel
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Gaussian convolution kernel")

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
    Q_INVOKABLE GaussianKernel();

protected:
    /** This function creates the image frame that describes the Gaussian kernel. */
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

    //======================== Data Members ========================

private:
    double _fwhm;
    int _dimension;
};

////////////////////////////////////////////////////////////////////

#endif // GAUSSIANKERNEL_HPP
