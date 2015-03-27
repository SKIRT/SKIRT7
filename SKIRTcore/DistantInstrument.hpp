/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DISTANTINSTRUMENT_HPP
#define DISTANTINSTRUMENT_HPP

#include "Array.hpp"
#include "Instrument.hpp"

////////////////////////////////////////////////////////////////////

/** DistantInstrument is an abstract class for representing instruments positioned at a
    sufficiently large distance from the system, so that the observable sky is a plane
    perpendicular to the line of sight rather than a part of a sphere. Consequently
    parallel projection is used and the distance is only important for flux calibration.

    The direction \f${\boldsymbol{k}}_{\text{obs}} = (\theta,\varphi)\f$ towards the instrument is
    specified through the inclination angle \f$\theta\f$ and the azimuth angle \f$\varphi\f$.
    Finally, the instrument can be rotated about the line of sight by specifying its position angle
    \f$\omega\f$. The table below lists some typical values for these angles, in degrees.

    <TABLE>
    <TR><TD><I>View</I></TD>     <TD>\f$\theta\f$</TD> <TD>\f$\varphi\f$</TD> <TD>\f$\omega\f$</TD> </TR>
    <TR><TD>XY-plane</TD>        <TD>0</TD>   <TD>0</TD>   <TD>90</TD>  </TR>
    <TR><TD>XZ-plane</TD>        <TD>90</TD>  <TD>-90</TD> <TD>0</TD>   </TR>
    <TR><TD>YZ-plane</TD>        <TD>90</TD>  <TD>0</TD>   <TD>0</TD>   </TR>
    <TR><TD>first octant</TD>    <TD>45</TD>  <TD>45</TD>  <TD>0</TD>   </TR>
    </TABLE>
    */
class DistantInstrument : public Instrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a distant instrument")

    Q_CLASSINFO("Property", "distance")
    Q_CLASSINFO("Title", "the distance to the system")
    Q_CLASSINFO("Quantity", "distance")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "inclination")
    Q_CLASSINFO("Title", "the inclination angle θ of the detector")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "180 deg")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "azimuth")
    Q_CLASSINFO("Title", "the azimuth angle φ of the detector")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "-360 deg")
    Q_CLASSINFO("MaxValue", "360 deg")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "positionAngle")
    Q_CLASSINFO("Title", "the position angle ω of the detector")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "-360 deg")
    Q_CLASSINFO("MaxValue", "360 deg")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    DistantInstrument();

    /** This function verifies that all attribute values have been appropriately set and performs
        setup for the instrument. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the distance between the observer and the instrument. The distance is assumed to be so
        large that we have effectively parallel projection, so that it is only important for flux
        calibration. */
    Q_INVOKABLE void setDistance(double value);

    /** Returns the distance between the observer and the instrument. */
    Q_INVOKABLE double distance() const;

    /** Sets the inclination \f$\theta\f$ for the instrument. */
    Q_INVOKABLE void setInclination(double value);

    /** Returns the inclination \f$\theta\f$ for the instrument. */
    Q_INVOKABLE double inclination() const;

    /** Sets the azimuth \f$\varphi\f$ for the instrument. */
    Q_INVOKABLE void setAzimuth(double value);

    /** Returns the azimuth \f$\varphi\f$ for the instrument. */
    Q_INVOKABLE double azimuth() const;

    /** Sets the position angle \f$\omega\f$ for the instrument. */
    Q_INVOKABLE void setPositionAngle(double value);

    /** Returns the position angle \f$\omega\f$ for the instrument. */
    Q_INVOKABLE double positionAngle() const;

    //======================== Other Functions =======================

public:
    /** Returns the direction towards the observer, as seen from the origin of the coordinate
        system. The provided photon package's launching position is not used; it is considered to
        be very close to the coordinate origin from the observer's standpoint, since the distance
        is sufficiently large. */
    Direction bfkobs(const Position& bfr) const;

    /** Returns the direction along the positive x-axis of the instrument frame, expressed in model
        coordinates. The function applies the inverse instrument transformation to the pixel
        frame's x-axis. */
    Direction bfkx() const;

    /** Returns the direction along the positive y-axis of the instrument frame, expressed in model
        coordinates. The function applies the inverse instrument transformation to the pixel
        frame's y-axis. */
    Direction bfky() const;

protected:
    /** This convenience function calibrates one or more integrated luminosity data vectors
        gathered by a DistantInstrument subclass, and outputs them as columns in a single SED text
        file. The incoming data is organized as a list of data arrays and a second list of
        corresponding human-readable names. Each array in the first list is a 1D vector containing
        a luminosity value per wavelength in the simulation's wavelength grid. The strings in
        the second list are used to identify the columns in the output file. The two lists must
        have the same size, and should not be empty. The generated SED text file is named for the
        instrument as in <tt>prefix_instrument_SED.dat</tt>. The first column lists the wavelength;
        subsequent columns provide the corresponding data point from one of the specified arrays.
        The calibration performed by this function takes care of the
        conversion from bolometric luminosity units to flux density units. Typical units for the
        quantities in the SED file are are \f$\text{W}\,\text{m}^{-2}\f$. The calibration is
        performed in-place in the arrays, so the incoming data is overwritten. */
    void calibrateAndWriteSEDs(QList< Array* > Farrays, QStringList Fnames);

    //======================== Data Members ========================

protected:
    // discoverable attributes of a distant instrument
    double _distance;
    double _azimuth;
    double _inclination;
    double _positionangle;

    // data members derived from the published attributes during setup
    double _cosphi, _sinphi;
    double _costheta, _sintheta;
    double _cospa, _sinpa;
    Direction _bfkobs;
    Direction _bfkx;
    Direction _bfky;
};

////////////////////////////////////////////////////////////////////

#endif // DISTANTINSTRUMENT_HPP
