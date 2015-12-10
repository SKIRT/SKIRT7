/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef THEMISDUSTMIX_HPP
#define THEMISDUSTMIX_HPP

#include "MultiGrainDustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The ThemisDustMix class is a subclass of the MultiGrainDustMix class and represents the THEMIS
    model for dust in the diffuse interstellar medium described by Jones et al. (2015, A&A,
    submitted) and the many references therein. In this model, there are two families of dust
    particles: amorphous hydrocarbon and amorphous silicates. For the amorphous hydrocarbon dust,
    the size distribution is a combination of a lognormal and a power-law distribution. For the
    silicates, it is assumed that 50\% of the mass is amorphous enstatite, and that the remaining
    half is amorphous forsterite, and the size distribution is a lognormal distribution (the same
    for both). The three populations in the mixture (hydrocarbon dust, enstatite and forsterite)
    can be subdivided into \f$N_{\text{ahc}}\f$, \f$N_{\text{ens}}\f$ and \f$N_{\text{for}}\f$
    subpopulations, each corresponding to a distinct grain size bin. */
class ThemisDustMix : public MultiGrainDustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a THEMIS multi-component dust mix")

    Q_CLASSINFO("Property", "hydrocarbonPops")
    Q_CLASSINFO("Title", "the number of amorphous hydrocarbon subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "enstatitePops")
    Q_CLASSINFO("Title", "the number of enstatite subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "forsteritePops")
    Q_CLASSINFO("Title", "the number of forsterite subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ThemisDustMix();

protected:
    /** This function adds the requested number of dust populations based on the
        AmHydrocarbonGrainComposition, EnstatiteGrainComposition and ForsteriteGrainComposition
        grain composition classes, and on the appropriate grain size distributions. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of amorphous hydrocarbon subpopulations. */
    Q_INVOKABLE void setHydrocarbonPops(int value);

    /** Returns the number of amorphous hydrocarbon subpopulations. */
    Q_INVOKABLE int hydrocarbonPops() const;

    /** Sets the number of enstatite subpopulations. */
    Q_INVOKABLE void setEnstatitePops(int value);

    /** Returns the number of enstatite subpopulations. */
    Q_INVOKABLE int enstatitePops() const;

    /** Sets the number of forsterite subpopulations. */
    Q_INVOKABLE void setForsteritePops(int value);

    /** Returns the number of forsterite subpopulations. */
    Q_INVOKABLE int forsteritePops() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    int _Nahc;
    int _Nens;
    int _Nfor;
};

////////////////////////////////////////////////////////////////////

#endif // THEMISDUSTMIX_HPP

