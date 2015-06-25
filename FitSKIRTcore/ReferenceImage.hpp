/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef REFERENCEIMAGE_HPP
#define REFERENCEIMAGE_HPP

#include "Image.hpp"
#include "SimulationItem.hpp"

class ConvolutionKernel;

////////////////////////////////////////////////////////////////////

/** The ReferenceImage class contains all information for a certain reference image.
    It contains the path to the file and information about the PSF.
    The \f$\chi^2\f$ value for a certain input frame can be calculated. */
class ReferenceImage : public SimulationItem, public Image
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a reference image")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name (or relative path) of this reference image")

    Q_CLASSINFO("Property", "kernel")
    Q_CLASSINFO("Title", "the convolution kernel")

    Q_CLASSINFO("Property","minLuminosities")
    Q_CLASSINFO("Title","the minimum luminosity boundaries")

    Q_CLASSINFO("Property","maxLuminosities")
    Q_CLASSINFO("Title","the maximum luminosity boundaries")

    //============ Construction - Setup - Destruction  =============

public:
    /** The default constructor. */
    Q_INVOKABLE ReferenceImage();

    /** This function reads in the actual reference image with the given name. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the reference image. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the reference image. */
    Q_INVOKABLE QString filename() const;

    /** Sets the convolution kernel of the reference image. */
    Q_INVOKABLE void setKernel(ConvolutionKernel* value);

    /** Returns the convolution kernel of the reference image. */
    Q_INVOKABLE ConvolutionKernel* kernel() const;

    /** Sets the list of minimum luminosities, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setMinLuminosities(QList<double> value);

    /** Returns the list of minimum luminosities, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> minLuminosities() const;

    /** Sets the list of maximum luminosities, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setMaxLuminosities(QList<double> value);

    /** Returns the list of maximum luminosities, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> maxLuminosities() const;


    //====================== Other functions =======================

public:
    /** Returns the \f$\chi^2\f$ value between the reference image and input frame.
        It returns the simulations which are altered in place to contain the same masks and
        it returns the best fitting luminosities. */
    double chi2(QList<Array> *frames, QList<double> *monoluminosities) const;

    /** Returns the simulations which are altered in place to contain the same masks
        and the corresponding residual frame. */
    void returnFrame(QList<Array> *frames) const;

    //======================== Data Members ========================

private:
    // data members
    QString _filename;
    ConvolutionKernel* _kernel;
    QList<double> _minLum;
    QList<double> _maxLum;
};

////////////////////////////////////////////////////////////////////

#endif // REFERENCEIMAGE_HPP
