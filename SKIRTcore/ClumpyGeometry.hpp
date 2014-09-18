/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CLUMPYGEOMETRY_HPP
#define CLUMPYGEOMETRY_HPP

#include <vector>
#include "GenGeometry.hpp"
#include "Position.hpp"
#include "SmoothingKernel.hpp"

////////////////////////////////////////////////////////////////////

/** The ClumpyGeometry class is a Geometry decorator that adds clumpiness to
    any geometry. It basically assigns a fraction \f$f\f$ of the mass of the original
    geometry to compact clumps, which are distributed statistically according to the same
    distribution. The properties of a ClumpyGeometry object are a reference
    to the original Geometry object being decorated, and the characteristics that
    describe the clumpiness, i.e. the fraction \f$f\f$ of the mass locked in clumps, the total
    number \f$N\f$ of clumps, the scale radius \f$h\f$ of a single clump, and the kernel
    \f$W({\bf{r}},h)\f$ that describes the mass distribution of a single clump. If
    the original geometry is characterized by the density \f$\rho_{\text{orig}}({\bf{r}})\f$, the
    new, clumpy stellar geometry is described by \f[ \rho({\bf{r}}) = (1-f)\, \rho_{\text{orig}}
    ({\bf{r}}) + \frac{f}{N} \sum_{i=1}^N W({\bf{r}}-{\bf{r}}_i,h). \f] where \f${\bf{r}}_i\f$ is
    the location of the centre of the \f$i\f$'th clump, each of them drawn stochastically from the
    three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
    \rho_{\text{orig}}({\bf{r}})\, {\text{d}}{\bf{r}}\f$.*/
class ClumpyGeometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry that adds clumpiness to any geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to which clumpiness is added")

    Q_CLASSINFO("Property", "clumpFraction")
    Q_CLASSINFO("Title", "the fraction of the mass locked up in clumps")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")

    Q_CLASSINFO("Property", "clumpCount")
    Q_CLASSINFO("Title", "the total number of clumps")
    Q_CLASSINFO("MinValue", "1")

    Q_CLASSINFO("Property", "clumpRadius")
    Q_CLASSINFO("Title", "the scale radius of a single clump")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "cutoff")
    Q_CLASSINFO("Title", "cut off clumps at the boundary of the underlying geometry")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "kernel")
    Q_CLASSINFO("Title", "the smoothing kernel that describes the density of a single clump")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ClumpyGeometry();

protected:
    /** This function verifies the validity of the property values. */
    void setupSelfBefore();

    /** This function generates the \f$N\f$ random positions corresponding
        to the centers of the individual clumps. They are chosen as random positions
        generated from the original geometry that is being decorated. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the original geometry to which clumpiness is added
        (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the original geometry to which clumpiness is added
        (i.e. the geometry being decorated). */
    Q_INVOKABLE Geometry* geometry() const;

    /** Sets the fraction of the dust mass locked up in clumps. */
    Q_INVOKABLE void setClumpFraction(double value);

    /** Returns the fraction of the dust mass locked up in clumps. */
    Q_INVOKABLE double clumpFraction() const;

    /** Sets the total number of clumps. */
    Q_INVOKABLE void setClumpCount(int value);

    /** Returns the total number of clumps. */
    Q_INVOKABLE int clumpCount() const;

    /** Sets the scale radius of a single clump. */
    Q_INVOKABLE void setClumpRadius(double value);

    /** Returns the scale radius of a single clump. */
    Q_INVOKABLE double clumpRadius() const;

    /** Sets the flag that determines whether to cut off clumps at the boundary of the geometry
        being decorated. */
    Q_INVOKABLE void setCutoff(bool value);

    /** Returns the flag that determines whether to cut off clumps at the boundary of the geometry
        being decorated. */
    Q_INVOKABLE bool cutoff() const;

    /** Sets the smoothing kernel that describes the density of a single clump. */
    Q_INVOKABLE void setKernel(SmoothingKernel* value);

    /** Returns the smoothing kernel that describes the density of a single clump. */
    Q_INVOKABLE SmoothingKernel* kernel() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random
        point from the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. */
    Position generatePosition() const;

    /** This pure virtual function returns the X-axis surface density. It simply passes on the
        value returned by the geometry being decorated. */
    double SigmaX() const;

    /** This pure virtual function returns the Y-axis surface density. It simply passes on the
        value returned by the geometry being decorated. */
    double SigmaY() const;

    /** This pure virtual function returns the Z-axis surface density. It simply passes on the
        value returned by the geometry being decorated. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    Geometry* _geometry;
    double _f;    // the fraction of the total mass locked up in clumps
    int _N;       // the number of clumps
    double _h;    // the scale radius of a single clump
    bool _cutoff; // cut off clumps at the boundary of the underlying geometry?
    SmoothingKernel* _kernel;

    // data members initialized during setup
    std::vector<Vec> _clumpv;
};

////////////////////////////////////////////////////////////////////

#endif // CLUMPYGEOMETRY_HPP
