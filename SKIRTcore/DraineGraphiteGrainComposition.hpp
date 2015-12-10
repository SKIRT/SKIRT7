/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DRAINEGRAPHITEGRAINCOMPOSITION_HPP
#define DRAINEGRAPHITEGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The DraineGraphiteGrainComposition class represents the optical and calorimetric properties of
    graphite dust grains. The optical properties are taken from Bruce Draine's website, the
    calorimetric properties follow the prescription of Draine & Li (2001). */
class DraineGraphiteGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Draine graphite dust grains composition")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE DraineGraphiteGrainComposition();

    /** This constructor can be invoked by dust mix classes that wish to hard-code the creation of
        a new grain composition object of this type (as opposed to creation through the ski file).
        Before the constructor returns, the newly created object is hooked up as a child to the
        specified parent in the simulation hierarchy (so it will automatically be deleted), and
        its setup() function has been called. */
    explicit DraineGraphiteGrainComposition(SimulationItem* parent);

    /** This static function calculates and returns the specific enthalpy of graphite according to
        equation (9) of Draine & Li (2001), properly integrated to obtain internal energy rather
        than heat capacity. It serves as a call-back for the calculateEnthalpyGrid() function
        defined in the GrainComposition class. It is declared public so that it can also be used by
        other grain composition classes. */
    static double enthalpyfunction(double T);

protected:
    /** This function reads the raw optical property data from a resource file, calculates the
        enthalpy data using the analytical function for graphite derived in Draine & Li 2001, and
        sets the bulk mass density to the standard value of 2240 kg m\f$^{-3}\f$ for graphite grains.
        */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // DRAINEGRAPHITEGRAINCOMPOSITION_HPP
