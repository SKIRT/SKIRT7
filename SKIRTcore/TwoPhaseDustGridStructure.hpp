/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TWOPHASEDUSTGRIDSTRUCTURE_HPP
#define TWOPHASEDUSTGRIDSTRUCTURE_HPP

#include "CubDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The TwoPhaseDustGridStructure class is a subclass of the CubDustGridStructure class. It can be
    used to add a two-phase aspect presciption to arbitrary three-dimensional dust distributions.
    The basic idea behind the TwoPhaseDustGridStructure class is that it represents a regular,
    linear cartesian grid structure, but an additional weight factor is attached to each dust cell.
    The weight factor of each cell is determined randomly using the method described by Witt &
    Gordon (1996, ApJ, 463, 681). When a smooth dust density distribution \f$\rho({\bf{r}})\f$ is
    then discretized on this grid, the grid structure can take into account this additional weight
    factor to simulate a two-phase distribution, with a low-density and a high-density medium.
    Internally, a two-phase dust grid structure just a regular three-dimensional cartesian dust
    grid structure, with a vector with the weight factor of each grid cell as an additional data
    member. */
class TwoPhaseDustGridStructure : public CubDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a 3D dust grid structure with a two-phase medium")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the outer radius in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "pointsX")
    Q_CLASSINFO("Title", "the number of grid points in the x direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the outer radius in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "pointsY")
    Q_CLASSINFO("Title", "the number of grid points in the y direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "extentZ")
    Q_CLASSINFO("Title", "the outer radius in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "pointsZ")
    Q_CLASSINFO("Title", "the number of grid points in the z direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "fillingFactor")
    Q_CLASSINFO("Title", "the volume filling factor of the high-density medium")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")

    Q_CLASSINFO("Property", "contrast")
    Q_CLASSINFO("Title", "the density contrast between the high- and low-density medium")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TwoPhaseDustGridStructure();

protected:
    /** This function verifies the validity of the number of bins and the maximum extent of the
        grid in the X, Y and Z directions, and then calculates the grid points along each of these
        axes as \f[ \begin{split} x_i &= -x_{\text{max}} + \frac{2i\,x_{\text{max}}}{N_x} \qquad
        i=0,\ldots,N_x, \\ y_j &= -y_{\text{max}} + \frac{2j\,y_{\text{max}}}{N_y} \qquad
        j=0,\ldots,N_y, \\ z_k &= -z_{\text{max}} + \frac{2k\,z_{\text{max}}}{N_z} \qquad
        k=0,\ldots,N_z. \end{split} \f] The second phase consists of randomly determining the
        weight factor for each dust cell. We follow the prescriptions of Witt & Gordon (1996, ApJ,
        463, 681): for each cell, we generate a uniform deviate \f${\cal{X}}\f$ and calculate the
        weight factor according to the formula \f[ w = \begin{cases}\;
        \dfrac{C}{C\,{\text{ff}}+1-{\text{ff}}} & \qquad {\text{if }} 0<{\cal{X}}<{\text{ff}}, \\
        \dfrac{1}{C\,{\text{ff}}+1-{\text{ff}}} & \qquad {\text{if }} {\text{ff}}<{\cal{X}}<1.
        \end{cases} \f] with \f$C\f$ the density contrast between the high-density and low-density
        medium, and \f${\text{ff}}\f$ the volume filling factor of the high-density medium. The
        mean weight factor is readily found by noting that, statistically, a fraction
        \f${\text{ff}}\f$ of all cells will belong to the high-density medium and a fraction
        \f$1-{\text{ff}}\f$ to the low-density medium, and hence \f[ \langle w \rangle =
        {\text{ff}}\, \dfrac{C}{C\,{\text{ff}}+1-{\text{ff}}} + (1-{\text{ff}})\,
        \dfrac{1}{C\,{\text{ff}}+1-{\text{ff}}} = 1, \f] as desired. All weights are stored in an
        internal data vector. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius in the x direction. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the outer radius in the x direction. */
    Q_INVOKABLE double extentX() const;

    /** Sets the number of grid points in the x direction. */
    Q_INVOKABLE void setPointsX(int value);

    /** Returns the number of grid points in the x direction. */
    Q_INVOKABLE int pointsX() const;

    /** Sets the outer radius in the y direction. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the outer radius in the y direction. */
    Q_INVOKABLE double extentY() const;

    /** Sets the number of grid points in the y direction. */
    Q_INVOKABLE void setPointsY(int value);

    /** Returns the number of grid points in the y direction. */
    Q_INVOKABLE int pointsY() const;

    /** Sets the outer radius in the z direction. */
    Q_INVOKABLE void setExtentZ(double value);

    /** Returns the outer radius in the z direction. */
    Q_INVOKABLE double extentZ() const;

    /** Sets the number of grid points in the z direction. */
    Q_INVOKABLE void setPointsZ(int value);

    /** Returns the number of grid points in the z direction. */
    Q_INVOKABLE int pointsZ() const;

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
        overwrites the default weight function of the general DustGridStructure class. */
    double weight(int m) const;

    //======================== Data Members ========================

private:
    double _contrast;
    double _fillingfactor;
    Array _weightv;
};

////////////////////////////////////////////////////////////////////

#endif // TWOPHASEDUSTGRIDSTRUCTURE_HPP
