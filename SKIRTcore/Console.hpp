/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "Log.hpp"

////////////////////////////////////////////////////////////////////

/** Console inherits from Log and implements logging to the standard console output. It also offers
    a facility to prompt the user for input; this is only used when interactively configuring a
    simulation hierarchy, and not while actually setting up and running the simulation. All Console
    instances share the same underlying console device. It is safe to mix multiple instances. For
    example, the main applicaton and the simulation hierarchy might each use a seperate Console
    instance. All functions in this class are thread-safe. */
class Console : public Log
{
    Q_OBJECT

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor performs any necessary initialization so that the Console instance
        can be used without invoking the setup() function. */
    Console();

    //======================== Other Functions =======================

protected:
    /** This function outputs a message to the console, colored according to the specified logging
        level. It overrides the pure virtual function in the base class. */
    void output(QString message, Level level);

public:
    /** This function prompts the user for an input string with the specified message. It returns
        the "simplified" user input string, i.e. white space at the start or end is removed and
        consecutive white space characters are replaced by a single space. */
    QString promptForInput(QString message);
};

////////////////////////////////////////////////////////////////////

#endif // CONSOLE_HPP
