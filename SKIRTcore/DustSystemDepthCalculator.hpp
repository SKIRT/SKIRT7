/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTSYSTEMDEPTHCALCULATOR_HPP
#define DUSTSYSTEMDEPTHCALCULATOR_HPP

#include "Array.hpp"
#include "Box.hpp"
#include "DustGridPath.hpp"
#include "ParallelTarget.hpp"
class DustDistribution;
class DustGrid;
class DustSystem;
class Random;

//////////////////////////////////////////////////////////////////////

/** This is a helper class used by the DustSystem class to derive a basic quality metric for the
    dust grid optical depth. The idea is to evaluate the difference between the theoretical optical
    depth \f$\tau_t\f$ (calculated directly from dust distribution density samples along a path
    without involving the dust grid) and the grid optical depth \f$\tau_g\f$ (obtained by
    constructing a path through in the dust grid) for a large number of straight paths with
    randomly chosen end points, uniformly distributed over the dust grid volume. The class
    estimates the mean value and the standard deviation for the absolute difference
    \f$|\tau_g-\tau_t|\f$. The class is designed to perform the density sampling in parallel. */
class DustSystemDepthCalculator : public ParallelTarget
{
public:
    /** The arguments to this constructor are: the dust system object from which to obtain
        information on theoretical and gridded optical depth; the maximum loop body index plus one;
        the number of random paths to be considered in each invocation of the loop body; and the
        number of density samples to be taken along each path for the theoretical optical density
        integration. This constructor does not perform any calculations; it just copies a reference
        to the provided arguments and caches some additional information. The actual sampling
        happens in the function body() which is designed for use as the body in a parallel loop. */
    DustSystemDepthCalculator(const DustSystem* ds, int numBodies, int numSamplesPerBody, int numSamplesPerPath);

    /** This function calculates and stores information on the optical depth for \em
        numSamplesPerBody random paths in the output slot with index n, which must be in range 0
        through \em numBodies-1. The function is designed for use as the body in a parallel loop; see
        the Parallel class. You must invoke this function for all indices in the range 0 through \em
        numBodies-1 before calling the other functions in this class. */
    void body(size_t index);

    /** This function returns an estimate for the mean value of the difference
        \f$|\tau_g-\tau_t|\f$. */
    double meanDelta();

    /** This function returns an estimate for the standard deviation of the difference
        \f$|\tau_g-\tau_t|\f$. */
    double stddevDelta();

private:
    /** This private function actually calculates the results. */
    void consolidate();

private:
    // input data; initialized in constructor
    const DustSystem* _ds;
    const DustDistribution* _dd;
    const DustGrid* _grid;
    Random* _random;
    int _numBodies, _numSamplesPerBody, _numSamplesPerPath;
    Box _extent;
    double _eps;

    // optical depth sample arrays; sized to fit in constructor
    // (d=delta; tau=optical depth; 2=squared; v=vector)
    Array _dtauv, _dtau2v;

    // results; trivially initialized in constructor and actually calculated in consolidate()
    double _meanDelta, _stddevDelta;
    bool _consolidated;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTSYSTEMDEPTHCALCULATOR_HPP
