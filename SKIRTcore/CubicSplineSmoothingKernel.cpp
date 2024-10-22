/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "CubicSplineSmoothingKernel.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

CubicSplineSmoothingKernel::CubicSplineSmoothingKernel()
{
}

//////////////////////////////////////////////////////////////////////

void CubicSplineSmoothingKernel::setupSelfBefore()
{
    SmoothingKernel::setupSelfBefore();

    _Nu = 400;
    double du = 1.0/_Nu;
    _Xv.resize(_Nu+1);
    for (int k=0; k<=_Nu; k++)
    {
        double u = k*du;
        double u2 = u*u;
        double u3 = u*u2;
        if (u<0.5)
            _Xv[k] = u3 * (32.0/3.0 - 192.0/5.0*u2 + 32.0*u3);
        else
            _Xv[k] = -1.0/15.0 - 64.0*u3*(-1.0/3.0+0.75*u-0.6*u2+u3/6.0);
    }
}

//////////////////////////////////////////////////////////////////////

double CubicSplineSmoothingKernel::density(double u) const
{
    if (u<0.0 || u>1.0)
        return 0.0;
    else if (u<0.5)
        return 8.0/M_PI * (1.0-6.0*u*u*(1.0-u));
    else
        return 8.0/M_PI * 2.0*(1.0-u)*(1.0-u)*(1.0-u);
}

//////////////////////////////////////////////////////////////////////

double CubicSplineSmoothingKernel::generateRadius() const
{
    double X = find<Random>()->uniform();
    int k = NR::locate_clip(_Xv,X);
    double p = (X-_Xv[k])/(_Xv[k+1]-_Xv[k]);
    double u = (k+p)/_Nu;
    return u;
}

//////////////////////////////////////////////////////////////////////
