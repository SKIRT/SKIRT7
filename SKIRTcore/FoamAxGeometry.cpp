/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Foam.hpp"
#include "FoamAxGeometry.hpp"
#include "Log.hpp"
#include "Random.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

FoamAxGeometry::FoamAxGeometry(int Ncells)
    : _Ncells(Ncells), _foam(0), _Rscale(0), _zscale(0)
{
}

///////////////////////////////////////////////////////////////////////////////

FoamAxGeometry::~FoamAxGeometry()
{
    delete _foam;
}

//////////////////////////////////////////////////////////////////////

void FoamAxGeometry::setupSelfAfter()
{
    AxGeometry::setupSelfAfter();

    _foam = Foam::createFoam(find<Log>(), _random, this, 2, _Ncells);
}

///////////////////////////////////////////////////////////////////////////////

Position
FoamAxGeometry::generatePosition()
const
{
    double par[2];
    _foam->MCgenerate(par);

    double a = _Rscale;
    double c = _zscale;
    double Rbar = par[0];
    double zbar = par[1];
    double R = -a * log(Rbar);
    double z = c/tan(M_PI*zbar);
    double phi = 2.0*M_PI*_random->uniform();
    return Position(R,phi,z,Position::CYLINDRICAL);
}

///////////////////////////////////////////////////////////////////////////////

double
FoamAxGeometry::foamdensity(int ndim, double* par) const
{
    if (ndim != 2) throw FATALERROR("Incorrect dimension (ndim = " + QString::number(ndim) + ")");

    double a = _Rscale;
    double c = _zscale;
    double Rbar = par[0];
    double zbar = par[1];
    double R = -a*log(Rbar);
    double z = c/tan(M_PI*zbar);
    double jacobian = a*c * M_PI / pow(sin(M_PI*zbar),2) / Rbar;
    Position bfr(R,0.0,z,Position::CYLINDRICAL);
    return density(bfr) * R * jacobian;
}

///////////////////////////////////////////////////////////////////////////////
