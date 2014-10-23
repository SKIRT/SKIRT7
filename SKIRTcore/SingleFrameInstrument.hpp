/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SINGLEFRAMEINSTRUMENT_HPP
#define SINGLEFRAMEINSTRUMENT_HPP

#include "DistantInstrument.hpp"

////////////////////////////////////////////////////////////////////

/** SingleFrameInstrument is an abstract class representing an integral field spectrograph,
    i.e. a three-dimensional detector with two spatial and one wavelength dimension. Depending on
    the subclass, this 3D detector can be subdivided into different subdetectors.

    In every dimension, the detector is specified by a fixed grid. The wavelength grid is determined
    by the simulation's wavelength grid; the resolution and the extent of the grids in the spatial
    dimensions can be set separately. The spatial grid consists of a linear \f$(x_i,y_j)\f$ grid
    with \f$x\f$ ranging from \f$x_0=-x_{\text{max}}\f$ to \f$x_{N_x-1}=x_{\text{max}}\f$ and
    \f$y\f$ ranging from \f$y_0=-y_{\text{max}}\f$ to \f$y_{N_y-1}=y_{\text{max}}\f$. These points
    are to be considered as the centres of pixels, each with a size \f$\Delta x \times \Delta y\f$
    with \f[ \begin{split} \Delta x &= \frac{2\,x_{\text{max}}}{N_x-1} \\ \Delta y &=
    \frac{2\,y_{\text{max}}}{N_y-1} \end{split} \f] This accounts also for the outermost pixels.
    For example the first pixel has its centre at \f$(x_0,y_0) = (-x_{\text{max}},-y_{\text{max}})
    \f$ and its bottom-left corner is at \f$ (x,y) = ( -x_{\text{max}}-\tfrac12\,\Delta
    x,-y_{\text{max}}-\tfrac12\,\Delta y ) \f$. The total field of view of the detectors is hence
    not \f$2x_{\text{max}} \times 2y_{\text{max}}\f$ but \f$ (2x_{\text{max}}+\Delta x) \times
    (2y_{\text{max}}+\Delta y) \f$.

    The position of the observing instrument is determined by the properties of the DistantInstrument
    base class. It is assumed that the distance to the system is sufficiently large so that parallel
    projection can be used.
*/
class SingleFrameInstrument : public DistantInstrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a single-frame instrument")

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

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    SingleFrameInstrument();

    /** This function verifies that all attribute values have been appropriately set and performs
        setup for the instrument. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of pixels \f$N_x\f$ in the instrument in the horizontal direction. */
    Q_INVOKABLE void setPixelsX(int value);

    /** Returns the number of pixels \f$N_x\f$ in the instrument in the horizontal direction. */
    Q_INVOKABLE int pixelsX() const;

    /** Sets the maximum extent \f$x_{\text{max}}\f$ for the instrument in the horizontal
        direction. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the maximum extent \f$x_{\text{max}}\f$ for the instrument in the horizontal
        direction. */
    Q_INVOKABLE double extentX() const;

    /** Sets the number of pixels \f$N_y\f$ in the instrument in the vertical direction. */
    Q_INVOKABLE void setPixelsY(int value);

    /** Returns the number of pixels \f$N_y\f$ in the instrument in the vertical direction. */
    Q_INVOKABLE int pixelsY() const;

    /** Sets the maximum extent \f$y_{\text{max}}\f$ for the instrument in the vertical direction.
        */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the maximum extent \f$y_{\text{max}}\f$ for the instrument in the vertical
        direction. */
    Q_INVOKABLE double extentY() const;

    //======================== Other Functions =======================

protected:
    /** This convenience function returns the index of the spatial pixel on the detector that will
        be hit by a photon package, or -1 if the photon package does not hit the detector. Given
        the position \f${\boldsymbol{x}}=(x,y,z)\f$ of the last emission or scattering event of the
        photon package, the direction \f${\boldsymbol{k}}_{\text{obs}} = (\theta,\varphi)\f$
        towards the observer, and the position angle \f$\omega\f$ of the instrument, the impact
        coordinates \f$(x_{\text{p}},y_{\text{p}})\f$ are given by the following Euler-like
        transformation, where \f$z_{\text{p}}\f$ is ignored: \f[ \begin{bmatrix}x_{\text{p}} \\
        y_{\text{p}} \\ z_{\text{p}} \end{bmatrix} = \begin{bmatrix}\cos\omega & -\sin\omega & 0\\
        \sin\omega & \cos\omega & 0\\ 0 & 0 & 1 \end{bmatrix} \begin{bmatrix} 0 & 1 & 0 \\ -1 & 0 &
        0 \\ 0 & 0 & 1 \end{bmatrix} \begin{bmatrix} \cos\theta & 0 & -\sin\theta \\ 0 & 1 & 0 \\
        \sin\theta & 0 & \cos\theta \end{bmatrix} \begin{bmatrix}\cos\varphi & -\sin(-\varphi) & 0
        \\ \sin(-\varphi) & \cos\varphi & 0 \\ 0 & 0 & 1 \end{bmatrix}
        \begin{bmatrix}x\\y\\z\end{bmatrix} \f] In other words, the originating position is rotated
        about the Z-axis over the azimuth angle \f$\varphi\f$ (with a minus sign because the
        observer is looking towards the center rather than along the specified direction), then
        rotated about the new Y-axis over the inclination angle \f$\theta\f$, and finally rotated
        about the new Z-axis over the position angle \f$\omega\f$ reduced by 90 degrees (this
        constant transformation over -90 degrees is represented above as a separate matrix). The 90
        degree correction on the position angle is introduced so that it would be more natural to
        specify this angle; in most cases it can be left to its default value of 0.

        Given these impact coordinates, the pixel indices \f$i\f$ and \f$j\f$ are determined as \f[
        \begin{split} i &= \frac{[x_{\text{p}}-x_{\text{max}}]}{\Delta x} \\ j &=
        \frac{[y_{\text{p}}-y_{\text{max}}]}{\Delta y} \end{split} \f] where \f$\Delta x\f$ and
        \f$\Delta y\f$ are the resolution of the detector array and \f$[z]\f$ is an operator
        rounding to the nearest integer. The spatial pixel number \f$l\f$ is then determined as
        \f$l=i+j\,N_x\f$, asuming \f$i\f$ and \f$j\f$ are indeed within the detector range. */
    int pixelondetector(const PhotonPackage* pp) const;

    /** This convenience function calibrates one or more luminosity data cubes gathered by a
        DistantInstrument subclass and outputs each data cube as a FITS file. The incoming data is
        organized as a list of data arrays and a second list of corresponding human-readable names.
        Each array in the first list is a 3D data cube containing a limunosity value per pixel in
        the 2D frame and per wavelength in the simulation's wavelength grid. The strings in the
        second list are used as part of the output file name for the corresponding data cube using
        the pattern <tt>prefix_instrument_array.fits</tt>. The two lists must have the same size;
        if they are empty no output is generated. The calibration performed by this function takes
        care of the conversion from bolometric luminosity units to surface brightness units. The
        unit in which the surface brightness is written depends on the global units choice, but
        typically it is in \f$\text{W}\,\text{m}^{-2}\,\text{arcsec}^{-2}\f$. The calibration is
        performed in-place in the arrays, so the incoming data is overwritten. */
    void calibrateAndWriteDataCubes(QList< Array* > farrays, QStringList fnames);

    //======================== Data Members ========================

protected:
    // discoverable attributes of a generic instrument
    int _Nxp;
    double _xpmax;
    int _Nyp;
    double _ypmax;

    // data members derived from the published attributes during setup
    double _xpres;
    double _ypres;
    double _xpmin;
    double _ypmin;
};

////////////////////////////////////////////////////////////////////

#endif // SINGLEFRAMEINSTRUMENT_HPP
