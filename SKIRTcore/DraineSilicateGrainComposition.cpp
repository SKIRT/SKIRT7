/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DraineSilicateGrainComposition.hpp"
#include "SpecialFunctions.hpp"
#include "Units.hpp"

//////////////////////////////////////////////////////////////////////

DraineSilicateGrainComposition::DraineSilicateGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

DraineSilicateGrainComposition::DraineSilicateGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

namespace
{
    // function f_n(x) as defined in equation (10) of Draine & Li (2001)
    // expressed in terms of the Debye function (perform substitution t = y/x)
    double fn(int n, double x)
    {
        return SpecialFunctions::DebyeD(n, 1./x) * x / (n*n);
    }
}

//////////////////////////////////////////////////////////////////////

double DraineSilicateGrainComposition::enthalpyfunction(double T)
{
    // equation (11) of Draine & Li (2001), integrated to obtain internal energy rather than heat capacity,
    // provides the enthalpy for a single atom (approximating N-2 in the equation by N)
    const double TD1 = 500.;
    const double TD2 = 1500.;
    double h = Units::k() * ( 2.*TD1*fn(2,T/TD1) + TD2*fn(3,T/TD2) );

    // convert to enthalpy per kg
    const double Msilicate = (24.3050+55.845+28.0855+15.9994*4)/7;     // mass of MgFeSiO4 in units of M_p
    return h / (Msilicate * Units::massproton());
}

//////////////////////////////////////////////////////////////////////

void DraineSilicateGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    loadOpticalGrid(true, "GrainComposition/Draine/suvSil_81.dat", true, false, false, false);
    calculateEnthalpyGrid(enthalpyfunction);
    setBulkDensity(3.0e3);
}

//////////////////////////////////////////////////////////////////////

QString DraineSilicateGrainComposition::name() const
{
    return "Draine_Silicate";
}

//////////////////////////////////////////////////////////////////////
