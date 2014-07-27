/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PHOTONPACKAGE_HPP
#define PHOTONPACKAGE_HPP

#include <cfloat>
#include "DustGridPath.hpp"
class AngularDistribution;

////////////////////////////////////////////////////////////////////

/** The PhotonPackage class is used to describe photon packages, the basic luminosity packets that
    are transported during a radiative transfer simulation. Photon packages are monochromatic, i.e.
    they contain photons at a single wavelength (one of the wavelengths in the simulation's
    wavelength grid). Apart from its luminosity and wavelength, a photon package carries
    information about its origin (e.g. emission by a star or by a dust grain), about the
    interactions it experienced since its emission (e.g. the number of scattering events), and
    about its current path (e.g. starting position, propagation direction, list of dust cells being
    crossed). For performance reasons, a PhotonPackage object is usually constructed once at the
    start of a loop and then reused in the loop body for many consecutive launches; this allows the
    vectors with path information to remain allocated. Also, some trivial functions are implemented
    inline in the header. */
class PhotonPackage : public DustGridPath
{
public:

    // ------- Construction, launch and lifecycle events -------

    /** The constructor initializes an empty photon package object. After construction, the photon
        package is ready to be launched through one of the launch() functions. The other functions
        in this class should be invoked only after the photon package has been launched. The same
        photon package object can be re-launched multiple times. */
    PhotonPackage();

    /** This function initializes the photon package for a new life cycle. The arguments specify
        the origin (emission by a star or by a dust grain), the luminosity, the wavelength index,
        the starting position and the propagation direction. The function copies the values
        provided in the arguments to the corresponding data members, clears the current path, and
        initializes all other data members to default values. All information about the previous
        life cycle is lost. */
    void launch(bool stellar, double L, int ell, Position bfr, Direction bfk);

    /** This function initializes a peel off photon package being sent to an instrument for an
        emission event. The arguments specify the base photon package from which the peel off
        derives and the direction towards the instrument. The function copies the relevant values
        from the base photon package to the peel off photon package, updates the peel off
        direction, applies the anistropic emission direction bias if needed, and clears the current
        path. The base photon package remains unchanged. All information about the previous life
        cycle in the peel off photon package is lost. */
    void launchEmissionPeelOff(const PhotonPackage* pp, Direction bfk);

    /** This function initializes a peel off photon package being sent to an instrument for a
        scattering event. The arguments specify the base photon package from which the peel off
        derives, the direction towards the instrument, and the luminosity bias (as a multiplication
        factor). The function copies the relevant values from the base photon package to the peel
        off photon package, updates the peel off direction and luminosity, increments the
        scattering counter, and clears the current path. The base photon package remains unchanged.
        All information about the previous life cycle in the peel off photon package is lost. */
    void launchScatteringPeelOff(const PhotonPackage* pp, Direction bfk, double w);

    /** This function sets the angular distribution of the emission at the photon package's origin.
        It should be called only just after launch. */
    void setAngularDistribution(const AngularDistribution* ad);

    /** This function causes the propagation of the photon package over a physical distance
        \f$s\f$. It updates the position from \f${\bf{r}}\f$ to \f${\bf{r}}+s\,{\bf{k}}\f$, where
        \f${\bf{k}}\f$ is the propagation direction of the photon package, and it clears the
        current path. */
    void propagate(double s);

    /** This function scatters the photon package into the new direction \f${\bf{k}}\f$. It
        increments the counter that keeps track of scattering events, updates the direction, and
        clears the current path. */
    void scatter(Direction bfk);

    /** This function sets the luminosity of the photon package to a new value. */
    void setLuminosity(double L);

    // ------- Getting trivial properties -------

    /** This function returns true if the photon package has a stellar origin, false otherwise. */
    bool isStellar() const { return _stellar; }

    /** This function returns the luminosity of the photon package. */
    double luminosity() const { return _L; }

    /** This function returns the wavelength index of the photon package. */
    int ell() const { return _ell; }

    /** This function returns the starting position of the photon package's current path, i.e. either the original position
        of emission or the position of the last interaction with a dust grain. */
    Position position() const { return _bfr; }

    /** This function returns the propagation direction the photon package's current path. */
    Direction direction() const { return _bfk; }

    /** This function returns the number of scattering events the photon package has experienced. */
    int nScatt() const { return _nscatt; }

    // ------- Data members -------

private:
    bool _stellar;
    double _L;
    int _ell;
    int _nscatt;
    const AngularDistribution* _ad;
};

////////////////////////////////////////////////////////////////////

#endif // PHOTONPACKAGE_HPP
