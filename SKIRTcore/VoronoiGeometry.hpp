/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef VORONOIGEOMETRY_HPP
#define VORONOIGEOMETRY_HPP

#include "Array.hpp"
#include "DustParticleInterface.hpp"
#include "GenGeometry.hpp"
#include "VoronoiMeshInterface.hpp"
class VoronoiMeshFile;

////////////////////////////////////////////////////////////////////

/** The VoronoiGeometry class describes an arbitrary 3D geometry defined by the
    probability distribution imported from a Voronoi mesh data file. The data file must have a
    format supported by one of the VoronoiMeshFile subclasses. The VoronoiGeometry class
    allows selecting the data column containing the probability distribution, and it offers the
    option to use a second column as a multiplication factor (i.e. the probability distribution is
    then defined by the product of the two column values). The geometry will be normalized anyway
    after importing the probability distribution, so the probability distribution in the data file
    does not have to be normalized, and the units of the values in the data file are irrelevant.
    Since the Voronoi mesh data format does not specify the size of the domain, this information
    must be provided as properties of this class as well. The domain size is assumed to be
    symmetrical relative to the origin. */
class VoronoiGeometry : public GenGeometry, public VoronoiMeshInterface, public DustParticleInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry imported from a Voronoi mesh data file")

    Q_CLASSINFO("Property", "voronoiMeshFile")
    Q_CLASSINFO("Title", "the Voronoi mesh data file")
    Q_CLASSINFO("Default", "VoronoiMeshAsciiFile")

    Q_CLASSINFO("Property", "densityIndex")
    Q_CLASSINFO("Title", "the index of the column defining the density distribution")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "99")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "multiplierIndex")
    Q_CLASSINFO("Title", "the index of the column defining an extra multiplication factor, or -1")
    Q_CLASSINFO("MinValue", "-1")
    Q_CLASSINFO("MaxValue", "99")
    Q_CLASSINFO("Default", "-1")

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

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE VoronoiGeometry();

    /** The destructor releases the data structure allocated during setup. */
    ~VoronoiGeometry();

protected:
    /** This function verifies the property values, imports the Voronoi mesh data, and calculates
        the total density integrated over the complete domain. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the file containing the Voronoi mesh data that defines this geometry. */
    Q_INVOKABLE void setVoronoiMeshFile(VoronoiMeshFile* value);

    /** Returns the file containing the Voronoi mesh data that defines this geometry. */
    Q_INVOKABLE VoronoiMeshFile* voronoiMeshFile() const;

    /** Sets the index of the column in the data file that defines the density distribution
        for this geometry. The geometry will be normalized anyway after importing the probability
        distribution, so the probability distribution in the data file does not have to be
        normalized, and the units of the values in the data file are irrelevant. */
    Q_INVOKABLE void setDensityIndex(int value);

    /** Returns the index of the column in the data file that defines the density distribution
        for this geometry. */
    Q_INVOKABLE int densityIndex() const;

    /** Sets the index of the column in the data file that defines an extra multiplication factor
        for the density distribution, or -1 if there is no such multiplication factor. If this
        column index is nonnegative, the probability distribution is effectively defined by the
        product of the two column values specified by \em densityIndex and \em multiplierIndex. */
    Q_INVOKABLE void setMultiplierIndex(int value);

    /** Returns the index of the column in the data file that defines an extra multiplication
        factor for the density distribution, or -1 if there is no such multiplication factor.
        */
    Q_INVOKABLE int multiplierIndex() const;

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

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ for this geometry at the
        position \f${\bf{r}}\f$. It forwards the call to the mesh, and applies the necessary
        normalization (the density on an adaptive mesh is not necessarily normalized to one). */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density of the geometry, defined as the integration
        of the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,
        {\text{d}}x.\f] It forwards the call to the mesh, and applies the necessary
        normalization (the density on an adaptive mesh is not necessarily normalized to one). */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the geometry, defined as the integration
        of the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,
        {\text{d}}y.\f] It forwards the call to the mesh, and applies the necessary
        normalization (the density on an adaptive mesh is not necessarily normalized to one). */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the geometry, defined as the integration
        of the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z.\f] It forwards the call to the mesh, and applies the necessary
        normalization (the density on an adaptive mesh is not necessarily normalized to one). */
    double SigmaZ() const;

    /** This function implements the VoronoiMeshInterface interface. It returns a
        pointer to the Voronoi mesh maintained by this geometry. */
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
    int _densityIndex;
    int _multiplierIndex;
    double _xmax;
    double _ymax;
    double _zmax;

    // other data members
    VoronoiMesh* _mesh;
    Array _cumrhov;
};

////////////////////////////////////////////////////////////////////

#endif // VORONOIGEOMETRY_HPP
