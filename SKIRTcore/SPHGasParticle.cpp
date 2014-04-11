/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <vector>
#include <QMutex>
#include "Box.hpp"
#include "SPHGasParticle.hpp"

////////////////////////////////////////////////////////////////////

// fast sampled version of the error function with better than 1 promille accuracy
namespace
{
    // mutex to guard initialization of the static vector with the sampled values
    QMutex _mutex;

    // flag becomes true if the static vector has been initialized
    bool _initialized = false;

    // the number of values in the static vector (after it has been initialized)
    const int _N = 5000;

    // the domain range (_Xmin is 0)
    const double _Xmax = 2.5;

    // the static vector with the sampled values:  _erf[i] = erf( (i*_Xmax)/_N )
    std::vector<double> _erf;

    // initialize the static vector with the sampled values
    void initialize_myerf()
    {
        // initialization must be locked to protect against race conditions when used in multiple threads
        QMutexLocker lock(&_mutex);

        if (!_initialized)
        {
            _erf.resize(_N+1);
            for (int i=0; i<=_N; i++) _erf[i] = erf( (i*_Xmax)/_N );

            _initialized = true;
        }
    }

    // returns the appropriate sampled value for erf(x)
    inline double myerf(double x)
    {
        if (x >= _Xmax) return 1.;
        if (x >= 0) return _erf[ static_cast<int>(x*(_N/_Xmax)) ];
        if (x > -_Xmax) return -_erf[ static_cast<int>(-x*(_N/_Xmax)) ];
        return -1;
    }
}

////////////////////////////////////////////////////////////////////

SPHGasParticle::SPHGasParticle(Vec rc, double h, double M, double Z)
    : _rc(rc), _h(h), _M(M), _Z(Z)
{
    _norm = 1/(h*h);
    _rho0 = 8.0/M_PI * M/(h*h*h) * Z;
    _rho2 = _rho0 * 2.0;
    _s = 2.42 / _h;
    _MZ8 = _M * _Z / 8.;
}

////////////////////////////////////////////////////////////////////

Vec SPHGasParticle::center() const
{
    return _rc;
}

////////////////////////////////////////////////////////////////////

double SPHGasParticle::center(int dir) const
{
    switch (dir)
    {
    case 1: return _rc.x();
    case 2: return _rc.y();
    case 3: return _rc.z();
    default: return 0.;
    }
}

////////////////////////////////////////////////////////////////////

double SPHGasParticle::radius() const
{
    return _h;
}

////////////////////////////////////////////////////////////////////

double SPHGasParticle::metalMass() const
{
    return _M * _Z;
}

////////////////////////////////////////////////////////////////////

double SPHGasParticle::metalMassInBox(const Box& box) const
{
    // ensure that the sampled version of the erf function is properly initialized
    if (!_initialized) initialize_myerf();
    Vec r1 = _s * (box.rmin()-_rc);
    Vec r2 = _s * (box.rmax()-_rc);
    return _MZ8 * (myerf(r2.x())-myerf(r1.x())) * (myerf(r2.y())-myerf(r1.y())) * (myerf(r2.z())-myerf(r1.z()));
}

////////////////////////////////////////////////////////////////////

double SPHGasParticle::metalDensity(Vec r) const
{
    double u2 = _norm * (r-_rc).norm2();
    if (u2 < 1.0)
    {
        double u = sqrt(u2);
        double u1m = 1.0-u;
        if (u<0.5)
            return _rho0*(1.0-6.0*u2*u1m);
        else
            return _rho2*u1m*u1m*u1m;
    }
    return 0.0;
}

////////////////////////////////////////////////////////////////////
