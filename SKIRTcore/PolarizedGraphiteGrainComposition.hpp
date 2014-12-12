/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef POLARIZEDGRAPHITEGRAINCOMPOSITION_HPP
#define POLARIZEDGRAPHITEGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The PolarizedGraphiteGrainComposition class represents the optical and calorimetric properties
    of graphite dust grains with support for polarization. The optical data, including scattering and
    absorption efficiency coefficients and Mueller matrix coefficients, are read from an
    appropriate resource file in the format as used by the STOKES code. The calorimetric properties
    follow the prescription of Draine & Li (2001). */
class PolarizedGraphiteGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a graphite dust grain composition with support for polarization")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PolarizedGraphiteGrainComposition();

protected:
    /** This function reads the optical and calorimetric properties from the appropriate resource files
        and sets the appropriate bulk mass density value. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;
};

////////////////////////////////////////////////////////////////////

#endif // POLARIZEDGRAPHITEGRAINCOMPOSITION_HPP
