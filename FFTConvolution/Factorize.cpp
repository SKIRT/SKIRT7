/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <stdexcept>
#include "Factorize.hpp"

////////////////////////////////////////////////////////////////////

void Factorize::factorize(const int n, int* n_factors, int factors[], int* implemented_factors)
{
    int nf = 0;
    int ntest = n;
    int factor;
    int i = 0;

    if (n == 0) throw std::invalid_argument("Length n must be a positive integer");
    if (n == 1)
    {
        factors[0] = 1;
        *n_factors = 1;
        return;
    }

    // Deal with the implemented factors
    while (implemented_factors[i] && ntest != 1)
    {
        factor = implemented_factors[i];
        while ((ntest % factor) == 0)
        {
            ntest = ntest / factor;
            factors[nf] = factor;
            nf++;
        }
        i++;
    }

    // Ok that's it
    if (ntest != 1)
    {
        factors[nf] = ntest;
        nf++;
    }

    // Check that the factorization is correct
    {
        int product = 1;
        for (i = 0; i < nf; i++) product *= factors[i];
        if (product != n) throw std::runtime_error("Factorization failed");
    }
    *n_factors = nf;
}

////////////////////////////////////////////////////////////////////

bool Factorize::is_optimal(int n, int* implemented_factors)
{
    // We check that n is not a multiple of 4*4*4*2
    if (n % 4*4*4*2 == 0) return false;

    int nf=0;
    int factors[64];
    int i = 0;
    factorize(n, &nf, factors, implemented_factors);

    // We just have to check if the last factor belongs to GSL_FACTORS
    while (implemented_factors[i])
    {
        if (factors[nf-1] == implemented_factors[i]) return true;
        ++i;
    }
    return false;
}

////////////////////////////////////////////////////////////////////

int Factorize::find_closest_factor(int n, int* implemented_factor)
{
    int j;
    if (is_optimal(n, implemented_factor)) return n;
    else
    {
        j = n+1;
        while (!is_optimal(j, implemented_factor)) ++j;
        return j;
    }
}

////////////////////////////////////////////////////////////////////
