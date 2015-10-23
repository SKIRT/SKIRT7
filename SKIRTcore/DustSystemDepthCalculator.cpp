/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "DustSystemDepthCalculator.hpp"
#include "DustDistribution.hpp"
#include "DustGrid.hpp"
#include "DustSystem.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DustSystemDepthCalculator::DustSystemDepthCalculator(const DustSystem *ds,
                                                     int numBodies, int numSamplesPerBody, int numSamplesPerPath)
    : _ds(ds), _dd(ds->dustDistribution()), _grid(ds->dustGrid()), _random(ds->find<Random>()),
      _numBodies(numBodies), _numSamplesPerBody(numSamplesPerBody), _numSamplesPerPath(numSamplesPerPath),
      _extent(_grid->boundingbox()),
      _eps(_extent.widths().norm()*1e-10),
      _dtauv(numBodies), _dtau2v(numBodies),
      _meanDelta(0), _stddevDelta(0),
      _consolidated(false)
{
}

//////////////////////////////////////////////////////////////////////

void DustSystemDepthCalculator::body(size_t index)
{
    // dust grid path: allocated once so it can be reused
    DustGridPath dgp;

    int k = _numSamplesPerBody;
    while (k--)
    {
        // generate two points that are at least somewhat apart, and determine the direction between them
        Vec r1, r2, k;
        double s;
        do
        {
            r1 = _random->position(_extent);
            r2 = _random->position(_extent);
            k = r2 - r1;
            s = k.norm();
        }
        while (s < _eps);
        k /= s;

        // determine the theoretical optical depth by sampling along the line segment
        double ds = s / _numSamplesPerPath;
        double sumrho = 0;
        for (int n=1; n<=_numSamplesPerPath; n++)
        {
            sumrho += _dd->density(Position(r1+n*ds*k));
        }
        double taut = Units::kappaV() * sumrho * ds;

        // determine the gridded optical depth by asking the grid for a path
        dgp.setPosition(Position(r1));
        dgp.setDirection(Direction(k));
        _grid->path(&dgp);
        double taug = Units::kappaV() * dgp.opticalDepth([this](int m){ return _ds->density(m); }, s);

        // store the results
        double dtau = fabs(taug-taut);
        _dtauv[index]  += dtau;
        _dtau2v[index] += dtau*dtau;
    }
    _dtauv[index]  /= _numSamplesPerBody;
    _dtau2v[index] /= _numSamplesPerBody;
}

//////////////////////////////////////////////////////////////////////

void DustSystemDepthCalculator::consolidate()
{
    _meanDelta = _dtauv.sum()/_numBodies;
    _stddevDelta = sqrt(_dtau2v.sum()/_numBodies - _meanDelta*_meanDelta);
    _consolidated = true;
}

//////////////////////////////////////////////////////////////////////

double DustSystemDepthCalculator::meanDelta()
{
    if (!_consolidated) consolidate();
    return _meanDelta;
}

//////////////////////////////////////////////////////////////////////

double DustSystemDepthCalculator::stddevDelta()
{
    if (!_consolidated) consolidate();
    return _stddevDelta;
}

//////////////////////////////////////////////////////////////////////
