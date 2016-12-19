/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ENSTATITEGRAINCOMPOSITION_HPP
#define ENSTATITEGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The EnstatiteGrainComposition class represents the optical properties of Enstatite dust grains,
    in two different forms:

    - \em Crystalline silicate MgSiO3 grains, for which Michiel Min <michiel.min.science@gmail.com>
      prepared the data. The refractive index data was taken from Jaeger et al. 1998. Further data was
      obtained from the Jena group (Fabian 2001, Zeidler 2011) for UV to near-IR. Below 0.2 micron the
      results are extrapolated using theoretical formulas. The calculations were performed with DHS
      using \f$f_\mathrm{max}=0.8\f$ (see Min et al. 2005). The calorimetric properties are taken from
      the DustEM data included with <tt>SKIRT</tt> (see the DustEmGrainComposition class).

    - \em Amorphous silicates with enstatite-normative composition from Köhler et al. 2014 (A&A,
      565, L9). Together with the amorphous silicates with forsterite-normative composition, they
      replace the silicate grains of Jones et al. 2013 (A&A, 558, A62). The calorimetric properties
      are calculated in DustEM. */
class EnstatiteGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an Enstatite dust grain composition")

    Q_CLASSINFO("Property", "type")
    Q_CLASSINFO("Title", "the type of Enstatite grains")
    Q_CLASSINFO("Crystalline", "crystalline")
    Q_CLASSINFO("Amorphous", "amorphous")
    Q_CLASSINFO("Default", "Crystalline")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE EnstatiteGrainComposition();

    /** The enumeration type indicating the type of Enstatite grains. */
    Q_ENUMS(GrainType)
    enum GrainType { Crystalline, Amorphous };

    /** This constructor can be invoked by dust mix classes that wish to hard-code the creation of
        a new grain composition object of this type (as opposed to creation through the ski file).
        Before the constructor returns, the newly created object is hooked up as a child to the
        specified parent in the simulation hierarchy (so it will automatically be deleted), and
        its setup() function has been called. */
    explicit EnstatiteGrainComposition(SimulationItem* parent, GrainType type);

protected:
    /** This function reads the raw optical and calorimetric data from resource files, and sets the
        bulk mass density to the value of 2800 kg m\f$^{-3}\f$ specified by Min for crystalline enstatite
        and 2190 kg m\f$^{-3}\f$ specified by Köhler for amorphous enstatite. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the enumeration value indicating the type of Enstatite grains to be used. */
    Q_INVOKABLE void setType(GrainType value);

    /** Returns the enumeration value indicating the type of Enstatite grains to be used. */
    Q_INVOKABLE GrainType type() const;

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;

    //======================== Data Members ========================

private:
    GrainType _type;
};

////////////////////////////////////////////////////////////////////

#endif // ENSTATITEGRAINCOMPOSITION_HPP
