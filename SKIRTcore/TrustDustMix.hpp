/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TRUSTDUSTMIX_HPP
#define TRUSTDUSTMIX_HPP

#include "MultiGrainDustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The TrustDustMix class is a subclass of the MultiGrainDustMix class and represents a realistic
    dust mixture of bare (i.e. non-composite) graphite, silicate, and PAH dust grains according to
    the dust model used for the TRUST benchmark simulations. The underlying data is provided by
    Karel Misselt as part of a download from the TRUST web site
    (http://ipag.osug.fr/RT13/RTTRUST/opa.php) describing the BARE-GR-S model of Zubko, Dwek, and
    Arendt 2004, ApJS, 152, 211.

    The graphite, silicate, and PAH populations can be subdivided into \f$N_{\text{gra}}\f$,
    \f$N_{\text{sil}}\f$, and \f$N_{\text{PAH}}\f$ subpopulations, each corresponding to a distinct
    grain size bin. */
class TrustDustMix : public MultiGrainDustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a multi-component dust mix from the TRUST benchmark")

    Q_CLASSINFO("Property", "graphitePops")
    Q_CLASSINFO("Title", "the number of graphite subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "silicatePops")
    Q_CLASSINFO("Title", "the number of silicate subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "PAHPops")
    Q_CLASSINFO("Title", "the number of PAH subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TrustDustMix();

protected:
    /** This function adds the requested number of dust populations based on the
        TrustGraphiteGrainComposition, TrustSilicateGrainComposition, and
        TrustNeutralPAHGrainComposition grain composition classes. The grain size distributions for
        the various populations are given as a complicated analytical formula, parameterized
        differently depending on the grain composition, which can be found in Zubko, Arendt & Dwek
        (2004, ApJS, 152, 211). */
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

    /** Sets the number of PAH subpopulations. */
    Q_INVOKABLE void setPAHPops(int value);

    /** Returns the number of PAH subpopulations. */
    Q_INVOKABLE int PAHPops() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    int _Ngra;
    int _Nsil;
    int _Npah;
};

////////////////////////////////////////////////////////////////////

#endif // TRUSTDUSTMIX_HPP
