/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef WORKSPACE_HPP
#define WORKSPACE_HPP

#ifdef USING_FFTW3
#include <fftw3.h>
#include <mutex>
#include "Array.hpp"
#endif

//////////////////////////////////////////////////////////////////////

typedef enum
{
    LINEAR_FULL,
    LINEAR_SAME_UNPADDED,
    LINEAR_SAME,
    LINEAR_VALID,
    CIRCULAR_SAME,
    CIRCULAR_SAME_PADDED,
    CIRCULAR_FULL_UNPADDED,
    CIRCULAR_FULL
} Convolution_Mode;

//////////////////////////////////////////////////////////////////////

/** The WorkSpace class provides the implementation of the FFT convolution procedure. */
class WorkSpace
{
    //============= Construction - Setup - Destruction =============

public:
    /** Default constructor. */
    WorkSpace();

#ifdef USING_FFTW3
    //======================== Other Functions =======================

public:
    /** This function initializes the workspace. */
    void initialize(Convolution_Mode mode, int w_src, int h_src, int w_kernel, int h_kernel);

    /** This function releases the data structures created to compute the convolution. */
    void clear();

    /** This function performs the actual convolution. As arguments, it takes the source Array, the
        kernel Array and the destination Array. */
    void convolve(const Array& src, const Array& kernel, Array& dst);

private:
    /** This private function calculates the circular convolution of  */
    void fftw_circular_convolution(const Array& src, const Array& kernel);

    //======================== Data Members ========================

private:
    double* _in_src, * _out_src, * _in_kernel, * _out_kernel;
    int _h_src, _w_src, _h_kernel, _w_kernel;
    int _w_fftw, _h_fftw;
    Convolution_Mode _mode;
    double* _dst_fft;
    int _h_dst, _w_dst;       // the size of the destination Array; this is automatically set by initialize
    fftw_plan _p_forw_src;
    fftw_plan _p_forw_kernel;
    fftw_plan _p_back;
    std::mutex _mutex;
#endif
};

////////////////////////////////////////////////////////////////////

#endif // WORKSPACE_HPP
