/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FITSCHEME_HPP
#define FITSCHEME_HPP

#include "SimulationItem.hpp"
class FilePaths;
class Log;
class MasterSlaveCommunicator;
class Units;

////////////////////////////////////////////////////////////////////

/** FitScheme is the abstract base class that represents a complete FitSKIRT fit scheme. The class
    inherits SimulationItem so that all functionality built for SKIRT simulation hierarchies
    (including the discovery mechanism) can also be used for fit schemes. A FitScheme instance sits
    at the top of a run-time fit scheme hierarchy (i.e. it has no parent). It holds basic
    attributes including a logging mechanism and a system of units. The constructor provides useful
    defaults for all of these attributes. This is an exception to the rule that all attributes in
    the fit scheme hierarchy must be explicitly set by the caller before invoking setup(). */
class FitScheme : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the fit scheme")

    // Note: the file paths, log and parallel properties are not declared because
    // they are initialized based on command-line options, not from the fski file

    Q_CLASSINFO("Property", "units")
    Q_CLASSINFO("Title", "the units system")
    Q_CLASSINFO("Default", "ExtragalacticUnits")

    //======== Construction - Setup - Run - Destruction  ===========

protected:
    /** The default constructor; it is protected since this is an abstract class. The constructor
        initializes the non-discoverable attributes and provides useful defaults for all
        discoverable attributes defined in this class.  This is an exception to
        the rule that all attributes in the fit scheme hierarchy must be explicitly set before
        invoking setup(). */
    FitScheme();

public:
    /** This function performs setup for the complete fit scheme hierarchy. It invokes the setup()
        function defined in the SimulationItem base class, surrounded by start/finish log messages.
        It is recommended to use the setupAndRun() function rather than setup() and run()
        separately. */
    void setup();

    /** This function performs the fit scheme by invoking the runSelf() function to be defined in a
        subclass, surrounded by start/finish log messages. The setup() function must have been
        called before invoking run(). It is recommended to use the setupAndRun() function rather
        than setup() and run() separately. */
    void run();

    /** This function performs setup and executes the fit scheme by invoking setup() and run() in
        succession. */
    void setupAndRun();

protected:
    /** This function actually runs the fit scheme, assuming that setup() has been already
        performed. Its implementation must be provided by a subclass. */
    virtual void runSelf() = 0;

    //======== Setters & Getters for Non-Discoverable Attributes =======

public:
    /** Returns the input/output file paths object for this fit scheme. */
    FilePaths* filePaths() const;

    /** Returns the logging mechanism for this fit scheme. */
    Log* log() const;

    /** Sets the number of SKIRT simulations performed in parallel by this fit scheme. By default,
        the number of parallel simulations is equal to one (i.e. simulations are serialized). */
    void setParallelSimulationCount(int value);

    /** Returns the number of SKIRT simulations performed in parallel by this fit scheme. */
    int parallelSimulationCount() const;

    /** Sets the number of parallel threads for each SKIRT simulation performed by this fit scheme.
        By default, the number of parallel threads is equal to the number of logical cores detected
        on the computer running the code. */
    void setParallelThreadCount(int value);

    /** Returns the number of parallel threads for each SKIRT simulation performed by this fit
        scheme. */
    int parallelThreadCount() const;

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the units system for this fit scheme. By default, an instance of the SIUnits class is
        used. */
    Q_INVOKABLE void setUnits(Units* value);

    /** Returns the units system for this fit scheme. */
    Q_INVOKABLE Units* units() const;

    //======================== Data Members ========================

protected:
    // data members
    FilePaths* _paths;                  // the file paths object for the fit scheme
    Log* _log;                          // the logging mechanism for the fit scheme
    MasterSlaveCommunicator* _comm;     // the mechanism to perform parallel tasks
    Units* _units;                      // the units system for the fit scheme
    int _parallelSimulations;           // the number of parallel simulations
    int _parallelThreads;               // the number of parallel theads per simulation
};

////////////////////////////////////////////////////////////////////

#endif // FITSCHEME_HPP
