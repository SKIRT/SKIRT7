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
    lighthouse scenario. It is to be used as one of the benchmark
    models for the suite of TRUST 3D benchmark models. The geometry consists of three cylindrical
    components with a constant density. Each of the three components is characterized by a cylindrical radius
    \f$R_{\text{max},i}\f$, a minimum and maximum height \f$z_{\text{min},i}\f$ and
    \f$z_{\text{max},i}\f$, and a density \f$\rho_i\f$. The values for these parameters are hard-coded,
    so there are no free parameters in the model. */
class Trust6Geometry : public AxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the geometry from the TRUST6 benchmark model")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Trust6Geometry();

protected:
    /** This function just sets the values of the parameters, and normalizes the density of the three
        components so that the total mass equals one. */
    void setupSelfBefore();

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
        \f[ \Sigma_R = \sum_i \rho_i\, R_{\text{max},i} \f] where the sum includes the components with
        \f$z_{\text{min},i} < 0 < z_{\text{max},i}\f$. */
    double SigmaR() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For the TRUST6 geometry, \f[ \Sigma_R = \sum_i \rho_i\, (z_{\text{max},i} -
        z_{\text{min},i}) \f] where the sum includes all components. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members initialized during setup
    std::vector<double> _Rmaxv, _zminv, _zmaxv, _rhov, _Mv;
};

////////////////////////////////////////////////////////////////////

#endif // TRUST6GEOMETRY_HPP
