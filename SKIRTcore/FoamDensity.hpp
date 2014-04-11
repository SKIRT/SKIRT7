/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef FOAMDENSITY_HPP
#define FOAMDENSITY_HPP

////////////////////////////////////////////////////////////////////

/** FoamDensity is a pure interface representing an arbitrary probability density distribution as
    an n-dimensional real integrand function. The interface is used as call-back by the Foam class,
    which uses it to construct an appropriate foam for generating random numbers with the
    implicitly specified non-analytical distribution. Derived classes must implement the
    foamdensity() function. */
class FoamDensity
{
protected:
    /** The empty constructor for the interface. */
    FoamDensity() { }
public:
    /** The empty destructor for the interface. */
    virtual ~FoamDensity() { }

    /** This function must be implemented in a derived class. The function returns the density in
        the point in the unit volume specified by the input parameters. The first argument \em ndim
        specifies the dimension of the space on which the probability density distribution is
        defined. In the context of SKIRT \em ndim is always 2 (unit square) or 3 (unit cube),
        depending on the options passed to the Foam class to begin with. The second argument is an
        array containing \em ndim coordinates specifying the point in the unit volume for which the
        density must be calculated. */
    virtual double foamdensity(int ndim, double* par) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

#endif // FOAMDENSITY_HPP
