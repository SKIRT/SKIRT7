/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTMIXPOPULATION_HPP
#define DUSTMIXPOPULATION_HPP

#include "SimulationItem.hpp"
class GrainComposition;
class GrainSizeDistribution;

////////////////////////////////////////////////////////////////////

/** DustMixPopulation is simple class that represents a single dust population for use with the
    ConfigurableDustMix class. It combines a dust grain composition type (an instance of a
    GrainComposition subclass), a dust grain size distribution (an instance of a
    GrainSizeDistribution subclass) to define a particular dust population. In addition, it
    provides the option to split the grain size distribution into \f$N_\text{bins}\f$ bins on a
    logarithmic scale, configuring a seperate dust population for each bin. For more information
    see the MultiGrainDustMix::addpopulations() function. */
class DustMixPopulation : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix population")

    Q_CLASSINFO("Property", "composition")
    Q_CLASSINFO("Title", "the dust grain composition")
    Q_CLASSINFO("Default", "DraineGraphiteGrainComposition")

    Q_CLASSINFO("Property", "sizeDistribution")
    Q_CLASSINFO("Title", "the dust grain size distribution")
    Q_CLASSINFO("Default", "PowerLawGrainSizeDistribution")

    Q_CLASSINFO("Property", "subPops")
    Q_CLASSINFO("Title", "the number of subpopulations (splitting the size distribution)")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE DustMixPopulation();

protected:
    /** This function verifies the attribute values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the dust grain composition. */
    Q_INVOKABLE void setComposition(GrainComposition* value);

    /** Returns the dust grain composition. */
    Q_INVOKABLE GrainComposition* composition() const;

    /** Sets the dust grain size distribution. */
    Q_INVOKABLE void setSizeDistribution(GrainSizeDistribution* value);

    /** Returns the dust grain size distribution. */
    Q_INVOKABLE GrainSizeDistribution* sizeDistribution() const;

    /** Sets the number of subpopulations \f$N_\text{bins}\f$. */
    Q_INVOKABLE void setSubPops(int value);

    /** Returns the number of subpopulations \f$N_\text{bins}\f$. */
    Q_INVOKABLE int subPops() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    GrainComposition* _gc;
    GrainSizeDistribution* _gs;
    int _Nbins;
};

////////////////////////////////////////////////////////////////////

#endif // DUSTMIXPOPULATION_HPP
