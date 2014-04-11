/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTEMGRAINCOMPOSITION_HPP
#define DUSTEMGRAINCOMPOSITION_HPP

#include "GrainComposition.hpp"

////////////////////////////////////////////////////////////////////

/** The DustEmGrainComposition class represents the optical and calorimetric properties of dust
    grains, obtained from the DustEM data included with SKIRT. The user must provide the name of a
    particlar DustEM grain type as an attribute of this class. Example grain types include
    "PAH0_DL07", "PAH1_DL07", "Gra", and "aSil".

    DustEM is described in Compiègne et al. 2011 (AA, 525, A103) and the data was downloaded from
    http://www.ias.u-psud.fr/DUSTEM/. */
class DustEmGrainComposition : public GrainComposition
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grain composition based on DustEM data")

    Q_CLASSINFO("Property", "grainType")
    Q_CLASSINFO("Title", "the DustEM grain type name")

    Q_CLASSINFO("Property", "bulkMassDensity")
    Q_CLASSINFO("Title", "the bulk mass density for this grain material")
    Q_CLASSINFO("Quantity", "bulkmassdensity")
    Q_CLASSINFO("MinValue", "100 kg/m3")
    Q_CLASSINFO("MaxValue", "10000 kg/m3")
    Q_CLASSINFO("Default", "2240 kg/m3")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE DustEmGrainComposition();

    /** This constructor can be invoked by dust mix classes that wish to hard-code the creation of
        a new grain composition object of this type (as opposed to creation through the ski file).
        Before the constructor returns, the newly created object is hooked up as a child to the
        specified parent in the simulation hierarchy (so it will automatically be deleted), the
        grain type attribute has been set to the value specified in the second argument, the bulk
        mass density has been set to the value specified in the third argument, and the setup()
        function has been called. */
    explicit DustEmGrainComposition(SimulationItem* parent, QString graintype, double rhobulk);

protected:
    /** This function reads the optical and calorimetric properties from the DustEM resource files
        corresponding to the value of the grain type attribute, and sets the bulk mass density to
        the value of the corresponding attribute. */
    void setupSelfBefore();

    //====================== Identifying =====================

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        represented by the instance. */
    QString name() const;

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the DustEM name for the grain type to be represented. Example grain types include
    "PAH0_DL07", "PAH1_DL07", "Gra", and "aSil". */
    Q_INVOKABLE void setGrainType(QString value);

    /** Returns the DustEM name for the grain type to be represented. */
    Q_INVOKABLE QString grainType() const;

    /** Sets the bulk mass density \f$\rho_\text{bulk}\f$ for this grain material. */
    Q_INVOKABLE void setBulkMassDensity(double value);

    /** Returns the bulk mass density \f$\rho_\text{bulk}\f$ for this grain material. */
    Q_INVOKABLE double bulkMassDensity() const;

    //======================== Data Members ========================

private:
    QString _graintype;
    double _bulkmassdensity;
};

////////////////////////////////////////////////////////////////////

#endif // DUSTEMGRAINCOMPOSITION_HPP
