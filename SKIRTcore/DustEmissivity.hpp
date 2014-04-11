/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTEMISSIVITY_HPP
#define DUSTEMISSIVITY_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"
class DustMix;

//////////////////////////////////////////////////////////////////////

/** The DustEmissivity class is the abstract base class for objects that calculate the wavelength
    dependent emissivity of a particular dust mix in a given radiation field. The emissivity can be
    determined from the optical properties of the dust mixture and the interstellar radiation field
    (both specified as arguments by the caller), and some additional assumptions. DustEmissivity
    subclasses implement various assumptions, and in particular, whether transient heating is taken
    into account or not. */
class DustEmissivity : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust emissivity type")

    //============= Construction - Setup - Destruction =============

protected:
    /** Default constructor. */
    DustEmissivity();

    //======================== Other Functions =======================

public:
    /** This function returns the dust emissivity \f$\varepsilon_\ell\f$ at all wavelength indices
        \f$\ell\f$ for a dust mix of the specified type residing in the specified mean radiation
        field \f$J_\ell\f$, assuming the simulation's wavelength grid. */
    virtual Array emissivity(const DustMix* mix, const Array& Jv) const = 0;

    /** The return value of this function indicates a meaningful frequency for console-logging when
        repeatly invoking emissivity(). A value of zero means that the calculation is fast and thus
        there should be no logging. A value of one means that the calculation is slow and thus
        every invocation should be logged. Other values may be used later to indicate a logging
        frequency. The function's implementation in this class returns zero, which means no logging
        is needed. */
    virtual int logfrequency() const;
};

////////////////////////////////////////////////////////////////////

#endif // DUSTEMISSIVITY_HPP
