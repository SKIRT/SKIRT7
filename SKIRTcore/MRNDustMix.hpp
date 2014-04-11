/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MRNDUSTMIX_HPP
#define MRNDUSTMIX_HPP

#include "MultiGrainDustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The MRNDustMix class is a subclass of the MultiGrainDustMix class and represents dust mixtures
    consisting of separate populations of graphite and silicate dust grains with a grain size
    distribution according to the famous MRN distribution (Mathis, Rumpl & Nordsieck 1977, ApJ,
    217, 425). The actual size distributions are taken from Weingartner & Draine (2001, ApJ, 548,
    296), the optical properties are taken from Bruce Draine's web site. The graphite and silicate
    populations can be subdivided into \f$N_{\text{gra}}\f$ and \f$N_{\text{sil}}\f$
    subpopulations, each corresponding to a distinct grain size bin. */
class MRNDustMix : public MultiGrainDustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an MRN multi-component dust mix")

    Q_CLASSINFO("Property", "graphitePops")
    Q_CLASSINFO("Title", "the number of graphite subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "silicatePops")
    Q_CLASSINFO("Title", "the number of silicate subpopulations")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("Default", "5")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE MRNDustMix();

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
    /** Sets the number of graphite subpopulations. */
    Q_INVOKABLE void setGraphitePops(int value);

    /** Returns the number of graphite subpopulations. */
    Q_INVOKABLE int graphitePops() const;

    /** Sets the number of silicate subpopulations. */
    Q_INVOKABLE void setSilicatePops(int value);

    /** Returns the number of silicate subpopulations. */
    Q_INVOKABLE int silicatePops() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    int _Ngra;
    int _Nsil;
};

////////////////////////////////////////////////////////////////////

#endif // MRNDUSTMIX_HPP
