/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ACHCOREMANGRAINCOMPOSITION_HPP
#define ACHCOREMANGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The aCHcoremanGrainComposition class represents the optical properties of a-C(:H) dust grains
    (carbonaceous grains) from Jones et al. 2013 (A&A, 558, A62). The calorimetric properties are
    calculated in DustEM. */
class aCHcoremanGrainComposition: public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "Jones+2013 aCH dust grain composition")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE aCHcoremanGrainComposition();

protected:
    /** This function reads the raw optical and calorimetric data from resource files, and sets the
        bulk mass density to the value of 1.5 g/cm\f$^3\f$ specified by Jones. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // ACHCOREMANGRAINCOMPOSITION_HPP
