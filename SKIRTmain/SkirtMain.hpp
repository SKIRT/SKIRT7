/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SKIRTMAIN_HPP
#define SKIRTMAIN_HPP

////////////////////////////////////////////////////////////////////

/** The SKIRT main function initializes some global data structures (such as the class registry
    used by the discovery engine) and then it invokes the command line handler to perform the
    functions requested by the user. */
int main(int argc, char** argv);

////////////////////////////////////////////////////////////////////

#endif // SKIRTMAIN_HPP
