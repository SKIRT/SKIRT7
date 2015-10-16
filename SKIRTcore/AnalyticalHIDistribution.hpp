/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ANALYTICALHIDISTRIBUTION_HPP
#define ANALYTICALHIDISTRIBUTION_HPP

#include "Array.hpp"
#include "HIDistribution.hpp"

//////////////////////////////////////////////////////////////////////

/** The AnalyticalHIDistribution class is a subclass of the HIDistribution class and represents "simple"
    neutral hydrogen distributions in which the density, gas temperature and bulk velocity are described
    by simple functions. It is mainly designed for simple test configurations. */
class AnalyticalHIDistribution : public HIDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an analytical neutral hydrogen distribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor, declared protected because this is an abstract class. */
    Q_INVOKABLE AnalyticalHIDistribution();

};

//////////////////////////////////////////////////////////////////////

#endif // ANALYTICALDUSTDISTRIBUTION_HPP
