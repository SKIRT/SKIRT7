/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SIMPLEINSTRUMENT_HPP
#define SIMPLEINSTRUMENT_HPP

#include "SingleFrameInstrument.hpp"

////////////////////////////////////////////////////////////////////

/** A SimpleInstrument object represents a basic instrument that records the total flux in every
    pixel and the total integrated flux. Internally, the class contains two vectors as data
    members: a simple 1D vector (the F-vector) that stores the total integrated flux at every
    wavelength index, and a 3D vector (the f-vector) corresponding to the surface brightness in
    every pixel, at every wavelength index. */
class SimpleInstrument : public SingleFrameInstrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a basic instrument that outputs the total flux as data cube and as SED")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    Q_INVOKABLE SimpleInstrument();

protected:
    /** This function completes setup for this instrument. */
    void setupSelfBefore();

    //======================== Other Functions =======================

protected:
    /** This function simulates the detection of a photon package by the instrument. The ingredients
        to be determined are the pixel that the photon package will hit and the luminosity that
        will be collected by the instrument (in the detection phase, the detectors store
        luminosities; these will be converted to fluxes and surface brightnesses once the
        simulation is finished). If the total luminosity of the photon package at the site of its
        last emission or scattering event is equal to \f$L_\ell\f$, the fraction of it that will
        reach the observer is equal to \f[ L_\ell^{\text{tot}} = L_\ell\,
        {\text{e}}^{-\tau_{\ell,{\text{path}}}} \f] with \f$\tau_{\ell,{\text{path}}}\f$ the
        optical depth through the dust system towards the observer. We now only have to add the
        luminosity to the stored luminosity in the correct bin of both the 1D F-vector and the 3D
        f-vector. */
    void detect(const PhotonPackage* pp);

    /** This function calibrates and outputs the instrument data.
        The calibration takes care of the conversion from bolometric luminosity units to flux
        density units (for the F-vector) and surface brightness units (for the f-vector). The
        function creates a FITS file called <tt>prefix_instrument_total.fits</tt> that contains
        the 3D vector with the surface brightness \f$\lambda f_\lambda = \nu f_\nu\f$ in every
        pixel (if there is only one wavelength, a 2D FITS file is created). The unit in which the
        surface brightness is written depends on the global units choice, but typically it is in
        \f$\text{W}\, \text{m}^{-2}\, \text{arcsec}^{-2}\f$. A file
        <tt>prefix_instrument_sed.dat</tt> is also created that contains two columns: the
        wavelength \f$\lambda\f$ and the observed integrated flux \f$\lambda F_\lambda = \nu F_\nu
        \f$ (typical unit is \f$\text{W}\,\text{m}^{-2}\f$). */
    void write();

    //======================== Data Members ========================

private:
    Array _ftotv;
    Array _Ftotv;
};

////////////////////////////////////////////////////////////////////

#endif // SIMPLEINSTRUMENT_HPP
