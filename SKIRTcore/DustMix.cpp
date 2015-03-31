/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include <iomanip>
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

        // create a table with the cumulative distribution of theta for each wavelength
        _thetaXvv.resize(_Nlambda,0);
        for (int ell=0; ell<_Nlambda; ell++)
        {
            NR::cdf(_thetaXvv[ell], _Ntheta-1, [this,ell,dt](int t){ return _S11vv(ell,t+1)*sin(_thetav[t+1])*dt; });
        }

        // create a table with the phase function normalization factor for each wavelength
        _pfnormv.resize(_Nlambda);
        for (int ell=0; ell<_Nlambda; ell++)
        {
            double N = 0.;
            for (int t=0; t<_Ntheta; t++)
            {
                N += _S11vv(ell,t)*sin(_thetav[t])*dt;
            }
            _pfnormv[ell] = 1./(2.0*M_PI*N);
        }

        // create tables listing phi, phi/2pi and sin(2phi)/4pi for a number of phi indices
        _phiv.resize(_Nphi);
        _phi1v.resize(_Nphi);
        _phi2v.resize(_Nphi);
        _phiXv.resize(_Nphi);
        double df = 2*M_PI/(_Nphi-1);
        for (int f=0; f<_Nphi; f++)
        {
            double phi = f * df;
            _phiv[f] = phi;
            _phi1v[f] = phi/(2*M_PI);
            _phi2v[f] = sin(2*phi)/(4*M_PI);
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

    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();

    // output the optical properties for all dust populations
    if (_writeMix && comm->isRoot())
    {
        QString filename = find<FilePaths>()->output("ds_mix_" + QString::number(h) + "_opti.dat");
        log->info("Writing optical dust population properties to " + filename + "...");
        ofstream file(filename.toLocal8Bit().constData());
        file << setprecision(6) << scientific;
        string section = units->usection().toStdString() + " per hydrogen atom";
        int col = 0;
        file << "# col " << ++col << ": lambda (" << units->uwavelength().toStdString() << ")\n";
        for (int c=0; c<_Npop; c++) file << "# col " << ++col << ": sigma_ext[" << c << "] (" << section << ")\n";
        for (int c=0; c<_Npop; c++) file << "# col " << ++col << ": sigma_abs[" << c << "] (" << section << ")\n";
        for (int c=0; c<_Npop; c++) file << "# col " << ++col << ": sigma_sca[" << c << "] (" << section << ")\n";
        for (int c=0; c<_Npop; c++) file << "# col " << ++col << ": asymmpar[" << c << "]\n";
        for (int ell=0; ell<_Nlambda; ell++)
        {
            file << units->owavelength(_lambdagrid->lambda(ell));
            for (int c=0; c<_Npop; c++) file << ' ' << units->osection(_sigmaabsvv[c][ell]+_sigmascavv[c][ell]);
            for (int c=0; c<_Npop; c++) file << ' ' << units->osection(_sigmaabsvv[c][ell]);
            for (int c=0; c<_Npop; c++) file << ' ' << units->osection(_sigmascavv[c][ell]);
            for (int c=0; c<_Npop; c++) file << ' ' << _asymmparvv[c][ell];
            file << '\n';
        }
        file.close();
        log->info("File " + filename + " created.");
    }

    // output the mass properties for all dust populations
    if (_writeMix && comm->isRoot())
    {
        QString filename = find<FilePaths>()->output("ds_mix_" + QString::number(h) + "_mass.dat");
        log->info("Writing dust population masses to " + filename + "...");
        ofstream file(filename.toLocal8Bit().constData());
        file << setprecision(6) << scientific;
        string bulkmass = units->ubulkmass().toStdString() + " per hydrogen atom";
        file << "# total dust mass: " << units->obulkmass(_mu) << ' ' << bulkmass << '\n';
        file << "# col 1: dust mix population index\n";
        file << "# col 2: dust mass (" << bulkmass << ")\n";
        file << "# col 3: dust mass (% of total)\n";
        for (int c=0; c<_Npop; c++)
        {
            file << setprecision(6) << scientific;
            file << c << '\t' << units->obulkmass(_muv[c]);
            file << setprecision(3) << fixed;
            file << '\t' << 100.*_muv[c]/_mu << '\n';
        }
        file.close();
        log->info("File " + filename + " created.");
    }

    // output the combined optical properties for the dust mixture
    if (_writeMeanMix && comm->isRoot())
    {
        QString filename = find<FilePaths>()->output("ds_mix_" + QString::number(h) + "_mean.dat");
        log->info("Writing combined dust mix properties to " + filename + "...");
        ofstream file(filename.toLocal8Bit().constData());
        file << setprecision(6) << scientific;
        file << "# col 1: lambda (" << units->uwavelength().toStdString() << ")\n";
        file << "# col 2: total kappa_ext (" << units->uopacity().toStdString() << ")\n";
        file << "# col 3: total kappa_abs (" << units->uopacity().toStdString() << ")\n";
        file << "# col 4: total kappa_sca (" << units->uopacity().toStdString() << ")\n";
        file << "# col 5: mean asymmpar\n";
        for (int ell=0; ell<_Nlambda; ell++)
        {
            file << units->owavelength(_lambdagrid->lambda(ell)) << ' '
                 << units->oopacity(_kappaextv[ell]) << ' '
                 << units->oopacity(_kappaabsv[ell]) << ' '
                 << units->oopacity(_kappascav[ell]) << ' '
                 << _asymmparv[ell] << '\n';
        }
        file.close();
        log->info("File " + filename + " created.");
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
    // given the previous, current and new propagation directions of the photon package.
    // It returns a zero angle when the previous and/or current scattering event is completely
    // forward or backward, because then one or both of the scattering planes are ill-defined.
    double angleBetweenScatteringPlanes(Direction kp, Direction kc, Direction kn)
    {
        Vec np = Vec::cross(kp,kc);
        Vec nc = Vec::cross(kc,kn);
        np /= np.norm();
        nc /= nc.norm();
        double cosphi = Vec::dot(np,nc);
        double sinphi = Vec::dot(Vec::cross(np,nc), kc);
        double phi = atan2(sinphi,cosphi);
        if (isfinite(phi)) return phi;
        return 0;
    }

    // This helper function returns the angle alpha between the reference axis in the peel-off scattering plane
    // and the x-axis of the instrument frame, given the current propagation direction of the photon package,
    // the new peel-off direction towards the instrument, and the direction of the instrument frame's x- and y-axes
    // expressed in model coordinates.
    // It returns a zero angle when the peel-off scattering event is completely forward or backward,
    // because then the scattering plane is ill-defined; and when the peel-off scattering plane is parallel to
    // the instrument plane (which means there can't be any detection).
    double angleBetweenScatteringAndInstrumentReference(Direction kc, Direction kn, Direction kx, Direction ky)
    {
        Vec nobs = Vec::cross(kx,ky);
        Vec ks = Vec::cross(Vec::cross(kc,kn), nobs);
        ks /= ks.norm();
        double cosalpha = Vec::dot(ks,kx);
        double sinalpha = Vec::dot(Vec::cross(ks,kx), nobs);
        double alpha = atan2(sinalpha,cosalpha);
        if (isfinite(alpha)) return alpha;
        return 0;
    }
}

////////////////////////////////////////////////////////////////////

Direction DustMix::scatteringDirectionAndPolarization(StokesVector* out, const PhotonPackage* pp) const
{
    // determine the angles between the previous and new direction
    double costheta, sintheta, cosphi, sinphi;
    if (_polarization)
    {
        int ell = pp->ell();
        double theta = sampleTheta(ell);
        double phi = pp->polarizationAngle() + samplePhi(ell, theta, pp->linearPolarizationDegree());
        cosphi = cos(phi);
        sinphi = sin(phi);
        costheta = cos(theta);
        sintheta = sin(theta);

        // also calculate and store the new polarization state
        *out = *pp;
        out->rotateStokes(phi);
        int t = indexForTheta(theta, _Ntheta);
        out->applyMueller(_S11vv(ell,t), _S12vv(ell,t), _S33vv(ell,t), _S34vv(ell,t));
    }
    else
    {
        double g = _asymmparv[pp->ell()];
        if (fabs(g)<1e-6) return _random->direction();
        double phi = 2.0*M_PI*_random->uniform();
        cosphi = cos(phi);
        sinphi = sin(phi);
        double f = ((1.0-g)*(1.0+g))/(1.0-g+2.0*g*_random->uniform());
        costheta = (1.0+g*g-f*f)/(2.0*g);
        sintheta = sqrt(fabs((1.0-costheta)*(1.0+costheta)));
    }

    // determine the new direction from the old direction and the relative change
    double kx, ky, kz;
    pp->direction().cartesian(kx,ky,kz);
    double kxnew, kynew, kznew;
    if (kz>0.99999)
    {
        kxnew = cosphi * sintheta;
        kynew = sinphi * sintheta;
        kznew = costheta;
    }
    else if (kz<-0.99999)
    {
        kxnew = cosphi * sintheta;
        kynew = sinphi * sintheta;
        kznew = -costheta;
    }
    else
    {
        double root = sqrt(fabs((1.0-kz)*(1.0+kz)));
        kxnew = sintheta/root*(-kx*kz*cosphi+ky*sinphi) + kx*costheta;
        kynew = -sintheta/root*(ky*kz*cosphi+kx*sinphi) + ky*costheta;
        kznew = root*sintheta*cosphi + kz*costheta;
    }
    return Direction(kxnew,kynew,kznew);
}

////////////////////////////////////////////////////////////////////

void DustMix::scatteringPeelOffPolarization(StokesVector* out, const PhotonPackage* pp, Direction bfknew,
                                            Direction bfkx, Direction bfky)
{
    if (_polarization)
    {
        // copy the polarization state
        *out = *pp;

        // rotate over the angle between scattering planes
        double phi = angleBetweenScatteringPlanes(pp->previousDirection(), pp->direction(), bfknew);
        if (phi) out->rotateStokes(phi);

        // apply the Mueller matrix
        double theta = acos(Vec::dot(pp->direction(),bfknew));
        int t = indexForTheta(theta, _Ntheta);
        int ell = pp->ell();
        out->applyMueller(_S11vv(ell,t), _S12vv(ell,t), _S33vv(ell,t), _S34vv(ell,t));

        // rotate over the angle between the reference axis in the peel-off scattering plane
        // and the x-axis in the instrument frame
        double alpha = angleBetweenScatteringAndInstrumentReference(pp->direction(), bfknew, bfkx, bfky);
        if (alpha) out->rotateStokes(alpha);
    }
}

////////////////////////////////////////////////////////////////////

double DustMix::phaseFunctionValue(const PhotonPackage* pp, Direction bfknew) const
{
    if (_polarization)
    {
        // determine the scattering angles
        double phi = angleBetweenScatteringPlanes(pp->previousDirection(), pp->direction(), bfknew);
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

double DustMix::samplePhi(int ell, double theta, double polDegree) const
{
    int t = indexForTheta(theta, _Ntheta);
    double PS = _S12vv(ell,t)/_S11vv(ell,t)*polDegree;
    const_cast<DustMix*>(this)->_phiXv = _phi1v + PS*_phi2v;
    return _random->cdf(_phiv, _phiXv);
}

//////////////////////////////////////////////////////////////////////
