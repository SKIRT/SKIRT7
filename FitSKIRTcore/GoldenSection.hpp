/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GOLDENSECTION_HPP
#define GOLDENSECTION_HPP

#include "Image.hpp"
#include "Log.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The GoldenSection class contains all the information used to optimize a 1D luminosity problem. It uses the
    golden section method to constrain the 1 variable parameter.
    http://www.aip.de/groups/soe/local/numres/bookcpdf/c10-1.pdf */

class GoldenSection : public SimulationItem
{
    Q_OBJECT

    Q_CLASSINFO("Title", "Golden Section optimization")

    Q_CLASSINFO("Property", "minlum")
    Q_CLASSINFO("Title", "the minimum luminosity in solar units")
    Q_CLASSINFO("Default", "1e8")

    Q_CLASSINFO("Property", "maxlum")
    Q_CLASSINFO("Title", "the maximum luminosity in solar units")
    Q_CLASSINFO("Default", "1e10")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE GoldenSection();

protected:
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the minimal luminosity. */
    Q_INVOKABLE void setMinlum(double value);

    /** Returns the minimal luminosity.*/
    Q_INVOKABLE double minlum() const;

    /** Sets the maximal luminosity. */
    Q_INVOKABLE void setMaxlum(double value);

    /** Returns the maximal luminosity.*/
    Q_INVOKABLE double maxlum() const;

    //======================== Other Functions =======================
public:
    /** This function sets the _ref and returns best fitting luminosity.
        The simulation is adapted so they contain the same mask as the reference image.
        Together with the adapted simulation and best fitting parameter, the lowest \f$\chi^2\f$ value is returned.*/
    void optimize(const Image& refframe, Image& frame, double& lum, double& chi2);

private:
    /** This function determines the \f$\chi^2\f$ value for a certain luminosity value x. */
    double function(Image& frame, double x);

    //======================== Data Members ========================

private:
    double _minLum;
    double _maxLum;
    const Image* _ref;
};

////////////////////////////////////////////////////////////////////

#endif // GOLDENSECTION_HPP
