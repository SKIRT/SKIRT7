/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STELLARSURFACEGEOMETRY_HPP
#define STELLARSURFACEGEOMETRY_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////


/** The StellarSurfaceGeometry class is a subclass of the Geometry class. It represents the
    surface of a sphere of radius \f$r_*\f$ from which radiation escapes in the outward direction.
    The density profile is spherically symmetric and is simply \f[ \rho(r) = \frac{1}{4\pi\,r_*^2}\,
    \delta(r-r_*). \f] The special aspect of this geometry is that the emissivity is anisotropic.
    There is no radiation outwards and the intensity is proportional to \f$\cos\theta'\f$ in the
    inward hemisphere, where \f$\theta'\f$ is the angle between the direction and the normal on the
    surface. */

class StellarSurfaceGeometry : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar surface geometry with an anisotropic outward radiation field")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the stellar radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE StellarSurfaceGeometry();

protected:
    /** This function verifies the validity of the scale length \f$r_*\f$. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the stellar radius \f$r_*\f$. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the stellar radius \f$r_*\f$. */
    Q_INVOKABLE double radius() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is 1 in this case (the
        geometry is spherically symmetric, even with the anisotropic emissivity). */
    int dimension() const;

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. For this geometry, the density takes the form of a
        Dirac delta function, \f$\rho({\bf{r}}) = \frac{1}{4\pi\,r_*^2}\,\delta(r-r_*)\f$.
        The function returns infinity if \f$r=r_*\f$ and zero in all other cases. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. For this
        geometry, it generates a random position on the unit sphere and this is scaled by
        the stellar radius \f$r_*\f$. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of
        the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,
        {\text{d}}x. \f] For this geometry, \f$ \Sigma_X = 1/(2\pi\,r_*^2) \f$. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of
        the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,
        {\text{d}}y. \f] For this geometry, \f$ \Sigma_Y = 1/(2\pi\,r_*^2) \f$. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For this geometry, \f$ \Sigma_Z = 1/(2\pi\,r_*^2) \f$. */
    double SigmaZ() const;

    // - - - - - - AngularDistribution interface - - - - - -

    /** This function returns the normalized probability for a direction \f${\bf{k}}\f$,
        given that the point of emission is \f${\bf{r}}\f$. This distribution is ill-defined
        if the position \f${\bf{r}}\f$ is not located on the stellar surface, and in this
        case an error is returned. For locations on this sphere, the appropriate probability
        distribution is given by \f[ p({\bf{k}})\, {\text{d}}\Omega = \begin{cases} 4\cos\theta'\,
        {\text{d}}\Omega & 0 \leq \theta' < \frac{\pi}{2} \\ 0 & \frac{\pi}{2} \leq \theta' < \pi
        \end{cases} \f] Here \f$\theta'\f$ is the angle between the direction \f${\bf{k}}\f$ and
        the outward normal on the stellar surface, i.e. \f[ \cos\theta' = \frac{{\bf{k}} \cdot
        {\bf{r}}}{r_{\text{bg}}}. \f] This distribution is correctly normalized in the sense that
        \f[ \frac{1}{4\pi} \iint p({\bf{k}})\, {\text{d}}\Omega = 1. \f] */
    double probabilityForDirection(Position bfr, Direction bfk) const;

    /** This function generates a random direction appropriate for the anisotropic outward
        radiation field at the location \f${\bf{r}}\f$. This distribution is ill-defined
        if the position \f${\bf{r}}\f$ is not located on the stellar surface, and in this
        case an error is returned. In a spherical coordinate system with the Z' axis normal
        to the stellar surface at the position \f${\bf{r}}\f$, the probability distribution is
        given by \f[ p(\theta',\varphi')\, {\text{d}}\theta'\, {\text{d}}\varphi'
        = \frac{1}{\pi}\cos\theta' \sin\theta'\,{\text{d}}\theta'\, {\text{d}}\varphi' \qquad
        0 \leq \theta' < \frac{\pi}{2}. \f] Random angles \f$\theta'\f$ and \f$\varphi'\f$
        can be determined by taking two uniform deviates \f${\cal{X}}_1\f$ and \f${\cal{X}}_2\f$
        and solving the two equations \f[ \begin{split} {\cal{X}}_1 &= \int_0^{\theta'}
        2\sin\theta^*\cos\theta^*\,{\text{d}}\theta^* \\ {\cal{X}}_2 &= \int_0^{\varphi'}
        \frac{{\text{d}}\varphi^*}{2\pi} \end{split} \f] for \f$\theta'\f$ and \f$\varphi'\f$.
        The solution is readily found, \f[ \begin{split} \theta' &= \arcsin
        \sqrt{{\cal{X}}_1} \\ \varphi' &= 2\pi\,{\cal{X}}_2. \end{split} \f] Once these values
        have been determined, we need to determine the coordinates of the vector \f${\bf{k}}\f$
        in the standard coordinate system. The link between the standard XYZ coordinate system
        and the one with the Z' axis determined by the normal on the surface is a joint rotation
        over an angle \f$\varphi\f$ along the Z-axis, followed by a rotation over an angle
        \f$\theta\f$ over the Y'-axis, where \f$\theta\f$ and \f$\varphi\f$ are the
        polar angle and azimuth of the position vector \f${\bf{r}}\f$. The connection is hence
        \f[ \begin{bmatrix} k'_x \\ k'_y \\ k'_z \end{bmatrix}
        =
        \begin{bmatrix}
        \cos\theta & 0 & -\sin\theta \\ 0 & 1 & 0 \\ \sin\theta & 0 & \cos\theta
        \end{bmatrix}
        \begin{bmatrix}
        \cos\phi & \sin\phi & 0 \\ -\sin\phi & \cos\phi & 0 \\ 0 & 0 & 1
        \end{bmatrix}
        \begin{bmatrix} k_x \\ k_y \\ k_z \end{bmatrix}
        \f]
        or in the other direction
        \f[
        \begin{bmatrix} k_x \\ k_y \\ k_z \end{bmatrix}
        =
        \begin{bmatrix}
        \cos\phi & -\sin\phi & 0 \\ \sin\phi & \cos\phi & 0 \\ 0 & 0 & 1
        \end{bmatrix}
        \begin{bmatrix}
        \cos\theta & 0 & \sin\theta \\ 0 & 1 & 0 \\ -\sin\theta & 0 & \cos\theta
        \end{bmatrix}
        \begin{bmatrix} k'_x \\ k'_y \\ k'_z \end{bmatrix}
        \f]
    */
    Direction generateDirection(Position bfr) const;

    //======================== Data Members ========================

private:
    // data members initialized during setup
    double _rstar;
};

////////////////////////////////////////////////////////////////////

#endif // STELLARSURFACEGEOMETRY_HPP
