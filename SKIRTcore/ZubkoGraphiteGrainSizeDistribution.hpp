/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ZUBKOGRAPHITEGRAINSIZEDISTRIBUTION_HPP
#define ZUBKOGRAPHITEGRAINSIZEDISTRIBUTION_HPP

#include "GrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** ZubkoGraphiteGrainSizeDistribution represents the dust grain size distribution for the graphite
    population in model BARE_GR_S of Zubko, Dwek & Arendt (2004, ApJS, 152, 211). The size range
    configured in the GrainSizeDistribution base class is ignored and replaced by the appropriate
    built-in values. The proportionality factor \f$C\f$ configured in the GrainSizeDistribution
    base class is combined with the built-in front-factor. It should usually be set to the default
    value of \f$C=1\f$. */
class ZubkoGraphiteGrainSizeDistribution: public GrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Zubko, Dwek & Arendt size distribution for graphite dust grains")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ZubkoGraphiteGrainSizeDistribution();

    /** This constructor can be invoked by dust mix classes that wish to hard-code the creation of
        a new grain size distribution object of this type (as opposed to creation through the ski
        file). Before the constructor returns, the newly created object is hooked up as a child to
        the specified parent in the simulation hierarchy (so it will automatically be deleted), and
        it's setup() function has been called. The optional second argument specifies the
        proportionality factor configured in the GrainSizeDistribution base class, with a default
        value of 1. */
    explicit ZubkoGraphiteGrainSizeDistribution(SimulationItem* parent, double C = 1.);

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function implements the missing part of the GrainSizeDistributionInterface. It returns
        the value of \f$\Omega(a)\f$ as described in the header for this class. */
    double dnda(double a) const;
};

////////////////////////////////////////////////////////////////////

#endif // ZUBKOGRAPHITEGRAINSIZEDISTRIBUTION_HPP
