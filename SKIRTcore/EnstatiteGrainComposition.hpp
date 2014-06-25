/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ENSTATITEGRAINCOMPOSITION_HPP
#define ENSTATITEGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The EnstatiteGrainComposition class represents the optical properties of Enstatite dust grains
    (crystalline silicate MgSiO3). Michiel Min <michiel.min.science@gmail.com> prepared the data.
    The refractive index data was taken from Jaeger et al. 1998. Further data was obtained from the
    Jena group (Fabian 2001, Zeidler 2011) for UV to near-IR. Below 0.2 micron the results are
    extrapolated using theoretical formulas. The calculations were performed with DHS using
    \f$f_\mathrm{max}=0.8\f$ (see Min et al. 2005). The calorimetric properties are taken from the
    DustEM data included with SKIRT (see the DustEmGrainComposition class). */
class EnstatiteGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an Enstatite dust grains composition")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE EnstatiteGrainComposition();

protected:
    /** This function reads the raw optical and calorimetric data from resource files, and sets the
        bulk mass density to the value of 2800 kg/m\f$^3\f$ specified by Min for Enstatite. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // ENSTATITEGRAINCOMPOSITION_HPP
