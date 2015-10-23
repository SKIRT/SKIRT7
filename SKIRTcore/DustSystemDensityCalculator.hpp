/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTSYSTEMDENSITYCALCULATOR_HPP
#define DUSTSYSTEMDENSITYCALCULATOR_HPP

#include "Array.hpp"
#include "Box.hpp"
#include "ParallelTarget.hpp"
class DustDistribution;
class DustGrid;
class DustSystem;
class Random;

//////////////////////////////////////////////////////////////////////

/** This is a helper class used by the DustSystem class to derive a basic quality metric for the
    dust grid density. The idea is to evaluate the difference between the theoretical density
    \f$\rho_t\f$ (obtained directly from the dust distribution without involving a grid) and the
    grid density \f$\rho_g\f$ (obtained from the relevant cell in the dust grid) in a large number
    of randomly chosen points, uniformly distributed over the dust grid volume. The class estimates
    the mean value and the standard deviation for the absolute difference \f$|\rho_g-\rho_t|\f$.
    The class is designed to perform the density sampling in parallel. */
class DustSystemDensityCalculator : public ParallelTarget
{
public:
    /** The arguments to this constructor are: the dust system object from which to obtain
        theoretical and grid density information; the maximum loop body index plus one; and the
        number of density samples to be taken in each invocation of the loop body. This constructor
        does not perform any calculations; it just copies a reference to the provided arguments and
        caches some additional information. The actual density sampling happens in the function
        body() which is designed for use as the body in a parallel loop. */
    DustSystemDensityCalculator(const DustSystem* ds, int numBodies, int numSamplesPerBody);

    /** This function calculates and stores information on \em numSamplesPerBody density samples in
        the output slot with index n, which must be in range 0 through \em numBodies-1. The function
        is designed for use as the body in a parallel loop; see the Parallel class. You must invoke
        this function for all indices in the range 0 through \em numBodies-1 before calling the other
        functions in this class. */
    void body(size_t n);

    /** This function returns an estimate for the mean value of the
        difference \f$|\rho_g-\rho_t|\f$. */
    double meanDelta();

    /** This function returns an estimate for the standard deviation of the
        difference \f$|\rho_g-\rho_t|\f$. */
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
    int _numBodies;
    int _numSamplesPerBody;
    Box _extent;

    // density sample arrays; sized to fit in constructor
    // (d=delta; rho=density; 2=squared; v=vector)
    Array _drhov, _drho2v;

    // results; trivially initialized in constructor and actually calculated in consolidate()
    double _meanDelta, _stddevDelta;
    bool _consolidated;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTSYSTEMDENSITYCALCULATOR_HPP
