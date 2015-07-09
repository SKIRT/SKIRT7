/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Array.hpp"
#include "FftConvolution.hpp"
#include "WorkSpace.hpp"

////////////////////////////////////////////////////////////////////

FftConvolution::FftConvolution(int input_xsize, int input_ysize, int kernel_xsize, int kernel_ysize)
{
#ifdef USING_FFTW3
    // Create a new workspace
    _ws = new WorkSpace();

    // Initialize the workspace
    _ws->initialize(LINEAR_SAME, input_xsize, input_ysize, kernel_xsize, kernel_ysize);
#else
    _ws = nullptr;
    (void)input_xsize; (void)input_ysize; (void)kernel_xsize; (void)kernel_ysize;
#endif
}

////////////////////////////////////////////////////////////////////

FftConvolution::~FftConvolution()
{
#ifdef USING_FFTW3
    // Clear the workspace
    _ws->clear();

    // Delete the workspace
    delete _ws;
#endif
}

////////////////////////////////////////////////////////////////////

void FftConvolution::perform(const Array& input, const Array& kernel, Array& output)
{
#ifdef USING_FFTW3
    // Do the convolution
    _ws->convolve(input, kernel, output);
#else
    (void)input; (void)kernel; (void)output;
#endif
}

////////////////////////////////////////////////////////////////////

bool FftConvolution::enabled()
{
#ifdef USING_FFTW3
    return true;
#else
    return false;
#endif
}

////////////////////////////////////////////////////////////////////
