/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef REGISTERFITSCHEMEITEMS_HPP
#define REGISTERFITSCHEMEITEMS_HPP

////////////////////////////////////////////////////////////////////

/** This namespace offers a single function to add all discoverable fit scheme classes to the
    simulation item registry. Putting this in a seperate file makes it easy for a developer to add
    new classes without having to wade through a lot of code. */
namespace RegisterFitSchemeItems
{
    /** Adds all discoverable fit scheme classes to the simulation item registry. The
        application should call this function from the main thread before any of the discovery
        functionality is used. */
    void registerAll();
}

////////////////////////////////////////////////////////////////////

#endif // REGISTERFITSCHEMEITEMS_HPP
