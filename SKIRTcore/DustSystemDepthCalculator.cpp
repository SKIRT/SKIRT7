/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "DustSystemDepthCalculator.hpp"
#include "DustDistribution.hpp"
#include "DustGridPath.hpp"
#include "DustGridStructure.hpp"
#include "DustSystem.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DustSystemDepthCalculator::DustSystemDepthCalculator(const DustSystem *ds,
                                                     int numBodies, int numSamplesPerBody, int numSamplesPerPath)
    : _ds(ds), _dd(ds->dustDistribution()), _grid(ds->dustGridStructure()), _random(ds->find<Random>()),
      _numBodies(numBodies), _numSamplesPerBody(numSamplesPerBody), _numSamplesPerPath(numSamplesPerPath),
      _extent(-_grid->xmax(), -_grid->ymax(), -_grid->zmax(), _grid->xmax(), _grid->ymax(), _grid->zmax()),
      _eps(_extent.widths().norm()*1e-10),
      _dtauv(numBodies), _dtau2v(numBodies),
      _meanDelta(0), _stddevDelta(0),
      _consolidated(false)
{
}

//////////////////////////////////////////////////////////////////////

void DustSystemDepthCalculator::body(int index)
{
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
        DustGridPath dgp = _grid->path(Position(r1), Direction(k));
        int N = dgp.size();
        const vector<int>& mv = dgp.mv();
        const vector<double>& sv = dgp.sv();
        const vector<double>& dsv = dgp.dsv();
        double sumrhods = 0;
        for (int n=0; n<N; n++)
        {
            if (sv[n] > s) break;
            sumrhods += _ds->density(mv[n]) * dsv[n];
        }
        double taug = Units::kappaV() * sumrhods;

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
