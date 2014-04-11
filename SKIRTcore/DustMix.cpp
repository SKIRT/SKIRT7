/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <fstream>
#include <iomanip>
#include "DustDistribution.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PlanckFunction.hpp"
#include "Random.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DustMix::DustMix()
    : _writeMix(true), _writeMeanMix(true), _lambdagrid(0), _Nlambda(0), _random(0), _Npop(0), _mu(0)
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
    if (_writeMix)
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
    if (_writeMeanMix)
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

//////////////////////////////////////////////////////////////////////

double DustMix::asymmpar(int ell, int c) const
{
    return _asymmparvv[c][ell];
}

//////////////////////////////////////////////////////////////////////

double DustMix::asymmpar(int ell) const
{
    return _asymmparv[ell];
}

////////////////////////////////////////////////////////////////////

namespace
{
    double static_phasefunction(double g, double cosalpha)
    {
        double t = 1.0+g*g-2*g*cosalpha;
        return (1.0-g)*(1.0+g)/sqrt(t*t*t);
    }
}

////////////////////////////////////////////////////////////////////

double DustMix::phasefunction(int ell, int c, Direction bfk1, Direction bfk2) const
{
    return static_phasefunction(asymmpar(ell,c), Direction::dot(bfk1,bfk2));
}

////////////////////////////////////////////////////////////////////

double DustMix::phasefunction(int ell, Direction bfk1, Direction bfk2) const
{
    return static_phasefunction(asymmpar(ell), Direction::dot(bfk1,bfk2));
}

////////////////////////////////////////////////////////////////////

namespace
{
    Direction static_newdirection(Random* random, double g, Direction bfk)
    {
        if (fabs(g)<1e-6) return random->direction();
        double phiprime = 2.0*M_PI*random->uniform();
        double cosphiprime = cos(phiprime);
        double sinphiprime = sin(phiprime);
        double f = ((1.0-g)*(1.0+g))/(1.0-g+2.0*g*random->uniform());
        double costhetaprime = (1.0+g*g-f*f)/(2.0*g);
        double sinthetaprime = sqrt(fabs((1.0-costhetaprime)*(1.0+costhetaprime)));
        double kx, ky, kz;
        bfk.cartesian(kx,ky,kz);
        double kxnew, kynew, kznew;
        if (kz>0.99999)
        {
            kxnew = cosphiprime * sinthetaprime;
            kynew = sinphiprime * sinthetaprime;
            kznew = costhetaprime;
        }
        else if (kz<-0.99999)
        {
            kxnew = cosphiprime * sinthetaprime;
            kynew = sinphiprime * sinthetaprime;
            kznew = -costhetaprime;
        }
        else
        {
            double root = sqrt(fabs((1.0-kz)*(1.0+kz)));
            kxnew = sinthetaprime/root*(-kx*kz*cosphiprime+ky*sinphiprime)
                    + kx*costhetaprime;
            kynew = -sinthetaprime/root*(ky*kz*cosphiprime+kx*sinphiprime)
                    + ky*costhetaprime;
            kznew = root*sinthetaprime*cosphiprime
                    + kz*costhetaprime;
        }
        return Direction(kxnew,kynew,kznew);
    }
}

////////////////////////////////////////////////////////////////////

Direction DustMix::generatenewdirection(int ell, int c, Direction bfk) const
{
    return static_newdirection(_random, asymmpar(ell,c), bfk);
}

////////////////////////////////////////////////////////////////////

Direction DustMix::generatenewdirection(int ell, Direction bfk) const
{
    return static_newdirection(_random, asymmpar(ell), bfk);
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

//////////////////////////////////////////////////////////////////////
