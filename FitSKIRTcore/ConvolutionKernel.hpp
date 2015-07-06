/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CONVOLUTIONKERNEL_HPP
#define CONVOLUTIONKERNEL_HPP

#include "Image.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The ConvolutionKernel class is used to describe a general convolution kernel. Subclasses of
    this class represent specific types of convolution kernels. */
class ConvolutionKernel : public SimulationItem, public Image
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a convolution kernel")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor. */
    Q_INVOKABLE ConvolutionKernel();

    /** This purpose of this function, which is re-implemented in each derived class, is to initialize
        the data that describes the kernel. */
    void setupSelfBefore();

    /** This function, only implemented in the base class, makes sure that the kernel is properly
        normalized. */
    void setupSelfAfter();
};

////////////////////////////////////////////////////////////////////

#endif // CONVOLUTIONKERNEL_HPP
