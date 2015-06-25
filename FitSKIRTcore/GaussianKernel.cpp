/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "GaussianKernel.hpp"
#include "FatalError.hpp"
#include <cmath>

////////////////////////////////////////////////////////////////////

GaussianKernel::GaussianKernel()
    : _fwhm(0), _dimension(0)
{
}

////////////////////////////////////////////////////////////////////

void GaussianKernel::setupSelfBefore()
{
    ConvolutionKernel::setupSelfBefore();

    // Verify the value of the FWHM
    if (_fwhm < 0) throw FATALERROR("FHWM should be positive");

    // Verify the dimension
    if (_dimension < 0) throw FATALERROR("Frame dimension should be positive");

    // From the FWHM, calculate the standard deviation
    double sigma = _fwhm/2.3548;

    // Resize the kernel image
    resize(_dimension, _dimension);

    // Set the kernel data
    for (int yk=1; yk<_dimension+1; yk++)
    {
        for (int xk=1; xk< _dimension+1; xk++)
        {
            int Xi = (_dimension+1)/2 - xk;
            int Yi = (_dimension+1)/2 - yk;

            (*this)(xk-1,yk-1) = exp(-(Xi*Xi+Yi*Yi)/(2*sigma*sigma));
        }
    }
}

////////////////////////////////////////////////////////////////////

void GaussianKernel::setFwhm(double value)
{
    _fwhm = value;
}

////////////////////////////////////////////////////////////////////

double GaussianKernel::fwhm() const
{
    return _fwhm;
}

////////////////////////////////////////////////////////////////////

void GaussianKernel::setDimension(int value)
{
    _dimension = value;
}

////////////////////////////////////////////////////////////////////

int GaussianKernel::dimension() const
{
    return _dimension;
}

////////////////////////////////////////////////////////////////////
