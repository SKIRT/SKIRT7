/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QCoreApplication>
#include <QDateTime>
#include "BoolPropertyHandler.hpp"
#include "DoublePropertyHandler.hpp"
#include "DoubleListPropertyHandler.hpp"
#include "EnumPropertyHandler.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "IntPropertyHandler.hpp"
#include "ItemListPropertyHandler.hpp"
#include "ItemPropertyHandler.hpp"
#include "LatexHierarchyWriter.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "StringPropertyHandler.hpp"

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

namespace
{
    // replacement pairs for characters in regular strings (just a small and arbitrary selection)
    const char* _Creplstr[][2] =
    {
        // latex escapes
        {"\\","\\textbackslash{}"},{"~","{\\raise.17ex\\hbox{$\\scriptstyle\\sim$}}"},
        {"#","\\#"},{"$","\\$"},{"%","\\%"},{"&","\\&"},{"_","\\_"},{"{","\\{"},{"}","\\}"},
        // greek
        {"α","$\\alpha$"},{"β","$\\beta$"},{"γ","$\\gamma$"},{"δ","$\\delta$"},{"ϵ","$\\epsilon$"},{"ε","$\\varepsilon$"},
        {"ζ","$\\zeta$"},{"η","$\\eta$"},{"θ","$\\theta$"},{"ϑ","$\\vartheta$"},{"κ","$\\kappa$"},{"λ","$\\lambda$"},
        {"μ","$\\mu$"},{"ν","$\\nu$"},{"ξ","$\\xi$"},{"π","$\\pi$"},{"ρ","$\\rho$"},{"σ","$\\sigma$"},{"τ","$\\tau$"},
        {"ϕ","$\\phi$"},{"φ","$\\varphi$"},{"χ","$\\chi$"},{"ψ","$\\psi$"},{"ω","$\\omega$"},
        {"Γ","$\\Gamma$"},{"Δ","$\\Delta$"},{"Θ","$\\Theta$"},{"Λ","$\\Lambda$"},{"Π","$\\Pi$"},{"Σ","$\\Sigma$"},
        {"Φ","$\\Phi$"},{"Ψ","$\\Psi$"},{"Ω","$\\Omega$"},
        // accented characters
        {"á","\\'{a}"},{"à","\\`{a}"},{"â","\\^{a}"},{"ä","\\\"{a}"},{"å","\\r{a}"},{"Å","\\r{A}"},
        {"é","\\'{e}"},{"è","\\`{e}"},{"ê","\\^{e}"},{"ë","\\\"{e}"},{"î","\\^{\\i}"},{"ï","\\\"{\\i}"},
        {"ó","\\'{o}"},{"ò","\\`{o}"},{"ô","\\^{o}"},{"ö","\\\"{o}"},
        {"ú","\\'{u}"},{"ù","\\`{u}"},{"û","\\^{u}"},{"ü","\\\"{u}"},{"ç","\\c{c}"},
        // special symbols
        {"∅","$\\emptyset$"},{"∞","$\\infty$"},{"×","$\\times$"},{"°","$^{\\circ}$"}
    };
    const int _Nreplstr = sizeof(_Creplstr) / sizeof(_Creplstr[0]);

    // replacement pairs for unit tokens
    const char* _Crepluni[][2] =
    {
        {"micron","\\mu\\textrm{m}"},{"A","\\textrm{\\r{A}}"},
        {"Lsun","\\textrm{L}_{\\odot}"},{"Msun","\\textrm{M}_{\\odot}"},
        {"deg","^{\\circ}"},{"arcsec","^{\\prime\\prime}"},{"arcsec2","(^{\\prime\\prime})^{2}"}
    };
    const int _Nrepluni = sizeof(_Crepluni) / sizeof(_Crepluni[0]);
}

////////////////////////////////////////////////////////////////////

LatexHierarchyWriter::LatexHierarchyWriter()
    : _indent(0)
{
    // copy the constant string pairs above in dictionaries for easy access
    for (int i=0; i < _Nreplstr; i++) _replstr.insert(_Creplstr[i][0], _Creplstr[i][1]);
    for (int i=0; i < _Nrepluni; i++) _repluni.insert(_Crepluni[i][0], _Crepluni[i][1]);
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::writeHierarchy(SimulationItem* item, QString filename)
{
    // open the file and setup a text stream
    _file.setFileName(filename);
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw FATALERROR("File couldn't be opened for writing LaTeX: " + filename);
    _out.setCodec("UTF-8");  // in case some unicode characters slip past our conversions to latex
    _out.setDevice(&_file);

    // write document preamble
    _out << "\\documentclass[10pt,english]{article}\n";
    _out << "\\usepackage[landscape,a4paper]{geometry}\n";
    _out << "\\geometry{verbose,tmargin=2cm,bmargin=2cm,lmargin=3cm,rmargin=2cm}\n";
    _out << "\\pagestyle{empty}\n";
    _out << "\\setlength{\\parskip}{\\bigskipamount}\n";
    _out << "\\setlength{\\parindent}{0pt}\n";
    _out << "\\usepackage{babel}\n";
    _out << "\\begin{document}\n";
    _out << "\n";

    // write header text
    _out << "\\section*{" << QCoreApplication::applicationName() << " parameter overview: "
         << texString(item->find<FilePaths>()->outputPrefix()) << "}\n";
    QDateTime stamp = QDateTime::currentDateTime();
    QLocale locale(QLocale::C);
    _out << "Generated on " << locale.toString(stamp, "MMM d yyyy") <<
            " at " << locale.toString(stamp, "hh:mm:ss") << "\\\\\n";
    _out << "\\copyright 2012-2014 Astronomical Observatory, Ghent University\\\\\n";
    _out << QCoreApplication::applicationName() << " " << QCoreApplication::applicationVersion() << "\\\\\n";
    _out << "\n";

    // write the first label/value line for the top-level item
    _out << texStrUpp(title(itemType(item))) << "\\\\\n";

    // recursively write all properties of the top-level item and its children
    _indent = 0;
    writeProperties(item);

    // write document end
    _out << "\n";
    _out << "\\end{document}\n";

    // close the file
    _file.close();
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::writeProperties(SimulationItem* item)
{
    _indent++;

    // handle all properties of the item
    foreach (PropertyHandlerPtr handler, createPropertyHandlersList(item))
    {
        // distribute to write methods depending on property type (using visitor pattern)
        handler->acceptVisitor(this);
    }

    _indent--;
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(BoolPropertyHandler* handler)
{
    // write a label/value line for this property
    writeIndent();
    _out << texStrUpp(handler->title()) << ": " << (handler->value() ? "yes" : "no") << "\\\\\n";
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(IntPropertyHandler* handler)
{
    // write a label/value line for this property
    writeIndent();
    _out << texStrUpp(handler->title()) << ": $" << handler->toString(handler->value()) << "$\\\\\n";
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(DoublePropertyHandler* handler)
{
    // write a label/value line for this property
    writeIndent();
    _out << texStrUpp(handler->title()) << ": " << texDouble(handler->toString(handler->value())) << "\\\\\n";
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(DoubleListPropertyHandler* handler)
{
    // write a label/value line for this property
    writeIndent();
    _out << texStrUpp(handler->title()) << ": " << texDoubleList(handler->toString(handler->value())) << "\\\\\n";
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(StringPropertyHandler* handler)
{
    // write a label/value line for this property
    writeIndent();
    _out << texStrUpp(handler->title()) << ": " << texString(handler->value()) << "\\\\\n";
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(EnumPropertyHandler* handler)
{
    // write a label/value line for this property
    writeIndent();
    _out << texStrUpp(handler->title()) << ": " << texString(handler->titleForValue()) << "\\\\\n";
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(ItemPropertyHandler* handler)
{
    if (handler->value())
    {
        // write a label/value line for this property
        writeIndent();
        _out << texStrUpp(handler->title()) << ": " << texString(title(itemType(handler->value()))) << "\\\\\n";

        // handle the properties of the item pointed to by the property
        writeProperties(handler->value());
    }
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::visitPropertyHandler(ItemListPropertyHandler* handler)
{
    // loop over all items in the list held by this property
    int index = 0;
    foreach(SimulationItem* item, handler->value())
    {
        index++;

        // write a label/value line for this item in the list
        writeIndent();
        _out << "Item \\#" << QString::number(index) << " in " << texString(handler->title()) << " list: "
             << texString(title(itemType(item))) << "\\\\\n";

        // handle the properties of the item pointed to by this item in the list
        writeProperties(item);
    }
}

////////////////////////////////////////////////////////////////////

void LatexHierarchyWriter::writeIndent()
{
    for (int i=0; i<_indent; i++) _out << ".\\quad{}";
}

////////////////////////////////////////////////////////////////////

QString LatexHierarchyWriter::texString(QString string)
{
    QString result;
    foreach (QChar ch, string)
    {
        if (_replstr.contains(ch)) result += _replstr.value(ch);
        else result += ch;
    }
    return result;
}

////////////////////////////////////////////////////////////////////

// "tex-ify" a regular string as above and make the first character uppercase
QString LatexHierarchyWriter::texStrUpp(QString string)
{
    QString result = string;
    if (!result.isEmpty()) result.replace(0, 1, result[0].toUpper());
    return texString(result);
}

////////////////////////////////////////////////////////////////////

QString LatexHierarchyWriter::texDouble(QString string)
{
    // get the segments
    QStringList segments = string.simplified().split(' ');
    if (segments.isEmpty() || segments.size()>2) return string;

    // treat the number (first handle the exponent since the special symbol replacements also contain 'e')
    QString result = segments[0];
    if (result.contains("e"))
    {
        result.replace("e","\\times 10^{");
        result += "}";
    }
    result.replace("∅","\\emptyset");
    result.replace("∞","\\infty");
    result.replace("-","\\textrm{-}");

    // treat the unit if present
    if (segments.size()>1)
    {
        // split tokens separated by slashes
        QStringList tokens = segments[1].split('/');

        for (int k=0; k < tokens.size(); k++)
        {
            // replace the complete token if it is in the list of specials
            // otherwise, put in in upright type (except for an exponent at the end)
            if (_repluni.contains(tokens[k])) tokens[k] = _repluni.value(tokens[k]);
            else
            {
                QString lastChar = tokens[k].right(1);
                if (lastChar=="2" || lastChar=="3" || lastChar=="4")
                {
                    tokens[k].chop(1);
                    tokens[k] = "\\textrm{" + tokens[k] + "}^{" + lastChar + "}";
                }
                else
                {
                    tokens[k] = "\\textrm{" + tokens[k] + "}";
                }
            }
        }
        result += "\\:" + tokens.join("/");
    }

    // return result as math expression
    return "$" + result + "$";
}

////////////////////////////////////////////////////////////////////

QString LatexHierarchyWriter::texDoubleList(QString string)
{
    QStringList result;
    QStringList items = string.split(',');
    foreach (QString item, items) result << texDouble(item.simplified());
    return result.join(", ");
}

////////////////////////////////////////////////////////////////////
