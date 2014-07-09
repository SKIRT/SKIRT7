/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TREENODESAMPLEDENSITYCALCULATOR_HPP
#define TREENODESAMPLEDENSITYCALCULATOR_HPP

#include <vector>
#include "Array.hpp"
#include "Box.hpp"
#include "ParallelTarget.hpp"
#include "TreeNodeDensityCalculator.hpp"
class DustDistribution;
class Position;
class Random;
class TreeNode;

//////////////////////////////////////////////////////////////////////

/** This is a helper class used by the TreeDustGridStructure and TreeNode classes. It estimates
    properties such as total mass or barycenter for the dust cell corresponding to a given tree
    node, by sampling the density of the dust distribution in a number of randomly chosen points.
    The class is designed to perform the density sampling in parallel (the density calculation for
    each point can run in a seperate thread). */
class TreeNodeSampleDensityCalculator : public TreeNodeDensityCalculator, public ParallelTarget
{
public:
    /** The arguments to this constructor are: the simulation's random generator; the number of
        density samples to be taken, the dust distribution object from which to obtain the dust
        density information, and the tree node for which to calculate the density-related
        properties. This constructor does not perform any calculations; it just copies a reference
        to the provided arguments and caches some additional information. The actual density
        sampling happens in the function body() which is designed for use as the body in a parallel
        loop. */
    TreeNodeSampleDensityCalculator(Random* random, int Nrandom, DustDistribution* dd, TreeNode* node);

    /** This function calculates and stores the density in the random point with index n. The
        function is designed for use as the body in a parallel loop; see the Parallel class. You
        must invoke this function for all indices in the sample range 0 before calling most of
        the other functions in this class. */
    void body(size_t n);

    /** This function calculates and returns the volume of the cell. */
    double volume() const;

    /** This function calculates and returns an estimate for the mass in the cell. The density
        samples must already have been calculated using the body() function. */
    double mass() const;

    /** This function calculates and returns an estimate for the barycenter of the cell. The
        density samples must already have been calculated using the body() function. */
    Vec barycenter() const;

    /** This function calculates and returns an estimate for the optical depth of the cell. The
        density samples must already have been calculated using the body() function. */
    double opticalDepth() const;

    /** This function calculates and returns an estimate for the density dispersion in the cell.
        The density samples must already have been calculated using the body() function. */
    double densityDispersion() const;

private:
    // density sample arrays; sized to fit in constructor
    std::vector<Position> _rv;
    Array _rhov;

    // input data; initialized in constructor
    Box _extent;
    int _Nrandom;
    Random* _random;
    DustDistribution* _dd;
};

//////////////////////////////////////////////////////////////////////

#endif // TREENODESAMPLEDENSITYCALCULATOR_HPP
