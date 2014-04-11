/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PHOTONPACKAGE_HPP
#define PHOTONPACKAGE_HPP

#include "Direction.hpp"
#include "Position.hpp"
class AngularDistribution;

////////////////////////////////////////////////////////////////////

/** The class PhotonPackage is used to describe photon packages, the basic luminosity packets that
    are transported during a radiative transfer simulation. Photon packages in SKIRT are
    monochromatic, i.e. they contain photons at a single wavelength (one of the wavelengths in the
    simulation's wavelength grid). Apart from its luminosity and wavelength, a photon package is
    characterized by its starting position and propagation direction, a flag indicating its origin
    (emission by a star or by a dust grain), and a counter indicating the number of scattering
    events it already experienced. */
class PhotonPackage
{
public:
    /** Basic constructor for the PhotonPackage class. All data members are initialized to their
        default values. In particular, the luminosity is set to zero. */
    PhotonPackage();

    /** Constructor for the PhotonPackage class. All data members are copied from the provided
        values: a flag indicating origin (emission by a star or by a dust grain), wavelength index,
        starting position, propagation direction, luminosity, and the number of scattering events
        already experienced. */
    PhotonPackage(bool ynstellar, int ell, Position bfr, Direction bfk, double L, int nscatt);

    /** This function sets the data members of the photon package to the provided values: a flag
        indicating origin (emission by a star or by a dust grain), wavelength index, starting
        position, propagation direction, luminosity, and the number of scattering events already
        experienced. */
    void set(bool ynstellar, int ell, Position bfr, Direction bfk, double L, int nscatt);

    /** This function returns true if the photon package has a stellar origin, false otherwise. */
    bool ynstellar() const;

    /** This function sets the origin of the photon package (emission by a star or by a dust grain). */
    void setynstellar(bool ynstellar);

    /** This function returns the wavelength index of the photon package. */
    int ell() const;

    /** This function sets the wavelength index for the photon package. */
    void setell(int ell);

    /** This function returns the position of the photon package, i.e. either the original position
        of emission or the position of the last interaction with a dust grain. */
    Position position() const;

    /** This function returns the propagation direction of the photon package. */
    Direction direction() const;

    /** This function sets the propagation direction of the photon package. */
    void setdirection(Direction bfk);

    /** This function returns the luminosity of the photon package. */
    double luminosity() const;

    /** This function sets the luminosity of the photon package. */
    void setluminosity(double L);

    /** This function returns the number of scattering events the photon package has experienced. */
    int nscatt() const;

    /** This function sets the number of scattering events the photon package has experienced. */
    void setnscatt(int nscatt);

    /** This function returns the angular distribution of the emission at the photon package's origin,
        or null if no angular distribution has been set (implying isotropic emission). */
    const AngularDistribution* angularDistribution() const;

    /** This function sets the angular distribution of the emission at the photon package's origin. */
    void setAngularDistribution(const AngularDistribution* ad);

    /** This function causes the propagation of the photon package over a physical distance
        \f$s\f$. It updates the position from \f${\bf{r}}\f$ to \f${\bf{r}}+s\,{\bf{k}}\f$,
        where \f${\bf{k}}\f$ is the propagation direction of the photon package. */
    void propagate(double s);

protected:
    bool _ynstellar;
    int _ell;
    Position _bfr;
    Direction _bfk;
    double _L;
    int _nscatt;
    const AngularDistribution* _ad;
};

////////////////////////////////////////////////////////////////////

#endif // PHOTONPACKAGE_HPP
