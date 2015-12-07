/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef THEMISDUSTMIX_HPP
#define THEMISDUSTMIX_HPP

#include "MultiGrainDustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The ThemisDustMix class is a subclass of the MultiGrainDustMix class and represents dust mixtures
    consisting of separate populations of graphite and silicate dust grains with a grain size
    distribution according to the famous MRN distribution (Mathis, Rumpl & Nordsieck 1977, ApJ,
    217, 425). The actual size distributions are taken from Weingartner & Draine (2001, ApJ, 548,
    296), the optical properties are taken from Bruce Draine's web site. The graphite and silicate
    populations can be subdivided into \f$N_{\text{gra}}\f$ and \f$N_{\text{sil}}\f$
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
        DraineGraphiteGrainComposition and DraineSilicateGrainComposition grain composition classes
        for graphite and silicate respectively, and on grain size distributions given by \f[
        \frac{\text{d}n}{\text{d}a} = C\, a^{-3.5} \qquad \text{for}\quad a_\text{min} \leq a \leq
        a_\text{max}, \f] with \f$C=10^{-25.13}\,\text{cm}^{2.5}\f$ for graphite and
        \f$C=10^{-25.11}\,\text{cm}^{2.5}\f$ for silicate, and with
        \f$a_\text{min}=50\,\text{\AA}\f$ and \f$a_\text{max}=0.25\,\mu\text{m}\f$ for both grain
        types. */
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
