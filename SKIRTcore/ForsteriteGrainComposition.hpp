/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FORSTERITEGRAINCOMPOSITION_HPP
#define FORSTERITEGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The ForsteriteGrainComposition class represents the optical properties of Forsterite dust
    grains (crystalline silicate Mg2SiO4). Michiel Min <michiel.min.science@gmail.com> prepared the
    data. The refractive index data was taken from Suto et al. 2006, using the lowest temperature
    50K. Further data was obtained from the Jena group (Fabian 2001, Zeidler 2011) for UV to
    near-IR. Below 0.2 micron the results are extrapolated using theoretical formulas. The
    calculations were performed with DHS using \f$f_\mathrm{max}=0.8\f$ (see Min et al. 2005). The
    calorimetric properties are taken from the DustEM data included with SKIRT (see the
    DustEmGrainComposition class). */
class ForsteriteGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Forsterite dust grains composition")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ForsteriteGrainComposition();

protected:
    /** This function reads the raw optical and calorimetric data from resource files, and sets the
        bulk mass density to the value of 3330 kg/m\f$^3\f$ specified by Min for Forsterite. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // FORSTERITEGRAINCOMPOSITION_HPP
