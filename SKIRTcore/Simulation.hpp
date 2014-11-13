/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "SimulationItem.hpp"
class FilePaths;
class Log;
class ParallelFactory;
class PeerToPeerCommunicator;
class Random;
class Units;

////////////////////////////////////////////////////////////////////

/** Simulation is the abstract base class for a simulation item that represents a complete
    simulation and sits at the top of a run-time simulation hierarchy (i.e. it has no parent). A
    Simulation instance holds basic attributes including a logging mechanism, a parallel execution
    instance, a random number generator, and a system of units. The constructor provides useful
    defaults for all of these attributes. This is an exception to the rule that all attributes in
    the simulation hierarchy must be explicitly set by the caller before invoking setup(). */
class Simulation : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the simulation")

    // Note: the filePaths, log and parallel properties are not declared because (in the current implementation)
    // they are initialized based on command-line options, not from the ski file

    Q_CLASSINFO("Property", "random")
    Q_CLASSINFO("Title", "the random number generator")
    Q_CLASSINFO("Default", "Random")

    Q_CLASSINFO("Property", "units")
    Q_CLASSINFO("Title", "the units system")
    Q_CLASSINFO("Default", "ExtragalacticUnits")

    //======== Construction - Setup - Run - Destruction  ===========

protected:
    /** The default constructor; it is protected since this is an abstract class. The constructor
        initializes the non-discoverable attributes and provides useful defaults for all
        discoverable attributes defined in this class. This is an exception to the rule that all
        attributes in the simulation hierarchy must be explicitly set before invoking setup().
        Specifically, the \em filePaths attribute is set to an instance of the FilePaths class with
        default paths and no filename prefix; the \em log attribute is set to an instance of the
        Console class; the \em parallelFactory attribute is set to an instance of the
        ParallelFactory class with the default maximum number of parallel threads; the \em
        communicator attribute is set to an instance of the PeerToPeerCommunicator class; the \em
        random attribute is set to an instance of the Random class, and the \em units attribute is
        set to an instance of the SIUnits class. */
    Simulation();

public:
    /** This function performs setup for the complete simulation hierarchy. It invokes the setup()
        function defined in the SimulationItem base class, surrounded by start/finish log messages.
        It is recommended to use the setupAndRun() function rather than setup() and run() separately.
        */
    void setup();

    /** This function performs the simulation by invoking the runSelf() function to be defined in a
        subclass, surrounded by start/finish log messages. The setup() function must have been called
        before invoking run(). It is recommended to use the setupAndRun() function rather than
        setup() and run() separately. */
    void run();

    /** This function performs setup and executes the simulation by invoking setup() and run() in
        succession. */
    void setupAndRun();

protected:
    /** This function actually runs the simulation, assuming that setup() has been already performed.
        Its implementation must be provided by a subclass. */
    virtual void runSelf() = 0;

    //======== Getters for Non-Discoverable Attributes =======

public:
    /** Returns the input/output file paths object for this simulation hierarchy. */
    FilePaths* filePaths() const;

    /** Returns the logging mechanism for this simulation hierarchy. */
    Log* log() const;

    /** Returns the logging mechanism for this simulation hierarchy. */
    ParallelFactory* parallelFactory() const;

    /** Returns the PeerToPeerCommunicator of the simulation. */
    PeerToPeerCommunicator* communicator() const;

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the random number generator for this simulation hierarchy. By default, an instance of
        the Random class is used with the default seed. */
    Q_INVOKABLE void setRandom(Random* value);

    /** Returns the random number generator for this simulation hierarchy. */
    Q_INVOKABLE Random* random() const;

    /** Sets the units system for this simulation hierarchy. By default, an instance of the SIUnits
        class is used. */
    Q_INVOKABLE void setUnits(Units* value);

    /** Returns the units system for this simulation hierarchy. */
    Q_INVOKABLE Units* units() const;

    //======================== Data Members ========================

protected:
    // data members
    FilePaths* _paths;                  // the file paths object for the simulation
    Log* _log;                          // the logging mechanism for the simulation
    ParallelFactory* _parfac;           // the parallel factory for the simulation
    PeerToPeerCommunicator* _comm;      // the peer-to-peer communicator for the simulation
    Random* _random;                    // the random number generator for the simulation
    Units* _units;                      // the units system for the simulation
};

////////////////////////////////////////////////////////////////////

#endif // SIMULATION_HPP
