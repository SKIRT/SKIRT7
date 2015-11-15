/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SKIRTMEMORY_HPP
#define SKIRTMEMORY_HPP

////////////////////////////////////////////////////////////////////

/** The SkirtMemory main function initializes some global data structures (such as the class registry
    used by the discovery engine) and then it invokes the command line handler to perform the
    functions requested by the user. */
int main(int argc, char** argv);

////////////////////////////////////////////////////////////////////

#endif // SKIRTMEMORY_HPP
