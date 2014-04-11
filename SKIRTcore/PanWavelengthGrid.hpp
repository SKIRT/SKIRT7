/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PANWAVELENGTHGRID_HPP
#define PANWAVELENGTHGRID_HPP

#include "WavelengthGrid.hpp"

//////////////////////////////////////////////////////////////////////

/** PanWavelengthGrid is an abstract subclass of the general WavelengthGrid class representing
    wavelength grids for use in panchromatic simulations. It calculates appropriate wavelength bin
    widths based on the wavelength vector setup by a subclass in its setupSelfAfter() function. */
class PanWavelengthGrid : public WavelengthGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a wavelength grid for use in a panchromatic simulation")

    Q_CLASSINFO("Property", "writeWavelengths")
    Q_CLASSINFO("Title", "output a data file listing the wavelength grid points")
    Q_CLASSINFO("Default", "no")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PanWavelengthGrid();

    /** This function calculates the wavelength bin widths \f$\Delta_\ell\f$ for each of the
        wavelength points in the wavelength grid. The border of each bin is determined as the
        geometric mean of the wavelengths bordering it. In concreto, the width of the \f$\ell\f$'th
        bin is \f[ \Delta_\ell = \sqrt{\lambda_\ell\lambda_{\ell+1}} -
        \sqrt{\lambda_{\ell-1}\lambda_\ell}. \f] For the first bin this becomes \f[ \Delta_0 =
        \sqrt{\lambda_0\lambda_1} - \lambda_0 = \sqrt{\lambda_{\text{min}}\lambda_1} -
        \lambda_{\text{min}} \f] while for the last bin we obtain \f[ \Delta_{N-1} = \lambda_{N-1}
        - \sqrt{\lambda_{N-2}\lambda_{N-1}} = \lambda_{\text{max}} -
        \sqrt{\lambda_{N-2}\lambda_{\text{max}}}. \f]

        If the \em writeWavelengths flag is turned on, this function also outputs a data file
        called <tt>prefix_wavelengths.dat</tt>
        listing the wavelength grid points and bin widths. Apart from the header comment line,
        the text file contains a line for each of the \f$N\f$ wavelength grid points. Each line
        has two columns: the first column specifies the wavelength \f$\lambda_\ell\f$ and the
        second columns specifies the corresponding bin width \f$\Delta_\ell\f$. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the flag that indicates whether or not to output a data file listing the wavelength
        grid points and bin widths. The default value is false. */
    Q_INVOKABLE void setWriteWavelengths(bool value);

    /** Returns the flag that indicates whether or not to output a data file listing the
        wavelength grid points and bin widths. */
    Q_INVOKABLE bool writeWavelengths() const;

    //======================== Other Functions =======================

public:
    /** This function's implementation for this class always returns true since a PanWavelengthGrid
        represents a sampled wavelength range, as required for panchromatic simulations. */
    bool issampledrange() const;

    //======================== Data Members ========================

private:
    // data members to be set before setup is invoked
    bool _writeWavelengths;
};

//////////////////////////////////////////////////////////////////////

#endif // PANWAVELENGTHGRID_HPP
