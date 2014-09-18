/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FILESED_HPP
#define FILESED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** FileSED is a simple class that represents spectral energy distributions read in directly from a
    file provided by the user. */
class FileSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a tabulated SED from a file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the file that contains the SED")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE FileSED();

protected:
    /** This function reads the stellar fluxes from a file provided by the user. This file should
        first contain a single line with the number of data points and subsequently contain lines
        with two columns: wavelength \f$\lambda\f$ in micron and flux density \f$F_\lambda\f$ in
        arbitrary units. This vector is regridded on the global wavelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file that contains the SED. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file that contains the SED. */
    Q_INVOKABLE QString filename() const;

    //======================== Data Members ========================

private:
    QString _filename;
};

////////////////////////////////////////////////////////////////////

#endif // FILESED_HPP
