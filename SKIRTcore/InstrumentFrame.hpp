/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef INSTRUMENTFRAME_HPP
#define INSTRUMENTFRAME_HPP

#include "ArrayTable.hpp"
#include "SimulationItem.hpp"
class PhotonPackage;
class MultiFrameInstrument;

////////////////////////////////////////////////////////////////////

/** The InstrumentFrame class implements a single instrument frame with a specific extent and pixel
    resolution. It is used by the MultiFrameInstrument class to support a different frame for each
    wavelength. The position of the instrument frame is determined by the properties of its parent
    MultiFrameInstrument object (in fact, by the angle attributes of its DistantInstrument base
    class). It is assumed that the distance to the system is sufficiently large so that parallel
    projection can be used. Refer to the SingleInstrument class for more information on the extent
    and pixel size attributes offered by this class. */
class InstrumentFrame : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a frame in the multi-frame instrument")

    Q_CLASSINFO("Property", "pixelsX")
    Q_CLASSINFO("Title", "the number of pixels in the horizontal direction")
    Q_CLASSINFO("MinValue", "25")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the maximal horizontal extent")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "pixelsY")
    Q_CLASSINFO("Title", "the number of pixels in the vertical direction")
    Q_CLASSINFO("MinValue", "25")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the maximal vertical extent")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE InstrumentFrame();

protected:
    /** This function verifies that all attribute values have been appropriately set and performs
        setup for the instrument. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of pixels \f$N_x\f$ in the instrument frame in the horizontal direction. */
    Q_INVOKABLE void setPixelsX(int value);

    /** Returns the number of pixels \f$N_x\f$ in the instrument frame in the horizontal direction.
        */
    Q_INVOKABLE int pixelsX() const;

    /** Sets the maximum extent \f$x_{\text{max}}\f$ for the instrument frame in the horizontal
        direction. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the maximum extent \f$x_{\text{max}}\f$ for the instrument frame in the horizontal
        direction. */
    Q_INVOKABLE double extentX() const;

    /** Sets the number of pixels \f$N_y\f$ in the instrument frame in the vertical direction. */
    Q_INVOKABLE void setPixelsY(int value);

    /** Returns the number of pixels \f$N_y\f$ in the instrument frame in the vertical direction.
        */
    Q_INVOKABLE int pixelsY() const;

    /** Sets the maximum extent \f$y_{\text{max}}\f$ for the instrument frame in the vertical
        direction. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the maximum extent \f$y_{\text{max}}\f$ for the instrument frame in the vertical
        direction. */
    Q_INVOKABLE double extentY() const;

    //======================== Other Functions =======================

private:
    /** This function returns the index of the spatial pixel on the detector that will be hit by a
        photon package, or -1 if the photon package does not hit the detector. It operates
        similarly to SingleFrameInstrument::pixelondetector(). */
    int pixelondetector(const PhotonPackage* pp) const;

public:
    /** This function simulates the detection of a photon package by the instrument frame. It
        operates similarly to SimpleInstrument::detect(), but for a single wavelength. If the
        parent multi-frame instrument has the writeTotal flag turned on, this function records the
        total flux. If the writeStellarComps flag is turned on, this function records the flux for
        each stellar component seperately. */
    void detect(PhotonPackage* pp);

    /** This function properly calibrates and outputs the instrument data. It operates similarly to
        SimpleInstrument::write(), but for the single wavelength specified through its wavelength
        index \f$\ell\f$. If the parent multi-frame instrument has the writeTotal flag turned on,
        this function outputs the total flux in a correspondingly named file. If the parent
        multi-frame instrument has the writeStellarComps flag turned on, this function writes the
        flux for each stellar component in a seperate output file, with a name that includes the
        stellar component index. In all cases, the name of each output file includes the wavelength
        index. */
    void calibrateAndWriteData(int ell);

private:
    /** This private function properly calibrates and outputs the instrument data. It is invoked
        from the public calibrateAndWriteData() function. */
    void calibrateAndWriteDataFrames(int ell, QList<Array*> farrays, QStringList fnames);

    //======================== Data Members ========================

private:
    // discoverable attributes of this frame
    int _Nxp;
    double _xpmax;
    int _Nyp;
    double _ypmax;

    // data members derived from discoverable attributes during setup
    double _xpres;
    double _ypres;
    double _xpmin;
    double _ypmin;

    // data members copied from the parent multi-frame instrument during setup
    MultiFrameInstrument* _instrument;
    bool _writeTotal;
    bool _writeStellarComps;
    double _distance;
    double _cosphi, _sinphi;
    double _costheta, _sintheta;
    double _cospa, _sinpa;

    // total flux per pixel
    Array _ftotv;
    ArrayTable<2> _fcompvv;
};

////////////////////////////////////////////////////////////////////

#endif // INSTRUMENTFRAME_HPP
