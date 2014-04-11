/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PEELOFFPHOTONPACKAGE_HPP
#define PEELOFFPHOTONPACKAGE_HPP

#include <cfloat>
#include "PhotonPackage.hpp"

class DustSystem;

////////////////////////////////////////////////////////////////////

/** The PeelOffPhotonPackage class is a subclass of the more general PhotonPackage class, and is
    used to characterize photon packages that are peeled off from other photon packages after an
    emission, scattering or re-emission event. These peel-off photon packages are ready to be
    detected by an observer. Apart from all the characteristics of a regular photon package, a
    peel-off photon package also contains a reference to the dust system through which it is
    moving, so that it can calculate the optical depth of the path along which it is moving.
    */
class PeelOffPhotonPackage : public PhotonPackage
{
public:
    /** Basic constructor. First a regular photon package of the base class PhotonPackage is
        constructed by copying all the relevant characteristics. Then a pointer to
        the dust system is added. */
    PeelOffPhotonPackage(bool ynstellar, int ell, Position bfr, Direction bfk,
                         double Lv, int nscatt, DustSystem* ds);

    /** This function calculates and returns the optical depth, at the wavelength index \f$\ell\f$,
        over the specified distance of the path along which the peel-off photon package is moving.
        If the distance is not specified, the complete path is taken into account. */
    double opticaldepth(double distance=DBL_MAX) const;

private:
    DustSystem* _ds;
};

////////////////////////////////////////////////////////////////////

#endif // PEELOFFPHOTONPACKAGE_HPP
