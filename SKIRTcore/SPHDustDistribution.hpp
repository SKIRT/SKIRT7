/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHDUSTDISTRIBUTION_HPP
#define SPHDUSTDISTRIBUTION_HPP

#include <vector>
#include "Array.hpp"
#include "DustDistribution.hpp"
#include "DustMassInBoxInterface.hpp"
#include "DustParticleInterface.hpp"
#include "SPHGasParticle.hpp"
class SPHGasParticleGrid;

////////////////////////////////////////////////////////////////////

/** The SPHDustDistribution represents dust distributions defined from a set of SPH gas particles,
    such as for example resulting from a cosmological simulation. The information on the SPH gas
    particles is read from a file formatted as described with the setFilename() function. */
class SPHDustDistribution : public DustDistribution, public DustMassInBoxInterface, public DustParticleInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust distribution derived from an SPH output file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the file with the SPH gas particles")

    Q_CLASSINFO("Property", "dustFraction")
    Q_CLASSINFO("Title", "the fraction of the metal content locked up in dust grains")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "0.3")

    Q_CLASSINFO("Property", "maximumTemperature")
    Q_CLASSINFO("Title", "the maximum temperature for a gas particle to contain dust")
    Q_CLASSINFO("Quantity", "temperature")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000000 K")
    Q_CLASSINFO("Default", "75000 K")

    Q_CLASSINFO("Property", "dustMix")
    Q_CLASSINFO("Title", "the dust mix describing the attributes of the dust")
    Q_CLASSINFO("Default", "InterstellarDustMix")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    Q_INVOKABLE SPHDustDistribution();

    /** The destructor deletes the data structures allocated during setup. */
    ~SPHDustDistribution();

protected:
    /** This function performs setup for the SPH dust distribution. It reads the properties for each
        of the SPH gas particles from the specified file, converting them to program units and
        storing them in the internal vectors. */
    virtual void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file containing the information on the SPH gas particles, optionally
        including an absolute or relative path. This text file should contain 6 or 7 columns of
        numbers separated by whitespace; lines starting with # are ignored. The first three columns
        are the \f$x\f$, \f$y\f$ and \f$z\f$ coordinates of the particle (in pc), the fourth column
        is the SPH smoothing length \f$h\f$ (in pc), the fifth column is the mass \f$M\f$ of the
        particle (in \f$M_\odot\f$), and the sixth column is the metallicity \f$Z\f$ of the gas
        (dimensionless fraction). The optional seventh column is the temperature of the gas (in K).
        If this value is provided and it is higher than the maximum temperature the particle is
        ignored. If the temperature value is missing, the particle is never ignored. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file containing the information on the SPH gas particles. */
    Q_INVOKABLE QString filename() const;

    /** Sets the fraction of the metals in the gas that is locked up in dust grains. */
    Q_INVOKABLE void setDustFraction(double value);

    /** Returns the fraction of the metals in the gas that is locked up in dust grains. */
    Q_INVOKABLE double dustFraction() const;

    /** Sets the maximum temperature for a gas particle to contain dust; any gas particles with a
        temperature above this value are ignored. If the temperature for a particle is not provided
        in the input file, the particle is never ignored. */
    Q_INVOKABLE void setMaximumTemperature(double value);

    /** Returns the maximum temperature for a gas particle to contain dust. */
    Q_INVOKABLE double maximumTemperature() const;

    /** Sets the DustMix instance that describes the attributes of the dust. */
    Q_INVOKABLE void setDustMix(DustMix* value);

    /** Returns the DustMix instance that describes the attributes of the dust. See also mix(). */
    Q_INVOKABLE DustMix* dustMix() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the dust distribution, which for this class is always 3
        since there are no symmetries in the geometry. */
    int dimension() const;

    /** This function returns the number of dust components that are involved in the dust
        distribution. For an SPH dust distribution this is equal to one. */
    int Ncomp() const;

    /** This function returns a pointer to the dust mixture corresponding to the \f$h\f$'th dust
        component. If \f$h\f$ is not equal to zero, an FatalError error is thrown. */
    DustMix* mix(int h) const;

    /** This function returns the mass density \f$\rho_h({\bf{r}})\f$ of the \f$h\f$'th component
        of the dust distribution at the position \f${\bf{r}}\f$. If \f$h\f$ is not equal to zero,
        a FatalError error is thrown. In the other case, the call is passed to the total density
        function. */
    double density(int h, Position bfr) const;

    /** This function returns the total mass density \f$\rho({\bf{r}})\f$ of the dust distribution
        at the position \f${\bf{r}}\f$. For an SPH dust distribution, the dust mass density is
        calculated by summing over all the particles \f[ \rho({\bf{r}}) = f_{\text{dust}} \sum_i
        Z_i\, M_i\, W(h_i,|{\bf{r}}-{\bf{r}}_i|) \f] with \f$f_{\text{dust}}\f$ the fraction of
        metals locked up in dust grains, \f$Z_i\f$ the metallicity and \f$M_i\f$ the
        (gas) mass of the \f$i\f$'th particle, \f$h_i\f$ the SPH smoothing length of the
        \f$i\f$'th particle, and \f$W(h,r)\f$ the SPH smoothing kernel. We assume a standard spline
        kernel, \f[ W(h,r) = \frac{8}{\pi\,h^3} \times \begin{cases} 1 - 6\,u^2\,(1-u) & \text{for
        }0<u<\tfrac12, \\ 2\,(1-u)^3 & \text{for }\tfrac12<u<1, \\ 0 & \text{else}. \end{cases} \f]
        with \f$u=r/h\f$. */
    double density(Position bfr) const;

    /** This function generates a random position from the dust distribution. It randomly chooses a
        particle using the normalized cumulative density distribution constructed during the setup
        phase. Then a position is determined randomly from the smoothed distribution around the
        particle center. The function assumes the scaled Gaussian smoothing kernel \f[ W(h,r) =
        \frac{a^3}{\pi^{3/2}\,h^3} \,\exp({-\frac{a^2 r^2}{h^2}}) \f] with the empirically
        determined value of \f$a=2.42\f$, which approximates the standard cubic spline kernel to
        within two percent accuracy. */
    Position generatePosition() const;

    /** This function returns the portion of the dust mass inside a given box (i.e. a cuboid lined
        up with the coordinate axes). If \f$h\f$ is not equal to zero, a FatalError error is
        thrown. In the other case, the call is passed to the total mass-in-box function. */
    double massInBox(int h, const Box& box) const;

    /** This function returns the portion of the total dust mass (i.e. for all dust components)
        inside a given box (i.e. a cuboid lined up with the coordinate axes). For an SPH dust
        distribution, the dust mass inside the box is calculated by summing over all the particles
        and integrating over the box \f[ M_{\text{box}} = f_{\text{dust}} \sum_i Z_i\, M_i
        \int_{x_\text{min}}^{x_\text{max}} \int_{y_\text{min}}^{y_\text{max}}
        \int_{z_\text{min}}^{z_\text{max}} W(h_i,|{\bf{r}}-{\bf{r}}_i|) \,\text{d}x \,\text{d}y
        \,\text{d}z\f] with \f$f_{\text{dust}}\f$ the fraction of metals locked up in dust grains,
        \f$Z_i\f$ the metallicity and \f$M_i\f$ the (gas) mass of the \f$i\f$'th particle,
        \f$h_i\f$ the SPH smoothing length of the \f$i\f$'th particle, and \f$W(h,r)\f$ the SPH
        smoothing kernel. To speed up the calculations, this function uses the scaled Gaussian
        kernel \f[ W(h,r) = \frac{a^3}{\pi^{3/2}\,h^3} \,\exp({-\frac{a^2 r^2}{h^2}}) \f] with the
        empirically determined value of \f$a=2.42\f$ to make this kernel approximate the standard
        spline kernel to within two percent accuracy. The advantage of this kernel is that the
        integration over a box can be written in terms of the error function \f[
        \int_{x_\text{min}}^{x_\text{max}} \int_{y_\text{min}}^{y_\text{max}}
        \int_{z_\text{min}}^{z_\text{max}} W(h,\sqrt{x^2+y^2+z^2}) \,\text{d}x \,\text{d}y
        \,\text{d}z = \tfrac18 \left(\text{erf}(\frac{a\,x_\text{max}}{h}) -
        (\text{erf}(\frac{a\,x_\text{min}}{h})\right) \left(\text{erf}(\frac{a\,y_\text{max}}{h}) -
        (\text{erf}(\frac{a\,y_\text{min}}{h})\right) \left(\text{erf}(\frac{a\,z_\text{max}}{h}) -
        (\text{erf}(\frac{a\,y_\text{min}}{h})\right) \f] */
    double massInBox(const Box& box) const;

    /** This function returns the total mass of the \f$h\f$'th component of the dust distribution. If
        \f$h\f$ is not equal to zero, a FatalError error is thrown. In the other case, the call is passed
        to the total mass. */
    double mass(int h) const;

    /** This function returns the total dust mass of the dust distribution. For a SPH dust
        distribution, the total dust mass is calculated as \f[ M = f_{\text{dust}} \sum_i Z_i\, M_i
        \f] with \f$f_{\text{dust}}\f$ the fraction of metals locked up in dust grains, and
        \f$Z_i\f$ the metallicity and \f$M_i\f$ the (gas) mass of the \f$i\f$'th particle. */
    double mass() const;

    /** This function returns the X-axis surface density of the dust distribution. For an SPH
        dust distribution, this integral is calculated numerically using 10000 samples along
        the X-axis. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the dust distribution. For an SPH
        dust distribution, this integral is calculated numerically using 10000 samples along
        the Y-axis. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the dust distribution. For an SPH
        dust distribution, this integral is calculated numerically using 10000 samples along
        the Z-axis. */
    double SigmaZ() const;

    /** This function implements the DustParticleInterface. It returns the number of SPH particles
        defining this dust distribution. */
    int numParticles() const;

    /** This function implements the DustParticleInterface. It returns the coordinates of the SPH
        particle with the specified zero-based index. If the index is out of range, a fatal error
        is thrown. */
    Vec particleCenter(int index) const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    QString _filename;
    double _fdust;
    double _Tmax;
    DustMix* _mix;

    // the SPH particles
    std::vector<SPHGasParticle> _pv;  // the particles in the order read from the file
    const SPHGasParticleGrid* _grid;  // a list of particles overlapping each grid cell
    Array _cumrhov;   // cumulative density distribution for particles in pv
};

////////////////////////////////////////////////////////////////////

#endif // SPHDUSTDISTRIBUTION_HPP
