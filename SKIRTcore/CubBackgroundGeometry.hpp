/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CUBBACKGROUNDGEOMETRY_HPP
#define CUBBACKGROUNDGEOMETRY_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The CubBackgroundGeometry class is a subclass of the Geometry class. It represents the
    surface (i.e. the combination of the six walls) of a cube with side \f$2h\f$ from which
    radiation escapes in the inward direction. The emissivity is anisotropic: there is no radiation
    outwards and the inward emissivity from each wall is proportional to \f$\cos\theta'\f$, where
    \f$\theta'\f$ is the angle between the direction and the normal on the wall. */

class CubBackgroundGeometry : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a cubic background geometry with an anisotropic inward radiation field")

    Q_CLASSINFO("Property", "extent")
    Q_CLASSINFO("Title", "the extent of the background cube")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE CubBackgroundGeometry();

protected:
    /** This function verifies the validity of the cube extent \f$h\f$. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the background cube extent \f$h\f$. */
    Q_INVOKABLE void setExtent(double value);

    /** Returns the background cube extent \f$h\f$. */
    Q_INVOKABLE double extent() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is 3 in this case. */
    int dimension() const;

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. The density is hard to write down in a closed formula, and this is not
        necessary in the end. The function returns infinity if the \f${\bf{r}}\f$ is located on
        one of the six walls, and zero in all other cases. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random
        point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. The function first generates
        a random wall, and subsequently a random position within this wall. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of
        the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,
        {\text{d}}x. \f] For this geometry, \f$ \Sigma_X = 1/(12h^2) \f$. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of
        the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,
        {\text{d}}y. \f] For this geometry, \f$ \Sigma_Y = 1/(12h^2) \f$. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For this geometry, \f$ \Sigma_Z = 1/(12h^2) \f$. */
    double SigmaZ() const;

    // - - - - - - AngularDistribution interface - - - - - -

    /** This function returns the normalized probability for a direction \f${\bf{k}}\f$,
        given that the point of emission is \f${\bf{r}}\f$. This distribution is ill-defined
        if the position \f${\bf{r}}\f$ is not located on one of the walls of the background
        cube, and in this case an error is returned. For locations on this cube, the appropriate
        probability distribution is given by \f[ p({\bf{k}})\, {\text{d}}\Omega = \begin{cases} 0
        & 0 \leq \theta' < \frac{\pi}{2} \\ -4\cos\theta'\, {\text{d}}\Omega & \frac{\pi}{2}
        \leq \theta' < \pi \end{cases} \f] Here \f$\theta'\f$ is the angle between the
        direction \f${\bf{k}}\f$ and the outward normal on the wall of the cube. For each of the
        walls, this angle is easily calculated. */
    double probabilityForDirection(int ell, Position bfr, Direction bfk) const;

    /** This function returns the normalized probability for a direction \f${\bf{k}}\f$,
        given that the point of emission is \f${\bf{r}}\f$. This distribution is ill-defined
        if the position \f${\bf{r}}\f$ is not located on one of the walls of the background
        cube, and in this case an error is returned. In a spherical coordinate system with the
        Z' axis normal to the surface at the position \f${\bf{r}}\f$, the probability distribution
        is given by \f[ p(\theta',\varphi')\, {\text{d}}\theta'\, {\text{d}}\varphi'
        = -\frac{1}{\pi}\cos\theta' \sin\theta'\,{\text{d}}\theta'\, {\text{d}}\varphi' \qquad
        \frac{\pi}{2} \leq \theta' < \pi. \f] Random angles \f$\theta'\f$ and \f$\varphi'\f$
        can be determined by taking two uniform deviates \f${\cal{X}}_1\f$ and \f${\cal{X}}_2\f$
        and solving the two equations \f[ \begin{split} {\cal{X}}_1 &= -\int_{\pi/2}^{\theta'}
        2\sin\theta^{\prime\prime}\cos\theta^{\prime\prime}\,{\text{d}}\theta^{\prime\prime}
        \\ {\cal{X}}_2 &= \int_0^{\varphi'} \frac{{\text{d}}\varphi^{\prime\prime}}{2\pi}
        \end{split} \f] for \f$\theta'\f$ and \f$\varphi'\f$.
        The solution is readily found, \f[ \begin{split} \theta' &= \pi - \arccos
        \sqrt{{\cal{X}}_1} \\ \varphi' &= 2\pi\,{\cal{X}}_2. \end{split} \f] Once these values
        have been determined, we need to rotate such that the Z' axis is perpendicular to the
        wall corresponding to the position \f${\bf{r}}\f$. */
    Direction generateDirection(int ell, Position bfr) const;

    //======================== Data Members ========================

private:
    // data members initialized during setup
    double _h;
};

////////////////////////////////////////////////////////////////////

#endif // CUBBACKGROUNDGEOMETRY_HPP
