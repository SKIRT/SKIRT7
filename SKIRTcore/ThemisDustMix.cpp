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
    : _Nahc(0), _Nens(0), _Nfor(0)
{
}

//////////////////////////////////////////////////////////////////////

namespace
{
    const double amax = 4.9e-6;
    const double amins = 0.001e-6;
    const double centroids = 0.008e-6;
    const double Cs = 3.80303826e-10;
    const double aminh1 = 0.0005e-6;
    const double centroidh = 0.007e-6;
    const double Ch1 = 3.80303826e-10;
    const double aminh2 = 0.0004e-6;
    const double alpha = -5.0;
    const double at = 0.01e-6;
    const double ac = 0.05e-6;
    const double Ch2 = 1.53786339e-41;
    const double aminh = min(aminh1,aminh2);

    // amorphous hydrocarbons
    double dndah(double a)
    {
        double dnda = 0.0;
        if (a > aminh1 && a < amax)
        {
            double x = log(a/centroidh);
            dnda += Ch1 / a * exp( -0.5*x*x );
        }
        if (a > aminh2 && a < amax)
        {
            double fed = (a<at) ? 1 : exp(-(a-at)/ac);
            dnda += Ch2 * pow(a,alpha) * fed;
        }
        return dnda;
    }

    // amorphous silicates (50% enstatite and 50% forsterite)
    double dndas(double a)
    {
        if (a < amins || a > amax) return 0.0;
        double x = log(a/centroids);
        return Cs / a * exp( -0.5*x*x );
    }
}

//////////////////////////////////////////////////////////////////////

void ThemisDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    addpopulations(new AmHydrocarbonGrainComposition(), aminh, amax, &dndah, _Nahc);
    addpopulations(new EnstatiteGrainComposition(), amins, amax, &dndas, _Nens);
    addpopulations(new ForsteriteGrainComposition(), amins, amax, &dndas, _Nfor);
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

void ThemisDustMix::setEnstatitePops(int value)
{
    _Nens = value;
}

////////////////////////////////////////////////////////////////////

int ThemisDustMix::enstatitePops() const
{
    return _Nens;
}

////////////////////////////////////////////////////////////////////

void ThemisDustMix::setForsteritePops(int value)
{
    _Nfor = value;
}

////////////////////////////////////////////////////////////////////

int ThemisDustMix::forsteritePops() const
{
    return _Nfor;
}

//////////////////////////////////////////////////////////////////////
