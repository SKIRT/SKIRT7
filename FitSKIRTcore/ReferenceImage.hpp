/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef REFERENCEIMAGE_HPP
#define REFERENCEIMAGE_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

class Convolution;

////////////////////////////////////////////////////////////////////

/** The ReferenceImage class contains all information for a certain reference image.
    It contains the path to the file and information about the PSF.
    The \f$\chi^2\f$ value for a certain input frame can be calculated. */
class ReferenceImage : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a reference image")

    Q_CLASSINFO("Property", "path")
    Q_CLASSINFO("Title", "the path to this reference image")

    Q_CLASSINFO("Property", "convolution")
    Q_CLASSINFO("Title", "the convolution properties")

    Q_CLASSINFO("Property","minLuminosities")
    Q_CLASSINFO("Title","the minimum luminosity boundaries")

    Q_CLASSINFO("Property","maxLuminosities")
    Q_CLASSINFO("Title","the maximum luminosity boundaries")

    //============ Construction - Setup - Destruction  =============

public:
    /** The default constructor. */
    Q_INVOKABLE ReferenceImage();

    /** This function reads in the actual reference image at the given path. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the path to the reference image. */
    Q_INVOKABLE void setPath(QString value);

    /** Returns the path to the reference image. */
    Q_INVOKABLE QString path() const;

    /** Sets the convolution properties for the reference image. */
    Q_INVOKABLE void setConvolution(Convolution* value);

    /** Returns the convolution properties for the reference image. */
    Q_INVOKABLE Convolution* convolution() const;

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

    /** Returns the number of pixels in the reference frame in the x direction. */
    int xdim() const;

    /** Returns the number of pixels in the reference frame in the y direction. */
    int ydim() const;

    /** Returns the array containing the reference image. */
    Array refImage() const;

    //======================== Data Members ========================

private:
    // data members
    QString _path;
    Convolution* _convolution;
    QList<double> _minLum;
    QList<double> _maxLum;
    Array _refim;
    int _xdim;
    int _ydim;
    int _zdim;
};

////////////////////////////////////////////////////////////////////

#endif // REFERENCEIMAGE_HPP
