/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PARAMETERRANGE_HPP
#define PARAMETERRANGE_HPP

#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The ParameterRange class represents a numeric parameter range, including a label,
     the type of physical quantity, and a minimum and a maximum value. */
class ParameterRange : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a parameter range")

    Q_CLASSINFO("Property", "label")
    Q_CLASSINFO("Title", "the label identifying this parameter range")

    Q_CLASSINFO("Property", "quantityType")
    Q_CLASSINFO("Title", "the type of physical quantity represented by this parameter range")
    Q_CLASSINFO("dimless", "dimensionless")
    Q_CLASSINFO("length", "length")
    Q_CLASSINFO("distance", "distance")
    Q_CLASSINFO("mass", "mass")
    Q_CLASSINFO("posangle", "position angle")
    Q_CLASSINFO("Default", "length")

    Q_CLASSINFO("Property", "minimumValue")
    Q_CLASSINFO("Title", "the minimum value in this parameter range")
    Q_CLASSINFO("Quantity", "@quantityType")

    Q_CLASSINFO("Property", "maximumValue")
    Q_CLASSINFO("Title", "the maximum value in this parameter range")
    Q_CLASSINFO("Quantity", "@quantityType")

    //============ Construction - Setup - Destruction  =============

public:
    /** The default constructor. */
    Q_INVOKABLE ParameterRange();

    /** This function verifies that the maximum value is larger than the minimum value. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the label identifying this parameter range. */
    Q_INVOKABLE void setLabel(QString value);

    /** Returns the label identifying this parameter range. */
    Q_INVOKABLE QString label() const;

    /** The enumeration type indicating the type of physical quantity represented by this
        parameter range. Implementation note: the enumeration identifiers must match quantity
        strings defined in the Units class. */
    Q_ENUMS(PhysicalQuantity)
    enum PhysicalQuantity { dimless, length, distance, mass, posangle };

    /** Sets the enumeration value indicating the type of physical quantity represented by
        this parameter range. */
    Q_INVOKABLE void setQuantityType(PhysicalQuantity value);

    /** Returns the enumeration value indicating the type of physical quantity represented
        by this parameter range. */
    Q_INVOKABLE PhysicalQuantity quantityType() const;

    /** Sets the minimum value for this parameter range, in SI units. */
    Q_INVOKABLE void setMinimumValue(double value);

    /** Returns the minimum value for this parameter range, in SI units. */
    Q_INVOKABLE double minimumValue() const;

    /** Sets the maximum value for this parameter range, in SI units. */
    Q_INVOKABLE void setMaximumValue(double value);

    /** Returns the maximum value for this parameter range, in SI units. */
    Q_INVOKABLE double maximumValue() const;

    //====================== Other functions =======================

public:
    /** Returns a string value indicating the physical quantity represented
        by this parameter range, as used in the Units class. */
    QString quantityString() const;

    //======================== Data Members ========================

private:
    // data members
    QString _label;
    PhysicalQuantity _quantityType;
    double _minimumValue;
    double _maximumValue;
};

////////////////////////////////////////////////////////////////////

#endif // PARAMETERRANGE_HPP
