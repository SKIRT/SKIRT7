/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef LATEXHIERARCHYWRITER_HPP
#define LATEXHIERARCHYWRITER_HPP

#include <QFile>
#include <QHash>
#include <QTextStream>
#include "PropertyHandlerVisitor.hpp"
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This class writes the properties of a simulation hierarchy to a LaTeX file so that it can be
    easily typeset. It inherits from PropertyHandlerVisitor to facilitate handling of the various
    types of properties. */
class LatexHierarchyWriter : public PropertyHandlerVisitor
{
public:
    /** Constructs a LatexHierarchyWriter instance without doing anything; call the
        writeHierarchy() function to actually write a simulation hierarchy to a file. */
    LatexHierarchyWriter();

    /** Writes the structure and properties of the specified simulation hierarchy to a LaTeX file
        with the specified name (which may include a relative or absolute path). If an error
        occurs, the function throws a fatal error. */
    void writeHierarchy(SimulationItem* item, QString filename);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(BoolPropertyHandler* handler);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(IntPropertyHandler* handler);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(DoublePropertyHandler* handler);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(DoubleListPropertyHandler* handler);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(StringPropertyHandler* handler);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(EnumPropertyHandler* handler);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(ItemPropertyHandler* handler);

    /** Writes appropriate LaTeX information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(ItemListPropertyHandler* handler);

private:
    /** Recursively writes the properties of the specified simulation item and its children. This
        is accomplished by asking each of the properties to accept "this" PropertyHandlerVisitor
        instance as a visitor, which causes a call-back to the visitPropertyHandler() function with
        the appropriate PropertyHandler subtype. */
    void writeProperties(SimulationItem* item);

    /** Outputs an amount of whitespace corresponding to the current indentation level. */
    void writeIndent();

    /** TeX-ifies a regular text string by replacing certain characters by tex commands. */
    QString texString(QString string);

    /** Converts the first character of the string to uppercase, and then TeX-ifies the result with
        texString(). */
    QString texStrUpp(QString string);

    /** TeX-ifies a string formatted as "number unit" by replacing the exponent and some of the
        unit strings by tex commands. */
    QString texDouble(QString string);

    /** TeX-ifies a string containing a comma-separated list of doubles, each formatted as
        described for texDouble(). */
    QString texDoubleList(QString string);

    // output file and stream
    QFile _file;
    QTextStream _out;

    // the current indentation level
    int _indent;

    // searchable copy of constant string pairs
    QHash<QString,QString> _replstr;
    QHash<QString,QString> _repluni;
};

////////////////////////////////////////////////////////////////////

#endif // LATEXHIERARCHYWRITER_HPP
