/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GALUMFIT_HPP
#define GALUMFIT_HPP

#include "Array.hpp"
#include "GAPopulation.h"
#include "GARealGenome.h"
#include "GASStateGA.h"
#include "Log.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The GALumfit class contains all the information used to optimize a 1D luminosity problem. It uses the
    golden section method to constrain the 1 variable parameter.
    http://www.aip.de/groups/soe/local/numres/bookcpdf/c10-1.pdf */

class GALumfit : public SimulationItem
{
    Q_OBJECT

    Q_CLASSINFO("Title", "GA luminosity optimization")

    Q_CLASSINFO("Property", "fixedSeed")
    Q_CLASSINFO("Title", "Do you want a fixed seed (only for testing purposes)")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "minlum")
    Q_CLASSINFO("Title", "the minimum luminosities in solar units")
    Q_CLASSINFO("Default", "1e8")

    Q_CLASSINFO("Property", "maxlum")
    Q_CLASSINFO("Title", "the maximum luminosities in solar units")
    Q_CLASSINFO("Default", "1e10")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE GALumfit();

protected:
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the boolean value whether to use a fixed seed or not. */
    Q_INVOKABLE void setFixedSeed(bool value);

    /** Returns the boolean value whether to use a fixed seed or not. */
    Q_INVOKABLE bool fixedSeed() const;

    /** Sets the list of minimum luminosities, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setMinLuminosities(QList<double> value);

    /** Returns the list of minimum luminosities, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> minLuminosities() const;

    /** Sets the list of maximum luminosities, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setMaxLuminosities(QList<double> value);

    /** Returns the list of maximum luminosities, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> maxLuminosities() const;

    //======================== Other Functions =======================
public:
    /** This function sets the _ref and returns best fitting luminosity.
        The simulation is adapted so they contain the same mask as the reference image.
        Together with the adapted simulation and best fitting parameter, the lowest \f$\chi^2\f$ value is returned.*/
    void optimize(const Array *Rframe, QList<Array> *frames, QList<double> *lumis, double &chi2);
private:

    /** This function determines the \f$\chi^2\f$ value for a certain list of luminosities. */
    double function(QList<Array> *frames, QList<double> var);

    //======================== Data Members ========================

private:
    QList<double> _minLum;
    QList<double> _maxLum;
    const Array *_ref;
    bool _fixedSeed = false;

};

////////////////////////////////////////////////////////////////////

#endif // GALUMFIT_HPP
