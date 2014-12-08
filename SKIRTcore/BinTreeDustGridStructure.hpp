/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BINTREEDUSTGRIDSTRUCTURE_HPP
#define BINTREEDUSTGRIDSTRUCTURE_HPP

#include "TreeDustGridStructure.hpp"

//////////////////////////////////////////////////////////////////////

/** BinTreeDustGridStructure is a subclass of the TreeDustGridStructure class that
    implements an binary tree dust grid (2 children per node), which is in fact
    a 3-dimensional k-d tree. */
class BinTreeDustGridStructure : public TreeDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a k-d tree (binary tree) dust grid structure")

    Q_CLASSINFO("Property", "directionMethod")
    Q_CLASSINFO("Title", "the method determining subdivision orientation at each level")
    Q_CLASSINFO("Alternating", "alternating between x, y, and z directions")
    Q_CLASSINFO("Barycenter", "parallel to the cell wall nearest the barycenter")
    Q_CLASSINFO("Default", "Alternating")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE BinTreeDustGridStructure();

protected:
    /** This function verifies that the search method has not been set to Bookkeeping,
        since that method is not compatible with a binary tree node. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** The enumeration type indicating the method to be used for determining the orientation for
        each node subdivision. The Alternating method (the default) alternates repeatedly between
        x, y, and z directions in a consistent fashion. The Barycenter method chooses a subdividing
        plane parallel to the cell wall that is nearest the barycenter of the cell. */
    Q_ENUMS(DirectionMethod)
    enum DirectionMethod { Alternating, Barycenter };

    /** Sets the enumeration value indicating the direction method to be used for determining the
        orientation of node subdivisions. */
    Q_INVOKABLE void setDirectionMethod(DirectionMethod value);

    /** Returns the enumeration value indicating the direction method to be used for determining
        the orientation of node subdivisions. */
    Q_INVOKABLE DirectionMethod directionMethod() const;

    //======================== Other Functions =======================

protected:
    /** This function creates a root node of type BinTreeNode using a node identifier of zero
        and the specified spatial extent, and returns a pointer to it. The caller must take
        ownership of the newly created object. */
    TreeNode* createRoot(const Box& extent);

    //======================== Data Members ========================

private:
    // discoverable attributes
    DirectionMethod _directionMethod;
};

//////////////////////////////////////////////////////////////////////

#endif // BINTREEDUSTGRIDSTRUCTURE_HPP
