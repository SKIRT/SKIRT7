/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ZUBKODUSTMIX_HPP
#define ZUBKODUSTMIX_HPP

#include "MultiGrainDustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The ZubkoDustMix class is a subclass of the MultiGrainDustMix class and represents a realistic
    dust mixture of bare (i.e. non-composite) graphite, silicate, neutral PAH and ionized PAH dust
    grains. The size distribution of each of these dust grain populations is finetuned in such a
    way that the global dust properties accurately reproduce the extinction, emission and abundance
    constraints on the Milky Way. The size distributions are taken from Zubko, Dwek & Arendt (2004,
    ApJS, 152, 211), model BARE_GR_S. The graphite, silicate, neutral PAH and ionized PAH
    populations can be subdivided into \f$N_{\text{gra}}\f$, \f$N_{\text{sil}}\f$, and
    \f$N_{\text{PAH}}\f$ subpopulations, each corresponding to a distinct grain size bin. */
class ZubkoDustMix : public MultiGrainDustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Zubko et al. multi-component dust mix")

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

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ZubkoDustMix();

protected:
    /** This function adds the requested number of dust populations based on the
        DraineGraphiteGrainComposition, DraineSilicateGrainComposition,
        DraineNeutralPAHGrainComposition, and DraineIonizedPAHGrainComposition grain composition
        classes. It is assumed that 50% of the PAH grains are neutral and 50% are ionized. The
        grain size distributions for the various populations are given as a complicated analytical
        formula, parameterized differently depending on the grain composition, which can be found
        in Zubko, Arendt & Dwek (2004, ApJS, 152, 211). */
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

    //======================== Data Members ========================

private:
    // discoverable attributes
    int _Ngra;
    int _Nsil;
    int _Npah;
};

////////////////////////////////////////////////////////////////////

#endif // ZUBKODUSTMIX_HPP
