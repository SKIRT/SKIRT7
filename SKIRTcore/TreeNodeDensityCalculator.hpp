/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TREENODEDENSITYCALCULATOR_HPP
#define TREENODEDENSITYCALCULATOR_HPP

class Vec;

//////////////////////////////////////////////////////////////////////

/** This is a pure interface used by the TreeDustGridStructure and TreeNode classes to obtain
    properties such as total mass or barycenter for the dust cell corresponding to a given tree
    node. Helper classes implementing this interface may use different strategies to calculate this
    information. */
class TreeNodeDensityCalculator
{
protected:
    /** The empty constructor for the interface. */
    TreeNodeDensityCalculator() { }

public:
    /** The empty destructor for the interface. */
    virtual ~TreeNodeDensityCalculator() { }

    /** This function returns the volume of the cell. */
    virtual double volume() const = 0;

    /** This function returns the dust mass contained in the cell. */
    virtual double mass() const = 0;

    /** This function returns the barycenter of the cell. */
    virtual Vec barycenter() const = 0;

    /** This function returns the optical depth of the cell. */
    virtual double opticalDepth() const = 0;

    /** This function returns the density dispersion in the cell. */
    virtual double densityDispersion() const = 0;
};

//////////////////////////////////////////////////////////////////////

#endif // TREENODEDENSITYCALCULATOR_HPP
