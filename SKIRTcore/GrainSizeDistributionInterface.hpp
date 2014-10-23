/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GRAINSIZEDISTRIBUTIONINTERFACE_HPP
#define GRAINSIZEDISTRIBUTIONINTERFACE_HPP

////////////////////////////////////////////////////////////////////

/** GrainSizeDistributionInterface is a pure interface to communicate the details on a particular
    grain size distribution \f$\Omega(a)=(\frac{\text{d}n_\text{D}}{\text{d}a})/n_\text{H}\f$
    including the range \f$[a_\text{min},a_\text{max}]\f$. */
class GrainSizeDistributionInterface
{
protected:
    /** The empty constructor for the interface. */
    GrainSizeDistributionInterface() { }

public:
    /** The empty destructor for the interface. */
    virtual ~GrainSizeDistributionInterface() { }

    /** This function returns the minimum grain size \f$a_\text{min}\f$, i.e. the lower limit of
        the distribution. */
    virtual double amin() const = 0;

    /** This function returns the maximum grain size \f$a_\text{max}\f$, i.e. the upper limit of
        the distribution. */
    virtual double amax() const = 0;

    /** This function returns the value of the distribution
        \f$\Omega(a)=(\frac{\text{d}n_\text{D}}{\text{d}a})/n_\text{H}\f$ for a given grain size
        \f$a\f$. If \f$a<a_\text{min}\f$ or \f$a>a_\text{max}\f$ the result is undefined (i.e. the
        function does not need to check the bounds). For historical reasons, the function is named
        dnda() while it in fact returns the value of \f$\Omega(a)\f$ defined above. */
    virtual double dnda(double a) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

#endif // GRAINSIZEDISTRIBUTIONINTERFACE_HPP
