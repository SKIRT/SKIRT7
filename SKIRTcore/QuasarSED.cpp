/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "QuasarSED.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

QuasarSED::QuasarSED()
{
}

////////////////////////////////////////////////////////////////////

void QuasarSED::setupSelfBefore()
{
    StellarSED::setupSelfBefore();

    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    double Nlambda = lambdagrid->Nlambda();

    Array jv(Nlambda);
    for (int ell=0; ell<Nlambda; ell++)
    {
        double lambda = lambdagrid->lambda(ell);

        lambda *= 1e6; // conversion from m to micron (the Quasar SED is defined using microns)
        double j = 0.0;
        double a = 1.0;
        double b = 0.003981072;
        double c = 0.001258926;
        double d = 0.070376103;

        if (lambda<0.001)
            j = 0.0;
        else if (lambda<0.01)
            j = a*pow(lambda,0.2);
        else if (lambda<0.1)
            j = b*pow(lambda,-1.0);
        else if (lambda<5.0)
            j = c*pow(lambda,-1.5);
        else if (lambda<1000.0)
            j = d*pow(lambda,-4.0);
        else
            j = 0.0;
        jv[ell] = j;
    }

    setemissivities(jv);
}

//////////////////////////////////////////////////////////////////////
