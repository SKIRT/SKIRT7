/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ISRF_HPP
#define ISRF_HPP

#include "Array.hpp"
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This namespace offers functions to obtain the interstellar radiation field in the solar
    neighborhood, sampled at the simulation's wavelength grid points. The user can choose between
    two different implementations of the ISRF. The first one is the ISRF as in Mathis et al. (1983,
    A&A, 128, 212), which is essentially a sum of three diluted blackbodies with a UV extension.
    The second option is the ISRF as in Kruegel (2007), which is read from a tabulated file
    (derived from the book using Dexter). */
namespace ISRF
{
    /** This function returns the mean intensity \f$J_\lambda\f$ of the ISRF according to Mathis et
        al. (1983, A&A, 128, 212), which is essentially a sum of three diluted blackbodies with a
        UV extension, sampled at the simulation's wavelength grid points \f$\lambda_\ell\f$. The
        argument specifies a simulation item that should be linked into the simulation hierarchy;
        it is used to retrieve the simulation's wavelength grid. Notice that the recipe in Mathis
        et al. (1983) describes the ISRF as \f$4\pi\,J_\lambda\f$, whereas this function returns
        the mean intensity \f$J_\lambda\f$ per steradian. */
    Array mathis(SimulationItem* simitem);

    /** This function returns the mean intensity \f$J_\lambda\f$ of the ISRF as in Kruegel (2007),
        sampled at the simulation's wavelength grid points \f$\lambda_\ell\f$. The ISRF data points
        are read from the resource file <tt>ISRF-Kruegel.dat</tt> (derived from the book using
        Dexter) and then resampled. The argument specifies a simulation item that should be linked
        into the simulation hierarchy; it is used to retrieve the simulation's wavelength grid. */
    Array kruegel(SimulationItem* simitem);

    /** This function returns the radiation intensity \f$B_\lambda(T)\f$ of a black body at the
        temperature \f$T\f$, sampled at the simulation's wavelength grid points \f$\lambda_\ell\f$.
        The first argument specifies a simulation item that should be linked into the simulation
        hierarchy; it is used to retrieve the simulation's wavelength grid. */
    Array blackbody(SimulationItem* simitem, double T);
}

////////////////////////////////////////////////////////////////////

#endif // ISRF_HPP
