/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "NestedLogWavelengthGrid.hpp"
#include "NR.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

NestedLogWavelengthGrid::NestedLogWavelengthGrid()
    : _lambdamin(0), _lambdamax(0), _lambdazoommin(0), _lambdazoommax(0), _Nlambdazoom(0)
{
}

////////////////////////////////////////////////////////////////////

void NestedLogWavelengthGrid::setupSelfBefore()
{
    PanWavelengthGrid::setupSelfBefore();

    // verify property values
    if (_Nlambda < 2) throw FATALERROR("the number of points in the low-resolution grid should be at least 2");
    if (_Nlambdazoom < 2) throw FATALERROR("the number of points in the high-resolution subgrid should be at least 2");
    if (_lambdamin <= 0) throw FATALERROR("the shortest wavelength should be positive");
    if (_lambdazoommin <= _lambdamin || _lambdazoommax <= _lambdazoommin || _lambdamax <= _lambdazoommax)
        throw FATALERROR("the high-resolution subgrid should be properly nested in the low-resolution grid");

    // build the high- and low-resolution grids, independently
    Array lambdalowv, lambdazoomv;
    NR::loggrid(lambdalowv, _lambdamin, _lambdamax, _Nlambda);
    NR::loggrid(lambdazoomv, _lambdazoommin, _lambdazoommax, _Nlambdazoom);

    // merge the two grids
    vector<double> lambdav;
    for (int ell=0; ell<_Nlambda; ell++)
    {
        double lambda = lambdalowv[ell];
        if (lambda<_lambdazoommin) lambdav.push_back(lambda);
    }
    for (int ell=0; ell<_Nlambdazoom; ell++)
    {
        double lambda = lambdazoomv[ell];
        lambdav.push_back(lambda);
    }
    for (int ell=0; ell<_Nlambda; ell++)
    {
        double lambda = lambdalowv[ell];
        if (lambda>_lambdazoommax) lambdav.push_back(lambda);
    }

    // store the result
    NR::assign(_lambdav, lambdav);
}

////////////////////////////////////////////////////////////////////

void NestedLogWavelengthGrid::setMinWavelength(double value)
{
    _lambdamin = value;
}

////////////////////////////////////////////////////////////////////

double NestedLogWavelengthGrid::minWavelength() const
{
    return _lambdamin;
}

////////////////////////////////////////////////////////////////////

void NestedLogWavelengthGrid::setMaxWavelength(double value)
{
    _lambdamax = value;
}

////////////////////////////////////////////////////////////////////

double NestedLogWavelengthGrid::maxWavelength() const
{
    return _lambdamax;
}

////////////////////////////////////////////////////////////////////

void NestedLogWavelengthGrid::setPoints(int value)
{
    _Nlambda = value;
}

////////////////////////////////////////////////////////////////////

int NestedLogWavelengthGrid::points() const
{
    return _Nlambda;
}

////////////////////////////////////////////////////////////////////

void NestedLogWavelengthGrid::setMinWavelengthSubGrid(double value)
{
    _lambdazoommin = value;
}

////////////////////////////////////////////////////////////////////

double NestedLogWavelengthGrid::minWavelengthSubGrid() const
{
    return _lambdazoommin;
}

////////////////////////////////////////////////////////////////////

void NestedLogWavelengthGrid::setMaxWavelengthSubGrid(double value)
{
    _lambdazoommax = value;
}

////////////////////////////////////////////////////////////////////

double NestedLogWavelengthGrid::maxWavelengthSubGrid() const
{
    return _lambdazoommax;
}

////////////////////////////////////////////////////////////////////

void NestedLogWavelengthGrid::setPointsSubGrid(int value)
{
    _Nlambdazoom = value;
}

////////////////////////////////////////////////////////////////////

int NestedLogWavelengthGrid::pointsSubGrid() const
{
    return _Nlambdazoom;
}

////////////////////////////////////////////////////////////////////
