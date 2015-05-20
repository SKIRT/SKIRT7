/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include <iomanip>
#include "FatalError.hpp"
#include "NR.hpp"
#include "SED.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SED::SED()
{
}

//////////////////////////////////////////////////////////////////////

void SED::setupSelfAfter()
{
    size_t Nlambda = find<WavelengthGrid>()->Nlambda();
    if (_Lv.size() != Nlambda) throw FATALERROR("The luminosities in the SED have not been properly set");
}

//////////////////////////////////////////////////////////////////////

double SED::luminosity(int ell) const
{
    return _Lv[ell];
}

//////////////////////////////////////////////////////////////////////

const Array& SED::luminosities() const
{
    return _Lv;
}

//////////////////////////////////////////////////////////////////////

void SED::setluminosities(const Array &Lv)
{
    // copy luminosities
    _Lv = Lv;

    // normalize luminosities to unity
    double sum = _Lv.sum();
    if (sum<=0) throw FATALERROR("The total luminosity in the SED is zero or negative (" + QString::number(sum) + ")");
    _Lv /= sum;
}

//////////////////////////////////////////////////////////////////////

void SED::setemissivities(const Array& jv)
{
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    setluminosities(jv * lambdagrid->dlambdav());
}

//////////////////////////////////////////////////////////////////////

void SED::setemissivities(const Array& lambdav, const Array& jv)
{
    setemissivities(NR::resample<NR::interpolate_loglog>(find<WavelengthGrid>()->lambdav(), lambdav, jv));
}

//////////////////////////////////////////////////////////////////////
