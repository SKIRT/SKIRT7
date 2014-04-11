/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "Random.hpp"
#include "SepAxGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SepAxGeometry::SepAxGeometry()
{
}

//////////////////////////////////////////////////////////////////////

Position
SepAxGeometry::generatePosition()
const
{
    // generate the random numbers in separate statements to guarantee evaluation order
    // (function arguments are evaluated in different order depending on the compiler)
    double R = randomR();
    double phi = 2.0*M_PI*_random->uniform();
    double z = randomz();
    return Position(R, phi, z, Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////
