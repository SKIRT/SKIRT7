/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PHOTONPACKAGE_HPP
#define PHOTONPACKAGE_HPP

#include "DustGridPath.hpp"
#include "StokesVector.hpp"
class AngularDistribution;

////////////////////////////////////////////////////////////////////

/** The PhotonPackage class is used to describe photon packages, the basic luminosity packets that
    are transported during a radiative transfer simulation. Photon packages are monochromatic, i.e.
    they contain photons at a single wavelength (one of the wavelengths in the simulation's
    wavelength grid). Apart from its luminosity, wavelength and polarization state, a photon
    package carries information about its origin (e.g. emission by a star or by a dust grain),
    about the interactions it experienced since its emission (e.g. the number of scattering
    events), and about its current path (e.g. starting position, propagation direction, list of
    dust cells being crossed). The current path and the polarization state are handled by publicly
    inheriting repectively the DustGridPath class and the StokesVector class. For performance
    reasons, a PhotonPackage object is usually constructed once at the start of a loop and then
    reused in the loop body for many consecutive launches; this allows the vectors with path
    information to remain allocated. Also, some trivial functions are implemented inline in the
    header. */
class PhotonPackage : public DustGridPath, public StokesVector
{
public:

    // ------- Construction, launch and lifecycle events -------

    /** The constructor initializes an empty photon package object. After construction, the photon
        package is ready to be launched through one of the launch() functions. The other functions
        in this class should be invoked only after the photon package has been launched. The same
        photon package object can be re-launched multiple times. */
    PhotonPackage();

    /** This function initializes the photon package for a new life cycle. The arguments specify
        the luminosity, the wavelength index, the starting position and the propagation direction.
        The function copies these values to the corresponding data members and initializes the
        other data members to default values. The number of scattering events is set to zero. The
        emission origin is set to the value corresponding to dust emission, which can be overridden
        through the setStellarOrigin() function. In the current implementation, all stellar and
        dust emission is considered to be unpolarized, so the polarization state is set
        accordingly. The current path is invalidated by these changes, and all information about
        the previous life cycle is lost. */
    void launch(double L, int ell, Position bfr, Direction bfk);

    /** This function initializes a peel off photon package being sent to an instrument for an
        emission event. The arguments specify the base photon package from which the peel off
        derives and the direction towards the instrument. The function copies the relevant values
        from the base photon package to the peel off photon package, updates the peel off
        direction, and applies the anisotropic emission direction bias if needed. In the current
        implementation, all stellar and dust emission is considered to be unpolarized, so the
        polarization state of the peel off photon package is set accordingly. The current path of
        the peel off photon package is invalidated, and all information about its previous life
        cycle is lost. The base photon package remains unchanged. */
    void launchEmissionPeelOff(const PhotonPackage* pp, Direction bfk);

    /** This function initializes a peel off photon package being sent to an instrument for a
        scattering event. The arguments specify the base photon package from which the peel off
        derives, the direction towards the instrument, and the luminosity bias (as a multiplication
        factor). The function copies the relevant values from the base photon package to the peel
        off photon package, updates the peel off direction and luminosity, and increments the
        scattering counter. The peel off photon package is initialized to an unpolarized state; the
        polarization state should be properly updated after the launch through the StokesVector
        class functions. The current path of the peel off photon package is invalidated, and all
        information about its previous life cycle is lost. The base photon package remains
        unchanged. */
    void launchScatteringPeelOff(const PhotonPackage* pp, Direction bfk, double w);

    /** This function initializes a peel off photon package being sent to an instrument for a
        scattering event. The arguments specify the base photon package from which the peel off
        derives, the position at which the peel off occurs, the direction towards the instrument,
        and the luminosity fraction (as a multiplication factor). The function copies the relevant
        values from the base photon package to the peel off photon package, updates the peel off
        position, direction and luminosity, and increments the scattering counter. The peel off
        photon package is initialized to an unpolarized state; the polarization state should be
        properly updated after the launch through the StokesVector class functions. The current
        path of the peel off photon package is invalidated, and all information about its previous
        life cycle is lost. The base photon package remains unchanged. */
    void launchScatteringPeelOff(const PhotonPackage* pp, Position bfr, Direction bfk, double w);

    /** This function establishes the origin of the photon package as stellar emission (the default
        is dust emission) and registers the index of the emitting stellar component. This
        information is used by some instruments to record fluxes seperately based on their origin.
        This function should be called only just after launch. */
    void setStellarOrigin(int stellarCompIndex);

    /** This function sets the angular distribution of the emission at the photon package's origin.
        It should be called only just after launch. */
    void setAngularDistribution(const AngularDistribution* ad);

    /** This function causes the propagation of the photon package over a physical distance
        \f$s\f$. It updates the position from \f${\bf{r}}\f$ to \f${\bf{r}}+s\,{\bf{k}}\f$, where
        \f${\bf{k}}\f$ is the propagation direction of the photon package, invalidating the current
        path. */
    void propagate(double s);

    /** This function scatters the photon package into the new direction \f${\bf{k}}\f$. It
        increments the counter that keeps track of scattering events and updates the direction,
        invalidating the current path. The polarization remains unchanged; it should be properly
        updated through the StokesVector class functions. */
    void scatter(Direction bfk);

    /** This function sets the luminosity of the photon package to a new value. */
    void setLuminosity(double L);

    // ------- Getting trivial properties -------

    /** This function returns true if the photon package has a stellar origin, false otherwise. */
    bool isStellar() const { return _stellar >= 0; }

    /** If the photon package has a stellar origin, this function returns the index of the emitting
        stellar component. If the photon package has a dust emission origin, this function returns
        -1. */
    int stellarCompIndex() const { return _stellar; }

    /** This function returns the luminosity of the photon package. */
    double luminosity() const { return _L; }

    /** This function returns the wavelength index of the photon package. */
    int ell() const { return _ell; }

    /** This function returns the number of scattering events the photon package has experienced.
        */
    int nScatt() const { return _nscatt; }

    /** This function returns the previous propagation direction of the photon package, i.e. the
        propagation direction just before the most recent scattering event. If the photon package
        has not yet been scattered, the function returns the current propagation direction instead.
        */
    Direction previousDirection() const { return _bfk_prev; }

    // ------- Data members -------

private:
    Direction _bfk_prev;
    double _L;
    int _ell;
    int _nscatt;
    int _stellar;
    const AngularDistribution* _ad;
};

////////////////////////////////////////////////////////////////////

#endif // PHOTONPACKAGE_HPP
