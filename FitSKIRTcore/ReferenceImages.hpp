/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef REFERENCEIMAGES_HPP
#define REFERENCEIMAGES_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

class ReferenceImage;

////////////////////////////////////////////////////////////////////

/** The ReferenceImages class represents a complete set of reference images.
    Objects of this class are essentially lists of pointers to ReferenceImage
    objects. */
class ReferenceImages: public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a list of images")

    Q_CLASSINFO("Property", "images")
    Q_CLASSINFO("Title", "the reference images")
    Q_CLASSINFO("Default", "ReferenceImage")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor; creates an empty ReferenceImages object. */
    Q_INVOKABLE ReferenceImages();

protected:
    /** This function verifies that at least one component has been added. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function inserts an image into the system at the specified index. */
    Q_INVOKABLE void insertImage(int index, ReferenceImage* value);

    /** This function removes the image with the specified index from the system. */
    Q_INVOKABLE void removeImage(int index);

    /** This function returns the list of reference images in the system. */
    Q_INVOKABLE QList<ReferenceImage*> images() const;

    //======================== Other Functions =======================

public:
    /** This function returns the path of the reference image at index \em rimi. */
    QString path(int rimi) const;

    /** This function returns the total number of reference images. */
    int size() const;

    /** This function returns the sum of the \f$\chi^2\f$ values over all the wavelengths.
        It requires a list of disk and bulge simulations and returns a list of best fitting luminosities.
        Apart from being used as input, both simulations are adapted in place to contain the same masked regions
        as their corresponding reference image. */
    double chi2(QList<QList<Array>> *frames, QList<QList<double>> *luminosities, QList<double> *Chis);

    /** Writes out the best fitting and residual frames between simulated and reference images. */
    void writeOutBest(int index, int consec) const;

    //======================== Data Members ========================

private:
    QList<ReferenceImage*> _rimages;
};

////////////////////////////////////////////////////////////////////

#endif // ReferenceImages_HPP
