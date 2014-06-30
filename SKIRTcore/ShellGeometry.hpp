/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SHELLGEOMETRY_HPP
#define SHELLGEOMETRY_HPP

#include "SpheGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The ShellGeometry class is a subclass of the SpheGeometry class and describes the
    geometry of a spherical shell, where the density behaves as a power law between an inner
    and an outer radius, \f[ \rho(r) = A\,r^{-p} \qquad\qquad r_{\text{min}} < r < r_{\text{max}}.
    \f] with \f$A\f$ a normalization constant. The range of \f$p\f$ is limited to \f$p\geq0\f$,
    and obviously the condition \f$r_{\text{min}} < r_{\text{max}}\f$ should be satisfied.
    This geometry is characterized by
    three free parameters: the inner radius \f$r_{\text{min}}\f$, the outer radius
    \f$r_{\text{max}}\f$ and the power law exponent \f$p\f$. */
class ShellGeometry : public SpheGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a shell geometry")

    Q_CLASSINFO("Property", "minRadius")
    Q_CLASSINFO("Title", "the inner radius of the shell")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "maxRadius")
    Q_CLASSINFO("Title", "the outer radius of the shell")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "expon")
    Q_CLASSINFO("Title", "the power law exponent")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ShellGeometry();

protected:
    /** This function verifies the validity of the inner radius \f$r_{\text{min}}\f$, the outer
        radius \f$r_{\text{max}}\f$ and the power law exponent \f$p\f$. The normalization parameter
        \f$A\f$ is set by the normalization condition that total mass equals one, i.e. \f[ 1 = 4\pi
        A \int_{r_{\text{min}}}^{r_{\text{max}}} r^{2-p}\, {\text{d}}r. \f] This results in \f[ A =
        \frac{1}{4\pi}\, \frac{1}{ {\text{gln}}_{p-2}\, r_{\text{max}} - {\text{gln}}_{p-2}\,
        r_{\text{min}} }, \f] with \f${\text{gln}}_p\,x\f$ the generalized logarithm defined in
        SpecialFunctions::gln. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the inner radius of the shell. */
    Q_INVOKABLE void setMinRadius(double value);

    /** Returns the inner radius of the shell. */
    Q_INVOKABLE double minRadius() const;

    /** Sets the outer radius of the shell. */
    Q_INVOKABLE void setMaxRadius(double value);

    /** Returns the outer radius of the shell. */
    Q_INVOKABLE double maxRadius() const;

    /** Sets the power law exponent. */
    Q_INVOKABLE void setExpon(double value);

    /** Returns the power law exponent. */
    Q_INVOKABLE double expon() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(r)\f$ at the radius \f$r\f$.
        It just implements the analytical formula. */
    double density(double r) const;

    /** This function returns the radius of a random position drawn from the shell density
        distribution. This is accomplished by generating a uniform deviate \f${\cal{X}}\f$, and
        solving the equation \f[ {\cal{X}} = M(r) = 4\pi \int_0^r \rho(r')\, r'{}^2\, {\text{d}}r'
        \f] for \f$r\f$. For the shell geometry, we obtain \f[ {\cal{X}} =
        \frac{ {\text{gln}}_{p-2}\, r - {\text{gln}}_{p-2}\, r_{\text{min}} }{
        {\text{gln}}_{p-2}\, r_{\text{max}} - {\text{gln}}_{p-2}\,
        r_{\text{min}} }. \f] Inverting this equation results in \f[ r =
        {\text{gexp}}_{p-2} \Big[ {\text{gln}}_{p-2}\, r_{\text{min}} + X (
        {\text{gln}}_{p-2}\, r_{\text{max}} - {\text{gln}}_{p-2}\, r_{\text{min}} )
        \Bigr]. \f] In these expressions, \f${\text{gln}}_p\,x\f$ and \f${\text{gexp}}_p\,x\f$ are the
        generalized logarithm and exponential functions defined in
        SpecialFunctions::gln and SpecialFunctions::gexp respectively. */
    double randomradius() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line starting at the centre of the coordinate
        system, \f[ \Sigma_r = \int_0^\infty \rho(r)\,{\text{d}}r. \f] For the shell
        geometry, one obtains
        \f[ \Sigma_r = A\, ( {\text{gln}}_p\, r_{\text{max}} -
        {\text{gln}}_p\, r_{\text{min}} ) \f] with \f${\text{gln}}_p\,x\f$ the generalized
        logarithm defined in SpecialFunctions::gln.*/
    double Sigmar() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _rmin;
    double _rmax;
    double _p;

    // data members initialized during setup
    double _smin;
    double _sdiff;
    double _A;
};

////////////////////////////////////////////////////////////////////

#endif // SHELLGEOMETRY_HPP
