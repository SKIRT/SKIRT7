/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "TrustGraphiteGrainComposition.hpp"
#include "TrustNeutralPAHGrainComposition.hpp"
#include "TrustSilicateGrainComposition.hpp"
#include "TrustDustMix.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TrustDustMix::TrustDustMix()
    : _Ngra(0), _Nsil(0), _Npah(0)
{
}

//////////////////////////////////////////////////////////////////////

namespace
{
    // grain size ranges for each of the dust composition types (in m)
    const double amin_gra = 0.00035e-6;
    const double amax_gra = 0.33e-6;
    const double amin_sil = 0.00035e-6;
    const double amax_sil = 0.37e-6;
    const double amin_pah = 0.00035e-6;
    const double amax_pah = 0.005e-6;

    // parameterized Zubko grain size distribution (assumes size in units of microns)
    double dnda(double a, double A, double c0, double b0,
                double a1, double b1, double m1,
                double a2, double b2, double m2,
                double a3, double b3, double m3,
                double a4, double b4, double m4)
    {
        a *= 1e6;  // convert from m to micron
        double logg = c0 + b0*log10(a)
                      - b1*pow(fabs(log10(a/a1)),m1)
                      - b2*pow(fabs(log10(a/a2)),m2)
                      - b3*pow(fabs(a-a3),m3)
                      - b4*pow(fabs(a-a4),m4);
        return 1e6 * A * pow(10.0,logg);  // convert from 1/micron to 1/m
    }

    // grain size distribution for graphite
    double dnda_gra(double a)
    {
        const double A = 1.905816e-7;
        const double c0 = -9.86;
        const double b0 = -5.02082;
        const double a1 = 0.415861; // in micron
        const double b1 = 5.81215e-3;
        const double m1 = 4.63229;
        const double a2 = 1.0; // not used
        const double b2 = 0.0; // not used
        const double m2 = 0.0; // not used
        const double a3 = 0.160344;
        const double b3 = 1125.02;
        const double m3 = 3.69897;
        const double a4 = 0.160501;
        const double b4 = 1126.02;
        const double m4 = 3.69967;
        return dnda(a, A, c0, b0, a1, b1, m1, a2, b2, m2, a3, b3, m3, a4, b4, m4);
    }

    // grain size distribution for silicate
    double dnda_sil(double a)
    {
        const double A = 1.471288e-7;
        const double c0 = -8.47091;
        const double b0 = -3.68708;
        const double a1 = 7.64943e-3; // in micron
        const double b1 = 2.37316e-5;
        const double m1 = 22.5489;
        const double a2 = 1.0; // not used
        const double b2 = 0.0; // not used
        const double m2 = 0.0; // not used
        const double a3 = 0.480229;
        const double b3 = 2961.28;
        const double m3 = 12.1717;
        const double a4 = 1.0; // not used
        const double b4 = 0.0; // not used
        const double m4 = 0.0; // not used
        return dnda(a, A, c0, b0, a1, b1, m1, a2, b2, m2, a3, b3, m3, a4, b4, m4);
    }

    // grain size distribution for PAH
    double dnda_pah(double a)
    {
        const double A = 2.227433e-7;
        const double c0 = -8.02895;
        const double b0 = -3.45764;
        const double a1 = 1.0; // in micron
        const double b1 = 1183.96;
        const double m1 = -8.20551;
        const double a2 = 1.0; // not used
        const double b2 = 0.0; // not used
        const double m2 = 0.0; // not used
        const double a3 = -5.29496e-3;
        const double b3 = 1.0e24;
        const double m3 = 12.0146;
        const double a4 = 1.0; // not used
        const double b4 = 0.0; // not used
        const double m4 = 0.0; // not used
        return dnda(a, A, c0, b0, a1, b1, m1, a2, b2, m2, a3, b3, m3, a4, b4, m4);
    }
}

//////////////////////////////////////////////////////////////////////

void TrustDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    addpopulations(new TrustGraphiteGrainComposition(this),   amin_gra, amax_gra, &dnda_gra, _Ngra);
    addpopulations(new TrustSilicateGrainComposition(this),   amin_sil, amax_sil, &dnda_sil, _Nsil);
    addpopulations(new TrustNeutralPAHGrainComposition(this), amin_pah, amax_pah, &dnda_pah, _Npah);
}

////////////////////////////////////////////////////////////////////

void TrustDustMix::setGraphitePops(int value)
{
    _Ngra = value;
}

////////////////////////////////////////////////////////////////////

int TrustDustMix::graphitePops() const
{
    return _Ngra;
}

////////////////////////////////////////////////////////////////////

void TrustDustMix::setSilicatePops(int value)
{
    _Nsil = value;
}

////////////////////////////////////////////////////////////////////

int TrustDustMix::silicatePops() const
{
    return _Nsil;
}

////////////////////////////////////////////////////////////////////

void TrustDustMix::setPAHPops(int value)
{
    _Npah = value;
}

////////////////////////////////////////////////////////////////////

int TrustDustMix::PAHPops() const
{
    return _Npah;
}

//////////////////////////////////////////////////////////////////////
