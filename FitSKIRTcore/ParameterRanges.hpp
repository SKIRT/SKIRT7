/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PARAMETERRANGES_HPP
#define PARAMETERRANGES_HPP

#include "SimulationItem.hpp"

class ParameterRange;

////////////////////////////////////////////////////////////////////

/** The ParameterRanges class represents a complete set of parameter ranges.
    Objects of this class are essentially lists of pointers to ParameterRange
    objects. */
class ParameterRanges: public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a list of parameter ranges")

    Q_CLASSINFO("Property", "ranges")
    Q_CLASSINFO("Title", "the parameter ranges")
    Q_CLASSINFO("Default", "ParameterRange")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor; creates an empty ParameterRanges object. */
    Q_INVOKABLE ParameterRanges();

protected:
    /** This function verifies that at least one component has been added. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function inserts a parameter range into the system at the specified index. */
    Q_INVOKABLE void insertRange(int index, ParameterRange* value);

    /** This function removes the parameter range with the specified index from the system. */
    Q_INVOKABLE void removeRange(int index);

    /** This function returns the list of parameter ranges in the system. */
    Q_INVOKABLE QList<ParameterRange*> ranges() const;

    //======================== Other Functions =======================
public:
    /** returns the minimal bound of the parameter at index  \em pri. */
    double minRange(int pri) const;

    /** returns the maximal bound of the parameter at index  \em pri. */
    double maxRange(int pri) const;

    /** This function returns the label of the parameter range at index \em pri. */
    QString label(int pri) const;

    //======================== Data Members ========================

private:
    QList<ParameterRange*> _prs;
};

////////////////////////////////////////////////////////////////////

#endif // ParameterRanges_HPP



