/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef CMAMENSTATITEGRAINCOMPOSITION_HPP
#define CMAMENSTATITEGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The CMamEnstatiteGrainComposition class represents the optical properties of amorphous
    silicates with enstatite-normative composition from Kohler et al. 2014 (A&A, 565, L9), and,
    together with the amorphous silicates with forsterite-normative composition replace the
    silicate grains of Jones et al. 2013 (A&A, 558, A62). The calorimetric properties are calculated
    in DustEM. */
class CMamEnstatiteGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "Koehler+2014 CMamEnstatite dust grain composition")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE CMamEnstatiteGrainComposition();

protected:
    /** This function reads the raw optical and calorimetric data from resource files, and sets the
        bulk mass density to the value of 1.6 g/cm\f$^3\f$ specified by Kohler for Enstatite. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // CMAMENSTATITEGRAINCOMPOSITION_HPP
