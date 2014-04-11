/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef POWAXDUSTGRIDSTRUCTURE_HPP
#define POWAXDUSTGRIDSTRUCTURE_HPP

#include "AxDustGridStructure.hpp"

//////////////////////////////////////////////////////////////////////

/** The PowAxDustGridStructure class is a subclass of the AxDustGridStructure class and represents
    two-dimensional, axisymmetric dust grid structures with a power-law distribution of both the
    radial and vertical grid points. */
class PowAxDustGridStructure : public AxDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a cylindrical grid structure with a power-law distribution")

    Q_CLASSINFO("Property", "radialExtent")
    Q_CLASSINFO("Title", "the outer radius in the radial direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "radialRatio")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths in the radial direction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "radialPoints")
    Q_CLASSINFO("Title", "the number of radial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "axialExtent")
    Q_CLASSINFO("Title", "the outer radius in the axial direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "axialRatio")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths in the axial direction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "axialPoints")
    Q_CLASSINFO("Title", "the number of axial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PowAxDustGridStructure();

protected:
    /** This function calculates the distribution of radial grid points from the number of radial
        bins \f$N_R\f$, the maximum radius \f$R_{\text{max}}\f$ and the ratio \f${\cal{R}}_R\f$ of
        the widths of the outermost and innermost radial bin. These can be found as \f[ R_i =
        R_{\text{max}}\, \left(\frac{1-q^i}{1-q^{N_R}}\right) \qquad i=0,\ldots,N_R, \f] with \f$ q
        = ({\cal{R}}_R)^{1/(N_R-1)} \f$. It is easy to check that the ratio between the widths of
        the outermost and innermost radial bins is indeed \f${\cal{R}}_R\f$, \f[ \frac{
        R_{N_R}-R_{N_R-1} }{ R_1-R_0 } = \frac{ q^{N_R-1} - q^{N_R} }{ 1-q } = q^{N_R-1} =
        {\cal{R}}_R. \f] The determination of the vertical grid points is similar, but is somewhat
        more complicated as the innermost bin is now the central and not the first bin (the
        vertical grid runs from \f$-z_{\text{max}}\f$ to \f$z_{\text{max}}\f$). If \f$N_z\f$ is
        even, we define \f$M=N_z/2\f$ and set \f$z_M=0\f$ and \f[ z_{M\pm k} = \pm z_{\text{max}}
        \left(\frac{1-q^k}{1-q^M}\right) \qquad k=1,\ldots,M \f] with \f$ q =
        ({\cal{R}}_z)^{1/(M-1)} \f$. The ratio between the widths of the outermost and the
        innermost bins is now \f[ \frac{ z_{2M}-z_{2M-1} }{ z_{M+1}-z_M } = \frac{ q^{M-1} - q^M }{
        1-q } = q^{M-1} = {\cal{R}}_z. \f] On the other hand, if \f$N_z\f$ is odd, we set \f$M =
        (N_z+1)/2\f$ and \f[ z_{M-\frac12\pm (k-\frac12)} = \pm z_{\text{max}} \left[ \frac{
        \frac12\,(1+q) - q^k }{ \frac12\,(1+q) - q^M } \right] \qquad k=1,\ldots,M, \f] with again
        \f$ q = ({\cal{R}}_z)^{1/(M-1)} \f$. The ratio between the widths of the outermost and the
        innermost bins is for this case \f[ \frac{ z_{2M-1}-z_{2M-2} }{ z_M-z_{M-1} } = \frac{
        q^{M-1} - q^M }{ 1-q } = q^{M-1} = {\cal{R}}_z. \f] Finally, this funtion writes out the
        distribution of the grid cells when the corresponding flag is set to true. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius in the radial direction. */
    Q_INVOKABLE void setRadialExtent(double value);

    /** Returns the outer radius in the radial direction. */
    Q_INVOKABLE double radialExtent() const;

    /** Sets the ratio of the widths of the inner- and outermost bin in the radial direction. */
    Q_INVOKABLE void setRadialRatio(double value);

    /** Returns the ratio of the widths of the inner- and outermost bin in the radial direction. */
    Q_INVOKABLE double radialRatio() const;

    /** Sets the number of radial grid points. */
    Q_INVOKABLE void setRadialPoints(int value);

    /** Returns the number of radial grid points. */
    Q_INVOKABLE int radialPoints() const;

    /** Sets the outer radius in the axial direction. */
    Q_INVOKABLE void setAxialExtent(double value);

    /** Returns the outer radius in the axial direction. */
    Q_INVOKABLE double axialExtent() const;

    /** Sets the ratio of the widths of the inner- and outermost bin in the axial direction. */
    Q_INVOKABLE void setAxialRatio(double value);

    /** Returns the ratio of the widths of the inner- and outermost bin in the axial direction. */
    Q_INVOKABLE double axialRatio() const;

    /** Sets the number of axial grid points. */
    Q_INVOKABLE void setAxialPoints(int value);

    /** Returns the number of axial grid points. */
    Q_INVOKABLE int axialPoints() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    //  - stored in base class: _NR, _Rmax, _Nz, _zmax, _zmin == -_zmax
    //  - in addition to data members in base class:
    double _Rratio;
    double _zratio;
};

//////////////////////////////////////////////////////////////////////

#endif // POWAXDUSTGRIDSTRUCTURE_HPP
