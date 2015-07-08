/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Convolution.hpp"
#include "ConvolutionKernel.hpp"
#include "FatalError.hpp"
#include "FftConvolution.hpp"
#include "Image.hpp"

////////////////////////////////////////////////////////////////////

void Convolution::fft(Image& image, const ConvolutionKernel& kernel)
{
    // Test whether FFT convolution is possible
    if (!FftConvolution::enabled()) throw FATALERROR("The library required for FFT is not present");

    // Initialize an output array
    Array output(image.numpixels());

    // Create an FftConvolution object
    FftConvolution fftc(image.xsize(), image.ysize(), kernel.xsize(), kernel.ysize());

    // Perform the convolution
    fftc.perform(image.data(), kernel.data(), output);

    // Move the output array to the image
    image.steal(output);
}

////////////////////////////////////////////////////////////////////

void Convolution::nested_loop(Image& image, const ConvolutionKernel& kernel)
{
    // Initialize a convolved image
    Array output(image.numpixels());

    // Do the convolution by looping over all input frame pixels and all kernel pixels
    for (int l = 0; l < image.xsize()*image.ysize(); l++)
    {
        // Determine the x and y coordinate of the input image
        int yi = l/image.xsize();
        int xi = l-image.xsize()*yi;

        // Loop over the kernel pixels in the x direction
        for (int xk = 0; xk < kernel.xsize(); xk++)
        {
            // Loop over the kernel pixels in the y direction
            for (int yk = 0; yk < kernel.ysize(); yk++)
            {
                int startX = xi - (kernel.xsize()-1)/2;
                int startY = yi - (kernel.ysize()-1)/2;

                // Exclude pixels out of bound
                if (startX + xk >= 0 && startY + yk >= 0 && startX + xk < image.xsize() && startY + yk < image.ysize())
                {
                    output[(startX + xk)+image.xsize()*(startY + yk)] += image[xi+image.xsize()*(yi)] * kernel(xk,yk);
                }
            }
        }
    }

    // Move the output array to the image
    image.steal(output);
}

////////////////////////////////////////////////////////////////////

void Convolution::convolve(Image& image, const ConvolutionKernel& kernel)
{
    // Use the Fast Fourier Transform method for the convolution if the kernel is sufficiently large
    if (kernel.numpixels() > 200 && FftConvolution::enabled()) fft(image, kernel);

    // Else, use the standard nested loop that iterates over all image and kernel pixels
    else nested_loop(image, kernel);
}

////////////////////////////////////////////////////////////////////
