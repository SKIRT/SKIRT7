/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LOCKFREE_HPP
#define LOCKFREE_HPP

#include <atomic>

////////////////////////////////////////////////////////////////////

/** This namespace contains functions that support lock-free programming in a multi-threaded shared
    memory environment. All implementations are provided inline in the header. */
namespace LockFree
{
    /** This function adds the specified double value (which can be an expression)
        to the specified target variable (passed as a reference to a memory location) in a
        thread-safe manner. The function avoids race conditions between concurrent threads by
        implementing a classical compare and swap (CAS) loop using the corresponding atomic
        operation on the target memory location. */
    inline void add(double& target, double value)
    {
        // construct an atom over the target location without initialization (this produces no assembly code)
        std::atomic<double>* atom = new(&target) std::atomic<double>;

        // make a local copy of the target location's value
        double old = *atom;

        // perform the compare and swap (CAS) loop:
        // - if the value of the target location didn't change since we copied it, move the incremented value into it
        // - if the value of the target location did change, make a new local copy and try again
        while( !atom->compare_exchange_weak(old, old+value) ) { }
    }
}

////////////////////////////////////////////////////////////////////

#endif // LOCKFREE_HPP
