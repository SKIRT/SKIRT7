/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DraineGraphiteGrainComposition.hpp"
#include "DraineSilicateGrainComposition.hpp"
#include "MRNDustMix.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

MRNDustMix::MRNDustMix()
    : _Ngra(0), _Nsil(0)
{
}

//////////////////////////////////////////////////////////////////////

namespace
{
    // MRN grain size distributions with values taken from Weingartner & Draine (2001, ApJ, 548, 296) page 296
    const double amin = 5e-9;          // 50 Armstrong
    const double amax = 250e-9;        // 0.25 micron
    const double Cg = pow(10,-25.13) * 1e-5;  // convert value in cm^2.5 to m^2.5
    const double Cs = pow(10,-25.11) * 1e-5;  // convert value in cm^2.5 to m^2.5

    double dnda_gra(double a)
    {
        return Cg * pow(a,-3.5);
    }

    double dnda_sil(double a)
    {
        return Cs * pow(a,-3.5);
    }
}

//////////////////////////////////////////////////////////////////////

void MRNDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    addpopulations(new DraineGraphiteGrainComposition(this), amin, amax, &dnda_gra, _Ngra);
    addpopulations(new DraineSilicateGrainComposition(this), amin, amax, &dnda_sil, _Nsil);
}

////////////////////////////////////////////////////////////////////

void MRNDustMix::setGraphitePops(int value)
{
    _Ngra = value;
}

////////////////////////////////////////////////////////////////////

int MRNDustMix::graphitePops() const
{
    return _Ngra;
}

////////////////////////////////////////////////////////////////////

void MRNDustMix::setSilicatePops(int value)
{
    _Nsil = value;
}

////////////////////////////////////////////////////////////////////

int MRNDustMix::silicatePops() const
{
    return _Nsil;
}

//////////////////////////////////////////////////////////////////////
