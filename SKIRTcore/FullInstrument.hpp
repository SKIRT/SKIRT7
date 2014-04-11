/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef FULLINSTRUMENT_HPP
#define FULLINSTRUMENT_HPP

#include "ArrayTable.hpp"
#include "SingleFrameInstrument.hpp"

////////////////////////////////////////////////////////////////////

/** A FullInstrument object represents an advanced instrument that records individual contributions
    to the flux from various sources. Rather than the total flux \f$F_\lambda^{\text{tot}}\f$, this
    instrument separately stores the contributions due to direct stellar radiation
    (\f$F_\lambda^{\text{dir}}\f$), due to scattered stellar radiation
    (\f$F_\lambda^{\text{sca}}\f$), and due to thermal radiation emitted by the dust
    (\f$F_\lambda^{\text{dus}}\f$), with obviously \f[ F_\lambda^{\text{tot}} =
    F_\lambda^{\text{dir}} + F_\lambda^{\text{sca}} + F_\lambda^{\text{dus}} \f] The instrument
    also stores the flux from the object as it would be seen without any dust attenuation, i.e. the
    transparent view of the system. Furthermore, the contributions from individual scattering
    levels (up to a certain maximum level \f$N_{\text{max}}\f$) are recorded separately. By
    default, the maximum level is set to zero, effectively disabling this feature. Internally, the
    class contains \f$N_{\text{max}}+4\f$ sets of two vectors each as data members: a simple 1D
    vector (the F-vector) that stores the integrated flux at every wavelength index, and a 3D
    vector (the f-vector) corresponding to the surface brightness in every pixel, at every
    wavelength index. */
class FullInstrument : public SingleFrameInstrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an advanced instrument that records individual contributions to the flux")

    Q_CLASSINFO("Property", "scatteringLevels")
    Q_CLASSINFO("Title", "the number of scattering levels to be recorded individually")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "25")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE FullInstrument();

protected:
    /** This function completes setup for this instrument. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of scattering levels \f$N_{\text{max}}\f$ to be recorded individually. */
    Q_INVOKABLE void setScatteringLevels(int value);

    /** Returns the number of scattering levels \f$N_{\text{max}}\f$ to be recorded individually. */
    Q_INVOKABLE int scatteringLevels() const;

    //======================== Other Functions =======================

protected:
    /** This function simulates the detection of a photon package by the instrument. The
        ingredients to be determined are the pixel that the photon package will hit and the
        luminosity that will be collected by the different subdetectors of the instrument (in the
        detection phase, the detectors store luminosities, these will be converted to fluxes and
        surface brightnesses once the simulation is finished). If the total luminosity of the
        photon at the site of its last emission or scattering event is equal to \f$L_\ell\f$, the
        fraction of it that will reach the observer is equal to \f[ L_\ell^{\text{tot}} = L_\ell\,
        {\text{e}}^{-\tau_{\ell,{\text{path}}}} \f] with \f$\tau_{\ell,{\text{path}}}\f$ the
        optical depth through the dust system towards the observer. Depending on the origin of the
        photon package (i.e. emitted by the stars or by the dust) and the number of scattering
        events the package has already experienced, this bit of luminosity must be assigned to the
        correct subdetector. The corresponding flux for the transparent case is simply
        \f$L_\ell^{\text{tra}}\f$. We now only have to add the luminosities to the stored
        luminosity in the correct bin of both the 1D F-vectors and the 3D f-vectors. */
    void detect(const PeelOffPhotonPackage* pp);

    /** This function calibrates and outputs the instrument data.
        The calibration takes care of the conversion from bolometric luminosity units to
        flux density units (for the F-vector) and surface brightness units (for the f-vector). The
        function creates five FITS files called <tt>prefix_instrument_total.fits</tt>,
        <tt>prefix_instrument_direct.fits</tt>, <tt>prefix_instrument_scattered.fits</tt>,
        <tt>prefix_instrument_dust.fits</tt> and <tt>prefix_instrument_transparent.fits</tt> that
        contain the 3D vectors with the surface brightness \f$\lambda f_\lambda^{\text{tot}} = \nu
        f_\nu^{\text{tot}} \f$ etc. in every pixel (if there is only one wavelength, 2D FITS files
        are created). The unit in which the surface brightness is written depends on the global
        units choice, but typically it is in \f$\text{W}\,\text{m}^{-2}\,\text{arcsec}^{-2}\f$. The
        function also creates \f$N_{\text{max}}\f$ separate FITS files
        <tt>prefix_instrument_scatteringlevel1.fits</tt>, etc. that contain the 3D vectors with the
        contribution of the individual scattering levels to the surface brightness. Finally, an
        ASCII file <tt>prefix_instrument_sed.dat</tt> is created that contains
        \f$6+N_{\text{max}}\f$ columns. The first six columns contain the wavelength \f$\lambda\f$,
        the total flux \f$\lambda F_\lambda^{\text{tot}}\f$, the direct flux \f$\lambda
        F_\lambda^{\text{dir}}\f$, the scattered flux \f$\lambda F_\lambda^{\text{sca}}\f$, the
        thermal dust flux \f$\lambda F_\lambda^{\text{dus}}\f$ and the transparent flux \f$\lambda
        F_\lambda^{\text{tra}}\f$. The last \f$N_{\text{max}}\f$ columns contain the contribution
        of the different scattering levels to the total flux. Typical units for these quantities
        are \f$\text{W}\,\text{m}^{-2}\f$. */
    void write();

    //======================== Data Members ========================

private:
    int _Nscatt;
    Array _fdirv;
    Array _fscav;
    Array _ftrav;
    Array _fdusv;
    ArrayTable<2> _fscavv;
    Array _Fdirv;
    Array _Fscav;
    Array _Ftrav;
    Array _Fdusv;
    ArrayTable<2> _Fscavv;
};

////////////////////////////////////////////////////////////////////

#endif // FULLINSTRUMENT_HPP
