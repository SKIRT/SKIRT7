/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TRUST7AGEOMETRY_HPP
#define TRUST7AGEOMETRY_HPP

#include "SepAxGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The Trust7aGeometry class is a subclass of the SepAxGeometry class and describes
    the geometry of a simple linear filament. It is to be used as one of the benchmark models for
    the suite of TRUST 3D benchmark models. The density profile of the filament is
    described by the expression
    \f[ \rho(R,z) = \frac{\rho_{\text{c}}}{1+\dfrac{R^2}{R_{\text{c}}^2}}, \qquad {\text{ for }}
    R \leqslant R_{\text{out}} {\text{ and }} |z| \leqslant -b/2. \f]
    For the geometrical parameters of the model, we adopt the following hardcoded values:
    \f$b = 10~{\text{pc}}\f$,
    \f$R_{\text{c}} = 0.1~{\text{pc}}\f$, and \f$R_{\text{out}} = 3~{\text{pc}}\f$. */
class Trust7aGeometry : public SepAxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the geometry from the TRUST7a benchmark model")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Trust7aGeometry();

protected:
    /** This function just sets the values of the parameters, and computes the density
        \f$\rho_{\text{c}}\f$ at the centre of the filament. It is determined by the
        normalization condition that the total mass equals one. This results in \f[
        \rho_{\text{c}} = \frac{1}{\pi\, b\, R_{\text{c}}^2\,
        \ln\left(1+\dfrac{R_{\text{out}}^2}{R_{\text{c}}^2}\right)}. \f] */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the dust mass density \f$\rho(R,z)\f$ at the cylindrical
        radius \f$R\f$ and height \f$z\f$. It just implements the analytical formula. */
    double density(double R, double z) const;

    /** This function returns the cylindrical radius \f$R\f$ of a random position drawn from the
        TRUST 7a geometry, by picking a uniform deviate \f${\cal{X}}\f$
        and solving the equation \f[ {\cal{X}} = 2\pi \int_0^R \rho_R(R')\, R'\, {\text{d}}R' \f]
        for \f$R\f$. Substituting the exponential radial profile (without truncation) into this
        equation, we obtain \f[ {\cal{X}} = \frac{ \ln (1+R^2/R_{\text{c}}^2) }{
        \ln (1+R^2/R_{\text{c}}^2) }. \f] The solution is \f[ R = R_{\text{c}}
        \sqrt{ \left(1+\frac{R_{\text{out}}^2}{R_{\text{c}}^2}\right)^{\cal{X}}-1 }. \f] */
    double randomR() const;

    /** This function returns the height \f$z\f$ of a random position drawn from the
        TRUST 7a geometry. Straightforward as the distribution is uniform in height between
        \f$-b/2\f$ and \f$b/2\f$. */
    double randomz() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line in the equatorial plane starting at the centre of the coordinate
        system, \f[ \Sigma_R = \int_0^\infty \rho(R,0)\,{\text{d}}R. \f] One obtains
        \f[ \Sigma_R = \rho_{\text{c}} \int_0^{R_{\text{c}}}
        \frac{ {\text{d}}x }{ 1+x^2/R_{\text{c}}^2 } = \rho_{\text{c}}\,
        R_{\text{c}} \arctan \left(\frac{R_{\text{out}}}{R_{\text{c}}}\right). \f] */
    double SigmaR() const;

    /** This function returns the surface mass density along the Z-axis, i.e.
        the integration of the mass density along the entire Z-axis, \f[
        \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z.\f] For this model we
        find \f$ \Sigma_Z = \rho_{\text{c}}\,b. \f$  */
    double SigmaZ() const;

    //======================== Data Members ========================

private:

    // data members initialized during setup
    double _Rc;
    double _Rout;
    double _b;
    double _rhoc;

};

////////////////////////////////////////////////////////////////////

#endif // TRUST7AGEOMETRY_HPP
