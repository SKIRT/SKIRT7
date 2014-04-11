/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "DustDistribution.hpp"
#include "Random.hpp"
#include "TreeNode.hpp"
#include "TreeNodeSampleDensityCalculator.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TreeNodeSampleDensityCalculator::TreeNodeSampleDensityCalculator(
        Random* random, int Nrandom, DustDistribution* dd, TreeNode* node)
    : _rv(Nrandom), _rhov(Nrandom),
      _extent(node->extent()), _Nrandom(Nrandom), _random(random), _dd(dd)
{
}

//////////////////////////////////////////////////////////////////////

void TreeNodeSampleDensityCalculator::body(int n)
{
    _rv[n] = _random->position(_extent);
    _rhov[n] = _dd->density(_rv[n]);
}

//////////////////////////////////////////////////////////////////////

double TreeNodeSampleDensityCalculator::volume() const
{
    return _extent.volume();
}

//////////////////////////////////////////////////////////////////////

double TreeNodeSampleDensityCalculator::mass() const
{
    return _rhov.sum()/_Nrandom * _extent.volume();
}

//////////////////////////////////////////////////////////////////////

Vec TreeNodeSampleDensityCalculator::barycenter() const
{
    double sumrho = 0.0;
    Vec sum;
    for (int n=0; n<_Nrandom; n++)
    {
        sumrho += _rhov[n];
        sum += _rhov[n] * _rv[n];
    }
    return sum/sumrho;
}

//////////////////////////////////////////////////////////////////////

double TreeNodeSampleDensityCalculator::densityDispersion() const
{
    double minrho = _rhov.min();
    double maxrho = _rhov.max();
    return maxrho > 0 ? (maxrho-minrho)/maxrho : 0;
}

//////////////////////////////////////////////////////////////////////

double TreeNodeSampleDensityCalculator::opticalDepth() const
{
    return Units::kappaV() * mass() / pow(volume(),2./3.);
}

//////////////////////////////////////////////////////////////////////
