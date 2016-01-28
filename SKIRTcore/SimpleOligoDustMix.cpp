/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "OligoWavelengthGrid.hpp"
#include "SimpleOligoDustMix.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SimpleOligoDustMix::SimpleOligoDustMix()
{
}

//////////////////////////////////////////////////////////////////////

void SimpleOligoDustMix::setupSelfBefore()
{
    DustMix::setupSelfBefore();

    // Verify that the wavelength grid (and thus the simulation) is of the Oligo type
    OligoWavelengthGrid* lambdagrid = find<OligoWavelengthGrid>();

    // Verify that the number of luminosities equals the number of wavelengths
    int Nlambda = lambdagrid->Nlambda();
    if (_kappaextv.size() != Nlambda)
        throw FATALERROR("The number of extinction coefficients differs from the number of wavelengths");
    if (_albedov.size() != Nlambda)
        throw FATALERROR("The number of albedos differs from the number of wavelengths");
    if (_asymmparv.size() != Nlambda)
        throw FATALERROR("The number of asymmetry parameters differs from the number of wavelengths");

    // Create temporary vectors with the absorption and scattering coefficients
    Array kappaabsv(Nlambda);
    Array kappascav(Nlambda);
    Array asymmparv(Nlambda);
    for (int ell=0; ell<Nlambda; ell++)
    {
        kappaabsv[ell] = _kappaextv[ell]*(_albedov[ell]+1.0);
        kappascav[ell] = _kappaextv[ell]*_albedov[ell];
        asymmparv[ell] = _asymmparv[ell];
    }

    // Add a dust population with these properties, provide a dummy for the dust mass with (at least)
    // the correct dimension.
    double Mdust = 1.0/_kappaextv[0];
    addpopulation(Mdust, kappaabsv, kappascav, asymmparv);
}

//////////////////////////////////////////////////////////////////////

void SimpleOligoDustMix::setOpacities(QList<double> value)
{
    _kappaextv = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> SimpleOligoDustMix::opacities() const
{
    return _kappaextv;
}

//////////////////////////////////////////////////////////////////////

void SimpleOligoDustMix::setAlbedos(QList<double> value)
{
    _albedov = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> SimpleOligoDustMix::albedos() const
{
    return _albedov;
}

//////////////////////////////////////////////////////////////////////

void SimpleOligoDustMix::setAsymmetryParameters(QList<double> value)
{
    _asymmparv = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> SimpleOligoDustMix::asymmetryParameters() const
{
    return _asymmparv;
}

//////////////////////////////////////////////////////////////////////
