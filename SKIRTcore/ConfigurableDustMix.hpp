/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CONFIGURABLEDUSTMIX_HPP
#define CONFIGURABLEDUSTMIX_HPP

#include "MultiGrainDustMix.hpp"
class DustMixPopulation;

////////////////////////////////////////////////////////////////////

/** The ConfigurableDustMix class is a subclass of the MultiGrainDustMix class and represents dust
    mixtures consisting of one or more dust populations, fully configurable through its attributes.
    Specifically, the class maintains a list DustMixPopulation instances, each of which represents
    a particular dust population with configurable grain composition and grain size distribution.
    */
class ConfigurableDustMix : public MultiGrainDustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a configurable multi-component dust mix")

    Q_CLASSINFO("Property", "populations")
    Q_CLASSINFO("Title", "the dust populations")
    Q_CLASSINFO("Default", "DustMixPopulation")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ConfigurableDustMix();

protected:
    /** This function adds the configured dust populations to the dust mix. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function adds a dust population to the dust mix. */
    Q_INVOKABLE void addPopulation(DustMixPopulation* value);

    /** This function returns the list of dust populations in the dust mix. */
    Q_INVOKABLE QList<DustMixPopulation*> populations() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    QList<DustMixPopulation*> _populations;
};

////////////////////////////////////////////////////////////////////

#endif // CONFIGURABLEDUSTMIX_HPP
