/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DustMix.hpp"
#include "GreyBodyDustEmissivity2.hpp"
#include "PlanckFunction.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

GreyBodyDustEmissivity2::GreyBodyDustEmissivity2()
{
}

////////////////////////////////////////////////////////////////////

Array GreyBodyDustEmissivity2::emissivity(const DustMix* mix, const Array& Jv) const
{
    // get basic information about the wavelength grid
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    int Nlambda = lambdagrid->Nlambda();

    // get basic information about the dust mix
    int Npop = mix->Npop();

    // accumulate the emissivities at the equilibrium temperature for all populations in the dust mix
    Array ev(Nlambda);
    for (int c=0; c<Npop; c++)
    {
        double T = mix->equilibrium(Jv,c);
        PlanckFunction B(T);
        for (int ell=0; ell<Nlambda; ell++)
        {
            ev[ell] += mix->sigmaabs(ell,c) * B(lambdagrid->lambda(ell));
        }
    }

    // convert emissivity from "per hydrogen atom" to "per unit mass"
    ev /= mix->mu();
    return ev;
}

////////////////////////////////////////////////////////////////////
