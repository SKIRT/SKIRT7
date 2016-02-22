/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PERSPECTIVEINSTRUMENT_HPP
#define PERSPECTIVEINSTRUMENT_HPP

#include "Array.hpp"
#include "HomogeneousTransform.hpp"
#include "Instrument.hpp"
#include "ParallelDataCube.hpp"

////////////////////////////////////////////////////////////////////

/** The PerspectiveInstrument class implements a full perspective view of the simulated model, with
    arbitrary placement of the viewport outside or inside the model. For each wavelength the
    instrument maintains the total luminosity per pixel for all photon packages arriving from the
    front; light emitted behind the viewport is ignored. The instrument does \em not maintain the
    luminosity fractions caused by various phenomena (such as scattered versus direct light), nor
    does it keep track of the integrated luminosity across the viewport.

    The perspective instrument is intended mostly for making movies. Each movie frame is generated
    by a separate perspective instrument with the appropriate parameters.

    For more information about this instrument and its implementation, refer to the separate document
    \ref PerInstr. */
class PerspectiveInstrument : public Instrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a perspective instrument (mostly for making movies)")

    Q_CLASSINFO("Property", "pixelsX")
    Q_CLASSINFO("Title", "the number of viewport pixels in the horizontal direction")
    Q_CLASSINFO("MinValue", "25")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "pixelsY")
    Q_CLASSINFO("Title", "the number of viewport pixels in the vertical direction")
    Q_CLASSINFO("MinValue", "25")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "width")
    Q_CLASSINFO("Title", "the width of the viewport")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "viewX")
    Q_CLASSINFO("Title", "the position of the viewport origin, x component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "viewY")
    Q_CLASSINFO("Title", "the position of the viewport origin, y component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "viewZ")
    Q_CLASSINFO("Title", "the position of the viewport origin, z component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "crossX")
    Q_CLASSINFO("Title", "the position of the crosshair, x component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "crossY")
    Q_CLASSINFO("Title", "the position of the crosshair, y component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "crossZ")
    Q_CLASSINFO("Title", "the position of the crosshair, z component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "upX")
    Q_CLASSINFO("Title", "the upwards direction, x component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "upY")
    Q_CLASSINFO("Title", "the upwards direction, y component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "upZ")
    Q_CLASSINFO("Title", "the upwards direction, z component")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "focal")
    Q_CLASSINFO("Title", "the distance from the eye to the viewport origin")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PerspectiveInstrument();

protected:
    /** This function verifies that all attribute values have been appropriately set and performs
        setup for the instrument. Its most important task is to determine the appropriate
        perspective transformation. For more information see \ref PerInstr. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets \f$N_x\f$, the number of viewport pixels in the horizontal direction. */
    Q_INVOKABLE void setPixelsX(int value);

    /** Returns \f$N_x\f$, the number of viewport pixels in the horizontal direction. */
    Q_INVOKABLE int pixelsX() const;

    /** Sets \f$N_y\f$, the number of viewport pixels in the vertical direction. */
    Q_INVOKABLE void setPixelsY(int value);

    /** Returns \f$N_y\f$, the number of viewport pixels in the vertical direction. */
    Q_INVOKABLE int pixelsY() const;

    /** Sets \f$S_x\f$, the width of the viewport in world coordinates. The height of the viewport
        \f$S_y\f$ automatically follows by imposing square pixels, i.e. \f$S_y/N_y=S_x/N_x\f$. */
    Q_INVOKABLE void setWidth(double value);

    /** Returns \f$S_x\f$, the width of the viewport in world coordinates. */
    Q_INVOKABLE double width() const;

    /** Sets \f$V_x\f$, the x component of the position of the viewport origin in world coordinates. */
    Q_INVOKABLE void setViewX(double value);

    /** Returns \f$V_x\f$, the x component of the position of the viewport origin in world coordinates. */
    Q_INVOKABLE double viewX() const;

    /** Sets \f$V_y\f$, the y component of the position of the viewport origin in world coordinates. */
    Q_INVOKABLE void setViewY(double value);

    /** Returns \f$V_y\f$, the y component of the position of the viewport origin in world coordinates. */
    Q_INVOKABLE double viewY() const;

    /** Sets \f$V_z\f$, the z component of the position of the viewport origin in world coordinates. */
    Q_INVOKABLE void setViewZ(double value);

    /** Returns \f$V_z\f$, the z component of the position of the viewport origin in world coordinates. */
    Q_INVOKABLE double viewZ() const;

    /** Sets \f$C_x\f$, the x component of the crosshair position in world coordinates. */
    Q_INVOKABLE void setCrossX(double value);

    /** Returns \f$C_x\f$, the x component of the crosshair position in world coordinates. */
    Q_INVOKABLE double crossX() const;

    /** Sets \f$C_y\f$, the y component of the crosshair position in world coordinates. */
    Q_INVOKABLE void setCrossY(double value);

    /** Returns \f$C_y\f$, the y component of the crosshair position in world coordinates. */
    Q_INVOKABLE double crossY() const;

    /** Sets \f$C_z\f$, the z component of the crosshair position in world coordinates. */
    Q_INVOKABLE void setCrossZ(double value);

    /** Returns \f$C_z\f$, the z component of the crosshair position in world coordinates. */
    Q_INVOKABLE double crossZ() const;

    /** Sets \f$U_x\f$, the x component of the upwards direction in world coordinates. */
    Q_INVOKABLE void setUpX(double value);

    /** Returns \f$U_x\f$, the x component of the upwards direction in world coordinates. */
    Q_INVOKABLE double upX() const;

    /** Sets \f$U_y\f$, the y component of the upwards direction in world coordinates. */
    Q_INVOKABLE void setUpY(double value);

    /** Returns \f$U_y\f$, the y component of the upwards direction in world coordinates. */
    Q_INVOKABLE double upY() const;

    /** Sets \f$U_z\f$, the z component of the upwards direction in world coordinates. */
    Q_INVOKABLE void setUpZ(double value);

    /** Returns \f$U_z\f$, the z component of the upwards direction in world coordinates. */
    Q_INVOKABLE double upZ() const;

    /** Sets \f$F_e\f$, the focal length (the distance from the eye to the viewport origin) in world coordinates. */
    Q_INVOKABLE void setFocal(double value);

    /** Returns \f$F_e\f$, the focal length (the distance from the eye to the viewport origin) in world coordinates. */
    Q_INVOKABLE double focal() const;

    //======================== Other Functions =======================

public:
    /** Returns the direction towards the eye from the given photon package launching position.
        For more information see \ref PerInstr. */
    Direction bfkobs(const Position& bfr) const;

    /** Returns the direction along the positive x-axis of the instrument frame, expressed in model
        coordinates. */
    Direction bfkx() const;

    /** Returns the direction along the positive y-axis of the instrument frame, expressed in model
        coordinates. */
    Direction bfky() const;

protected:
    /** This function simulates the detection of a photon package by the instrument.
        For more information see \ref PerInstr. */
    void detect(PhotonPackage* pp);

    /** This function calibrates and outputs the instrument data.
        For more information see \ref PerInstr. */
    void write();

    //======================== Data Members ========================

private:
    // discoverable attributes of a distant instrument
    int _Nx, _Ny;           // number of pixels
    double _Sx;             // viewport width
    double _Vx, _Vy, _Vz;   // viewport position (origin)
    double _Cx, _Cy, _Cz;   // crosshair position
    double _Ux, _Uy, _Uz;   // upwards position
    double _Fe;             // focal length

    // data members derived from the published attributes during setup
    double _s;              // width and height of a pixel
    double _Ex, _Ey, _Ez;   // eye position
    Direction _bfkx;        // unit vector along the viewport's x-axis
    Direction _bfky;        // unit vector along the viewport's y-axis
    HomogeneousTransform _transform;    // transform from world to pixel coordinates

    // data cube
    Array _ftotv;
    ParallelDataCube _distftotv;
};

////////////////////////////////////////////////////////////////////

#endif // PERSPECTIVEINSTRUMENT_HPP
