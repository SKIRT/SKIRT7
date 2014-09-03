/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef NETZERACCRETIONDISKGEOMETRY_HPP
#define NETZERACCRETIONDISKGEOMETRY_HPP

#include "Array.hpp"
#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The NetzerAccretionDiskGeometry class is a subclass of the Geometry class. It approximates an
    AGN accretion disk as a single point positioned in the centre of the coordinate system, with
    anisotropic emission distributed as proposed by Netzer (1987, MNRAS.225...55N, eq (5)):
    \f[L(\theta)\propto \begin{cases} \cos\theta\,(2\cos\theta+1) & 0\le\theta\le\pi/2 \\
    \cos\theta\,(2\cos\theta-1) & \pi/2\le\theta\le\pi \end{cases} \f]
    The emission pattern is axisymmetric, so this geometry has a dimension of 2. */
class NetzerAccretionDiskGeometry : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a point-like accretion disk geometry with anisotropic Netzer emission")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE NetzerAccretionDiskGeometry();

protected:
    /** This function contructs a vector with the cumulative distribution of the anisotropic
        luminosity in function of \f$\theta\f$. For the Netzer luminosity function \f$L(\theta)\f$
        defined in the header of this class, the cumulative distribution is given by \f[ X(\theta)
        \propto \int_0^\theta L(\theta') \sin\theta' \,{\mathrm{d}}\theta' \f] which, after proper
        normalization, leads to \f[ X(\theta) = \begin{cases} \frac{1}{2} - \frac{2}{7}\cos^3\theta
        - \frac{3}{14}\cos^2\theta & 0\le\theta\le\pi/2 \\ \frac{1}{2} - \frac{2}{7}\cos^3\theta +
        \frac{3}{14}\cos^2\theta & \pi/2\le\theta\le\pi \end{cases} \f] */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is 2 in this case. */
    int dimension() const;

    /** This function returns the stellar density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. For this geometry, the density takes the form of a
        Dirac delta function, \f$\rho({\bf{r}}) = \delta({\bf{r}})\f$. The function returns
        infinity if \f${\bf{r}} = {\bf{0}}\f$ and zero in all other cases. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. For this
        geometry, it always returns the origin of the coordinate system. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of
        the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,
        {\text{d}}x. \f] For this geometry, this integral is infinity. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of
        the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,
        {\text{d}}y. \f] For this geometry, this integral is infinity. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For this geometry, this integral is infinity. */
    double SigmaZ() const;

    // - - - - - - AngularDistribution interface - - - - - -

    /** This function returns the normalized probability for a given direction \f$(\theta,\phi)\f$
        according to the Netzer luminosity profile. It simply implements a properly normalized
        version of the function \f$L(\theta)\f$ defined in the header of this class. */
    double probabilityForDirection(Position bfr, Direction bfk) const;

    /** This function generates a random direction \f$(\phi,\theta)\f$ according to the Netzer
        luminosity profile i.e. with \f$\phi\f$ distributed uniformly over the interval
        \f$0\le\phi\le 2\pi\f$, and \f$\theta\f$ determined from the cumulative distribution
        calculated during setup. */
    Direction generateDirection(Position bfr) const;

    //======================== Data Members ========================

private:
    // data members initialized during setup
    Array _thetav;
    Array _Xv;
};

////////////////////////////////////////////////////////////////////

#endif // NETZERACCRETIONDISKGEOMETRY_HPP
