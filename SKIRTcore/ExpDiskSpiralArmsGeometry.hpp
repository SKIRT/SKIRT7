/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef EXPDISKSPIRALARMSGEOMETRY_HPP
#define EXPDISKSPIRALARMSGEOMETRY_HPP

#include "ArrayTable.hpp"
#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The ExpDiskSpiralArmsGeometry class is a subclass of the GenGeometry class, and describes
    geometries characterized by a double-exponential profile with a spiral arm perturbation. The
    undisturbed density decreases exponentially in the radial and the vertical directions, whereas
    the spiral arm perturbation (with an arbitrary weight factor) is a logarithmic spiral arm
    pattern; see Misiriotis et al. (2000, A&A, 353, 117–123). A truncation can be applied in both
    the radial and vertical direction. In formula form, the density behaves as \f[ \rho(R,\phi,z) =
    \rho_0\, {\text{e}}^{-\frac{R}{h_R}-\frac{|z|}{h_z}} \left\{ 1+w\sin\left[m\left(
    \frac{\ln(R/h_R)}{\tan p}-(\phi-\phi_0)\right)\right]\right\} \f] for \f$R\leq
    R_{\text{max}}\f$ and \f$|z|\leq z_{\text{max}}\f$. The model contains eight parameters: the
    radial scale length \f$h_R\f$, the vertical scale height \f$h_z\f$, the radial truncation
    radius \f$R_{\text{max}}\f$, the vertical truncation radius \f$z_{\text{max}}\f$, the number of
    spiral arms \f$m\f$, the pitch angle \f$p\f$, the spiral arm phase zero-point \f$\phi_0\f$, and
    the spiral perturbation weight \f$w\f$. */
class ExpDiskSpiralArmsGeometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an exponential disk with spiral arms geometry")

    Q_CLASSINFO("Property", "radialScale")
    Q_CLASSINFO("Title", "the radial scale length of the disk")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "axialScale")
    Q_CLASSINFO("Title", "the axial scale height of the disk")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "radialTrunc")
    Q_CLASSINFO("Title", "the radial truncation length (zero means no truncation)")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "axialTrunc")
    Q_CLASSINFO("Title", "the axial truncation height (zero means no truncation)")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "0")

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

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ExpDiskSpiralArmsGeometry();

protected:
    /** This function verifies the validity of the different parameters. If the truncation radii
        \f$R_{\text{max}}\f$ or \f$z_{\text{max}}\f$ are zero, no truncation is applied in that
        direction (i.e. the disc has an infinite extent in that case). The central density
        \f$\rho_0\f$ is set by the normalization condition that the total mass equals one. The
        integration over the azimuth is surprisingly simple \f[ \int_0^{2\pi} \rho({\bf{r}})\,
        {\text{d}}\phi = 2\pi\, \rho_0 \exp\left(-\frac{R}{h_R}-\frac{|z|}{h_z}\right), \f] and
        after integrating over \f$z\f$ and \f$R\f$ one obtains \f[ \int \rho({\bf{r}})\,
        {\text{d}}{\bf{r}} = 4\pi\, \rho_0\, h_R^2\, h_z \left( 1 -
        {\text{e}}^{-z_{\text{max}}/h_z} \left[ 1 - \left( 1+\frac{R_{\text{max}}}{h_R}
        \right) {\text{e}}^{-R_{\text{max}}/h_R} \right) \right]. \f] Setting this equal to one
        immediately gives the desired expression for \f$\rho_0\f$. This setup function also fills
        a two-dimensional vector with a discretized version of the function \f[ P(\phi) =
        \frac{m\phi + w\cos m(\gamma-\phi) -w \cos m\gamma}{2m\pi}, \f] for different values of
        \f$\phi\f$ and \f$\gamma\f$ between 0 and \f$2\pi\f$. This 2D vector is used as a lookup
        table for the generation of random positions. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the disk scale length \f$h_R\f$. */
    Q_INVOKABLE void setRadialScale(double value);

    /** Returns the disk scale length \f$h_R\f$. */
    Q_INVOKABLE double radialScale() const;

    /** Sets the disk scale height \f$h_z\f$. */
    Q_INVOKABLE void setAxialScale(double value);

    /** Returns the disk scale height \f$h_z\f$. */
    Q_INVOKABLE double axialScale() const;

    /** Sets the radial truncation length \f$R_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE void setRadialTrunc(double value);

    /** Returns the radial truncation length \f$R_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE double radialTrunc() const;

    /** Sets the axial truncation height \f$z_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE void setAxialTrunc(double value);

    /** Returns the axial truncation height \f$z_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE double axialTrunc() const;

    /** Sets the number of spiral arms. */
    Q_INVOKABLE void setArms(int value);

    /** Returns the number of spiral arms. */
    Q_INVOKABLE int arms() const;

    /** Sets the pitch angle \f$p\f$. */
    Q_INVOKABLE void setPitch(double value);

    /** Returns the pitch angle \f$p\f$. */
    Q_INVOKABLE double pitch() const;

    /** Sets the phase zero-point \f$\phi_0\f$. */
    Q_INVOKABLE void setPhase(double value);

    /** Returns the phase zero-point \f$\phi_0\f$. */
    Q_INVOKABLE double phase() const;

    /** Sets the weight \f$w\f$ of the spiral perturbation. */
    Q_INVOKABLE void setPerturbWeight(double value);

    /** Returns the weight \f$w\f$ of the spiral perturbation. */
    Q_INVOKABLE double perturbWeight() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. It
        just implements the analytical formula. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. In the present case, we can
        generate a random position by picking a random cylindrical radius \f$R\f$, a random height
        \f$z\f$, and a random azimuth \f$\phi\f$ from the appropriate one-dimensional probability
        distribution functions (PDF). For \f$R\f$ and \f$z\f$, we can simply use the same technique
        as for an exponential disk without spiral arm perturbation (see the ExpDiskGeometry
        class), as the perturbation is purely azimuthal. For the azimuth \f$\phi\f$, the PDF is not
        a uniform distribution as in the case of an exponential disk without perturbation, but
        \f[ p(\phi)\,{\text{d}}\phi = \frac{1-w\sin m(\gamma-\phi)}{2\pi}\,{\text{d}}\phi \f]
        with \f[ \gamma = \frac{\ln(R/h_R)}{\tan p}+\phi_0 , \f] where \f$R\f$ is the cylindrical
        radius that we have already determined previously. The corresponding cumulative
        distribution function is \f[ P(\phi) = \int_0^\phi p(\phi')\,{\text{d}}\phi' =
        \frac{m\phi + w\cos m(\gamma-\phi)-w \cos m\gamma}{2m\pi}. \f] Generating a random
        \f$\phi\f$ can be realized by picking a uniform deviate \f${\cal{X}}\f$ and solving the
        equation \f$ P(\phi) = {\cal{X}}\f$ for \f$\phi\f$. This equation cannot be solved
        analytically in this case. We therefore use a lookup table in which a discretized version
        of \f$P(\phi)\f$ is stored for a large number of possible \f$\gamma\f$ values, and we use
        linear interpolation to determine \f$\phi\f$. */
    Position generatePosition() const;

    /** This pure virtual function returns the surface mass density along the X-axis, i.e.
        the integration of the mass density along the entire X-axis, \f[
        \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\, {\text{d}}x.\f] For an exponential
        disc with spiral arm perturbation, this integral cannot be calculated
        analytically. However, when averaged over all lines-of-sight in the equatorial plane,
        the contribution of the spiral arm perturbation cancels out, and we recover the
        surface density of the corresponding unperturbed model, \f[ \Sigma_X
        = 2 \rho_0 h_R \left( 1 - {\text{e}}^{-R_{\text{max}}/h_R}
        \right) , \f] which reduces to \f$ \Sigma_X = 2\, \rho_0 h_R \f$
        if there is no radial truncation. */
    double SigmaX() const;

    /** This pure virtual function returns the surface mass density along the Y-axis, i.e.
        the integration of the mass density along the entire Y-axis, \f[
        \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\, {\text{d}}y.\f] For an exponential
        disc with spiral arm perturbation, this integral cannot be calculated
        analytically. However, when averaged over all lines-of-sight in the equatorial plane,
        the contribution of the spiral arm perturbation cancels out, and we recover the
        surface density of the corresponding unperturbed model, \f[ \Sigma_X
        = 2 \rho_0 h_R \left( 1 - {\text{e}}^{-R_{\text{max}}/h_R}
        \right) , \f] which reduces to \f$ \Sigma_X = 2\, \rho_0 h_R \f$
        if there is no radial truncation. */
    double SigmaY() const;

    /** This pure virtual function returns the surface mass density along the Z-axis, i.e.
        the integration of the mass density along the entire Z-axis, \f[
        \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z.\f] For an exponential disc
        geometrt with spiral arm perturbation, this integral is not really well defined, as the
        logarithmic spiral perturbation winds ever stronger when we get closer to the Z-axis. We
        use the Z-axis surface density of the corresponding unperturbed model, i.e. \f[ \Sigma_Z
        = 2\,\rho_0 h_z \left( 1 - {\text{e}}^{-z_{\text{max}}/h_z} \right), \f]
        which reduces to \f$ \Sigma_Z = 2\, \rho_0 h_z \f$ if there is no vertical
        truncation. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _hR;
    double _hz;
    double _Rmax;
    double _zmax;
    int _m;
    double _p;
    double _phi0;
    double _w;

    // data members initialized during setup
    double _tanp;
    double _rho0;
    int _Nphi;
    double _dphi;
    int _Ngamma;
    double _dgamma;
    ArrayTable<2> _Xvv;
};

////////////////////////////////////////////////////////////////////

#endif // EXPDISKSPIRALARMSGEOMETRY_HPP
