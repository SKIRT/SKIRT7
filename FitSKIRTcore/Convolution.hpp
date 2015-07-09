/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CONVOLUTION_HPP
#define CONVOLUTION_HPP

class Image;
class ConvolutionKernel;

////////////////////////////////////////////////////////////////////

/** This namespace offers functions that perform convolutions on an image with a certain kernel. */
namespace Convolution
{
    /** This function performs the convolution of the image using the Fast Fourier Transform (FFT) method. */
    void fft(Image& image, const ConvolutionKernel& kernel);

    /** This function performs the convolution using nested loops (a loop over the image pixels inside
        a loop over the kernel pixels). */
    void nested_loop(Image& image, const ConvolutionKernel& kernel);

    /** This function convolves a certain image with a given convolution kernel */
    void convolve(Image& Image, const ConvolutionKernel& kernel);
}

////////////////////////////////////////////////////////////////////

#endif // CONVOLUTION_HPP
