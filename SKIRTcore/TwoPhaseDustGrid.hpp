/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TWOPHASEDUSTGRID_HPP
#define TWOPHASEDUSTGRID_HPP

#include "CartesianDustGrid.hpp"

////////////////////////////////////////////////////////////////////

/** The TwoPhaseDustGrid class is a subclass of the CartesianDustGrid class. It can be used to add
    a two-phase aspect presciption to arbitrary three-dimensional dust distributions. The basic
    idea behind the TwoPhaseDustGrid class is that it represents a regular cartesian grid, but an
    additional weight factor is attached to each dust cell. The weight factor of each cell is
    determined randomly using the method described by Witt & Gordon (1996, ApJ, 463, 681). When a
    smooth dust density distribution \f$\rho({\bf{r}})\f$ is then discretized on this grid, the
    grid can take into account this additional weight factor to simulate a two-phase distribution,
    with a low-density and a high-density medium. Internally, a two-phase dust grid is just a
    regular three-dimensional cartesian dust grid, with a vector with the weight factor of each
    grid cell as an additional data member. */
class TwoPhaseDustGrid : public CartesianDustGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a 3D dust grid with a two-phase medium")

    Q_CLASSINFO("Property", "fillingFactor")
    Q_CLASSINFO("Title", "the volume filling factor of the high-density medium")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")

    Q_CLASSINFO("Property", "contrast")
    Q_CLASSINFO("Title", "the density contrast between the high- and low-density medium")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TwoPhaseDustGrid();

protected:
    /** This function randomly determines the weight factor for each dust cell. We follow the
        prescriptions of Witt & Gordon (1996, ApJ, 463, 681): for each cell, we generate a uniform
        deviate \f${\cal{X}}\f$ and calculate the weight factor according to the formula \f[ w =
        \begin{cases}\; \dfrac{C}{C\,{\text{ff}}+1-{\text{ff}}} & \qquad {\text{if }}
        0<{\cal{X}}<{\text{ff}}, \\ \dfrac{1}{C\,{\text{ff}}+1-{\text{ff}}} & \qquad {\text{if }}
        {\text{ff}}<{\cal{X}}<1. \end{cases} \f] with \f$C\f$ the density contrast between the
        high-density and low-density medium, and \f${\text{ff}}\f$ the volume filling factor of the
        high-density medium. The mean weight factor is readily found by noting that, statistically,
        a fraction \f${\text{ff}}\f$ of all cells will belong to the high-density medium and a
        fraction \f$1-{\text{ff}}\f$ to the low-density medium, and hence \f[ \langle w \rangle =
        {\text{ff}}\, \dfrac{C}{C\,{\text{ff}}+1-{\text{ff}}} + (1-{\text{ff}})\,
        \dfrac{1}{C\,{\text{ff}}+1-{\text{ff}}} = 1, \f] as desired. All weights are stored in an
        internal data vector. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the volume filling factor of the high-density medium. */
    Q_INVOKABLE void setFillingFactor(double value);

    /** Returns the volume filling factor of the high-density medium. */
    Q_INVOKABLE double fillingFactor() const;

    /** Sets the density contrast between the high- and low-density medium. */
    Q_INVOKABLE void setContrast(double value);

    /** Returns the density contrast between the high- and low-density medium. */
    Q_INVOKABLE double contrast() const;

    //======================== Other Functions =======================

public:
    /** This function returns the weight corresponding to the cell with cell number \f$m\f$. It
        overwrites the default weight function. */
    double weight(int m) const;

    //======================== Data Members ========================

private:
    double _contrast;
    double _fillingfactor;
    Array _weightv;
};

////////////////////////////////////////////////////////////////////

#endif // TWOPHASEDUSTGRID_HPP
