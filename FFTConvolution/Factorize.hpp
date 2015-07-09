/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FACTORIZE_HPP
#define FACTORIZE_HPP

////////////////////////////////////////////////////////////////////

/** This namespace provides some convenience functions used by the WorkSpace class. */
namespace Factorize
{
    // The code for this function is adapted from the GNU Scientific Library / fft / factorize.c
    void factorize(const int n, int* n_factors, int factors[], int* implemented_factors);
    bool is_optimal(int n, int* implemented_factors);
    int find_closest_factor(int n, int* implemented_factor);
}

////////////////////////////////////////////////////////////////////

#endif // FACTORIZE_HPP
