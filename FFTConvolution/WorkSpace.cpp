/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifdef USING_FFTW3
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "WorkSpace.hpp"
#include "Factorize.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    int FFTW_FACTORS[7] = {13,11,7,5,3,2,0};    // end with zero to detect the end of the array
}

////////////////////////////////////////////////////////////////////

WorkSpace::WorkSpace()
    : _in_src(0), _out_src(0), _in_kernel(0), _out_kernel(0), _h_src(0), _w_src(0), _h_kernel(0),
      _w_kernel(0), _w_fftw(0), _h_fftw(0), _dst_fft(0), _h_dst(0), _w_dst(0)
{
}

////////////////////////////////////////////////////////////////////

void WorkSpace::initialize(Convolution_Mode mode, int w_src, int h_src, int w_kernel, int h_kernel)
{
    // Copy the arguments
    _h_src = h_src;
    _w_src = w_src;
    _h_kernel = h_kernel;
    _w_kernel = w_kernel;
    _mode = mode;

    switch(mode)
    {
    case LINEAR_FULL:
        // Full linear convolution
        _h_fftw = Factorize::find_closest_factor(_h_src + _h_kernel - 1, FFTW_FACTORS);
        _w_fftw = Factorize::find_closest_factor(_w_src + _w_kernel - 1, FFTW_FACTORS);
        _h_dst = _h_src + _h_kernel - 1;
        _w_dst = _w_src + _w_kernel - 1;
        break;
    case LINEAR_SAME_UNPADDED:
        // Same linear convolution
        _h_fftw = _h_src + int(_h_kernel/2.0);
        _w_fftw = _w_src + int(_w_kernel/2.0);
        _h_dst = _h_src;
        _w_dst = _w_src;
        break;
    case LINEAR_SAME:
        // Same linear convolution
        _h_fftw = Factorize::find_closest_factor(_h_src + int(_h_kernel/2.0), FFTW_FACTORS);
        _w_fftw = Factorize::find_closest_factor(_w_src + int(_w_kernel/2.0), FFTW_FACTORS);
        _h_dst = _h_src;
        _w_dst = _w_src;
        break;
    case LINEAR_VALID:
        // Valid linear convolution
        if (_h_kernel > _h_src || _w_kernel > _w_src)
        {
            _h_fftw = 0;
            _w_fftw = 0;
            _h_dst = 0;
            _w_dst = 0;
        }
        else
        {
            _h_fftw = Factorize::find_closest_factor(_h_src, FFTW_FACTORS);
            _w_fftw = Factorize::find_closest_factor(_w_src, FFTW_FACTORS);
            _h_dst = _h_src - _h_kernel + 1;
            _w_dst = _w_src - _w_kernel + 1;
        }
        break;
    case CIRCULAR_SAME:
        _h_dst = _h_src;
        _w_dst = _w_src;
        _h_fftw = _h_dst;
        _w_fftw = _w_dst;
        break;
    case CIRCULAR_SAME_PADDED:
        // Cicular convolution with optimal sizes
        _h_fftw = Factorize::find_closest_factor(_h_src + _h_kernel, FFTW_FACTORS);
        _w_fftw = Factorize::find_closest_factor(_w_src + _w_kernel, FFTW_FACTORS);
        _h_dst = _h_src;
        _w_dst = _w_src;
        break;
    case CIRCULAR_FULL_UNPADDED:
        // We here want to compute a circular convolution modulo h_dst, w_dst
        // These two variables must have been set before calling init_workscape !!
        _h_dst = _h_src + _h_kernel - 1;
        _w_dst = _w_src + _w_kernel - 1;
        _h_fftw = Factorize::find_closest_factor(_h_src + _h_kernel - 1, FFTW_FACTORS);
        _w_fftw = Factorize::find_closest_factor(_w_src + _w_kernel - 1, FFTW_FACTORS);
        break;
    case CIRCULAR_FULL:
        // We here want to compute a circular convolution modulo h_dst, w_dst
        // These two variables must have been set before calling init_workscape !!
        _h_dst = _h_src + _h_kernel - 1;
        _w_dst = _w_src + _w_kernel - 1;
        _h_fftw = _h_dst;
        _w_fftw = _w_dst;
        break;
    default:
        throw std::invalid_argument("Unrecognized convolution mode, possible modes are:\n"
                                    "   - LINEAR_FULL\n"
                                    "   - LINEAR_SAME\n"
                                    "   - LINEAR_SAME_UNPADDED\n"
                                    "   - LINEAR_VALID\n"
                                    "   - CIRCULAR_SAME\n"
                                    "   - CIRCULAR_SAME_PADDED\n"
                                    "   - CIRCULAR_SAME_UNPADDED\n"
                                    "   - CIRCULAR_FULL\n");
    }

    // Allocate memory
    _in_src = new double[_h_fftw * _w_fftw];
    _out_src = (double*) fftw_malloc(sizeof(fftw_complex) * _h_fftw * (_w_fftw/2+1));
    _in_kernel = new double[_h_fftw * _w_fftw];
    _out_kernel = (double*) fftw_malloc(sizeof(fftw_complex) * _h_fftw * (_w_fftw/2+1));
    _dst_fft = new double[_h_fftw * _w_fftw];

    // Initialization of the plans
    _p_forw_src = fftw_plan_dft_r2c_2d(_h_fftw, _w_fftw, _in_src, (fftw_complex*)_out_src, FFTW_ESTIMATE);
    _p_forw_kernel = fftw_plan_dft_r2c_2d(_h_fftw, _w_fftw, _in_kernel, (fftw_complex*)_out_kernel, FFTW_ESTIMATE);

    // The backward FFT takes _out_kernel as input !!
    _p_back = fftw_plan_dft_c2r_2d(_h_fftw, _w_fftw, (fftw_complex*)_out_kernel, _dst_fft, FFTW_ESTIMATE);
}

////////////////////////////////////////////////////////////////////

void WorkSpace::clear()
{
    // Release memory that is no longer used
    delete[] _in_src;
    fftw_free((fftw_complex*)_out_src);
    delete[] _in_kernel;
    fftw_free((fftw_complex*)_out_kernel);
    delete[] _dst_fft;

    // Destroy the plans
    fftw_destroy_plan(_p_forw_src);
    fftw_destroy_plan(_p_forw_kernel);
    fftw_destroy_plan(_p_back);
}

////////////////////////////////////////////////////////////////////

void WorkSpace::convolve(const Array& src, const Array& kernel, Array& dst)
{
    if (_h_fftw <= 0 || _w_fftw <= 0) return;

    // Compute the circular convolution
    fftw_circular_convolution(src, kernel);

    // Depending on the type of convolution one is looking for, we extract the appropriate part of the result from out_src
    int h_offset, w_offset;

    switch(_mode)
    {
    case LINEAR_FULL:
        // Full Linear convolution
        // Here we just keep the first [0:h_dst-1 ; 0:w_dst-1] real part elements of out_src
        for (int i = 0 ; i < _h_dst  ; ++i)
            memcpy(&dst[i*_w_dst], &_dst_fft[i*_w_fftw], _w_dst*sizeof(double));
        break;
    case LINEAR_SAME_UNPADDED:
    case LINEAR_SAME:
        // Same linear convolution
        // Here we just keep the first [h_filt/2:h_filt/2+h_dst-1 ; w_filt/2:w_filt/2+w_dst-1] real part elements of out_src
        h_offset = int(_h_kernel/2.0);
        w_offset = int(_w_kernel/2.0);
        for (int i = 0 ; i < _h_dst ; ++i)
            memcpy(&dst[i*_w_dst], &_dst_fft[(i+h_offset)*_w_fftw + w_offset], _w_dst*sizeof(double));
        break;
    case LINEAR_VALID:
        // Valid linear convolution
        // Here we just take [h_dst x w_dst] elements starting at [h_kernel-1;w_kernel-1]
        h_offset = _h_kernel - 1;
        w_offset = _w_kernel - 1;
        for (int i = 0 ; i < _h_dst ; ++i)
            memcpy(&dst[i*_w_dst], &_dst_fft[(i+h_offset)*_w_fftw + w_offset], _w_dst*sizeof(double));
        break;
    case CIRCULAR_SAME:
    case CIRCULAR_FULL:
    case CIRCULAR_SAME_PADDED:
    case CIRCULAR_FULL_UNPADDED:
        // Circular convolution
        // We copy the first [0:h_dst-1 ; 0:w_dst-1] real part elements of out_src
        for (int i = 0 ; i < _h_dst ; ++i)
            memcpy(&dst[i*_w_dst], &_dst_fft[i*_w_fftw], _w_dst*sizeof(double));
        break;
    default:
        throw std::invalid_argument("Unrecognized convolution mode, possible modes are:\n"
                                    "   - LINEAR_FULL\n"
                                    "   - LINEAR_SAME\n"
                                    "   - LINEAR_SAME_UNPADDED\n"
                                    "   - LINEAR_VALID\n"
                                    "   - CIRCULAR_SAME\n"
                                    "   - CIRCULAR_SAME_PADDED\n"
                                    "   - CIRCULAR_SAME_UNPADDED\n"
                                    "   - CIRCULAR_FULL\n");
    }
}

////////////////////////////////////////////////////////////////////

void WorkSpace::fftw_circular_convolution(const Array& src, const Array& kernel)
{
    double* ptr,* ptr_end,* ptr2;

    // Reset the content of _in_src
    for (ptr = _in_src, ptr_end = _in_src + _h_fftw*_w_fftw ; ptr != ptr_end ; ++ptr)
        *ptr = 0.0;
    for (ptr = _in_kernel, ptr_end = _in_kernel + _h_fftw*_w_fftw ; ptr != ptr_end ; ++ptr)
        *ptr = 0.0;

    // Build the periodic signals
    for (int i = 0 ; i < _h_src ; ++i)
        for (int j = 0 ; j < _w_src ; ++j, ++ptr)
            _in_src[(i%_h_fftw)*_w_fftw+(j%_w_fftw)] += src[i*_w_src + j];
    for (int i = 0 ; i < _h_kernel ; ++i)
        for (int j = 0 ; j < _w_kernel ; ++j, ++ptr)
            _in_kernel[(i%_h_fftw)*_w_fftw+(j%_w_fftw)] += kernel[i*_w_kernel + j];

    // Compute their packed FFT
    fftw_execute(_p_forw_src);
    fftw_execute(_p_forw_kernel);

    // Compute the element-wise product on the packed terms
    // Let's put the element-wise products in _in_kernel
    double re_s, im_s, re_k, im_k;
    for (ptr = _out_src, ptr2 = _out_kernel, ptr_end = _out_src+2*_h_fftw * (_w_fftw/2+1); ptr != ptr_end ; ++ptr, ++ptr2)
    {
        re_s = *ptr;
        im_s = *(++ptr);
        re_k = *ptr2;
        im_k = *(++ptr2);
        *(ptr2-1) = re_s * re_k - im_s * im_k;
        *ptr2 = re_s * im_k + im_s * re_k;
    }

    // Compute the backward FFT. Careful, the backward FFT does not preserve the output
    fftw_execute(_p_back);

    // Scale the transform
    for (ptr = _dst_fft, ptr_end = _dst_fft + _w_fftw*_h_fftw ; ptr != ptr_end ; ++ptr)
        *ptr /= double(_h_fftw*_w_fftw);
}

////////////////////////////////////////////////////////////////////

#endif
