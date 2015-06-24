/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ConvolutionKernel.hpp"

////////////////////////////////////////////////////////////////////

ConvolutionKernel::ConvolutionKernel()
{
}

////////////////////////////////////////////////////////////////////

void ConvolutionKernel::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void ConvolutionKernel::setupSelfAfter()
{
    _data /= _data.sum();
}

////////////////////////////////////////////////////////////////////

void ConvolutionKernel::convolve(Array& input, int xdim, int ydim) const
{
    // Initialize a convolved image
    Array output(input.size());

    // Do the convolution by looping over all input frame pixels and all kernel pixels
    for (int l=0; l<xdim*ydim; l++)
    {
        // Determine the x and y coordinate of the input image
        int yi = l/xdim;
        int xi = l-xdim*yi;

        // Loop over the kernel pixels in the x direction
        for (int xk=0; xk< xsize(); xk++)
        {
            // Loop over the kernel pixels in the y direction
            for (int yk=0; yk< ysize(); yk++)
            {
                int startX = xi - (xsize()-1)/2;
                int startY = yi - (ysize()-1)/2;

                // Exclude pixels out of bound
                if (startX + xk >= 0 && startY + yk >= 0 && startX + xk < xdim && startY + yk < ydim)
                {
                    output[(startX + xk)+xdim*(startY + yk)] += input[xi+xdim*(yi)] * Image::operator()(xk,yk);
                }
            }
        }
    }

    // Replace the input image by the convolved image
    for (int m=0; m<xdim*ydim; m++)
    {
        input[m] = output[m];
    }
}

////////////////////////////////////////////////////////////////////
