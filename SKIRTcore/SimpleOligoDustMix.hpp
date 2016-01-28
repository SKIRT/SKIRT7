/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SIMPLEOLIGODUSTMIX_HPP
#define SIMPLEOLIGODUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The SimpleOligoDustMix class represents, as its name indicates, a simple dust mixture that
    can be used for oligochromatic simulations. For each wavelength in the global wavelength grid, it just
    reads in user-provided values for the extinction coefficient, the albedo and scattering asymmetry parameter.  */
class SimpleOligoDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a simple oligochromatic dust mix")
    Q_CLASSINFO("AllowedIf", "OligoMonteCarloSimulation")

    Q_CLASSINFO("Property", "opacities")
    Q_CLASSINFO("Title", "the extinction coefficients, one for each wavelength")
    Q_CLASSINFO("Quantity", "opacity")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "albedos")
    Q_CLASSINFO("Title", "the scattering albedos, one for each wavelength")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")

    Q_CLASSINFO("Property", "asymmetryParameters")
    Q_CLASSINFO("Title", "the asymmetry parameters, one for each wavelength")
    Q_CLASSINFO("MinValue", "-1")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SimpleOligoDustMix();

private:
    /** This function directly calculates all dust mix properties on the simulation's wavelength
        grid. It then adds a single dust population to the dust mix. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the list of extinction coefficients, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setOpacities(QList<double> value);

    /** Returns the list of extinction coefficients, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> opacities() const;

    /** Sets the list of scattering albedos, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setAlbedos(QList<double> value);

    /** Returns the list of scattering albedos, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> albedos() const;

    /** Sets the list of asymmetry parameters, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setAsymmetryParameters(QList<double> value);

    /** Returns the list of asymmetry parameters, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> asymmetryParameters() const;

    //======================== Data Members ========================

private:
    QList<double> _kappaextv;
    QList<double> _albedov;
    QList<double> _asymmparv;
};

////////////////////////////////////////////////////////////////////

#endif // BENCHMARK1DDUSTMIX_HPP
