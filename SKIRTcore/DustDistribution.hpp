/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTDISTRIBUTION_HPP
#define DUSTDISTRIBUTION_HPP

#include "Position.hpp"
#include "SimulationItem.hpp"

class DustMix;

//////////////////////////////////////////////////////////////////////

/** The DustDistribution class is the abstract base class to represent a complete dust
    distribution. This implies a full description of the amount, spatial distribution and optical
    properties of arbitrary dust mixtures. The difference with the DustSystem class is that a dust
    distribution just represents the theoretical distribution, whereas a dust system represents the
    discretized representation on a grid structure. */
class DustDistribution : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust distribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    DustDistribution();

    //======================== Other Functions =======================

public:
    /** This pure virtual function returns the dimension of the dust distribution, which depends on
        the (lack of) symmetry in its geometry. A value of 1 means spherical symmetry, 2 means
        axial symmetry and 3 means none of these symmetries. The function's implementation must be
        provided in a subclass. */
    virtual int dimension() const = 0;

    /** This pure virtual function returns the number of dust components that are involved in the
        dust distribution. */
    virtual int Ncomp() const = 0;

    /** This pure virtual function returns a pointer to the dust mixture corresponding to the
        \f$h\f$'th dust component. */
    virtual DustMix* mix(int h) const = 0;

    /** This function returns the index \f$h\f$ of the dust component that uses the specified
        dust mixture, or -1 if none of the dust components do. This is somewhat of a hack so that
        each dust mix in the distribution can output statistics files with a different name. */
    int indexformix(const DustMix* dustmix);

    /** This pure virtual function returns the mass density \f$\rho_h({\bf{r}})\f$ of the
        \f$h\f$'th component of the dust distribution at the position \f${\bf{r}}\f$. */
    virtual double density(int h, Position bfr) const = 0;

    /** This pure virtual function returns the total mass density \f$\rho({\bf{r}})\f$ of the dust
        distribution at the position \f${\bf{r}}\f$. */
    virtual double density(Position bfr) const = 0;

    /** This pure virtual function generates a random position from the dust distribution, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$, where \f$\rho({\bf{r}})\f$ is
        the total mass density of the dust. */
    virtual Position generatePosition() const = 0;

    /** This pure virtual function returns the total dust mass of the dust distribution. */
    virtual double mass() const = 0;

    /** This pure virtual function returns the X-axis surface density of the dust distribution. */
    virtual double SigmaX() const = 0;

    /** This pure virtual function returns the Y-axis surface density of the dust distribution. */
    virtual double SigmaY() const = 0;

    /** This pure virtual function returns the Z-axis surface density of the dust distribution. */
    virtual double SigmaZ() const = 0;

};

//////////////////////////////////////////////////////////////////////

#endif // DUSTDISTRIBUTION_HPP
