/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DraineGraphiteGrainComposition.hpp"
#include "SpecialFunctions.hpp"
#include "Units.hpp"

//////////////////////////////////////////////////////////////////////

DraineGraphiteGrainComposition::DraineGraphiteGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

DraineGraphiteGrainComposition::DraineGraphiteGrainComposition(SimulationItem *parent)
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

double DraineGraphiteGrainComposition::enthalpyfunction(double T)
{
    // equation (9) of Draine & Li (2001), integrated to obtain internal energy rather than heat capacity,
    // provides the enthalpy for a single atom (approximating N-2 in the equation by N)
    const double TD1 = 863.;
    const double TD2 = 2504.;
    double h = Units::k() * ( TD1*fn(2,T/TD1) + 2.*TD2*fn(2,T/TD2) );

    // convert to enthalpy per kg
    const double A_r = 12.0107;     // relative atomic mass for carbon (in atomic mass units == 1 g/mol)
    const double M_u = 1e-3;        // g -> kg
    return h * Units::NA() / (A_r * M_u);
}

//////////////////////////////////////////////////////////////////////

void DraineGraphiteGrainComposition::setupSelfBefore()
{
    loadOpticalGrid("GrainComposition/Gra_81.dat", true, false, false, false);
    calculateEnthalpyGrid(enthalpyfunction);
    setBulkDensity(2.24e3);
}

//////////////////////////////////////////////////////////////////////

QString DraineGraphiteGrainComposition::name() const
{
    return "Draine_Graphite";
}

//////////////////////////////////////////////////////////////////////
