/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ADAPTIVEMESHINTERFACE_HPP
#define ADAPTIVEMESHINTERFACE_HPP

class AdaptiveMesh;

////////////////////////////////////////////////////////////////////

/** AdaptiveMeshInterface is a pure interface. It provides access to the adaptive mesh
    maintained by the object that implements the interface. */
class AdaptiveMeshInterface
{
protected:
    /** The empty constructor for the interface. */
    AdaptiveMeshInterface() { }
public:
    /** The empty destructor for the interface. */
    virtual ~AdaptiveMeshInterface() { }

    /** This function must be implemented in a derived class. It returns a pointer to the
        adaptive mesh maintained by the object that implements the interface. */
    virtual AdaptiveMesh* mesh() const = 0;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHINTERFACE_HPP
