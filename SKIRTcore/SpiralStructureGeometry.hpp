/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPIRALSTRUCTUREGEOMETRY_HPP
#define SPIRALSTRUCTUREGEOMETRY_HPP

// #include "ArrayTable.hpp"
#include "AxGeometry.hpp"
#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The SpiralStructureGeometry class is a Geometry decorator that adds spiral
    structure to any axisymmetric geometry. The spiral arm perturbation (with an
    arbitrary weight factor) is a logarithmic spiral arm pattern, based on the
    formulation of Schechtman-Rook et al. (2012, ApJ, 746, 70). The decorator
    basically alters the uniform distribution in azimuth (by definition, the
    density \f$\rho_{\text{ax}}\f$ of the original geometry is independent of
    \f$\phi\f$). In formula form, the density of the new geometry behaves as \f[
    \rho(R,\phi,z) = \rho_{\text{ax}}(R,z)\, \xi(R,\phi) \f]
    where \f$\xi(R,\phi)\f$ is a perturbation given by
    \f[ \xi(R,\phi) = (1-w) + w\, C_N \sin^{2N}
    \left[\frac{m}{2}\left( \frac{\ln (R/R_0)}{\tan p}-(\phi-\phi_0)\right) +
    \frac{\pi}{4} \right]. \f] Apart from the reference to the original
    geometry that is being decorated, the model contains six parameters: the number
    of spiral arms \f$m\f$, the pitch angle \f$p\f$, the spiral arm radius and
    phase zero-points \f$R_0\f$ and \f$\phi_0\f$, the spiral perturbation weight
    \f$w\f$, and the integer index \f$N>0\f$ that sets the arm-interarm size ratio
    (larger values of \f$N\f$ correspond to larger arm-interarm size ratios). The
    factor \f$C_N\f$ is not a free parameter, but a normalization factor that ensures
    that the total mass equals one, \f[ C_N = \frac{\sqrt{\pi}\,
    \Gamma(N+1)}{\Gamma(N+\tfrac12)}. \f] For \f$N=1\f$ the expression for the
    perturbation reduces to \f[ \xi(R,\phi) = 1 +
    w\, \sin \left[m \left( \frac{\ln (R/R_0)}{\tan p} - (\phi-\phi_0)\right)
    \right], \f] as in Misiriotis et al. (2000, A&A, 353, 117). Note that
    the parameters \f$R_0\f$ and \f$\phi_0\f$ in fact have the same effect (both of
    them add an offset to the spiral structure). In principle one of them could be
    suppressed, but it is confortable to include both of them. */
class SpiralStructureGeometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that adds spiral structure to an axisymmetric geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the original axisymmetric geometry")

    Q_CLASSINFO("Property", "arms")
    Q_CLASSINFO("Title", "the number of spiral arms")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("MaxValue", "100")
    Q_CLASSINFO("Default", "1")

    Q_CLASSINFO("Property", "pitch")
    Q_CLASSINFO("Title", "the pitch angle")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "90 deg")
    Q_CLASSINFO("Default", "10 deg")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the radius zero-point")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "phase")
    Q_CLASSINFO("Title", "the phase zero-point")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "360 deg")
    Q_CLASSINFO("Default", "0 deg")

    Q_CLASSINFO("Property", "perturbWeight")
    Q_CLASSINFO("Title", "the weight of the spiral perturbation")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")

    Q_CLASSINFO("Property", "index")
    Q_CLASSINFO("Title", "the arm-interarm size ratio index")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SpiralStructureGeometry();

    /** This function verifies the validity of the different parameters, and caches some
        frequently used combinations of them. */
    //This setup function also fills
    //    a two-dimensional vector with a discretized version of the function \f[
    //    P(\phi) = \frac{1}{2\pi}\left\{ (1-w)\,\phi + \frac{2\,w\,C_N}{m}
    //    \left[ I_N\left(\frac{m\gamma}{2}\right) - I_N\left(\frac{m(\gamma-\phi)}{2}\right) \right]
    //    \right\}, \f] for different values of
    //    \f$\phi\f$ and \f$\gamma\f$ between 0 and \f$2\pi\f$. This 2D vector is used as a lookup
    //    table for the generation of random positions. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the original axisymmetric geometry (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(AxGeometry* value);

    /** Returns the original axisymmetric geometry. */
    Q_INVOKABLE AxGeometry* geometry() const;

    /** Sets the number of spiral arms. */
    Q_INVOKABLE void setArms(int value);

    /** Returns the number of spiral arms. */
    Q_INVOKABLE int arms() const;

    /** Sets the pitch angle \f$p\f$. */
    Q_INVOKABLE void setPitch(double value);

    /** Returns the pitch angle \f$p\f$. */
    Q_INVOKABLE double pitch() const;

    /** Sets the radius zero-point \f$R_0\f$. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the radius zero-point \f$R_0\f$. */
    Q_INVOKABLE double radius() const;

    /** Sets the phase zero-point \f$\phi_0\f$. */
    Q_INVOKABLE void setPhase(double value);

    /** Returns the phase zero-point \f$\phi_0\f$. */
    Q_INVOKABLE double phase() const;

    /** Sets the weight \f$w\f$ of the spiral perturbation. */
    Q_INVOKABLE void setPerturbWeight(double value);

    /** Returns the weight \f$w\f$ of the spiral perturbation. */
    Q_INVOKABLE double perturbWeight() const;

    /** Sets the arm-interarm size ratio index \f$N\f$. */
    Q_INVOKABLE void setIndex(int value);

    /** Returns the arm-interarm size ratio index \f$N\f$. */
    Q_INVOKABLE int index() const;

    //======================== Other Functions =======================

public:

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. It
        just implements the analytical formula. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry by
        drawing a random point from the three-dimensional probability density
        \f$p({\bf{r}})\,{\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. We use a
        combination of the conditional distribution technique and the rejection technique. */
//        As
//        the decorator basically just alters the azimuthal distribution, we start by generating a
//        random position from the original axisymmetric geometry that is being decorated, and we can
//        use the cylindrical radius \f$R\f$ and height \f$z\f$.
//
//        Remains to determine a random azimuth \f$\phi\f$ from the appropriate one-dimensional probability
//        distribution function (PDF). The PDF is not
//        a uniform distribution as it was for the original axisymmetric model, but
//        \f[ p(\phi)\,{\text{d}}\phi = \frac{1}{2\pi}
//        \left\{ (1-w) + w\, C_N \sin^{2N} \left[ \frac{m\,(\gamma-\phi)}{2} \right]  \right\} \f]
//        where \f[ \gamma = \frac{\ln (R/R_0)}{\tan p}+\phi_0+\frac{\pi}{2m} , \f] and \f$R\f$ is the cylindrical
//        radius that we have already determined previously. The corresponding cumulative
//        distribution function is \f[ P(\phi) =
//        \frac{1}{2\pi}\left\{ (1-w)\,\phi + w\,C_N\,\int_0^\phi \sin^{2N} \left[ \frac{m\,(\gamma-\phi')}{2}
//        \right] {\text{d}}\phi'. \right\} \f] If we use the special function \f[ I_n(x) = \int_0^x
//        \sin^{2n} t\, {\text{d}}t, \f] this can be written as
//        \f[ P(\phi) = \frac{1}{2\pi}\left\{ (1-w)\,\phi + \frac{2\,w\,C_N}{m}
//        \left[ I_N\left(\frac{m\gamma}{2}\right) - I_N\left(\frac{m(\gamma-\phi)}{2}\right) \right]
//        \right\}. \f]
//        Generating a random
//        \f$\phi\f$ can be realized by picking a uniform deviate \f${\cal{X}}\f$ and solving the
//        equation \f$ P(\phi) = {\cal{X}}\f$ for \f$\phi\f$. This equation cannot be solved
//        analytically in this case. We therefore use a lookup table in which a discretized version
//        of \f$P(\phi)\f$ is stored for a large number of possible \f$\gamma\f$ values, and we use
//        linear interpolation to determine \f$\phi\f$. */
    Position generatePosition() const;

    /** This function returns the surface mass density along the X-axis, i.e.
        the integration of the mass density along the entire X-axis, \f[
        \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\, {\text{d}}x.\f] This integral cannot be
        calculated analytically, but when averaged over all lines-of-sight in the equatorial plane,
        the contribution of the spiral arm perturbation cancels out, and we recover the
        X-axis surface density of the corresponding unperturbed model. */
    double SigmaX() const;

    /** This function returns the surface mass density along the Y-axis, i.e.
        the integration of the mass density along the entire Y-axis, \f[
        \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\, {\text{d}}y.\f] This integral cannot be
        calculated analytically, but when averaged over all lines-of-sight in the equatorial plane,
        the contribution of the spiral arm perturbation cancels out, and we recover the
        Y-axis surface density of the corresponding unperturbed model. */
    double SigmaY() const;

    /** This function returns the surface mass density along the Z-axis, i.e.
        the integration of the mass density along the entire Z-axis, \f[
        \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z.\f] For the present decorator, this integral
        is not really well defined, as the logarithmic spiral perturbation winds ever stronger when we get closer
        to the Z-axis. We use the Z-axis surface density of the corresponding unperturbed model. */
    double SigmaZ() const;

private:

//   /* This private function returns the integral \f[ I_n(x) = \int_0^x
//      \sin^{2n} t\, {\text{d}}t \f] for a positive integer value of \f$n\f$.
//      It is calculated using the recursion formula
//      \f[ I_n(x) = \frac{1}{2n}\left[-\sin^{2n-1}x\,\cos x + (2n-1)\,I_{n-1} \right],
//      \f] together with the trivial seed \f$ I_0(x) = x \f$. */
//   double integratesin2n(double x, int n) const;

    /** This private function implements the analytical formula for the perturbation \f$\xi(R,\phi)\f$. */
    double perturbation(double R, double phi) const;


    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    AxGeometry* _geometry;
    int _m;
    double _p;
    double _R0;
    double _phi0;
    double _w;
    int _N;

    // data members initialized during setup
    double _tanp;
    double _CN;
    double _c;
    // int _Nphi;
    // double _dphi;
    // int _Ngamma;
    // double _dgamma;
    // ArrayTable<2> _Xvv;

};

////////////////////////////////////////////////////////////////////

#endif // SPIRALSTRUCTUREGEOMETRY_HPP
