/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef OLIGOFITSCHEME_HPP
#define OLIGOFITSCHEME_HPP

#include "AdjustableSkirtSimulation.hpp"
#include "FitScheme.hpp"

class ParameterRanges;
class ReferenceImages;
class Optimization;

////////////////////////////////////////////////////////////////////

/** OligoFitScheme represents a complete FitSKIRT fit scheme for oligochromatic fits/simulations.
    It contains the SKIRT simulation, parameter ranges, reference images and the optimization type.*/
class OligoFitScheme : public FitScheme
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an oligochromatic fit scheme")

    Q_CLASSINFO("Property", "simulation")
    Q_CLASSINFO("Title", "the SKIRT simulation to be run for this fit scheme")

    Q_CLASSINFO("Property", "fixedSeed")
    Q_CLASSINFO("Title", "have a fixed seed (only for testing purposes)")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "parameterRanges")
    Q_CLASSINFO("Title", "the parameter ranges")

    Q_CLASSINFO("Property", "referenceImages")
    Q_CLASSINFO("Title", "the refence images")

    Q_CLASSINFO("Property", "optim")
    Q_CLASSINFO("Title", "the optimization properties")

    //======== Construction - Setup - Run - Destruction  ===========

public:
    /** The default constructor. */
    Q_INVOKABLE OligoFitScheme();

protected:
    /** This function actually runs the fit scheme. It assumes that setup() has been already
        performed. */
    void runSelf();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the SKIRT simulation to be run for this fit scheme. */
    Q_INVOKABLE void setSimulation(AdjustableSkirtSimulation* value);

    /** Returns the SKIRT simulation to be run for this fit scheme. */
    Q_INVOKABLE AdjustableSkirtSimulation* simulation() const;

    /** Sets the boolean value whether to use a fixed seed or not. */
    Q_INVOKABLE void setFixedSeed(bool value);

    /** Returns the boolean value whether to use a fixed seed or not. */
    Q_INVOKABLE bool fixedSeed() const;

    /** Sets the parameterranges for this fit scheme. */
    Q_INVOKABLE void setParameterRanges(ParameterRanges* value);

    /** This function returns the list of parameter ranges in the fit scheme. */
    Q_INVOKABLE ParameterRanges* parameterRanges() const;

    /** Sets the reference images for this fit scheme. */
    Q_INVOKABLE void setReferenceImages(ReferenceImages* value);

    /** This function returns the reference images in this fit scheme. */
    Q_INVOKABLE ReferenceImages* referenceImages() const;

    /** Sets the optimization properties for this fit scheme. */
    Q_INVOKABLE void setOptim(Optimization* value);

    /** This function returns the optimization properties in this fit scheme. */
    Q_INVOKABLE Optimization* optim() const;

    //======================== Other Functions =======================

    /** This function is used by the Optimization object. It requires a ReplacementDict for the
        AdjustableSkirtSimulation and returns the total \f$\chi^2\f$ value together with lists of the best fitting
        luminosities, the separate \f$\chi^2\f$ values and the masked simulations. */
    double objective(AdjustableSkirtSimulation::ReplacementDict replacement, QList<QList<double>>& luminosities,
                     QList<double>& chis, int index);

    //======================== Data Members ========================

protected:
    // data members
    AdjustableSkirtSimulation* _simulation;
    bool _fixedSeed;
    ParameterRanges* _ranges;
    ReferenceImages* _rimages;
    Optimization* _optim;
};

////////////////////////////////////////////////////////////////////

#endif // OLIGOFITSCHEME_HPP
