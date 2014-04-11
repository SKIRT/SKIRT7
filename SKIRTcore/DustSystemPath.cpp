/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DustSystemPath.hpp"
#include "FatalError.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustSystemPath::DustSystemPath()
{
}

////////////////////////////////////////////////////////////////////

void
DustSystemPath::initialize(Position bfr, Direction bfk,
                           const vector<int>& mv,
                           const vector<double>& dsv,
                           const vector<double>& sv,
                           int ell,
                           const vector<double>& dtauv,
                           const vector<double>& tauv)
{
    _bfr = bfr;
    _bfk = bfk;
    _mv = mv;
    _dsv = dsv;
    _sv = sv;
    _ell = ell;
    _dtauv = dtauv;
    _tauv = tauv;
}

////////////////////////////////////////////////////////////////////

Position
DustSystemPath::position()
const
{
    return _bfr;
}

////////////////////////////////////////////////////////////////////

Direction
DustSystemPath::direction()
const
{
    return _bfk;
}

////////////////////////////////////////////////////////////////////

int
DustSystemPath::cellnumber(unsigned int n)
const
{
    return _mv[n];
}

////////////////////////////////////////////////////////////////////

double
DustSystemPath::ds(unsigned int n)
const
{
    return _dsv[n];
}

////////////////////////////////////////////////////////////////////

double
DustSystemPath::s(unsigned int n)
const
{
    return _sv[n];
}

////////////////////////////////////////////////////////////////////

int
DustSystemPath::ell()
const
{
    return _ell;
}

////////////////////////////////////////////////////////////////////

double
DustSystemPath::tau(unsigned int n)
const
{
    return _tauv[n];
}

////////////////////////////////////////////////////////////////////

double
DustSystemPath::dtau(unsigned int n)
const
{
    return _dtauv[n];
}

////////////////////////////////////////////////////////////////////

double
DustSystemPath::opticaldepth()
const
{
    unsigned int N = _mv.size();
    if (N==0) return 0.0;
    return _tauv[N-1];
}

//////////////////////////////////////////////////////////////////////

double
DustSystemPath::pathlength(double tau)
const
{
    if (tau<0.0)
        throw FATALERROR("Tau should be positive");

    int N = size();
    if (N==0)
        throw FATALERROR("The DustSystemPath object is empty");

    if (tau==0.0) return 0.0;
    double sL, sR, tauL, tauR;
    for (int n=0; n<N; n++)
    {
        if (_tauv[n]>tau)
        {
            if (n==0)
            {
                sL = 0.0;
                tauL = 0.0;
            }
            else
            {
                sL = _sv[n-1];
                tauL = _tauv[n-1];
            }
            sR = _sv[n];
            tauR = _tauv[n];
            double s = sL + (tau-tauL)/(tauR-tauL)*(sR-sL);
            return s;
        }
    }
    throw FATALERROR("I should never reach this point");
}

////////////////////////////////////////////////////////////////////

int
DustSystemPath::size()
const
{
    return _mv.size();
}

////////////////////////////////////////////////////////////////////
