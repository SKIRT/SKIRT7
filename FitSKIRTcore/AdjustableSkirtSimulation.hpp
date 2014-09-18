/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ADJUSTABLESKIRTSIMULATION_HPP
#define ADJUSTABLESKIRTSIMULATION_HPP

#include <QHash>
#include <QPair>
#include "SimulationItem.hpp"

class Units;

////////////////////////////////////////////////////////////////////

/** The AdjustableSkirtSimulation class allows performing a SKIRT simulation loaded from a ski
    file. The contents of the ski file can be adjusted before the simulation hierarchy is
    actually created, as described for the performWith() function. */
class AdjustableSkirtSimulation : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an adjustable SKIRT simulation")

    Q_CLASSINFO("Property", "skiName")
    Q_CLASSINFO("Title", "the name of the ski file specifying the SKIRT simulation")

    //======== Construction - Setup - Run - Destruction  ===========

public:
    /** The default constructor. */
    Q_INVOKABLE AdjustableSkirtSimulation();

    /** The destructor deletes the simulation items stolen from the default simulation. */
    ~AdjustableSkirtSimulation();

protected:
    /** This function reads the specified ski file into memory, performs a single simulation
        using the default values provided in the ski file, and outputs an xml and tex version
        of this default ski file. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the ski file (with or without the ".ski" filename extension) specifying
        the SKIRT simulation. */
    Q_INVOKABLE void setSkiName(QString value);

    /** Returns the name of the ski file (with or without the ".ski" filename extension) specifying
        the SKIRT simulation. */
    Q_INVOKABLE QString skiName() const;

    //====================== Other functions =======================

public:
    /** A shorthand type definition for a condition dictionary as passed to performWith(). */
    typedef QHash<QString, bool> ConditionDict;

    /** A shorthand type definition for a replacement dictionary as passed to performWith(). */
    typedef QHash<QString, QPair<double,QString> > ReplacementDict;

    /** This function returns the wavelength of frame at positon \em ind. */
    double wavelength(int ind) const;

    /** This function returns the x increment of frame at positon \em ind. */
    double xpress(int ind) const;

    /** This function returns the y increment of frame at positon \em ind. */
    double ypress(int ind) const;

/**
This function runs the SKIRT simulation specified by the previously loaded ski file after
adjusting its contents as specified through the \em conditions and \em replacements
dictionary arguments (see detailed description below). The prefix string, if present,
is appended to the filename prefix for all output files of this simulation run.

The \em conditions dictionary contains a set of key/value pairs controlling inclusion or
exclusion of certain marked sections in the ski file. The key for each dictionary item
is a string matching a section label in the ski file, as explained below. The value for
each dictionary item is boolean \c true (to indicate inclusion of the section) or \c
false (to indicate exclusion of the section).

The \em replacements dictionary contains a set of key/value pairs controlling
replacement of labeled attribute values in the ski file. The key for each dictionary item
is a string matching an attribute label in the ski file, as explained below. The value for
each dictionary item is in fact a pair of values: the numeric replacement value in SI units,
and a physical quantity specifier such as "length" (or the empty string if the value is
dimensionless).

To mark a section of the ski file for conditional inclusion, enclose the section in XML
comments including curly brackets as in the following example:

\verbatim
  <!--{bulge:-->
    <OligoStellarComp luminosities="1e15, 1e15, 1e15">
      ...
    </OligoStellarComp>
  <!--}-->
\endverbatim

The section start and end markers must be written exactly as shown, embedded in XML comments,
with the section label immediately following the left curly bracket and immediately preceding
a colon. The section label must start with a letter and can contain only letters, digits and
underscore characters. Nested sections are not allowed.

If the section label matches one of the keys in the condition dictionary handed to this function,
and the corresponding value is \c false, this function removes the section from the XML file.
If the corresponding value is \c true, or if the label does not match a key in the dictionary,
then the section is preserved (i.e. the default is to leave the section in).

To mark a numeric attribute value for replacement in the ski file, enclose the
value in square brackets and provide a label, as in the following example:

\verbatim
  <SersicGeometry index="3" radius="[stellar_scale:1500 pc]"/>
\endverbatim

The brackets must be just within the quotes delimiting the attribute value. The label must
start with a letter and contain only letters, digits and underscores. It must be
immediately followed by a colon and then the regular attribute value, possibly including a
unit specifier. Spaces are not allowed except between the value and the unit specifier,
where a single space is required.

This function replaces each labeled attribute value by a regular value (i.e. without the
brackets and the label). If the label matches one of the keys in the replacement dictionary
handed to this function, the corresponding value is substituted in the ski file. If there
is no match, the value provided in the ski file (after the colon) serves as a default.
*/
    void performWith(ConditionDict conditions, ReplacementDict replacements, QString prefix=QString());

private:
    /** This private function performs the specified adjustments on the previously loaded ski content
        as described for the performWith() function, and returns the result. If the arguments are missing,
        all conditions are considered to be "true" (i.e. all conditional content is included) and all
        attributes use the default values as provided in the original ski file. */
    QByteArray adjustedSkiContent(ConditionDict conditions = ConditionDict(),
                                  ReplacementDict replacements = ReplacementDict());

    //======================== Data Members ========================

private:
    // data members
    QString _skiName;         // the name of the ski file
    QByteArray _skiContent;   // the content of the ski file, without modifications

    Units* _units;                      // the units system stolen from the default simulation hierarchy
    QList<double> _wavelengthgrid;      // the wavelengths stolen from the default simulation hierarchy
    QList<double> _xpress;              // the x increment stolen from the default simulation hierarchy
    QList<double> _ypress;              // the y increment stolen from the default simulation hierarchy

};

////////////////////////////////////////////////////////////////////

#endif // ADJUSTABLESKIRTSIMULATION_HPP
