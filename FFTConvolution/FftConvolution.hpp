/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FFTCONVOLUTION_HPP
#define FFTCONVOLUTION_HPP

class Array;
class WorkSpace;

////////////////////////////////////////////////////////////////////

/** This class can be used to compute the convolution of 2D data using the Fast Fourier
    Transform method. */
class FftConvolution
{
    //============= Construction - Setup - Destruction =============

public:
    /** The constructor initializes the workspace which is used to compute the convolution. */
    FftConvolution(int input_xsize, int input_ysize, int kernel_xsize, int kernel_ysize);

    /** The destructor clears the workspace which was used to compute the convolution. */
    ~FftConvolution();

    //======================== Other Functions =======================

    /** This function perform the convolution. As its first two arguments, it takes const references to
        the input Array and the kernel Array. The third argument is a reference to the output Array,
        which will contain the results after the convolution is completed. */
    void perform(const Array& input, const Array& kernel, Array& output);

    /** This function returns whether FFT convolution is enabled (the required external library is
        present) or not. */
    static bool enabled();

    //======================== Data Members ========================

private:
    WorkSpace* _ws;
};

////////////////////////////////////////////////////////////////////

#endif // FFTCONVOLUTION_HPP
