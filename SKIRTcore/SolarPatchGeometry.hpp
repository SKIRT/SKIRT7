/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SOLARPATCHGEOMETRY_HPP
#define SOLARPATCHGEOMETRY_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The SolarPatchGeometry class is a subclass of the Geometry class. It represents a flat circular
    patch of radius \f$R_{\text{max}}\f$, and it can be considered
    as a circular patch cut out from the solar surface. The density profile is trivially axisymmetric and is given
    by \f[ \rho(R,z) = \begin{cases} \; \dfrac{\delta(z)}{\pi R_{\text{max}}^2} &\qquad
    {\text{if $R<R_{\text{max}}$,}} \\
    \; 0 &\qquad {\text{else.}} \end{cases} \f] The special
    aspect of this geometry is that the radiation is not isotropic. From a given point on the
    circle the angular distribution of the emission is zero in the hemisphere below the surface and proportional
    to \f$\cos\theta\f$ in the outward hemisphere, where \f$\theta\f$ is the polar angle between the outward
    direction and the Z-axis. */

class SolarPatchGeometry : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a solar patch geometry with an anisotropic outward radiation field")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the patch radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SolarPatchGeometry();

protected:
    /** This function verifies the validity of the patch radius \f$R_{\text{max}}\f$. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the patch radius \f$R_{\text{max}}\f$. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the path radius \f$R_{\text{max}}\f$. */
    Q_INVOKABLE double radius() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is 2 in this case. */
    int dimension() const;

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. As the density formally includes a Dirac delta function, it returns
        infinity if \f$z=0\f$ and \f$R<R_{\text{max}}\f$, and zero in all other cases. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. For this
        geometry, it generates a random position within a uniform density circle with radius
        \f$R_{\text{max}}\f$ in the \f$z=0\f$ plane. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of
        the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,
        {\text{d}}x. \f] For this geometry, the result is infinity. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of
        the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,
        {\text{d}}y. \f] For this geometry, the result is infinity. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For this geometry, \f$ \Sigma_Z = 1/\pi R_{\text{max}}^2 \f$. */
    double SigmaZ() const;

    // - - - - - - AngularDistribution interface - - - - - -

    /** This function returns the normalized probability for a direction \f${\bf{k}}\f$, given that
        the point of emission is \f${\bf{r}}\f$. The appropriate probability distribution is given
        by \f[ p({\bf{k}})\, {\text{d}}\Omega = \begin{cases} \; 4\cos\theta\, {\text{d}}\Omega & \quad 0
        \leq \theta < \frac{\pi}{2} \\ \; 0 & \quad \frac{\pi}{2} \leq \theta < \pi \end{cases} \f] Here
        \f$\theta\f$ is the polar angle, i.e. the angle between the direction \f${\bf{k}}\f$ and the Z-axis.
        This distribution is correctly normalized in the sense that \f[ \frac{1}{4\pi} \iint
        p({\bf{k}})\, {\text{d}}\Omega = 1. \f] */
    double probabilityForDirection(Position bfr, Direction bfk) const;

    /** This function generates a random direction appropriate for the anisotropic outward radiation
        field at the location \f${\bf{r}}\f$. The probability distribution is given by
        \f[ p(\theta,\varphi)\, {\text{d}}\theta\, {\text{d}}\varphi
        = \frac{1}{\pi}\cos\theta \sin\theta\,
        {\text{d}}\theta\, {\text{d}}\varphi \qquad 0 \leq \theta < \frac{\pi}{2}. \f] Random angles
        \f$\theta\f$ and \f$\varphi\f$ can be determined
        by taking two uniform deviates \f${\cal{X}}_1\f$ and \f${\cal{X}}_2\f$ and
        solving the two equations \f[ \begin{split} {\cal{X}}_1 &= \int_0^\theta
        2\sin\theta'\cos\theta'\,{\text{d}}\theta' \\ {\cal{X}}_2 &= \int_0^\varphi
        \frac{{\text{d}}\varphi'}{2\pi}
        \end{split} \f] for \f$\theta\f$ and \f$\varphi\f$. The solution is readily found, \f[
        \begin{split} \theta &= \arcsin \sqrt{{\cal{X}}_1} \\ \varphi &= 2\pi\,{\cal{X}}_2.
        \end{split} \f] */
    Direction generateDirection(Position bfr) const;

    //======================== Data Members ========================

private:
    // data members initialized during setup
    double _Rmax;
};

////////////////////////////////////////////////////////////////////

#endif // SOLARPATCHGEOMETRY_HPP
