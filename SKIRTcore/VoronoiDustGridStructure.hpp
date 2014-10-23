/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef VORONOIDUSTGRIDSTRUCTURE_HPP
#define VORONOIDUSTGRIDSTRUCTURE_HPP

#include "Box.hpp"
#include "GenDustGridStructure.hpp"
class DustDistribution;
class VoronoiMesh;
class VoronoiMeshFile;

//////////////////////////////////////////////////////////////////////

/** VoronoiDustGridStructure is a concrete subclass of the GenDustGridStructure class. It is used
    for representing a three-dimensional dust grid structure based on a Voronoi tesselation of the
    cuboid containing substantially all of the dust. See the VoronoiMesh class for more information
    on Voronoi tesselations.

    The class offers several options for determining the locations of the particles generating the
    Voronoi tesselation. A specified number of particles can be distributed randomly over the
    domain, either uniformly or with the same overall density distribution as the dust.
    Alternatively, the locations can be copied from the particles in an SPH dust distribution.

    This class uses the Voro++ code written by Chris H. Rycroft (LBL / UC Berkeley) to generate
    output files for plotting the Voronoi grid.
 */
class VoronoiDustGridStructure : public GenDustGridStructure, public Box
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Voronoi dust grid structure")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the extent of the domain in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the extent of the domain in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentZ")
    Q_CLASSINFO("Title", "the extent of the domain in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "numParticles")
    Q_CLASSINFO("Title", "the number of random particles (or cells in the grid)")
    Q_CLASSINFO("MinValue", "10")
    Q_CLASSINFO("Default", "500")

    Q_CLASSINFO("Property", "distribution")
    Q_CLASSINFO("Title", "the probablity distribution for the particles")
    Q_CLASSINFO("Uniform", "uniform")
    Q_CLASSINFO("CentralPeak", "with a steep central peak")
    Q_CLASSINFO("DustDensity", "same as dust density")
    Q_CLASSINFO("DustTesselation", "copy Voronoi tesselation in dust distribution")
    Q_CLASSINFO("SPHParticles", "exact locations of SPH particles in dust distribution")
    Q_CLASSINFO("File", "particle locations in a specified data file")
    Q_CLASSINFO("Default", "DustDensity")
    Q_CLASSINFO("TrueIf", "File")

    Q_CLASSINFO("Property", "voronoiMeshFile")
    Q_CLASSINFO("Title", "the Voronoi mesh data file")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Default", "VoronoiMeshAsciiFile")
    Q_CLASSINFO("RelevantIf", "distribution")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE VoronoiDustGridStructure();

    /** The destructor cleans up all memory allocated in this class. */
    ~VoronoiDustGridStructure();

protected:
    /** This function verifies that the attributes have been appropriately set, selects the
        requested particles for generating the Voronoi tesselation, and finally constructs the
        Voronoi tesselation through an instance of the VoronoiMesh class. If requested, it also
        outputs files that can be used for plotting the grid structure.

        To generate the random particles according to the simulation's dust density distribution,
        the setup function partitions the domain using a three-dimensional cuboidal cell structure,
        called the foam. The distribution of the foam cells is determined automatically from the
        dust density distribution. The foam allows to efficiently generate random points drawn from
        this probability distribution. Once the particles have been generated, the foam is discarded.
    */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the maximum extent of the grid structure in the X direction. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the maximum extent of the grid structure in the X direction. See also xmax(). */
    Q_INVOKABLE double extentX() const;

    /** Sets the maximum extent of the grid structure in the Y direction. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the maximum extent of the grid structure in the Y direction. See also ymax(). */
    Q_INVOKABLE double extentY() const;

    /** Sets the maximum extent of the grid structure in the Z direction. */
    Q_INVOKABLE void setExtentZ(double value);

    /** Returns the maximum extent of the grid structure in the Z direction. See also zmax(). */
    Q_INVOKABLE double extentZ() const;

    /** Sets the number of random particles (or cells in the grid). */
    Q_INVOKABLE void setNumParticles(int value);

    /** Returns number of random particles (or cells in the grid). */
    Q_INVOKABLE int numParticles() const;

    /** The enumeration type indicating the probability distribution used for generating the random
        particles. */
    Q_ENUMS(Distribution)
    enum Distribution { Uniform, CentralPeak, DustDensity, DustTesselation, SPHParticles, File };

    /** Sets the enumeration value indicating the probability distribution used for generating the
        random particles. */
    Q_INVOKABLE void setDistribution(Distribution value);

    /** Returns the enumeration value indicating the probability distribution used for generating
        the random particles. */
    Q_INVOKABLE Distribution distribution() const;

    /** Sets the file containing the Voronoi particle locations in case \em distribution has the
        value \em File. */
    Q_INVOKABLE void setVoronoiMeshFile(VoronoiMeshFile* value);

    /** Returns the file containing the Voronoi particle locations in case \em distribution has the
        value \em File. */
    Q_INVOKABLE VoronoiMeshFile* voronoiMeshFile() const;

    //======================== Other Functions =======================

public:
    /** This function returns the maximum extent \f$x_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. */
    double xmax() const;

    /** This function returns the maximum extent \f$y_{\text{max}}\f$ of the grid structure in the
        \f$y\f$ direction. */
    double ymax() const;

    /** This function returns the maximum extent \f$z_{\text{max}}\f$ of the grid structure in the
        \f$z\f$ direction. */
    double zmax() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. See the VoronoiMesh class for more information. */
    int whichcell(Position bfr) const;

    /** This function returns the central location of the dust cell with cell number \f$m\f$.
        In this class the function returns the centroid of the Voronoi cell. */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. */
    Position randomPositionInCell(int m) const;

    /** This function calculates a path through the grid. The DustGridPath object passed as an
        argument specifies the starting position \f${\bf{r}}\f$ and the direction \f${\bf{k}}\f$
        for the path. The data on the calculated path are added back into the same object. See the
        VoronoiMesh class for more information. */
    void path(DustGridPath* path) const;

    //======================== Data Members ========================

private:
    // discoverable attributes (in addition to extent which is stored in inherited Box)
    int _numParticles;
    Distribution _distribution;
    VoronoiMeshFile* _meshfile;

    // data members initialized during setup
    VoronoiMesh* _mesh;
    bool _meshOwned;
};

//////////////////////////////////////////////////////////////////////

#endif // VORONOIDUSTGRIDSTRUCTURE_HPP
