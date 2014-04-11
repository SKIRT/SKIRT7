/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "Convolution.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include <cmath>

using namespace std;

////////////////////////////////////////////////////////////////////

Convolution::Convolution()
    : _fwhm(0), _dimension(0)
{
}

////////////////////////////////////////////////////////////////////

void Convolution::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify fwhm values
    if (_fwhm < 0) throw FATALERROR("FHWM should be positive");

    // verify dimension value
     if (_dimension < 0) throw FATALERROR("Frame dimension should be positive");
}

////////////////////////////////////////////////////////////////////

void Convolution::setFwhm(double value)
{
    _fwhm = value;
}

////////////////////////////////////////////////////////////////////

double Convolution::fwhm() const
{
    return _fwhm;
}

////////////////////////////////////////////////////////////////////

void Convolution::setDimension(int value)
{
    _dimension = value;
}

////////////////////////////////////////////////////////////////////

int Convolution::dimension() const
{
    return _dimension;
}

////////////////////////////////////////////////////////////////////

void Convolution::convolve(Array *frame, int xdim, int ydim) const
{
    double sum = 0;
    double sigma = _fwhm/2.3548;

    //create kernel frame
    double conv[_dimension][_dimension];
    for (int yk=1; yk< _dimension+1; yk++)
    {
        for (int xk=1; xk< _dimension+1; xk++)
        {
            int Xi = (_dimension+1)/2 - xk;
            int Yi = (_dimension+1)/2 - yk;
            conv[xk-1][yk-1] = 1/(2*M_PI*sigma*sigma) * exp(-(Xi*Xi+Yi*Yi)/(2*sigma*sigma));
            sum += conv[xk-1][yk-1];
        }
    }

    //create convolved image by looping over all frame pixels and all kernel pixels
    Array convolved_frame(frame->size());

    for (int l=0; l< xdim*ydim; l++)
    {
        int yi = l/xdim;
        int xi = l-xdim*yi;

        for (int xk=0; xk< _dimension; xk++)
        {
            for (int yk=0; yk< _dimension; yk++)
            {
                int startX = xi - (_dimension-1)/2;
                int startY = yi - (_dimension-1)/2;

                //excluding pixels out of bound
                if (startX + xk >= 0 && startY + yk >= 0
                    && startX + xk < xdim && startY + yk < ydim)
                    convolved_frame[(startX + xk)+xdim*(startY + yk)] += (*frame)[xi+xdim*(yi)] * 1/sum * conv[xk][yk];
            }
        }
    }

    //create output array
    for (int m=0;m<xdim*ydim;m++)
    {
        (*frame)[m] = convolved_frame[m];
    }
}

////////////////////////////////////////////////////////////////////
