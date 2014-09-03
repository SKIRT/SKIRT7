/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTMASSINBOXINTERFACE_HPP
#define DUSTMASSINBOXINTERFACE_HPP

class Box;

////////////////////////////////////////////////////////////////////

/** DustMassInBoxInterface is a pure interface. It is implemented by dust distributions that are
    capable of quickly calculating the dust mass contained in given box (i.e. a cuboid lined up
    with the coordinate axes). This can dramatically enhance performance compared to sampling the
    density through random points. */
class DustMassInBoxInterface
{
protected:
    /** The empty constructor for the interface. */
    DustMassInBoxInterface() { }

public:
    /** The empty destructor for the interface. */
    virtual ~DustMassInBoxInterface() { }

    /** This function returns the portion of the dust mass for the dust component \em h inside a
        given box (i.e. a cuboid lined up with the coordinate axes). */
    virtual double massInBox(int h, const Box& box) const = 0;

    /** This function returns the portion of the total dust mass (i.e. for all dust components)
        inside a given box (i.e. a cuboid lined up with the coordinate axes). */
    virtual double massInBox(const Box& box) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

#endif // DUSTMASSINBOXINTERFACE_HPP
