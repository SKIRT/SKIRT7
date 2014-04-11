/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DraineGraphiteGrainComposition.hpp"
#include "DraineIonizedPAHGrainComposition.hpp"
#include "DraineNeutralPAHGrainComposition.hpp"
#include "DraineSilicateGrainComposition.hpp"
#include "WeingartnerDraineDustMix.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

WeingartnerDraineDustMix::WeingartnerDraineDustMix()
    : _env(MilkyWay), _Ngra(0), _Nsil(0), _Npah(0)
{
}

//////////////////////////////////////////////////////////////////////

namespace
{
    // grain size ranges for each of the dust composition types (in m)
    const double amin_gra = 0.001e-6;
    const double amax_gra = 10.0e-6;
    const double amin_sil = 0.001e-6;
    const double amax_sil = 10.0e-6;
    const double amin_pah = 0.0003548e-6;
    const double amax_pah = 0.01e-6;

    // parameterized grain size distribution for graphite and silicate
    double dnda_grasil(double a, double C, double at, double ac, double alpha, double beta)
    {
        double f0 = C/a * pow(a/at,alpha);
        double f1 = 0.0;
        if (beta>0)
            f1 = 1.0+beta*a/at;
        else
            f1 = 1.0/(1.0-beta*a/at);
        double f2 = 0.0;
        if (a<at)
            f2 = 1.0;
        else
            f2 = exp(-pow((a-at)/ac,3));
        return f0*f1*f2;
    }

    // parameterized grain size distribution for pah (whether neutral or ionized)
    double dnda_pah(double a, double sigma, const double a0[2], const double bc[2])
    {
        const double mC = 1.9944e-26;    // mass of C atom in kg
        const double rho = 2.24e3;       // mass density of graphite in kg/m^3
        const double amin = 3.5e-10;     // 3.5 Angstrom in m
        double B[2];
        for (int i=0; i<2; i++)
        {
            double t0 = 3.0/pow(2*M_PI,1.5);
            double t1 = exp(-4.5*sigma*sigma);
            double t2 = 1.0/rho/pow(a0[i],3)/sigma;
            double erffac = 3.0*sigma/sqrt(2.0) + log(a0[i]/amin)/sqrt(2.0)/sigma;
            double t3 = bc[i]*mC/(1.0+erf(erffac));
            B[i] = t0*t1*t2*t3;
        }
        double sum = 0.0;
        for (int i=0; i<2; i++)
        {
            double u = log(a/a0[i])/sigma;
            sum += B[i]/a*exp(-0.5*u*u);
        }
        return sum;
    }

    // grain size distributions for Milky Way environment
    double dnda_gra_mwy(double a)
    {
        const double C = 9.99e-12;      // C abundance in double log normal WD01a tabel 1 p300
        const double at = 0.0107e-6;    // WD01a tabel 1 p300
        const double ac = 0.428e-6;     // WD01a tabel 1 p300
        const double alpha = -1.54;     // WD01a tabel 1 p300
        const double beta = -0.165;     // WD01a tabel 1 p300
        return dnda_grasil(a, C, at, ac, alpha, beta);
    }

    double dnda_sil_mwy(double a)
    {
        const double C = 1.00e-13;      // C abundance in double log normal WD01a tabel 1 p300
        const double at = 0.164e-6;     // WD01a tabel 1 p300
        const double ac = 0.1e-6;       // WD01a tabel 1 p300
        const double alpha = -2.21;     // WD01a tabel 1 p300
        const double beta = 0.300;      // WD01a tabel 1 p300
        return dnda_grasil(a, C, at, ac, alpha, beta);
    }

    double dnda_pah_mwy(double a)
    {
        const double sigma = 0.4;                // DL2001 II p786
        const double a0[2] = {3.5e-10, 30e-10};  // DL2001 II p786 = 3.5A, 30A
        const double bc[2] = {4.5e-5, 1.5e-5};   // DL2001 II p786 = 45ppm, 15ppm
        return 0.5 * dnda_pah(a, sigma, a0, bc); // 50% of the PAH grains are neutral, 50% are ionized
    }

    // grain size distributions for LMC environment
    double dnda_gra_lmc(double a)
    {
        const double C = 3.51e-15;      // C abundance in double log normal WD01a tabel 1 p300
        const double at = 0.0980e-6;    // WD01a tabel 1 p300
        const double ac = 0.641e-6;     // WD01a tabel 1 p300
        const double alpha = -2.99;     // WD01a tabel 1 p300
        const double beta = 2.46;       // WD01a tabel 1 p300
        return dnda_grasil(a, C, at, ac, alpha, beta);
    }

    double dnda_sil_lmc(double a)
    {
        const double C = 1.78e-14;      // C abundance in double log normal WD01a tabel 1 p300
        const double at = 0.184e-6;     // WD01a tabel 1 p300
        const double ac = 0.1e-6;       // WD01a tabel 1 p300
        const double alpha = -2.49;     // WD01a tabel 1 p300
        const double beta = 0.345;      // WD01a tabel 1 p300
        return dnda_grasil(a, C, at, ac, alpha, beta);
    }

    double dnda_pah_lmc(double a)
    {
        const double sigma = 0.4;                // DL2001 II p786
        const double a0[2] = {3.5e-10, 30e-10};  // DL2001 II p786 = 3.5A, 30A
        const double bc[2] = {0.75e-5, 0.25e-5}; // DL2001 II p786
        return 0.5 * dnda_pah(a, sigma, a0, bc); // 50% of the PAH grains are neutral, 50% are ionized
    }
}

//////////////////////////////////////////////////////////////////////

void WeingartnerDraineDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    addpopulations(new DraineGraphiteGrainComposition(this),
                   amin_gra, amax_gra, _env==MilkyWay ? &dnda_gra_mwy : &dnda_gra_lmc, _Ngra);
    addpopulations(new DraineSilicateGrainComposition(this),
                   amin_sil, amax_sil, _env==MilkyWay ? &dnda_sil_mwy : &dnda_sil_lmc, _Nsil);
    addpopulations(new DraineNeutralPAHGrainComposition(this),
                   amin_pah, amax_pah, _env==MilkyWay ? &dnda_pah_mwy : &dnda_pah_lmc, _Npah);
    addpopulations(new DraineIonizedPAHGrainComposition(this),
                   amin_pah, amax_pah, _env==MilkyWay ? &dnda_pah_mwy : &dnda_pah_lmc, _Npah);
}

////////////////////////////////////////////////////////////////////

void WeingartnerDraineDustMix::setGraphitePops(int value)
{
    _Ngra = value;
}

////////////////////////////////////////////////////////////////////

int WeingartnerDraineDustMix::graphitePops() const
{
    return _Ngra;
}

////////////////////////////////////////////////////////////////////

void WeingartnerDraineDustMix::setSilicatePops(int value)
{
    _Nsil = value;
}

////////////////////////////////////////////////////////////////////

int WeingartnerDraineDustMix::silicatePops() const
{
    return _Nsil;
}

////////////////////////////////////////////////////////////////////

void WeingartnerDraineDustMix::setPAHPops(int value)
{
    _Npah = value;
}

////////////////////////////////////////////////////////////////////

int WeingartnerDraineDustMix::PAHPops() const
{
    return _Npah;
}

////////////////////////////////////////////////////////////////////

void WeingartnerDraineDustMix::setEnvironment(WeingartnerDraineDustMix::Environment value)
{
    _env = value;
}

////////////////////////////////////////////////////////////////////

WeingartnerDraineDustMix::Environment WeingartnerDraineDustMix::environment() const
{
    return _env;
}

//////////////////////////////////////////////////////////////////////
