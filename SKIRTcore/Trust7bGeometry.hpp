/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TRUST7BGEOMETRY_HPP
#define TRUST7BGEOMETRY_HPP

#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The Trust7bGeometry class is a subclass of the GenGeometry class and describes
    the geometry of a helical filament. It is to be used as one of the benchmark models for
    the suite of TRUST 3D benchmark models. The density profile of the filament is
    described by the expression
    \f[ \rho(x,y,z) = \frac{\rho_{\text{c}}}{1+\left[\dfrac{R_{\text{f}}(x,y,z)}{R_{\text{c}}}
    \right]^2}, \f] where
    \f[ R_{\text{f}}(x,y,z) = \sqrt{[x-x_{\text{f}}(z)]^2 + [y-y_{\text{f}}(z)]^2} \f] is the
    distance in the XY-plane to the centre of the filament, defined as
    \f[ \begin{split} x_{\text{f}}(z) = a \sin\left(\frac{2\pi z}{b}\right), \\
    y_{\text{f}}(z) = -a \cos\left(\frac{2\pi z}{b}\right). \end{split} \f]
    The density distribution is set to zero if \f$R_{\text{f}}(x,y,z)>R_{\text{out}}\f$ or
    \f$|z|>b/2\f$.
    For the geometrical parameters of the model, we adopt the following hardcoded values:
    \f$a = 1~{\text{pc}}\f$, \f$b = 10~{\text{pc}}\f$,
    \f$R_{\text{c}} = 0.1~{\text{pc}}\f$, and \f$R_{\text{out}} = 3~{\text{pc}}\f$. */
class Trust7bGeometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the geometry from the TRUST7b benchmark model")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Trust7bGeometry();

protected:
    /** This function just sets the values of the parameters, and computes the density
        \f$\rho_{\text{c}}\f$ at the centre of the filament. It is determined by the
        normalization condition that the total mass equals one. This results in \f[
        \rho_{\text{c}} = \frac{1}{\pi\, b\, R_{\text{c}}^2\,
        \ln\left(1+\dfrac{R_{\text{out}}^2}{R_{\text{c}}^2}\right)}. \f] */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the dust mass density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. It just implements the analytical formula. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. The \f$z\f$ coordinate is sampled uniformly between
        \f$-b/2\f$ and \f$b/2\f$. An azimuth angle \f$\phi\f$ is sampled uniformly as well. Then
        the function generates a random horizontal distance from the central helix
        \f$R_{\text{f}}\f$ using the equation for the cylindrical coordinate in the
        Trust7aGeometry, \f[ R_{\text{f}} = R_{\text{c}} \sqrt{
        \left(1+\frac{R_{\text{out}}^2}{R_{\text{c}}^2}\right)^{\cal{X}}-1 }. \f] Finally the
        function calculates the position in the \f$xy\f$-plane by offsetting the central helix
        point by the random distance \f$R_{\text{f}}\f$ in the direction indicated by the random
        angle \f$\phi\f$. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of
        the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,
        {\text{d}}x. \f] The density along the X-axis is easy to calculate:
        \f[ \rho(x,0,0) =
        \frac{\rho_{\text{c}}}{1+\dfrac{x^2+a^2}{R_{\text{c}}^2}}. \f] Integrating this
        value between \f$x_{\text{min}}=-\sqrt{R_{\text{out}}^2-a^2}\f$ and
        \f$x_{\text{max}} =
        \sqrt{R_{\text{out}}^2-a^2}\f$ gives
        \f[ \Sigma_X = \frac{2\,\rho_{\text{c}}\,
        R_{\text{c}}^2}{\sqrt{R_{\text{c}}^2+a^2}} \arctan
        \sqrt{\frac{R_{\text{out}}^2-a^2}{R_{\text{c}}^2+a^2}}. \f] */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of
        the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,
        {\text{d}}y. \f] The density along the Y-axis is easy to calculate:
        \f[ \rho(0,y,0) =
        \frac{\rho_{\text{c}}}{1+\dfrac{(y+a)^2}{R_{\text{c}}^2}}. \f] Integrating this
        value between \f$y_{\text{min}}=-R_{\text{out}}-a\f$ and
        \f$y_{\text{max}} = R_{\text{out}}-a\f$ gives
        \f[ \Sigma_Y = 2\,\rho_{\text{c}}\, R_{\text{c}}\, {\text{arctan}}
        \left(\frac{R_{\text{out}}}{R_{\text{c}}}\right). \f] */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] Since \f[ \rho(0,0,z)
        = \frac{\rho_{\text{c}}}{1+\dfrac{a^2}{R_{\text{c}}^2}}, \f] we obtain
        \f[ \Sigma_Z
        = \frac{\rho_{\text{c}}\,b}{1+\dfrac{a^2}{R_{\text{c}}^2}}. \f]  */
    double SigmaZ() const;

    //======================== Data Members ========================

private:

    // data members initialized during setup
    double _Rc;
    double _Rout;
    double _a;
    double _b;
    double _rhoc;

};

////////////////////////////////////////////////////////////////////

#endif // TRUST7BGEOMETRY_HPP
