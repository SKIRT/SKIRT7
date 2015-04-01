/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "ElectronDustMix.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ElectronDustMix::ElectronDustMix()
{
}

//////////////////////////////////////////////////////////////////////

void ElectronDustMix::setupSelfBefore()
{
    DustMix::setupSelfBefore();

    // get the simulation's wavelength grid
    const Array& lambdav = simlambdav();
    int Nlambda = lambdav.size();

    // arbitrarily set the resolution of the theta grid
    int Ntheta = 181;

    // create temporary vectors and tables with the appropriate size
    Array sigmaabsv(Nlambda), sigmascav(Nlambda), asymmparv(Nlambda);
    Table<2> S11vv(Nlambda,Ntheta), S12vv(Nlambda,Ntheta), S33vv(Nlambda,Ntheta), S34vv(Nlambda,Ntheta);

    // set the constant scattering cross section, and leave absorption at zero
    // (leave asymmpar at zero as well - it is not used since we support polarization)
    sigmascav = Units::sigmaThomson();

    // calculate the wavelength-independent Sxx values in the Mueller matrix according to
    // equation (C.7) of Wolf 2003 (Computer Physics Communications, 150, 99–115)
    double dt = M_PI/(Ntheta-1);
    for (int t=0; t<Ntheta; t++)
    {
        double theta = t * dt;
        double costheta = cos(theta);
        double S11 = 0.5*(costheta*costheta+1.);
        double S12 = 0.5*(costheta*costheta-1.);
        double S33 = costheta;
        for (int ell=0; ell<Nlambda; ell++)
        {
            S11vv(ell,t) = S11;
            S12vv(ell,t) = S12;
            S33vv(ell,t) = S33;
        }
    }

    // calculate the weight of an electron per hydrogen atom
    double mu = Units::masselectron()/(Units::masselectron()+Units::massproton());

    // add a single dust population with these properties
    addpopulation(mu, sigmaabsv, sigmascav, asymmparv);
    addpolarization(S11vv, S12vv, S33vv, S34vv);
}

//////////////////////////////////////////////////////////////////////
