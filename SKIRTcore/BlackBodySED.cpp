/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "BlackBodySED.hpp"
#include "FatalError.hpp"
#include "PlanckFunction.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

BlackBodySED::BlackBodySED()
    : _T(0)
{
}

////////////////////////////////////////////////////////////////////

void BlackBodySED::setupSelfBefore()
{
    StellarSED::setupSelfBefore();

    // verify property values
    if (_T <= 0) throw FATALERROR("the black body temperature T should be positive");

    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    double Nlambda = lambdagrid->Nlambda();
    PlanckFunction B(_T);

    Array Lv(Nlambda);
    for (int ell=0; ell<Nlambda; ell++)
    {
        // we take averages over each bin
        int N = 100;
        double loglambdamin = log10(lambdagrid->lambdamin(ell));
        double loglambdamax = log10(lambdagrid->lambdamax(ell));
        double dloglambda = (loglambdamax-loglambdamin)/N;
        double sum = 0;
        for (int i=0; i<=N; i++)
        {
            double weight = 1.0;
            if (i==0 || i==N) weight = 0.5;
            double loglambda = loglambdamin + i*dloglambda;
            double lambda = pow(10,loglambda);
            sum += weight*B(lambda)*lambda;
        }
        Lv[ell] = sum * M_LN10 * dloglambda;
    }

    // finish up
    setluminosities(Lv);
}

////////////////////////////////////////////////////////////////////

void BlackBodySED::setTemperature(double value)
{
    _T = value;
}

////////////////////////////////////////////////////////////////////

double BlackBodySED::temperature() const
{
    return _T;
}

//////////////////////////////////////////////////////////////////////
