/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHSTELLARCOMP_HPP
#define SPHSTELLARCOMP_HPP

#include "ArrayTable.hpp"
#include "StellarComp.hpp"
#include "Vec.hpp"
class Random;

//////////////////////////////////////////////////////////////////////

/** The SPHStellarComp class represents a stellar component defined from a set of SPH star particles,
    such as for example resulting from a cosmological simulation. The information on the SPH star
    particles is read from a file formatted as described with the setFilename() function. */
class SPHStellarComp : public StellarComp
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar component derived from an SPH output file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the file with the SPH star particles")

    Q_CLASSINFO("Property", "writeLuminosities")
    Q_CLASSINFO("Title", "output a data file with the luminosities per wavelength bin")
    Q_CLASSINFO("Default", "no")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SPHStellarComp();

protected:
    /** This function performs setup for the SPH stellar component. The first step is to load the
        properties for each of the SPH star particles from the specified file. The second step is
        to calculate the luminosity \f$L_{\ell,i}\f$ for each particle \f$i\f$ and at each
        wavelength grid point \f$\ell\f$. The luminosity distribution for each particle is
        determined using bilinear interpolation on age and metallicity in the family of Bruzual &
        Charlot SEDs (read from the appropriate resource files). These particle-specific SEDs are
        resampled on the simulation's global wavelength grid. Summing over all particles, a vector
        with the total luminosity for each wavelength bin is constructed. Finally, a matrix
        \f$X_{\ell,i}\f$ is filled that contains the normalized cumulative luminosity, \f[
        X_{\ell,i} = \frac{ \sum_{j=0}^{i-1} L_{\ell,j} }{ \sum_{j=0}^{N-1} L_{\ell,j} } \f] . This
        matrix will be used for the efficient generation of random photon packages from the stellar
        component. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file containing the information on the SPH star particles, optionally
        including an absolute or relative path. This text file should contain exactly 7 columns of
        numbers separated by whitespace; lines starting with # are ignored. The first three columns
        are the \f$x\f$, \f$y\f$ and \f$z\f$ coordinates of the particles (in pc), the fourth
        column is the SPH smoothing length \f$h\f$ (in pc), the fifth column is the initial mass of
        the stellar population (in \f$M_\odot\f$ at \f$t=0\f$), the sixth column is the metallicity
        \f$Z\f$ of the stellar population (dimensionless fraction), and the seventh column is the
        age of the stellar population (in yr). */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file containing the information on the SPH star particles. */
    Q_INVOKABLE QString filename() const;

    /** Sets the flag that indicates whether or not to output a data file with the luminosities per
        wavelength bin. The default value is false. */
    Q_INVOKABLE void setWriteLuminosities(bool value);

    /** Returns the flag that indicates whether or not to output a data file with the luminosities
        per wavelength bin. */
    Q_INVOKABLE bool writeLuminosities() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the stellar component, which for this class is
        always 3 since there are no symmetries in the geometry. */
    int dimension() const;

    /** This function returns the monochromatic luminosity \f$L_\ell\f$ of the stellar component at
        the wavelength index \f$\ell\f$. It just reads the appropriate number from the internally
        stored vector. */
    double luminosity(int ell) const;

    /** This function simulates the emission of a monochromatic photon package with a monochromatic
        luminosity \f$L\f$ at wavelength index \f$\ell\f$ from the stellar component. It just
        randomly chooses an SPH particle from the \f$N\f$ possible particles by generating a random
        number \f${\cal{X}}\f$ and determining the particle number \f$i\f$ for which
        \f$X_{\ell,i}\leq{\cal{X}}<X_{\ell,i+1}\f$, with \f$X{\ell,i}\f$ the normalized cumulative
        luminosity matrix defined in the setup phase and stored internally. Once the SPH particle
        has been determined, a position is determined randomly from the smoothed distribution
        around the particle centre, a random propagation direction is determined, and a photon
        package with these properties is constructed and returned. */
    void launch(PhotonPackage* pp, int ell, double L) const;

    //======================== Data Members ========================

private:
    QString _filename;
    bool _writeLuminosities;

    std::vector<Vec> _rv;
    std::vector<double> _hv;
    std::vector<double> _Mv;
    std::vector<double> _Zv;
    std::vector<double> _tv;

    Array _Ltotv;
    ArrayTable<2> _Xvv;

    Random* _random;
};

////////////////////////////////////////////////////////////////////

#endif // SPHSTELLARCOMP_HPP
