/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MAPPINGSSED_HPP
#define MAPPINGSSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** MappingsSED is a class that represents spectral energy distributions of starbursting regions,
    parameterized on metallicity, compactness, ISM pressure and PDR covering factor, obtained from
    the MAPPINGS III templates described in Groves et al. (2008). See the MappingsSEDFamily class
    for more information. */
class MappingsSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a starburst SED from the MAPPINGS III library")

    Q_CLASSINFO("Property", "metallicity")
    Q_CLASSINFO("Title", "the metallicity")
    Q_CLASSINFO("MinValue", "0.0006")
    Q_CLASSINFO("MaxValue", "0.025")
    Q_CLASSINFO("Default", "0.0122")

    Q_CLASSINFO("Property", "compactness")
    Q_CLASSINFO("Title", "the logarithm of the compactness parameter")
    Q_CLASSINFO("MinValue", "4.0")
    Q_CLASSINFO("MaxValue", "6.5")
    Q_CLASSINFO("Default", "6.0")

    Q_CLASSINFO("Property", "pressure")
    Q_CLASSINFO("Title", "the ISM pressure")
    Q_CLASSINFO("Quantity", "pressure")
    Q_CLASSINFO("MinValue", "1e10 K/m3")
    Q_CLASSINFO("MaxValue", "1e14 K/m3")
    Q_CLASSINFO("Default", "1e11 K/m3")

    Q_CLASSINFO("Property", "coveringFactor")
    Q_CLASSINFO("Title", "the PDR covering factor")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "0.2")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE MappingsSED();

protected:
    /** This function constructs a temporary instance of the MappingsSEDFamily class to obtain an
        SED that corresponds to the values of the metallicity, compactness, ISM pressure and PDR
        covering factor specified in the attributes. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the metallicity. */
    Q_INVOKABLE void setMetallicity(double value);

    /** Returns the metallicity. */
    Q_INVOKABLE double metallicity() const;

    /** Sets the logarithm of the compactness parameter. */
    Q_INVOKABLE void setCompactness(double value);

    /** Returns the logarithm of the compactness parameter. */
    Q_INVOKABLE double compactness() const;

    /** Sets the logarithm of the pressure. */
    Q_INVOKABLE void setPressure(double value);

    /** Returns the logarithm of the pressure. */
    Q_INVOKABLE double pressure() const;

    /** Sets the PDR covering factor. */
    Q_INVOKABLE void setCoveringFactor(double value);

    /** Returns the PDR covering factor. */
    Q_INVOKABLE double coveringFactor() const;

    //======================== Data Members ========================

private:
    double _Z;
    double _logC;
    double _pressure;
    double _fPDR;
};

////////////////////////////////////////////////////////////////////

#endif // MAPPINGSSED_HPP
