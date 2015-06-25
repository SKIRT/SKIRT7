/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FITSKERNEL_HPP
#define FITSKERNEL_HPP

#include "ConvolutionKernel.hpp"

////////////////////////////////////////////////////////////////////

/** The FitsKernel class represents a convolution kernel that is imported from a FITS file. */
class FitsKernel : public ConvolutionKernel
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a convolution kernel read from a FITS file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the input image file")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE FitsKernel();

protected:
    /** This function makes sure that the FITS file is imported. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the FITS file describing the PSF. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file describing the PSF. */
    Q_INVOKABLE QString filename() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    QString _filename;
};

////////////////////////////////////////////////////////////////////

#endif // FITSKERNEL_HPP
