/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef WEINGARTNERDRAINEDUSTMIX_HPP
#define WEINGARTNERDRAINEDUSTMIX_HPP

#include "MultiGrainDustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The WeingartnerDraineDustMix class is a subclass of the MultiGrainDustMix class and represents
    realistic dust mixtures consisting of populations of graphite, silicate and PAH dust grains.
    The size distributions of each of these grains are fitted in such a way that they can reproduce
    the extinction curve of the Milky Way or the LMC. For details we refer to Weingartner & Draine
    (2001, ApJ, 548, 296). The graphite, silicate, and PAH populations (for both neutral and
    ionized PAHs) can be subdivided into \f$N_{\text{gra}}\f$, \f$N_{\text{sil}}\f$, and
    \f$N_{\text{PAH}}\f$ subpopulations, each corresponding to a distinct grain size bin. */
class WeingartnerDraineDustMix : public MultiGrainDustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Weingartner & Draine multi-component dust mix")

    Q_CLASSINFO("Property", "graphitePops")
    Q_CLASSINFO("Title", "the number of graphite subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "silicatePops")
    Q_CLASSINFO("Title", "the number of silicate subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "PAHPops")
    Q_CLASSINFO("Title", "the number of PAH subpopulations (for both neutral and ionized PAHs)")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "environment")
    Q_CLASSINFO("Title", "the typical environment for the dust")
    Q_CLASSINFO("MilkyWay", "the Milky Way")
    Q_CLASSINFO("LMC", "the Large Magellanic Cloud")
    Q_CLASSINFO("Default", "MilkyWay")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE WeingartnerDraineDustMix();

protected:
    /** This function adds the requested number of dust populations based on the
        DraineGraphiteGrainComposition, DraineSilicateGrainComposition,
        DraineNeutralPAHGrainComposition, and DraineIonizedPAHGrainComposition grain composition
        classes. It is assumed that 50% of the PAH grains are neutral and 50% are ionized. The
        grain size distributions for the various populations are given as complicated analytical
        formulas with predefined constant parameters depending on the selected environment (Milky
        Way or LMC). For the graphite and silicate populations, the distribution is a power-law
        function with a curvature and an exponential cutoff. For the PAH populations, it is the sum
        of two log-normal distributions, cut off at some upper grain size. This represents the
        common distribution for neutral and ionized PAH grains; to obtain the distribution for one
        of both subpopulations, we need to divide by two (assuming an ionization fraction of 50%).
        The exact forms of the grain size distribution functions can be found in Weingartner &
        Draine (2001, ApJ, 548, 296). */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of graphite subpopulations. */
    Q_INVOKABLE void setGraphitePops(int value);

    /** Returns the number of graphite subpopulations. */
    Q_INVOKABLE int graphitePops() const;

    /** Sets the number of silicate subpopulations. */
    Q_INVOKABLE void setSilicatePops(int value);

    /** Returns the number of silicate subpopulations. */
    Q_INVOKABLE int silicatePops() const;

    /** Sets the number of PAH subpopulations (for both neutral and ionized PAHs). */
    Q_INVOKABLE void setPAHPops(int value);

    /** Returns the number of PAH subpopulations (for both neutral and ionized PAHs). */
    Q_INVOKABLE int PAHPops() const;

    /** The enumeration type indicating the environment for the Weingartner-Draine dust. */
    Q_ENUMS(Environment)
    enum Environment { MilkyWay, LMC };

    /** Sets the typical environment for the dust. */
    Q_INVOKABLE void setEnvironment(Environment value);

    /** Returns the typical environment for the dust. */
    Q_INVOKABLE Environment environment() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    Environment _env;
    int _Ngra;
    int _Nsil;
    int _Npah;
};

////////////////////////////////////////////////////////////////////

#endif // WEINGARTNERDRAINEDUSTMIX_HPP
