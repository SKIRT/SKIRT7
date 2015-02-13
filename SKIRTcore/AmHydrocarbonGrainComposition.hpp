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

protected:
    /** This function reads the raw optical and calorimetric data from resource files, and sets the
        bulk mass density to the value of 1.5 g/cm\f$^3\f$ specified by Jones. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // AMHYDROCARBONGRAINCOMPOSITION_HPP
