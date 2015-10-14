/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TREENODEBOXDENSITYCALCULATOR_HPP
#define TREENODEBOXDENSITYCALCULATOR_HPP

#include "Box.hpp"
#include "TreeNodeDensityCalculator.hpp"
class DustMassInBoxInterface;
class TreeNode;

//////////////////////////////////////////////////////////////////////

/** This is a helper class used by the TreeDustGrid and TreeNode classes. It calculates
    properties such as total mass and optical depth for the dust cell corresponding to a given tree
    node using the DustMassInBoxInterface interface. This class does \em not support calculation of
    the barycenter and density dispersion. */
class TreeNodeBoxDensityCalculator : public TreeNodeDensityCalculator
{
public:
    /** The arguments to this constructor are a DustMassInBoxInterface object and the tree node for
        which to calculate the density-related properties. This constructor does not perform any
        calculations; it just copies a reference to the provided arguments and caches some
        additional information. */
    TreeNodeBoxDensityCalculator(DustMassInBoxInterface* dmib, TreeNode* node);

    /** This function returns the volume of the cell. */
    double volume() const;

    /** This function returns the dust mass in the cell. */
    double mass() const;

    /** This function throws a fatal error since the barycenter can't be calculated. */
    Vec barycenter() const;

    /** This function returns the optical depth of the cell. */
    double opticalDepth() const;

    /** This function throws a fatal error since the density dispersion can't be calculated. */
    double densityDispersion() const;

private:
    // input data; initialized in constructor
    DustMassInBoxInterface* _dmib;
    Box _extent;

    // cached data to avoid recalculation
    double _mass;
};

//////////////////////////////////////////////////////////////////////

#endif // TREENODEBOXDENSITYCALCULATOR_HPP
