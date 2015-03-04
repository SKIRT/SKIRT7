/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Random.hpp"
#include "Trust6Geometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

namespace
{
    const double AU = 1.49597871e11;    // 1 Astronomical Unit in m
    const double GCM3 = 1000;           // 1 g/cm3 in kg/m3

    // radial limits
    const double RMAX1 =  400 *AU;
    const double RMAX2 =  400 *AU;
    const double RMAX3 =  100 *AU;

    // vertical limits
    const double ZMIN1 =  -90 *AU;
    const double ZMAX1 =  230 *AU;
    const double ZMIN2 = -230 *AU;
    const double ZMAX2 = -220 *AU;
    const double ZMIN3 =  -80 *AU;
    const double ZMAX3 =  -70 *AU;
    const double ZCTR3 =  -75 *AU;

    // densities
    const double RHO1 =  1e-21 *GCM3;
    const double RHO2 =  1e-18 *GCM3;
    const double RHOA3 = 8e-15 *GCM3;
    const double RHOB3 = 4e-18 *GCM3;

    // sigma of gaussian
    const double SIGMA3 = sqrt(3.)/2. *AU;

    // 2*sigma^2 in denominator of gaussian
    const double TWO_SIGMA3_SQUARE = 1.5 *AU*AU;

    // equivalent width of gaussian disk, calculated as int_{-5}^{+5} [exp(-2/3 z^2) dz]
    const double Z_WIDTH3_GAUSS = 2.17080374682060324 *AU;

    // masses
    const double M1 = M_PI*RMAX1*RMAX1 * (ZMAX1-ZMIN1) * RHO1;
    const double M2 = M_PI*RMAX2*RMAX2 * (ZMAX2-ZMIN2) * RHO2;
    const double M3 = M_PI*RMAX3*RMAX3 * ( Z_WIDTH3_GAUSS * RHOA3 + (ZMAX3-ZMIN3) * RHOB3 );
    const double M = M1 + M2 + M3;

    // mass fractions
    const double MF1 = M1 / M;
    const double MF2 = M2 / M;
    //const double MF3 = M3 / M;

    // normalized densities
    const double NRHO1 =  RHO1 / M;
    const double NRHO2 =  RHO2 / M;
    const double NRHOA3 = RHOA3 / M;
    const double NRHOB3 = RHOB3 / M;
}

//////////////////////////////////////////////////////////////////////

Trust6Geometry::Trust6Geometry()
{
}

//////////////////////////////////////////////////////////////////////

double Trust6Geometry::density(double R, double z) const
{
    double rho = 0;
    if (R<RMAX1 && z>ZMIN1 && z<ZMAX1) rho += NRHO1;
    if (R<RMAX2 && z>ZMIN2 && z<ZMAX2) rho += NRHO2;
    if (R<RMAX3 && z>ZMIN3 && z<ZMAX3) rho += NRHOA3*exp(-(z-ZCTR3)*(z-ZCTR3)/TWO_SIGMA3_SQUARE) + NRHOB3;
    return rho;
}

//////////////////////////////////////////////////////////////////////

Position Trust6Geometry::generatePosition() const
{
    // determine a random azimuth, independent of the component from which the position is generated
    double phi = 2.0*M_PI*_random->uniform();
    double R,z;

    // select a component and generate a position within that component
    double X = _random->uniform();
    if (X < MF1)
    {
        R = RMAX1 * sqrt(_random->uniform());
        z = ZMIN1 + _random->uniform()*(ZMAX1-ZMIN1);
    }
    else if (X < MF1+MF2)
    {
        R = RMAX2 * sqrt(_random->uniform());
        z = ZMIN2 + _random->uniform()*(ZMAX2-ZMIN2);
    }
    else
    {
        R = RMAX3 * sqrt(_random->uniform());
        z = ZMIN3 + _random->gauss()*SIGMA3;
    }

    // return the position
    return Position(R, phi, z, Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

double Trust6Geometry::SigmaR() const
{
    return RMAX1*NRHO1;
}

//////////////////////////////////////////////////////////////////////

double Trust6Geometry::SigmaZ() const
{
    return (ZMAX1-ZMIN1)*NRHO1 + (ZMAX2-ZMIN2)*NRHO2 + Z_WIDTH3_GAUSS*NRHOA3 + (ZMAX3-ZMIN3) * NRHOB3;
}

//////////////////////////////////////////////////////////////////////
