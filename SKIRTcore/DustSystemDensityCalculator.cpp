/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "DustSystemDensityCalculator.hpp"
#include "DustDistribution.hpp"
#include "DustGridStructure.hpp"
#include "DustSystem.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DustSystemDensityCalculator::DustSystemDensityCalculator(const DustSystem *ds, int numBodies, int numSamplesPerBody)
    : _ds(ds), _dd(ds->dustDistribution()), _grid(ds->dustGridStructure()), _random(ds->find<Random>()),
      _numBodies(numBodies), _numSamplesPerBody(numSamplesPerBody),
      _extent(-_grid->xmax(), -_grid->ymax(), -_grid->zmax(), _grid->xmax(), _grid->ymax(), _grid->zmax()),
      _drhov(numBodies), _drho2v(numBodies),
      _meanDelta(0), _stddevDelta(0),
      _consolidated(false)
{
}

//////////////////////////////////////////////////////////////////////

void DustSystemDensityCalculator::body(int n)
{
    int k = _numSamplesPerBody;
    while (k--)
    {
        Position pos = _random->position(_extent);
        double rhot = _dd->density(pos);
        double rhog = _ds->density(_grid->whichcell(pos));
        double drho = fabs(rhog-rhot);
        _drhov[n]  += drho;
        _drho2v[n] += drho*drho;
    }
    _drhov[n]  /= _numSamplesPerBody;
    _drho2v[n] /= _numSamplesPerBody;
}

//////////////////////////////////////////////////////////////////////

void DustSystemDensityCalculator::consolidate()
{
    _meanDelta = _drhov.sum()/_numBodies;
    _stddevDelta = sqrt(_drho2v.sum()/_numBodies - _meanDelta*_meanDelta);
    _consolidated = true;
}

//////////////////////////////////////////////////////////////////////

double DustSystemDensityCalculator::meanDelta()
{
    if (!_consolidated) consolidate();
    return _meanDelta;
}

//////////////////////////////////////////////////////////////////////

double DustSystemDensityCalculator::stddevDelta()
{
    if (!_consolidated) consolidate();
    return _stddevDelta;
}

//////////////////////////////////////////////////////////////////////
