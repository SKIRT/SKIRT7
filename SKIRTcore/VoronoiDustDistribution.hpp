/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef VORONOIDUSTDISTRIBUTION_HPP
#define VORONOIDUSTDISTRIBUTION_HPP

#include "Array.hpp"
#include "DustDistribution.hpp"
#include "DustParticleInterface.hpp"
#include "VoronoiMeshInterface.hpp"
class MeshDustComponent;
class VoronoiMeshFile;

////////////////////////////////////////////////////////////////////

/** The VoronoiDustDistribution class represents a dust distribution imported from a Voronoi
    mesh data file. The data file must have one of the supported formats; refer to the
    VoronoiMeshFile class and its subclasses. Since the Voronoi mesh data format does not specify
    the size of the domain, this information must be provided as properties of this class. The
    domain size is assumed to be symmetrical relative to the origin. This class supports multiple
    dust components, as long as the dust density distributions for all components are defined on
    the same mesh in the same Voronoi mesh data file. Each dust component is represented by an
    instance of the MeshDustComponent class, which specifies the data column index defining
    the dust density distribution for the component and the corresponding dust mix. */
class VoronoiDustDistribution : public DustDistribution, public VoronoiMeshInterface, public DustParticleInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust distribution imported from a Voronoi mesh data file")

    Q_CLASSINFO("Property", "voronoiMeshFile")
    Q_CLASSINFO("Title", "the Voronoi mesh data file")
    Q_CLASSINFO("Default", "VoronoiMeshAsciiFile")

    Q_CLASSINFO("Property", "densityUnits")
    Q_CLASSINFO("Title", "the units in which the file specifies density values")
    Q_CLASSINFO("Quantity", "massvolumedensity")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "1 Msun/pc3")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the outer radius of the domain in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the outer radius of the domain in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentZ")
    Q_CLASSINFO("Title", "the outer radius of the domain in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "components")
    Q_CLASSINFO("Title", "the dust components")
    Q_CLASSINFO("Default", "MeshDustComponent")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    Q_INVOKABLE VoronoiDustDistribution();

    /** The destructor deletes the data structures allocated during setup. */
    ~VoronoiDustDistribution();

protected:
    /** This function verifies the property values. */
    virtual void setupSelfBefore();

    /** This function imports the Voronoi mesh data (we need to know the number of required data
        fields, so our dust components must already have been setup). */
    virtual void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the file containing the Voronoi mesh data that defines this dust distribution. */
    Q_INVOKABLE void setVoronoiMeshFile(VoronoiMeshFile* value);

    /** Returns the file containing the Voronoi mesh data that defines this dust distribution. */
    Q_INVOKABLE VoronoiMeshFile* voronoiMeshFile() const;

    /** Sets the units in which the file specifies density values. */
    Q_INVOKABLE void setDensityUnits(double value);

    /** Returns the units in which the file specifies density values. */
    Q_INVOKABLE double densityUnits() const;

    /** Sets the outer radius of the domain in the X direction. The total size of the domain in
        this direction is twice as large. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the outer radius of the domain in the X direction. */
    Q_INVOKABLE double extentX() const;

    /** Sets the outer radius of the domain in the Y direction. The total size of the domain in
        this direction is twice as large. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the outer radius of the domain in the Y direction. */
    Q_INVOKABLE double extentY() const;

    /** Sets the outer radius of the domain in the Z direction. The total size of the domain in
        this direction is twice as large. */
    Q_INVOKABLE void setExtentZ(double value);

    /** Returns the outer radius of the domain in the Z direction. */
    Q_INVOKABLE double extentZ() const;

    /** This function inserts a dust component into the distribution at the specified index. */
    Q_INVOKABLE void insertComponent(int index, MeshDustComponent* value);

    /** This function removes the dust component with the specified index from the distribution. */
    Q_INVOKABLE void removeComponent(int index);

    /** This function returns the list of dust components in the distribution. */
    Q_INVOKABLE QList<MeshDustComponent*> components() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the dust distribution, which for this class is
        always 3 since there are no symmetries in the geometry. */
    int dimension() const;

    /** This function returns the number of dust components that are involved in the dust
        distribution. */
    int Ncomp() const;

    /** This function returns a pointer to the dust mixture corresponding to the \f$h\f$'th dust
        component. */
    DustMix* mix(int h) const;

    /** This function returns the mass density \f$\rho_h({\bf{r}})\f$ of the \f$h\f$'th component
        of the dust distribution at the position \f${\bf{r}}\f$. */
    double density(int h, Position bfr) const;

    /** This function returns the total mass density \f$\rho({\bf{r}})\f$ of the dust distribution
        at the position \f${\bf{r}}\f$. For this type of dust distribution, it just sums the
        contribution of the different components. */
    double density(Position bfr) const;

    /** This function generates a random position from the dust distribution. It randomly chooses a
        mesh cell from the normalized cumulative density distribution constructed during the setup
        phase. Then a position is determined randomly within the cell boundaries. */
    Position generatePosition() const;

    /** This function returns the total dust mass of the dust distribution. For this type of dust
        distribution, it just sums the contribution of the different components. */
    double mass() const;

    /** This function returns the X-axis surface density of the dust distribution, defined as the
        mass density integrated along the entire X-axis, \f[ \Sigma_X =
        \int_{-\infty}^\infty \rho(x,0,0)\, {\text{d}}x.\f] For this type of dust distribution, it
        just sums the contribution of the different components. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the dust distribution, defined as the
        mass density integrated along the entire Y-axis, \f[ \Sigma_Y =
        \int_{-\infty}^\infty \rho(0,y,0)\, {\text{d}}y.\f] For this type of dust distribution, it
        just sums the contribution of the different components. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the dust distribution, defined as the
        mass density integrated along the entire Z-axis, \f[ \Sigma_Z =
        \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z.\f] For this type of dust distribution, it
        just sums the contribution of the different components. */
    double SigmaZ() const;

    /** This function implements the VoronoiMeshInterface interface. It returns a pointer to the
        Voronoi mesh maintained by this dust distribution. */
    VoronoiMesh* mesh() const;

    /** This function implements (part of) the DustParticleInterface interface. It returns the
        number of particles defining the dust distribution. */
    int numParticles() const;

    /** This function implements (part of) the DustParticleInterface interface. It returns the
        coordinates of the dust-distribution-defining particle with the specified zero-based index.
        If the index is out of range, a fatal error is thrown. */
    Vec particleCenter(int index) const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    VoronoiMeshFile* _meshfile;
    double _densityUnits;
    double _xmax;
    double _ymax;
    double _zmax;
    QList<MeshDustComponent*> _dcv;

    // other data members
    VoronoiMesh* _mesh;
    Array _cumrhov;
};

////////////////////////////////////////////////////////////////////

#endif // VORONOIDUSTDISTRIBUTION_HPP
