/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ZUBKOPAHGRAINSIZEDISTRIBUTION_HPP
#define ZUBKOPAHGRAINSIZEDISTRIBUTION_HPP

#include "GrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** ZubkoPAHGrainSizeDistribution represents the dust grain size distribution and grain size range
    for the PAH population in model BARE_GR_S of Zubko, Dwek & Arendt (2004, ApJS, 152, 211). The
    proportionality factor \f$C\f$ configured in the GrainSizeDistribution base class is combined
    with the built-in front-factor. It should usually be set to the default value of \f$C=1\f$,
    unless the dust mix includes multiple types of PAH molecules. For example, if there is a
    neutral and ionized PAH population, one could set \f$C=0.5\f$ for each. */
class ZubkoPAHGrainSizeDistribution: public GrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Zubko, Dwek & Arendt size distribution for PAH molecules")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ZubkoPAHGrainSizeDistribution();

    /** This constructor can be invoked by dust mix classes that wish to hard-code the creation of
        a new grain size distribution object of this type (as opposed to creation through the ski
        file). Before the constructor returns, the newly created object is hooked up as a child to
        the specified parent in the simulation hierarchy (so it will automatically be deleted), and
        it's setup() function has been called. The optional second argument specifies the
        proportionality factor configured in the GrainSizeDistribution base class, with a default
        value of 1. */
    explicit ZubkoPAHGrainSizeDistribution(SimulationItem* parent, double C = 1.);

    //======================== Other Functions =======================

public:
    /** This function implements part of the GrainSizeDistributionInterface. It returns the
        built-in minimum grain size \f$a_\text{min}\f$ as described in the header for this class.
        */
    double amin() const;

    /** This function implements part of the GrainSizeDistributionInterface. It returns the
        built-in maximum grain size \f$a_\text{max}\f$ as described in the header for this class.
        */
    double amax() const;

    /** This function implements the missing part of the GrainSizeDistributionInterface. It returns
        the built-in value of \f$\Omega(a)\f$ as described in the header for this class. */
    double dnda(double a) const;
};

////////////////////////////////////////////////////////////////////

#endif // ZUBKOPAHGRAINSIZEDISTRIBUTION_HPP
