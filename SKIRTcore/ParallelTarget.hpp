/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PARALLELTARGET_HPP
#define PARALLELTARGET_HPP

#include <cstddef>

////////////////////////////////////////////////////////////////////

/** ParallelTarget is an abstract base class for objects that serve as a target for the call()
    function in the Parallel class. */
class ParallelTarget
{
public:
    /** The default constructor; does nothing. */
    ParallelTarget();

    /** The default destructor; does nothing. */
    virtual ~ParallelTarget();

    /** The function that will be invoked by the Parallel class as the body of a parallelized for
        loop. The argument is the index of the loop. This function must be implemented in the
        derived class. */
    virtual void body(size_t index) = 0;
};

////////////////////////////////////////////////////////////////////

#endif // PARALLELTARGET_HPP
