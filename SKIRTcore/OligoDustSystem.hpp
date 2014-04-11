/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef OLIGODUSTSYSTEM_HPP
#define OLIGODUSTSYSTEM_HPP

#include "DustSystem.hpp"

//////////////////////////////////////////////////////////////////////

/** An OligoDustSystem class object represents a complete dust system for use with oligochromatic
    simulations. All functionality is implemented in the DustSystem base class, except for the
    factory method creating a dust cell of the appropriate OligoDustCell type. */
class OligoDustSystem : public DustSystem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust system for use with oligochromatic simulations")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE OligoDustSystem();

protected:
    /** This factory function creates and returns a pointer to a new dust cell of the appropriate
        OligoDustCell type. */
    virtual DustCell* createDustCell();
};

//////////////////////////////////////////////////////////////////////

#endif // OLIGODUSTSYSTEM_HPP
