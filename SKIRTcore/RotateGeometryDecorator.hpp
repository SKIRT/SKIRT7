/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ROTATEGEOMETRYDECORATOR_HPP
#define ROTATEGEOMETRYDECORATOR_HPP

#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The RotateGeometryDecorator class is a decorator that applies an arbitrary rotation to any
    geometry, including anisotropic geometries. For the rotation, we use the general framework of
    the three Euler angles that can be used to decompose any rotation into a sequence of three
    individual rotations over the principle axes. We apply the following set of rotations (the
    so-called X-convention):
    <ul>
    <li> the first rotation is by an angle \f$\alpha\f$ about the Z axis.
    <li> the second rotation is by an angle \f$\beta\f$ about the new X' axis.
    <li> the third rotation is by an angle \f$\gamma\f$ about the new Z'' axis.
    </ul>
    If the original position of a vector is denoted as \f${\bf{r}}_{\text{orig}}\f$, the
    new position can be found as \f${\bf{r}} = {\bf{R}}\,{\bf{r}}_{\text{orig}}\f$, where
    the rotation matrix \f${\bf{R}}\f$ is given by
    \f[
    {\bf{R}}
    =
    \begin{pmatrix}
    \cos\gamma & \sin\gamma & 0 \\ -\sin\gamma & \cos\gamma & 0 \\ 0 & 0 & 1
    \end{pmatrix}
    \begin{pmatrix}
    1 & 0 & 0 \\ 0 & \cos\beta & \sin\beta \\ 0 & -\sin\beta & \cos\beta
    \end{pmatrix}
    \begin{pmatrix}
    \cos\alpha & \sin\alpha & 0 \\ -\sin\alpha & \cos\alpha & 0 \\ 0 & 0 & 1
    \end{pmatrix}
    \f]
    or explicitly
    \f[
    {\bf{R}}
    =
    \begin{pmatrix}
    \cos\alpha\cos\gamma-\sin\alpha\cos\beta\sin\gamma &
    \cos\gamma\sin\alpha+\cos\alpha\cos\beta\sin\gamma &
    \sin\beta\sin\gamma \\
    -\cos\alpha\sin\gamma-\sin\alpha\cos\beta\cos\gamma &
    -\sin\alpha\sin\gamma+\cos\alpha\cos\beta\cos\gamma &
    \sin\beta\cos\gamma \\
    \sin\alpha\sin\beta &
    -\cos\alpha\sin\beta &
    \cos\beta
    \end{pmatrix}
    \f]
    The properties of a RotateGeometryDecorator object are a reference to the Geometry object
    being decorated, and the three Euler angles \f$(\alpha,\beta,\gamma)\f$ that describe the
    rotation. The resulting geometry is identical to the geometry being decorated, except that
    the density distribution is rotated over the three Euler angles.
*/
class RotateGeometryDecorator : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that adds a rotation to any geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to be rotated")

    Q_CLASSINFO("Property", "euleralpha")
    Q_CLASSINFO("Title", "the first Euler angle alpha")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "360 deg")
    Q_CLASSINFO("Default", "0 deg")

    Q_CLASSINFO("Property", "eulerbeta")
    Q_CLASSINFO("Title", "the second Euler angle beta")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "180 deg")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "eulergamma")
    Q_CLASSINFO("Title", "the third Euler angle gamma")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "360 deg")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE RotateGeometryDecorator();

    /** This function verifies the validity of the different parameters and
        calculates some auxiliary parameters. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the geometry to be offset (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the geometry to be offset (i.e. the geometry being decorated). */
    Q_INVOKABLE Geometry* geometry() const;

    /** Sets the first Euler angle \f$\alpha\f$. */
    Q_INVOKABLE void setEuleralpha(double value);

    /** Returns the first Euler angle \f$\alpha\f$. */
    Q_INVOKABLE double euleralpha() const;

    /** Sets the second Euler angle \f$\beta\f$. */
    Q_INVOKABLE void setEulerbeta(double value);

    /** Returns the second Euler angle \f$\beta\f$. */
    Q_INVOKABLE double eulerbeta() const;

    /** Sets the third Euler angle \f$\gamma\f$. */
    Q_INVOKABLE void setEulergamma(double value);

    /** Returns the third Euler angle \f$\gamma\f$. */
    Q_INVOKABLE double eulergamma() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. It calls the density() function for the geometry being decorated with
        the derotated position \f${\bf{r}}_{\text{orig}} = {\bf{R}}^{\text{T}}\,{\bf{r}}\f$ as
        the argument. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random
        point from the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It calls the density() function for the geometry
        being decorated and rotates the resulting position \f${\bf{r}}_{\text{orig}}\f$. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density
        along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f]
        It is impossible to calculate this value for a random value of the rotation angles. We simply
        return the X-axis surface density of the original geometry. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density
        along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f]
        It is impossible to calculate this value for a random value of the rotation angles. We simply
        return the Y-axis surface density of the original geometry. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density
        along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f]
        It is impossible to calculate this value for a random value of the rotation angles. We simply
        return the Z-axis surface density of the original geometry. */
    double SigmaZ() const;

    /** This function implements part of the AngularDistribution interface. It returns the
        probability \f$P(\Omega)\f$ for a given direction \f$(\theta,\phi)\f$ at the specified
        position. It calls the corresponding function for the geometry being decorated with
        the derotated position \f${\bf{r}}_{\text{orig}} = {\bf{R}}^{\text{T}}\,{\bf{r}}\f$ and
        the derotated direction \f${\bf{k}}_{\text{orig}} = {\bf{R}}^{\text{T}}\,{\bf{k}}\f$ as
        arguments. */
    double probabilityForDirection(Position bfr, Direction bfk) const;

    /** This function implements part of the AngularDistribution interface. It generates a random
        direction \f$(\theta,\phi)\f$ drawn from the probability distribution \f$P(\Omega)
        \,{\mathrm{d}}\Omega\f$ at the specified position. The routine first calls the corresponding
        function for the geometry being decorated with the derotated position \f${\bf{r}}_{\text{orig}}
        = {\bf{R}}^{\text{T}}\,{\bf{r}}\f$ as an argument, and subsequently rotates the resulting
        direction vector \f${\bf{k}}_{\text{orig}}\f$. */
    Direction generateDirection(Position bfr) const;

private:
    /** This function rotates a position \f${\bf{r}}_{\text{orig}}\f$, i.e.\ it returns the rotated
        position vector \f${\bf{r}} = {\bf{R}}\,{\bf{r}}_{\text{orig}}\f$. */
    Position rotate(Position bfrorig) const;

    /** This function derotates a position \f${\bf{r}}\f$, i.e.\ it returns the derotated
        position vector \f${\bf{r}}_{\text{orig}} = {\bf{R}}^{\text{T}}\,{\bf{r}}\f$. */
    Position derotate(Position bfr) const;

    /** This function rotates a direction \f${\bf{k}}_{\text{orig}}\f$, i.e.\ it returns the rotated
        direction vector \f${\bf{k}} = {\bf{R}}\,{\bf{k}}_{\text{orig}}\f$. */
    Direction rotate(Direction bfkorig) const;

    /** This function derotates a direction \f${\bf{k}}\f$, i.e.\ it returns the derotated
        direction vector \f${\bf{k}}_{\text{orig}} = {\bf{R}}^{\text{T}}\,{\bf{k}}\f$. */
    Direction derotate(Direction bfk) const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    Geometry* _geometry;
    double _alpha, _beta, _gamma;

    // data members initialized during setup
    double _sinalpha, _cosalpha, _sinbeta, _cosbeta, _singamma, _cosgamma;
    double _R11, _R12, _R13, _R21, _R22, _R23, _R31, _R32, _R33;
};

////////////////////////////////////////////////////////////////////

#endif // ROTATEGEOMETRYDECORATOR_HPP
