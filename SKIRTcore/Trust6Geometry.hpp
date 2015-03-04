/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TRUST6GEOMETRY_HPP
#define TRUST6GEOMETRY_HPP

#include "AxGeometry.hpp"
#include <vector>

////////////////////////////////////////////////////////////////////

/** The Trust6Geometry class is a subclass of the AxGeometry class and describes the geometry of a
    lighthouse scenario, which is one of the models in the suite of TRUST 3D benchmarks. The
    geometry consists of three cylindrical components, each characterized by a cylindrical radius
    \f$R_{\text{max},i}\f$, a minimum and maximum height \f$z_{\text{min},i}\f$ and
    \f$z_{\text{max},i}\f$, and a density distribution \f$\rho_i(R,z)\f$. The first two cylinders
    have a constant density. The third cylinder has a Gaussian profile in the vertical
    direction, and is also constant in the radial direction. So within the radial and vertical
    limits of each cylinder, the density distributions are given by:
    \f[ \rho_1(R,z) = \rho_1 \f]
    \f[ \rho_2(R,z) = \rho_2 \f]
    \f[ \rho_3(R,z) = \rho_{\text{A},3} \exp\left(-\frac{(z-z_{\text{ctr},3})^2}{2\sigma_3^2}\right)
                    + \rho_{\text{B},3} \f]
    where \f$z_{\text{ctr},3}=(z_{\text{min},3}-z_{\text{min},3})/2\f$.

    The values for all parameters are hard-coded as listed in the table below, so there are no
    free parameters in the model.

    | Parameter | Value
    |-----------|-------
    | \f$R_{\text{max},1}\f$ | \f$400\,\text{AU}\f$
    | \f$R_{\text{max},2}\f$ | \f$400\,\text{AU}\f$
    | \f$R_{\text{max},3}\f$ | \f$100\,\text{AU}\f$
    | \f$z_{\text{min},1};\;z_{\text{max},1}\f$ | \f$-90\,\text{AU};\; 230\,\text{AU}\f$
    | \f$z_{\text{min},2};\;z_{\text{max},2}\f$ | \f$-230\,\text{AU};\; -220\,\text{AU}\f$
    | \f$z_{\text{min},3};\;z_{\text{max},3}\f$ | \f$-80\,\text{AU};\; -70\,\text{AU}\f$
    | \f$\rho_1\f$ | \f$10^{-21}\,\text{g/cm}^3\f$
    | \f$\rho_2\f$ | \f$10^{-18}\,\text{g/cm}^3\f$
    | \f$\rho_{\text{A},3};\;\rho_{\text{B},3}\f$ | \f$8\cdot10^{-15}\,\text{g/cm}^3;\; 4\cdot10^{-18}\,\text{g/cm}^3\f$
    | \f$\sigma_3\f$ | \f$\frac{\sqrt{3}}{2}\,\text{AU}\f$

*/
class Trust6Geometry : public AxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the geometry from the TRUST6 benchmark model")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Trust6Geometry();

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(R,z)\f$ at the cylindrical radius
        \f$R\f$ and height \f$z\f$. It just sums the contribution of each component. */
    double density(double R, double z) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. The routine consists of two steps. In the first step
        we generate a uniform deviate that is used to determine from which of the three components the
        location should be drawn. Once this is determined, we locate a random position from the selected
        cylindrical volume. */
    Position generatePosition() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line in the equatorial plane starting at the centre of the coordinate
        system, \f[ \Sigma_R = \int_0^\infty \rho(R,0)\,{\text{d}}R. \f] For the TRUST6 geometry,
        \f$ \Sigma_R = \rho_1\, R_{\text{max},1} \f$ since the second and third cylinder do not overlap
        the equatorial plane. */
    double SigmaR() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For the TRUST6 geometry, the function just sums the contribution of each
        component. */
    double SigmaZ() const;
};

////////////////////////////////////////////////////////////////////

#endif // TRUST6GEOMETRY_HPP
