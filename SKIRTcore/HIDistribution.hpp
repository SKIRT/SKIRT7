/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef HIDISTRIBUTION_HPP
#define HIDISTRIBUTION_HPP

#include "Position.hpp"
#include "SimulationItem.hpp"

//////////////////////////////////////////////////////////////////////

/** The HIDistribution class is the abstract base class to represent a neutral hydrogen
    distribution. This implies a full description of the amount of neutral hydrogen gas, as well as
    the spatial distribution. The difference with the HISystem class is that a neutral hydrogen
    distribution just represents the theoretical distribution, whereas a neutral hydrogen system
    represents the discretized representation on a grid. */
class HIDistribution : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a neutral hydrogen distribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    HIDistribution();

    //======================== Other Functions =======================

public:
    /** This pure virtual function returns the dimension of the neutral hydrogen distribution,
        which depends on the (lack of) symmetry in its geometry. A value of 1 means spherical
        symmetry, 2 means axial symmetry and 3 means none of these symmetries. The function's
        implementation must be provided in a subclass. */
    virtual int dimension() const = 0;

    /** This pure virtual function returns the mass density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. */
    virtual double density(Position bfr) const = 0;

    /** This pure virtual function returns the gas temperature \f$T({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. */
    virtual double gastemperature(Position bfr) const = 0;

    /** This pure virtual function returns the bulk velocity \f$\bar{\bf{v}}({\bf{r}})\f$ at the
        position \f${\bf{r}}\f$. */
    virtual Vec bulkvelocity(Position bfr) const = 0;

    /** This pure virtual function generates a random position from the neutral hydrogen gas
        distribution, by drawing a random point from the three-dimensional probability density
        \f$p({\bf{r}})\, {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$, where
        \f$\rho({\bf{r}})\f$ is the total mass density. */
    virtual Position generatePosition() const = 0;

    /** This pure virtual function returns the total neutral hydrogen mass. */
    virtual double mass() const = 0;

};

//////////////////////////////////////////////////////////////////////

#endif // HIDISTRIBUTION_HPP
