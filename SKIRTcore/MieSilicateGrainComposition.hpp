/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MIESILICATEGRAINCOMPOSITION_HPP
#define MIESILICATEGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The MieSilicateGrainComposition class represents the optical and calorimetric properties of
    spherical silicate dust grains, based on the dielectric constants of astronomical silicate
    taken from Bruce Draine's website. The optical properties are read from a file containing
    precomputed values calculated using Mie theory. Sebastian Wolf's Mie program MieX was used to
    do the computations (it allows for large grains). This dust mixture is similar to the
    DraineSilicateGrainComposition class, except that the current class contains optical properties
    over a much wider grain size range: the current dust mixture contains 301 grain sizes ranging
    from 1 nm to 1 mm, whereas DraineSilicateGrainComposition contains 81 grain sizes from 1 nm to
    10 micron. The calorimetric properties follow the prescription of Draine & Li (2001). */
class MieSilicateGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Mie-based Draine silicate dust grains composition")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE MieSilicateGrainComposition();

    /** This constructor can be invoked by dust mix classes that wish to hard-code the creation of
        a new grain composition object of this type (as opposed to creation through the ski file).
        Before the constructor returns, the newly created object is hooked up as a child to the
        specified parent in the simulation hierarchy (so it will automatically be deleted), and
        it's setup() function has been called. */
    explicit MieSilicateGrainComposition(SimulationItem* parent);

protected:
    /** This function reads the raw optical property data from a resource file, calculates the
        enthalpy data using the analytical function for silicate derived in Draine & Li 2001, and
        sets the bulk mass density to the standard value of 3000 kg/m\f$^3\f$ for silicate grains.
        */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // MIESILICATEGRAINCOMPOSITION_HPP
