/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef REGISTERSIMULATIONITEMS_HPP
#define REGISTERSIMULATIONITEMS_HPP

////////////////////////////////////////////////////////////////////

/** This namespace offers a single function to add all discoverable simulation item classes to the
    simulation item registry. Putting this in a seperate file makes it easy for a developer to add
    new classes without having to wade through a lot of code. */
namespace RegisterSimulationItems
{
    /** Adds all discoverable simulation item classes to the simulation item registry. The
        application should call this function from the main thread before any of the discovery
        functionality is used. */
    void registerAll();
}

////////////////////////////////////////////////////////////////////

#endif // REGISTERSIMULATIONITEMS_HPP
