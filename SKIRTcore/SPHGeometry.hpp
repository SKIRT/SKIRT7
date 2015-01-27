/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHGEOMETRY_HPP
#define SPHGEOMETRY_HPP

#include "Array.hpp"
#include "DustParticleInterface.hpp"
#include "GenGeometry.hpp"
#include "SPHGasParticle.hpp"
class SPHGasParticleGrid;

////////////////////////////////////////////////////////////////////

/** The SPHGeometry class describes an arbitrary 3D geometry from a set of SPH gas particles, such
    as for example resulting from a cosmological simulation. The information on the SPH gas
    particles is read from a file formatted as described with the setFilename() function. The total
    mass in the geometry is normalized to unity after importing the data, so the units of the mass
    values in the data file are in fact irrelevant. */
class SPHGeometry : public GenGeometry, public DustParticleInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry derived from an SPH output file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the file with the SPH gas particles")

    Q_CLASSINFO("Property", "maximumTemperature")
    Q_CLASSINFO("Title", "the maximum temperature for a gas particle to be taken into account")
    Q_CLASSINFO("Quantity", "temperature")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000000 K")
    Q_CLASSINFO("Default", "75000 K")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SPHGeometry();

    /** The destructor releases the data structure allocated during setup. */
    ~SPHGeometry();

protected:
    /** This function reads the properties for each of the SPH gas particles from the specified
        file, converting them to program units and storing them in the internal vectors. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file containing the information on the SPH gas particles, optionally
        including an absolute or relative path. This text file should contain 6 or 7 columns of
        numbers separated by whitespace; lines starting with # are ignored. The first three columns
        are the \f$x\f$, \f$y\f$ and \f$z\f$ coordinates of the particle (in pc), the fourth column
        is the SPH smoothing length \f$h\f$ (in pc), the fifth column is the mass \f$M\f$ of the
        particle (in \f$M_\odot\f$; the total mass in the geometry will be normalized to unity
        after importing the data, so the mass units in the data file are in fact irrelevant), and
        the sixth column is the metallicity \f$Z\f$ of the gas (dimensionless fraction). The
        optional seventh column is the temperature of the gas (in K). If this value is provided and
        it is higher than the maximum temperature the particle is ignored. If the temperature value
        is missing, the particle is never ignored. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file containing the information on the SPH gas particles. */
    Q_INVOKABLE QString filename() const;

    /** Sets the maximum temperature for a gas particle to be taken into account; any gas particles
        with a temperature above this value are ignored. If the temperature for a particle is not
        provided in the input file, the particle is never ignored. */
    Q_INVOKABLE void setMaximumTemperature(double value);

    /** Returns the maximum temperature for a gas particle to be taken into account. */
    Q_INVOKABLE double maximumTemperature() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ for this geometry at the position
        \f${\bf{r}}\f$. For an SPH geometry, the density is calculated by summing over all the
        particles, assuming a standard spline kernel. The total mass is normalized to unity. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry. It randomly chooses a particle
        using the normalized cumulative density distribution constructed during the setup phase.
        Then a position is determined randomly from the smoothed distribution around the particle
        center. The function assumes the scaled Gaussian smoothing kernel \f[ W(h,r) =
        \frac{a^3}{\pi^{3/2}\,h^3} \,\exp({-\frac{a^2 r^2}{h^2}}) \f] with the empirically
        determined value of \f$a=2.42\f$, which approximates the standard cubic spline kernel to
        within two percent accuracy. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density of the geometry. For an SPH geometry, this
        integral is calculated numerically using 10000 samples along the X-axis. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the geometry. For an SPH geometry, this
        integral is calculated numerically using 10000 samples along the Y-axis. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the geometry. For an SPH geometry, this
        integral is calculated numerically using 10000 samples along the Z-axis. */
    double SigmaZ() const;

    /** This function implements the DustParticleInterface. It returns the number of SPH particles
        defining this geometry. */
    int numParticles() const;

    /** This function implements the DustParticleInterface. It returns the coordinates of the SPH
        particle with the specified zero-based index. If the index is out of range, a fatal error
        is thrown. */
    Vec particleCenter(int index) const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    QString _filename;
    double _Tmax;

    // the SPH particles
    std::vector<SPHGasParticle> _pv;  // the particles in the order read from the file
    const SPHGasParticleGrid* _grid;  // a list of particles overlapping each grid cell
    Array _cumrhov;   // cumulative density distribution for particles in pv
    double _norm;     // normalization factor ( 1 / M_tot )
};

////////////////////////////////////////////////////////////////////

#endif // SPHGEOMETRY_HPP
