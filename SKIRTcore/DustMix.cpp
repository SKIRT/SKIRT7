/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "DustDistribution.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "PlanckFunction.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "Random.hpp"
#include "TextOutFile.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DustMix::DustMix()
    : _writeMix(true), _writeMeanMix(true), _lambdagrid(0), _Nlambda(0), _random(0), _Npop(0), _mu(0),
      _polarization(false), _Ntheta(0)
{
}

////////////////////////////////////////////////////////////////////

void DustMix::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    _lambdagrid = find<WavelengthGrid>();
    _Nlambda = _lambdagrid->Nlambda();
    _random = find<Random>();
}

////////////////////////////////////////////////////////////////////

void DustMix::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    // Verify that the subclass added at least one dust population
    if (_Npop < 1) throw FATALERROR("Dust mixture must contain at least one dust population");

    // -------------------------------------------------------------
    // Calculate total and mean values of the fundamental properties
    // -------------------------------------------------------------

    _sigmaabsv.resize(_Nlambda);
    _sigmascav.resize(_Nlambda);
    _sigmaextv.resize(_Nlambda);
    _albedov.resize(_Nlambda);
    _asymmparv.resize(_Nlambda);
    for (int ell=0; ell<_Nlambda; ell++)
    {
        double sumabs = 0.0;
        double sumsca = 0.0;
        double sumgsca = 0.0;
        for (int c=0; c<_Npop; c++)
        {
            sumabs += _sigmaabsvv[c][ell];
            sumsca += _sigmascavv[c][ell];
            sumgsca += _asymmparvv[c][ell] * _sigmascavv[c][ell];
        }
        double sumext = sumabs+sumsca;
        _sigmaabsv[ell] = sumabs;
        _sigmascav[ell] = sumsca;
        _sigmaextv[ell] = sumext;
        _albedov[ell] = sumext ? sumsca/sumext : 0.;
        _asymmparv[ell] = sumsca ? sumgsca/sumsca : 0.;
    }
    _mu = 0.0;
    for (int c=0; c<_Npop; c++)
    {
        _mu += _muv[c];
    }

    // -------------------------------------------------------------
    // Calculate the total absorption and scattering coefficients
    // -------------------------------------------------------------

    _kappaabsv = _sigmaabsv / _mu;
    _kappascav = _sigmascav / _mu;
    _kappaextv = _sigmaextv / _mu;

    // -------------------------------------------------------------
    // Calculate the cumulative distribution of theta and phi
    // -------------------------------------------------------------

    if (_polarization)
    {
        // create a table containing the theta value corresponding to each index
        _thetav.resize(_Ntheta);
        double dt = M_PI/(_Ntheta-1);
        for (int t=0; t<_Ntheta; t++)
        {
            _thetav[t] = t * dt;
        }

        // create a table with the normalized cumulative distribution of theta for each wavelength
        _thetaXvv.resize(_Nlambda,0);
        for (int ell=0; ell<_Nlambda; ell++)
        {
            NR::cdf(_thetaXvv[ell], _Ntheta-1, [this,ell,dt](int t){ return _S11vv(ell,t+1)*sin(_thetav[t+1])*dt; });
        }

        // create a table with the phase function normalization factor for each wavelength
        _pfnormv.resize(_Nlambda);
        for (int ell=0; ell<_Nlambda; ell++)
        {
            double sum = 0.;
            for (int t=0; t<_Ntheta; t++)
            {
                sum += _S11vv(ell,t)*sin(_thetav[t])*dt;
            }
            _pfnormv[ell] = 2.0/sum;
        }

        // create tables listing phi, phi/(2 pi), sin(2 phi) and 1-cos(2 phi) for a number of phi indices
        _phiv.resize(_Nphi);
        _phi1v.resize(_Nphi);
        _phisv.resize(_Nphi);
        _phicv.resize(_Nphi);
        _phiXv.resize(_Nphi);
        double df = 2*M_PI/(_Nphi-1);
        for (int f=0; f<_Nphi; f++)
        {
            double phi = f * df;
            _phiv[f] = phi;
            _phi1v[f] = phi/(2*M_PI);
            _phisv[f] = sin(2*phi);
            _phicv[f] = 1-cos(2*phi);
        }
    }

    // -------------------------------------------------------------
    // Calculate and log the extinction in the V-band
    // -------------------------------------------------------------

    Log* log = find<Log>();
    Units* units = find<Units>();
    int h = find<DustDistribution>()->indexformix(this);
    log->info("Dust mixture " + QString::number(h));
    try
    {
        double kappaV = kappaext(Units::lambdaV());
        log->info("Total extinction coefficient in the V band: kappaV = "
                  + QString::number(units->oopacity(kappaV)) + " " + units->uopacity());
    }
    catch (FatalError&) { }  // ignore error in kappaext() for wavelength grids that don't contain lambdaV

    // -------------------------------------------------------------
    // Output the optical properties to a text file
    // -------------------------------------------------------------

    // output the optical properties for all dust populations
    if (_writeMix)
    {
        // Create a text file
        TextOutFile file(this, "ds_mix_" + QString::number(h) + "_opti", "optical dust population properties");

        // Write the header
        file.addColumn("lambda (" + units->uwavelength() + ")");
        QString section = units->usection() + " per hydrogen atom";
        for (int c=0; c<_Npop; c++) file.addColumn("sigma_ext[" + QString::number(c) + "] (" + section + ")");
        for (int c=0; c<_Npop; c++) file.addColumn("sigma_abs[" + QString::number(c) + "] (" + section + ")");
        for (int c=0; c<_Npop; c++) file.addColumn("sigma_sca[" + QString::number(c) + "] (" + section + ")");
        for (int c=0; c<_Npop; c++) file.addColumn("asymmpar[" + QString::number(c) + "]");

        // Write the body
        for (int ell=0; ell<_Nlambda; ell++)
        {
            QList<double> values;
            values << units->owavelength(_lambdagrid->lambda(ell));
            for (int c=0; c<_Npop; c++) values << units->osection(_sigmaabsvv[c][ell]+_sigmascavv[c][ell]);
            for (int c=0; c<_Npop; c++) values << units->osection(_sigmaabsvv[c][ell]);
            for (int c=0; c<_Npop; c++) values << units->osection(_sigmascavv[c][ell]);
            for (int c=0; c<_Npop; c++) values << _asymmparvv[c][ell];
            file.writeRow(values);
        }
    }

    // output the mass properties for all dust populations
    if (_writeMix)
    {
        // Create a text file
        TextOutFile file(this, "ds_mix_" + QString::number(h) + "_mass", "dust population masses");

        // Write the header
        QString bulkmass = units->ubulkmass() + " per hydrogen atom";
        file.writeLine("# total dust mass: " + QString::number(units->obulkmass(_mu)) + ' ' + bulkmass);
        file.addColumn("dust mix population index", 'd');
        file.addColumn("dust mass (" + bulkmass + ")");
        file.addColumn("dust mass (% of total)", 'f', 3);

        // Write the body
        for (int c=0; c<_Npop; c++)
        {
            file.writeRow(QList<double>() << c << units->obulkmass(_muv[c]) << 100.*_muv[c]/_mu);
        }
    }

    // output the combined optical properties for the dust mixture
    if (_writeMeanMix)
    {
        // Create a text file
        TextOutFile file(this, "ds_mix_" + QString::number(h) + "_mean", "combined dust mix properties");

        // Write the header
        file.addColumn("lambda (" + units->uwavelength() + ")");
        file.addColumn("total kappa_ext (" + units->uopacity() + ")");
        file.addColumn("total kappa_abs (" + units->uopacity() + ")");
        file.addColumn("total kappa_sca (" + units->uopacity() + ")");
        file.addColumn("mean asymmpar");

        // Write the body
        for (int ell=0; ell<_Nlambda; ell++)
        {
            file.writeRow(QList<double>() << units->owavelength(_lambdagrid->lambda(ell))
                                          << units->oopacity(_kappaextv[ell])
                                          << units->oopacity(_kappaabsv[ell])
                                          << units->oopacity(_kappascav[ell])
                                          << _asymmparv[ell]);
        }
    }

    // -------------------------------------------------------------
    // Construct a temperature grid and
    // calculate the Planck-integrated absorption cross sections
    // -------------------------------------------------------------

    // The calculation is possible (and required) only for panchromatic wavelength grids
    if (_lambdagrid->issampledrange())
    {
        // the temperature grid
        const int NT = 1000;
        NR::powgrid(_Tv, 0., 5000., NT, 500.);

        // the Planck-integrated absorption coefficients
        _planckabsvv.resize(_Npop+1,NT+1);
        for (int p=1; p<=NT; p++)   // leave values for p==0 at zero
        {
            for (int c=0; c<=_Npop; c++)
            {
                const Array& sigmaabsv = c<_Npop ? _sigmaabsvv[c] : _sigmaabsv;
                PlanckFunction B(_Tv[p]);
                double planckabs = 0.0;
                for (int ell=0; ell<_Nlambda; ell++)
                {
                    double lambda = _lambdagrid->lambda(ell);
                    double dlambda = _lambdagrid->dlambda(ell);
                    planckabs += sigmaabsv[ell] * B(lambda) * dlambda;
                }
                _planckabsvv(c,p) = planckabs;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////

const Array& DustMix::simlambdav() const
{
    return _lambdagrid->lambdav();
}

////////////////////////////////////////////////////////////////////

void DustMix::addpopulation(double mu, const Array& sigmaabsv, const Array& sigmascav, const Array& asymmparv)
{
    // verify the vector lengths
    if (sigmaabsv.size()!=(unsigned)_Nlambda ||
        sigmascav.size()!=(unsigned)_Nlambda ||
        asymmparv.size()!=(unsigned)_Nlambda)
    {
        throw FATALERROR("Dust property vectors must have same size as simulation's lambda grid");
    }

    // don't add populations with zero mass contribution (which may arise for small size bins),
    // because they lead to division by zero and NaN results later on
    if (mu > 0)
    {
        // add the property values to our cache
        _Npop++;
        _muv.push_back(mu);
        _sigmaabsvv.push_back(sigmaabsv);
        _sigmascavv.push_back(sigmascav);
        _asymmparvv.push_back(asymmparv);
    }
}

////////////////////////////////////////////////////////////////////

void DustMix::addpopulation(double mu, const Array& lambdav,
                            const Array& sigmaabsv, const Array& sigmascav, const Array& asymmparv)
{
    // get the simulation's wavelength grid
    const Array& lambdagridv = _lambdagrid->lambdav();

    // verify that the dust properties are defined for the complete wavelength range
    double eps = 0.5e-5;
    if (lambdagridv[0]<lambdav[0]*(1-eps) || lambdagridv[lambdagridv.size()-1]>lambdav[lambdav.size()-1]*(1+eps))
    {
        Units* units = find<Units>();
        throw FATALERROR(
                    "Properties for this dust population are only defined for wavelengths between " +
                    QString::number(units->owavelength(lambdav[0])) + " and " +
                    QString::number(units->owavelength(lambdav[lambdav.size()-1])) + " " + units->uwavelength() );
    }

    // add the population using resampled properties
    addpopulation(mu, NR::resample<NR::interpolate_loglog>(lambdagridv,lambdav,sigmaabsv),
                      NR::resample<NR::interpolate_loglog>(lambdagridv,lambdav,sigmascav),
                  NR::resample<NR::interpolate_loglin>(lambdagridv,lambdav,asymmparv) );
}

//////////////////////////////////////////////////////////////////////

void DustMix::addpolarization(const Table<2>& S11vv, const Table<2>& S12vv,
                              const Table<2>& S33vv, const Table<2>& S34vv)
{
    // in the first invocation of this function, remember the number of theta samples, and resize our tables
    if (!_polarization)
    {
        _polarization = true;
        _Ntheta = S11vv.size(1);
        _S11vv.resize(_Nlambda,_Ntheta);
        _S12vv.resize(_Nlambda,_Ntheta);
        _S33vv.resize(_Nlambda,_Ntheta);
        _S34vv.resize(_Nlambda,_Ntheta);
    }

    // verify the incoming table sizes
    if (S11vv.size(0)!=(unsigned)_Nlambda || S12vv.size(0)!=(unsigned)_Nlambda ||
        S33vv.size(0)!=(unsigned)_Nlambda || S34vv.size(0)!=(unsigned)_Nlambda ||
        S11vv.size(1)!=(unsigned)_Ntheta || S12vv.size(1)!=(unsigned)_Ntheta ||
        S33vv.size(1)!=(unsigned)_Ntheta || S34vv.size(1)!=(unsigned)_Ntheta)
    {
        throw FATALERROR("Mueller tables must have same size as simulation's lambda grid");
    }

    // accumulate the incoming Mueller coefficients into our tables
    for (int ell=0; ell<_Nlambda; ell++)
    {
        for (int t=0; t<_Ntheta; t++)
        {
            _S11vv(ell,t) += S11vv(ell,t);
            _S12vv(ell,t) += S12vv(ell,t);
            _S33vv(ell,t) += S33vv(ell,t);
            _S34vv(ell,t) += S34vv(ell,t);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void DustMix::setWriteMix(bool value)
{
    _writeMix = value;
}

//////////////////////////////////////////////////////////////////////

bool DustMix::writeMix() const
{
    return _writeMix;
}

//////////////////////////////////////////////////////////////////////

void DustMix::setWriteMeanMix(bool value)
{
    _writeMeanMix = value;
}

//////////////////////////////////////////////////////////////////////

bool DustMix::writeMeanMix() const
{
    return _writeMeanMix;
}

//////////////////////////////////////////////////////////////////////

int DustMix::Npop() const
{
    return _Npop;
}

//////////////////////////////////////////////////////////////////////

double DustMix::mu(int c) const
{
    return _muv[c];
}

//////////////////////////////////////////////////////////////////////

double DustMix::mu() const
{
    return _mu;
}

//////////////////////////////////////////////////////////////////////

double DustMix::sigmaabs(int ell, int c) const
{
    return _sigmaabsvv[c][ell];
}

//////////////////////////////////////////////////////////////////////

const Array& DustMix::sigmaabsv(int c) const
{
    return _sigmaabsvv[c];
}

//////////////////////////////////////////////////////////////////////

double DustMix::sigmaabs(int ell) const
{
    return _sigmaabsv[ell];
}

//////////////////////////////////////////////////////////////////////

double DustMix::sigmasca(int ell, int c) const
{
    return _sigmascavv[c][ell];
}

//////////////////////////////////////////////////////////////////////

double DustMix::sigmasca(int ell) const
{
    return _sigmaextv[ell];
}

//////////////////////////////////////////////////////////////////////

double DustMix::sigmaext(int ell) const
{
    return _sigmaextv[ell];
}

//////////////////////////////////////////////////////////////////////

double DustMix::kappaabs(int ell) const
{
    return _kappaabsv[ell];
}

//////////////////////////////////////////////////////////////////////

const Array& DustMix::kappaabsv() const
{
    return _kappaabsv;
}

//////////////////////////////////////////////////////////////////////

double DustMix::kappasca(int ell) const
{
    return _kappascav[ell];
}

//////////////////////////////////////////////////////////////////////

double DustMix::kappaext(int ell) const
{
    return _kappaextv[ell];
}

//////////////////////////////////////////////////////////////////////

double DustMix::kappaext(double lambda) const
{
    // for panchromatic wavelength grids
    if (_lambdagrid->issampledrange())
    {
        const Array& lambdav = _lambdagrid->lambdav();
        int ell = NR::locate_fail(lambdav,lambda);
        if (ell<0) throw FATALERROR("Optical properties are not defined for this wavelength");

        double loglambda = log10(lambda);
        double loglambdaL = log10(lambdav[ell]);
        double loglambdaR = log10(lambdav[ell+1]);
        double p = (loglambda-loglambdaL)/(loglambdaR-loglambdaL);
        double kappaextL = _kappaextv[ell];
        double kappaextR = _kappaextv[ell+1];
        if (kappaextL>0 && kappaextR>0)
        {
            double logkappaextL = log10(kappaextL);
            double logkappaextR = log10(kappaextR);
            double logkappaext = logkappaextL + p*(logkappaextR-logkappaextL);
            return pow(10,logkappaext);
        }
        else
        {
            return kappaextL + p*(kappaextR-kappaextL);
        }
    }

    // for oligochromatic wavelength grids
    else
    {
        for (int ell=0; ell<_lambdagrid->Nlambda(); ell++)
        {
            double lambdasim = _lambdagrid->lambda(ell);
            if (fabs(lambda/lambdasim-1.0)<1e-5)
            {
                return _kappaextv[ell];
            }
        }
        throw FATALERROR("Optical properties are not defined for this wavelength");
    }
}

//////////////////////////////////////////////////////////////////////

double DustMix::albedo(int ell) const
{
    return _albedov[ell];
}

////////////////////////////////////////////////////////////////////

bool DustMix::polarization() const
{
    return _polarization;
}

////////////////////////////////////////////////////////////////////

namespace
{
    // This helper function returns the appropriate t index for the specified value of theta,
    // given the specified number of theta values in the arrays.
    int indexForTheta(double theta, int Ntheta)
    {
        double dt = M_PI/(Ntheta-1);
        int t = static_cast<int>(theta/dt+0.5);
        if (t<0) t = 0;
        else if (t>=Ntheta) t = Ntheta-1;
        return t;
    }

    // This helper function returns the angle phi between the previous and current scattering planes
    // given the normal to the previous scattering plane and the current and new propagation directions
    // of the photon package. The function returns a zero angle if the light is unpolarized or when the
    // current scattering event is completely forward or backward.
    double angleBetweenScatteringPlanes(Direction np, Direction kc, Direction kn)
    {
        Vec nc = Vec::cross(kc,kn);
        nc /= nc.norm();
        double cosphi = Vec::dot(np,nc);
        double sinphi = Vec::dot(Vec::cross(np,nc), kc);
        double phi = atan2(sinphi,cosphi);
        if (isfinite(phi)) return phi;
        return 0;
    }
}

////////////////////////////////////////////////////////////////////

Direction DustMix::scatteringDirectionAndPolarization(StokesVector* out, const PhotonPackage* pp) const
{
    if (_polarization)
    {
        // determine the angles between the previous and new direction
        int ell = pp->ell();
        double theta = sampleTheta(ell);
        double phi = samplePhi(ell, theta, pp->linearPolarizationDegree(), pp->polarizationAngle());

        // rotate the Stokes vector (and the scattering plane)
        *out = *pp;
        out->rotateStokes(phi, pp->direction());

        // apply Mueller matrix
        int t = indexForTheta(theta, _Ntheta);
        out->applyMueller(_S11vv(ell,t), _S12vv(ell,t), _S33vv(ell,t), _S34vv(ell,t));

        // rotate the propagation direction in the scattering plane
        Vec newdir = pp->direction()*cos(theta) + Vec::cross(out->normal(), pp->direction())*sin(theta);

        // normalize direction to prevent degradation
        return Direction(newdir/newdir.norm());
    }
    else
    {
        double g = _asymmparv[pp->ell()];
        if (fabs(g)<1e-6) return _random->direction();
        double f = ((1.0-g)*(1.0+g))/(1.0-g+2.0*g*_random->uniform());
        double costheta = (1.0+g*g-f*f)/(2.0*g);
        return _random->direction(pp->direction(), costheta);
    }
}

////////////////////////////////////////////////////////////////////

void DustMix::scatteringPeelOffPolarization(StokesVector* out, const PhotonPackage* pp, Direction bfknew,
                                            Direction bfkx, Direction /*bfky*/)
{
    if (_polarization)
    {
        // copy the polarization state
        *out = *pp;

        // rotate the Stokes vector reference direction into the scattering plane
        out->rotateIntoPlane(pp->direction(),bfknew);

        // apply the Mueller matrix
        double theta = acos(Vec::dot(pp->direction(),bfknew));
        int t = indexForTheta(theta, _Ntheta);
        int ell = pp->ell();
        out->applyMueller(_S11vv(ell,t), _S12vv(ell,t), _S33vv(ell,t), _S34vv(ell,t));

        // rotate the Stokes vector reference direction parallel to the instrument frame x-axis
        // it is given bfknew, because the photon is at this point aimed towards the observer,
        // but the propagation direction has not been updated.
        out->rotateIntoPlane(bfknew,bfkx);
    }
}

////////////////////////////////////////////////////////////////////

double DustMix::phaseFunctionValue(const PhotonPackage* pp, Direction bfknew) const
{
    if (_polarization)
    {
        // determine the scattering angles
        double phi = angleBetweenScatteringPlanes(pp->normal(), pp->direction(), bfknew);
        double theta = acos(Vec::dot(pp->direction(),bfknew));

        // calculate the phase function value
        int t = indexForTheta(theta, _Ntheta);
        int ell = pp->ell();
        double polDegree = pp->linearPolarizationDegree();
        double polAngle = pp->polarizationAngle();
        return _pfnormv[ell]*(_S11vv(ell,t)+polDegree*_S12vv(ell,t)*cos(2.*(phi-polAngle)));
    }
    else
    {
        double cosalpha = Direction::dot(pp->direction(), bfknew);
        double g = _asymmparv[pp->ell()];
        double t = 1.0+g*g-2*g*cosalpha;
        return (1.0-g)*(1.0+g)/sqrt(t*t*t);
    }
}

////////////////////////////////////////////////////////////////////

double DustMix::planckabs(double T, int c) const
{
    int p = NR::locate_clip(_Tv, T);
    return NR::interpolate_linlin(T, _Tv[p], _Tv[p+1], _planckabsvv(c,p), _planckabsvv(c,p+1));
}

////////////////////////////////////////////////////////////////////

double DustMix::planckabs(double T) const
{
    return planckabs(T, _Npop);
}

////////////////////////////////////////////////////////////////////

double DustMix::invplanckabs(double planckabs, int c) const
{
    int p = NR::locate_clip(_planckabsvv[c], planckabs);
    return NR::interpolate_linlin(planckabs, _planckabsvv(c,p), _planckabsvv(c,p+1), _Tv[p], _Tv[p+1]);
}

////////////////////////////////////////////////////////////////////

double DustMix::invplanckabs(double planckabs) const
{
    return invplanckabs(planckabs, _Npop);
}

////////////////////////////////////////////////////////////////////

double DustMix::equilibrium(const Array& Jv, int c) const
{
    double planckabs = 0.0;
    for (int ell=0; ell<_Nlambda; ell++)
    {
        planckabs += sigmaabs(ell,c) * Jv[ell] * _lambdagrid->dlambda(ell);
    }
    return invplanckabs(planckabs,c);
}

////////////////////////////////////////////////////////////////////

double DustMix::sampleTheta(int ell) const
{
    return _random->cdf(_thetav, _thetaXvv[ell]);
}

////////////////////////////////////////////////////////////////////

double DustMix::samplePhi(int ell, double theta, double polDegree, double polAngle) const
{
    int t = indexForTheta(theta, _Ntheta);
    double PF = polDegree * _S12vv(ell,t)/_S11vv(ell,t) / (4*M_PI);
    double cos2polAngle = cos(2*polAngle) * PF;
    double sin2polAngle = sin(2*polAngle) * PF;
    const_cast<DustMix*>(this)->_phiXv = _phi1v + cos2polAngle*_phisv + sin2polAngle*_phicv;
    return _random->cdf(_phiv, _phiXv);
}

//////////////////////////////////////////////////////////////////////
