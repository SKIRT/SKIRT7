/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AmHydrocarbonGrainComposition.hpp"
#include "EnstatiteGrainComposition.hpp"
#include "ForsteriteGrainComposition.hpp"
#include "ThemisDustMix.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ThemisDustMix::ThemisDustMix()
    : _Nahc(0), _Nsil(0)
{
}

//////////////////////////////////////////////////////////////////////

namespace
{
    const double aminhpl = 0.0004e-6;
    const double aminhln = 0.0005e-6;
    const double amins = 0.001e-6;
    const double amax = 4.9e-6;

    // the power-law size distribution for the hydrocarbons
    double dndahpl(double a)
    {
        const double alpha = -5.0;
        const double at = 0.01e-6;
        const double ac = 0.05e-6;
        const double C = 1.71726298266e-43;

        if (a < aminhpl || a > amax) return 0.0;
        return C * pow(a,alpha) * ( (a<=at) ? 1.0 : exp(-(a-at)/ac) );
    }

    // the lognormal size distribution for the hydrocarbons
    double dndahln(double a)
    {
        const double a0 = 0.007e-6;
        const double C = 2.05052478683e-12;

        if (a < aminhln || a > amax) return 0.0;
        double x = log(a/a0);
        return C/a * exp(-0.5*x*x);
    }

    // lognormal size distribution for the silicates (the same distribution for enstatite and forsterite)
    double dndas(double a)
    {
        const double a0 = 0.008e-6;
        const double C = 4.02595019205e-12;

        if (a < amins || a > amax) return 0.0;
        double x = log(a/a0);
        return C/a * exp(-0.5*x*x);
    }
}

//////////////////////////////////////////////////////////////////////

void ThemisDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    AmHydrocarbonGrainComposition* gchpl = new AmHydrocarbonGrainComposition(this, 1600.);
    AmHydrocarbonGrainComposition* gchln = new AmHydrocarbonGrainComposition(this, 1510.);
    EnstatiteGrainComposition* gcens = new EnstatiteGrainComposition(this, EnstatiteGrainComposition::Amorphous);
    ForsteriteGrainComposition* gcfor = new ForsteriteGrainComposition(this, ForsteriteGrainComposition::Amorphous);

    addpopulations(gchpl, aminhpl, amax, &dndahpl, _Nahc);
    addpopulations(gchln, aminhln, amax, &dndahln, _Nahc);
    addpopulations(gcens, amins, amax, &dndas, _Nsil);
    addpopulations(gcfor, amins, amax, &dndas, _Nsil);
}

////////////////////////////////////////////////////////////////////

void ThemisDustMix::setHydrocarbonPops(int value)
{
    _Nahc = value;
}

////////////////////////////////////////////////////////////////////

int ThemisDustMix::hydrocarbonPops() const
{
    return _Nahc;
}

////////////////////////////////////////////////////////////////////

void ThemisDustMix::setSilicatePops(int value)
{
    _Nsil = value;
}

////////////////////////////////////////////////////////////////////

int ThemisDustMix::silicatePops() const
{
    return _Nsil;
}

//////////////////////////////////////////////////////////////////////
