/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "LogWavelengthGrid.hpp"
#include "NR.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

LogWavelengthGrid::LogWavelengthGrid()
    : _lambdamin(0), _lambdamax(0)
{
}

////////////////////////////////////////////////////////////////////

void LogWavelengthGrid::setupSelfBefore()
{
    PanWavelengthGrid::setupSelfBefore();

    // verify property values
    if (_lambdamin <= 0) throw FATALERROR("the shortest wavelength should be positive");
    if (_lambdamax <= _lambdamin) throw FATALERROR("the longest wavelength should be larger than the shortest");
    if (_Nlambda < 2) throw FATALERROR("the number of wavelength grid points should be at least 2");

    NR::loggrid(_lambdav, _lambdamin, _lambdamax, _Nlambda-1);
}

////////////////////////////////////////////////////////////////////

void LogWavelengthGrid::setMinWavelength(double value)
{
    _lambdamin = value;
}

////////////////////////////////////////////////////////////////////

double LogWavelengthGrid::minWavelength() const
{
    return _lambdamin;
}

////////////////////////////////////////////////////////////////////

void LogWavelengthGrid::setMaxWavelength(double value)
{
    _lambdamax = value;
}

////////////////////////////////////////////////////////////////////

double LogWavelengthGrid::maxWavelength() const
{
    return _lambdamax;
}

////////////////////////////////////////////////////////////////////

void LogWavelengthGrid::setPoints(int value)
{
    _Nlambda = value;
}

////////////////////////////////////////////////////////////////////

int LogWavelengthGrid::points() const
{
    return _Nlambda;
}

////////////////////////////////////////////////////////////////////
