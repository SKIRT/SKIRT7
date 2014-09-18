/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Foam.hpp"
#include "FoamGeometry.hpp"
#include "Log.hpp"
#include "Random.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

FoamGeometry::FoamGeometry(int Ncells)
    : _Ncells(Ncells), _foam(0)
{
}

///////////////////////////////////////////////////////////////////////////////

FoamGeometry::~FoamGeometry()
{
    delete _foam;
}

//////////////////////////////////////////////////////////////////////

void FoamGeometry::setupSelfAfter()
{
    GenGeometry::setupSelfAfter();

    _foam = Foam::createFoam(find<Log>(), _random, this, 3, _Ncells);
}

///////////////////////////////////////////////////////////////////////////////

Position
FoamGeometry::generatePosition()
const
{
    double par[3];
    _foam->MCgenerate(par);

    double a = _xscale;
    double b = _yscale;
    double c = _zscale;
    double xbar = par[0];
    double ybar = par[1];
    double zbar = par[2];
    double x = a/tan(M_PI*xbar);
    double y = b/tan(M_PI*ybar);
    double z = c/tan(M_PI*zbar);

    return Position(x,y,z);
}

/////////////////////////////////////////////////////////////////////

double
FoamGeometry::foamdensity(int ndim, double* par) const
{
    if (ndim != 3) throw FATALERROR("Incorrect dimension (ndim = " + QString::number(ndim) + ")");

    double a = _xscale;
    double b = _yscale;
    double c = _zscale;
    double xbar = par[0];
    double ybar = par[1];
    double zbar = par[2];
    double x = a/tan(M_PI*xbar);
    double y = b/tan(M_PI*ybar);
    double z = c/tan(M_PI*zbar);
    double jacobian = a*b*c*pow(M_PI,3)/pow(sin(M_PI*xbar)*sin(M_PI*ybar)*sin(M_PI*zbar),2);
    Position bfr(x,y,z);
    return density(bfr)*jacobian;
}

///////////////////////////////////////////////////////////////////////////////
