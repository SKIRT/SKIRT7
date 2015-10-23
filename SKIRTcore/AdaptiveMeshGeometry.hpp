/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ADAPTIVEMESHGEOMETRY_HPP
#define ADAPTIVEMESHGEOMETRY_HPP

#include "AdaptiveMeshInterface.hpp"
#include "Array.hpp"
#include "BoxGeometry.hpp"
class AdaptiveMeshFile;

////////////////////////////////////////////////////////////////////

/** The AdaptiveMeshGeometry class describes an arbitrary 3D geometry defined by the
    probability distribution imported from an adaptive mesh data file. The data file must have a
    format supported by one of the AdaptiveMeshFile subclasses. The AdaptiveMeshGeometry class
    allows selecting the data column containing the probability distribution, and it offers the
    option to use a second column as a multiplication factor (i.e. the probability distribution is
    then defined by the product of the two column values). The geometry will be normalized anyway
    after importing the probability distribution, so the probability distribution in the data file
    does not have to be normalized, and the units of the values in the data file are irrelevant.
    Since the adaptive mesh data format does not specify the size of the domain, this information
    must be provided as properties of this class as well. */
class AdaptiveMeshGeometry : public BoxGeometry, public AdaptiveMeshInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry imported from an adaptive mesh data file")

    Q_CLASSINFO("Property", "adaptiveMeshFile")
    Q_CLASSINFO("Title", "the adaptive mesh data file")
    Q_CLASSINFO("Default", "AdaptiveMeshAsciiFile")

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

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE AdaptiveMeshGeometry();

    /** The destructor releases the data structure allocated during setup. */
    ~AdaptiveMeshGeometry();

protected:
    /** This function verifies the property values, imports the adaptive mesh data, and calculates
        the total density integrated over the complete domain. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the file containing the adaptive mesh data that defines this geometry. */
    Q_INVOKABLE void setAdaptiveMeshFile(AdaptiveMeshFile* value);

    /** Returns the file containing the adaptive mesh data that defines this geometry. */
    Q_INVOKABLE AdaptiveMeshFile* adaptiveMeshFile() const;

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

    /** This function implements the AdaptiveMeshInterface interface. It returns a
        pointer to the adaptive mesh maintained by this geometry. */
    AdaptiveMesh* mesh() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    AdaptiveMeshFile* _meshfile;
    int _densityIndex;
    int _multiplierIndex;

    // other data members
    AdaptiveMesh* _mesh;
    Array _cumrhov;
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHGEOMETRY_HPP
