/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TORUSGEOMETRY_HPP
#define TORUSGEOMETRY_HPP

#include "AxGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The TorusGeometry class is a subclass of the AxGeometry class and describes the geometry of an
    axisymmetric torus as assumed to be present in the centre of active galactic nuclei (AGN). This
    geometry is described by a radial power-law density with a finite opening angle; see Stalevski
    et al. (2012, MNRAS, 420, 2756–2772) and Granato & Danese (1994, MNRAS, 268, 235). In formula,
    it is most easily expressed in spherical coordinates as \f[ \rho(r,\theta) = A\,
    r^{-p}\,{\text{e}}^{-q|\cos\theta|} \quad\text{for } r_{\text{min}}<r<r_{\text{max}} \text{ and
    } \frac{\pi}{2}-\Delta<\theta<\frac{\pi}{2} +\Delta. \f] There are five free parameters
    describing this dust geometry: the inner and outer radii \f$r_{\text{min}}\f$ and
    \f$r_{\text{max}}\f$ of the torus, the radial power law index \f$p\f$, the polar index \f$q\f$
    and the angle \f$\Delta\f$ describing the opening angle of the torus.

    If the dusty system under consideration is in the vicinity of an AGN central engine or another
    source which is luminous enough to heat the dust up to sublimation temperature, the inner
    radius should correspond to sublimation radius and scale as \f$ r_{\text{min}} \propto
    L(\theta)^{0.5}\f$ (Barvainis, 1987, ApJ, 320, 537, eq (5)). If the primary source assumes
    anisotropic emission, the inner radius must follow the same dependence as the distribution of
    the primary source luminosity. Otherwise, dust temperature on the inner boundary of geometry is
    very likely to be under- or over-estimated. Thus, if the NetzerAccretionDiskGeometry
    distribution is chosen to describe primary source emission, it is recommended to turn on the
    anisotropic inner radius option for the torus geometry. The inner radius will then be set by
    the following formula: \f[ r_{\text{min}} \propto (\cos\theta\,(2\cos\theta+1))^{0.5}.\f] This
    should allow dust to approach all the way to the primary central source in the equatorial
    plane. However, due to the finite resolution of dust cells, it may happen that some of the
    innermost cells end up with unphysically high temperatures. For this reason, there is an
    additional input parameter, the cutoff radius \f$r_{\text{cut}}\f$. The value of the cutoff
    radius is usually found after a few trial-and-error experiments by inspecting temperature
    distribution maps, until the inner wall of the geometry is at the expected sublimation
    temperature for a given dust population.

    The total dust mass of the model corresponds to the mass of the original geometry, before the
    inner wall is reshaped to account for anisotropy; the difference is usually rather small. */
class TorusGeometry : public AxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a torus geometry")

    Q_CLASSINFO("Property", "expon")
    Q_CLASSINFO("Title", "the radial powerlaw exponent p of the torus")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "index")
    Q_CLASSINFO("Title", "the polar index q of the torus")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "openAngle")
    Q_CLASSINFO("Title", "the half opening angle of the torus")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "90 deg")

    Q_CLASSINFO("Property", "minRadius")
    Q_CLASSINFO("Title", "the minimum radius of the torus")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "maxRadius")
    Q_CLASSINFO("Title", "the maximum radius of the torus")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "anisoRadius")
    Q_CLASSINFO("Title", "reshape the inner radius according to the Netzer luminosity profile")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "cutRadius")
    Q_CLASSINFO("Title", "the inner cutoff radius of the torus")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "0")
    Q_CLASSINFO("RelevantIf", "anisoRadius")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TorusGeometry();

protected:
    /** This function verifies the validity of the geometry parameters. The normalization
        parameter \f$A\f$ is set by the normalization
        condition that total mass equals one, i.e. \f[ 1 = 2\pi\, A\,
        \int_{\pi/2-\Delta}^{\pi/2+\Delta} e^{-q|\cos\theta|}\sin\theta\, {\text{d}}\theta
        \int_{r_{\text{min}}}^{r_{\text{max}}} r^{2-p}\, {\text{d}}r. \f] This results in \f[ A =
        \frac{q}{4\pi\, (1-{\text{e}}^{-q\sin\Delta})}\, \frac{1}{ {\text{gln}}_{p-2}\,
        r_{\text{max}} - {\text{gln}}_{p-2}\, r_{\text{min}} }, \f] with \f${\text{gln}}_p\, x\f$
        the generalized logarithm defined in SpecialFunctions::gln. If \f$q=0\f$, this expression
        reduces to \f[ A = \frac{1}{4\pi\,\sin\Delta\, ({\text{gln}}_{p-2}\,
        r_{\text{max}} - {\text{gln}}_{p-2}\, r_{\text{min}} )}. \f] */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the radial power law exponent \f$p\f$ of the torus. */
    Q_INVOKABLE void setExpon(double value);

    /** Returns the radial power law exponent \f$p\f$ of the torus. */
    Q_INVOKABLE double expon() const;

    /** Sets the polar index \f$q\f$ of the torus. */
    Q_INVOKABLE void setIndex(double value);

    /** Returns the polar index \f$q\f$ of the torus. */
    Q_INVOKABLE double index() const;

    /** Sets the half opening angle of the torus. */
    Q_INVOKABLE void setOpenAngle(double value);

    /** Returns the half opening angle of the torus. */
    Q_INVOKABLE double openAngle() const;

    /** Sets the minimum radius of the torus. */
    Q_INVOKABLE void setMinRadius(double value);

    /** Returns the minimum radius of the torus. */
    Q_INVOKABLE double minRadius() const;

    /** Sets the maximum radius of the torus. */
    Q_INVOKABLE void setMaxRadius(double value);

    /** Returns the maximum radius of the torus. */
    Q_INVOKABLE double maxRadius() const;

    /** Sets the flag indicating whether to reshape the inner wall of the torus according to the
        Netzer luminosity profile. */
    Q_INVOKABLE void setAnisoRadius(bool value);

    /** Returns the flag indicating whether to reshape the inner wall of the torus according to the
        Netzer luminosity profile. */
    Q_INVOKABLE bool anisoRadius() const;

    /** Sets the inner cutoff radius of the torus. */
    Q_INVOKABLE void setCutRadius(double value);

    /** Returns the inner cutoff radius of the torus. */
    Q_INVOKABLE double cutRadius() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(R,z)\f$ at the cylindrical radius
        \f$R\f$ and height \f$z\f$. It just implements the analytical formula. */
    double density(double R, double z) const;

    /** This function generates a random position from the torus geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. For the torus geometry,
        the density is a separable function of \f$r\f$ and \f$\theta\f$, so that a random
        position can hence be constructed by combining random spherical coordinates, each chosen
        from their own probability distributions. A random azimuth \f$\phi\f$ is readily found by
        chosing a random deviate \f${\cal{X}}\f$ and setting \f$ \phi = 2\pi {\cal{X}} \f$.

        For the
        radial coordinate, the appropriate probability distribution is \f$ p(r)\,{\text{d}}r \propto
        r^{p-2}\,{\text{d}}r \f$. A random radius is generated by picking a new uniform deviate
        \f${\cal{X}}\f$, and solving the equation \f[ {\cal{X}} = \int_0^r p(r')\, {\text{d}}r'
        \f] for \f$r\f$. We obtain \f[ {\cal{X}} =
        \frac{ {\text{gln}}_{p-2}\, r - {\text{gln}}_{p-2}\, r_{\text{min}} }{
        {\text{gln}}_{p-2}\, r_{\text{max}} - {\text{gln}}_{p-2}\,
        r_{\text{min}} }. \f] Inverting this equation results in \f[ r =
        {\text{gexp}}_{p-2} \Big[ {\text{gln}}_{p-2}\, r_{\text{min}} + X (
        {\text{gln}}_{p-2}\, r_{\text{max}} - {\text{gln}}_{p-2}\, r_{\text{min}} )
        \Bigr]. \f] In these expressions, \f${\text{gln}}_p\,x\f$ and \f${\text{gexp}}_p\,x\f$ are the
        generalized logarithm and exponential functions defined in
        SpecialFunctions::gln and SpecialFunctions::gexp respectively.

        Finally, for the polar angle, the appropriate distribution function is \f[ p(\theta)\,
        {\text{d}}\theta \propto e^{-q|\cos\theta|}\sin\theta\, {\text{d}}\theta. \f] A random polar
        angle is generated by picking a new uniform deviate \f${\cal{X}}\f$, and solving the equation
        \f[ {\cal{X}} = \int_0^\theta p(\theta')\, {\text{d}}\theta' \f] for \f$\theta\f$. We obtain
        after some calculation \f[ {\cal{X}} = \begin{cases} \; \dfrac12 \left( 1 -
        \dfrac{1-{\text{e}}^{-q\cos\theta}}{1-{\text{e}}^{-q\sin\Delta}} \right) & \quad\text{for }
        \frac{\pi}{2}-\Delta < \theta < \frac{\pi}{2} \\[1.2em]  \;\dfrac12 \left( 1 +
        \dfrac{1-{\text{e}}^{q\cos\theta}}{1-{\text{e}}^{-q\sin\Delta}} \right) & \quad\text{for }
        \frac{\pi}{2} < \theta < \frac{\pi}{2}+\Delta \end{cases} \f] Inverting this
        gives \f[ \cos\theta = \begin{cases}\; -\dfrac{1}{q} \ln\left[ 1-\left(1-
        {\text{e}}^{-q\sin\Delta}\right) (1-2{\cal{X}}) \right] &
        \quad\text{if $0<{\cal{X}}<\tfrac12$} \\[1.2em] \; \dfrac{1}{q} \ln\left[ 1-\left(1
        -{\text{e}}^{-q\sin\Delta}\right) (2{\cal{X}}-1) \right] & \quad\text{if $\tfrac12<{\cal{X}}<1$}
        \end{cases}. \f] */
    Position generatePosition() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line in the equatorial plane starting at the centre of the coordinate
        system, \f[ \Sigma_R = \int_0^\infty \rho(R,0)\,{\text{d}}R. \f] For the torus geometry,
        \f[ \Sigma_R = A\, ( {\text{gln}}_p\, r_{\text{max}} -
        {\text{gln}}_p\, r_{\text{min}} ) \f] with \f${\text{gln}}_p\,x\f$ the generalized
        logarithm defined in SpecialFunctions::gln.*/
    double SigmaR() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For the torus geometry this integral is simply zero (we exclude the special
        limiting case where \f$\Delta=\tfrac{\pi}{2}\f$). */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _p;
    double _q;
    double _Delta;
    double _rmin;
    double _rmax;
    bool _rani;
    double _rcut;

    // data members initialized during setup
    double _sinDelta;
    double _smin;
    double _sdiff;
    double _A;
};

////////////////////////////////////////////////////////////////////

#endif // TORUSGEOMETRY_HPP
