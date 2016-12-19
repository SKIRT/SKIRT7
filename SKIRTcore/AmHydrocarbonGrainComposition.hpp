/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef AMHYDROCARBONGRAINCOMPOSITION_HPP
#define AMHYDROCARBONGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The AmHydrocarbonGrainComposition class represents the optical properties of a-C(:H) dust grains
    (carbonaceous grains) from Jones et al. 2013 (A&A, 558, A62). The calorimetric properties are
    calculated in DustEM. */
class AmHydrocarbonGrainComposition: public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "Jones et al. 2013 amorphous hydrocarbon dust grain composition")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE AmHydrocarbonGrainComposition();

    /** This constructor can be invoked by dust mix classes that wish to hard-code the creation of
        a new grain composition object of this type (as opposed to creation through the ski file).
        Before the constructor returns, the newly created object is hooked up as a child to the
        specified parent in the simulation hierarchy (so it will automatically be deleted), and
        its setup() function has been called. */
    explicit AmHydrocarbonGrainComposition(SimulationItem* parent);

protected:
    /** This function reads the raw optical and calorimetric data from resource files, and sets the
        bulk mass density to the value of 1.6 g/cm\f$^3\f$ specified by Jones. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // AMHYDROCARBONGRAINCOMPOSITION_HPP
