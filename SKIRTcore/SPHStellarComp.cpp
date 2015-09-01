/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AngularDistribution.hpp"
#include "BruzualCharlotSEDFamily.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "SPHStellarComp.hpp"
#include "TextInFile.hpp"
#include "TextOutFile.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"
#include <mutex>

using namespace std;

//////////////////////////////////////////////////////////////////////

namespace SPHStellarComp_Private
{
    Array _costhetav;            // grid of cos(theta) values indexed on t
    const int _Ncostheta = 45;   // number of values in cos(theta) grid
    std::once_flag _costhetav_initialized; // flag indicating whether cos(theta) grid has been initialized

    /** An instance of this class holds all relevant luminosity information to implement the
        AngularDistribution interface for an SPH source particle with velocity information. */
    class VelocityAnisotropy : public AngularDistribution
    {
    public:
        /** The constructor */
        VelocityAnisotropy(const Array& particle, const SEDFamily* sedFamily, Random* random)
            : _random(random)
        {
            // get velocity, converting from km/s to m/s, and remember normalized direction of velocity
            Vec bfv = Vec(particle[4],particle[5],particle[6])*1e3;
            _bfkv = Direction(bfv/bfv.norm());

            // initialize the global cos(theta) grid upon first invocation
            std::call_once(_costhetav_initialized, [] { NR::lingrid(_costhetav, -1., +1., _Ncostheta-1); });

            // TODO: properly initialize _Lvv and _Xvv
            (void)sedFamily;
        }

        /** This function returns the probability \f$P(\Omega)\f$ for a given direction
            \f$(\theta,\phi)\f$ for the particle represented by this instance. The function ignores
            the specified position, which is assumed to be near the position of the particle. */
        double probabilityForDirection(Position /*bfr*/, Direction bfk) const
        {
            int ell = 0; // TODO: this will become a parameter of the function!
            double costheta = Vec::dot(bfk,_bfkv);
            int t = NR::locate_clip(_costhetav, costheta);
            return NR::interpolate_linlin(costheta, _costhetav[t], _costhetav[t+1], _Lvv(ell,t), _Lvv(ell,t+1));
        }

        /** This function generates a random direction \f$(\theta,\phi)\f$ drawn from the
            probability distribution \f$P(\Omega)\,{\mathrm{d}}\Omega\f$ for the particle
            represented by this instance. The function ignores the specified position, which is
            assumed to be near the position of the particle. */
        Direction generateDirection(Position /*bfr*/) const
        {
            int ell = 0; // TODO: this will become a parameter of the function!
            double costheta = _random->cdf(_costhetav, _Xvv[ell]);
            return _random->direction(_bfkv, costheta);
        }

    private:
        Random* _random;    // pointer to the simulation's random generator
        Direction _bfkv;    // unit vector along the direction of the particle's velocity
        ArrayTable<2> _Lvv; // [ell,t] normalized luminosity distribution over cos(theta), for each wavelength index
        ArrayTable<2> _Xvv; // [ell,t] cumulative luminosity distribution over cos(theta), for each wavelength index

    };
}

//////////////////////////////////////////////////////////////////////

SPHStellarComp::SPHStellarComp()
    : _sedFamily(0), _writeLuminosities(false), _velocity(false)
{
}

//////////////////////////////////////////////////////////////////////

SPHStellarComp::~SPHStellarComp()
{
    // destroy any anisotropy objects created during setup
    for (auto a : _av) delete a;
}

//////////////////////////////////////////////////////////////////////

void SPHStellarComp::setupSelfBefore()
{
    StellarComp::setupSelfBefore();

    // make sure we have an SED family
    if (!_sedFamily) throw FATALERROR("SED family was not specified");

    // cache the random generator
    _random = find<Random>();
}

//////////////////////////////////////////////////////////////////////

void SPHStellarComp::setupSelfAfter()
{
    StellarComp::setupSelfAfter();

    // local constant for units
    const double pc = Units::pc();

    // load the SPH source particles, including the parameters for our SED family
    // and including the velocity, if requested
    int Nbase = _velocity ? 7 : 4;
    int Nsed = _sedFamily->nparams();
    QString description = "SPH " + _sedFamily->sourceDescription() + " particles";
    const vector<Array>& particles = TextInFile(this, _filename, description).readAllRows(Nbase+Nsed);

    find<Log>()->info("Processing the particle properties... ");

    // store the particle positions and sizes, and calculate the total mass in Msun
    int Np = particles.size();
    _rv.resize(Np);
    _hv.resize(Np);
    double Mtot = 0;
    for (int i=0; i!=Np; ++i)
    {
        const Array& particle = particles[i];
        _rv[i] = Vec(particle[0],particle[1],particle[2])*pc;
        _hv[i] = particle[3]*pc;
        Mtot += _sedFamily->mass_generic(particle, Nbase);
    }

    // construct a temporary matrix with the luminosity of each particle at each wavelength
    ArrayTable<2> Lvv(Np,0);  // [i,ell]
    for (int i=0; i!=Np; ++i)
    {
        Lvv[i] = _sedFamily->luminosities_generic(particles[i], Nbase);
    }

    // calculate the total luminosity for every wavelength bin, and the grand total luminosity
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _Ltotv.resize(Nlambda);
    for (int i=0; i!=Np; ++i) _Ltotv += Lvv[i];
    double Ltot = _Ltotv.sum();

    // construct the normalized cumulative luminosity distribution over particles, for each wavelength bin
    _Xvv.resize(Nlambda,0);  // [ell,i]
    for (int ell=0; ell<Nlambda; ell++)
    {
        NR::cdf(_Xvv[ell], Np, [&Lvv, ell](int i) { return Lvv(i,ell); });
    }

    // construct anisotropy information for each particle, if requested
    if (_velocity)
    {
        _av.resize(Np);
        for (int i=0; i!=Np; ++i)
        {
            _av[i] = new SPHStellarComp_Private::VelocityAnisotropy(particles[i], _sedFamily, _random);
        }
    }

    // log key statistics
    find<Log>()->info("  Number of particles: " + QString::number(Np));
    find<Log>()->info("  Total mass: " + QString::number(Mtot) + " Msun");
    find<Log>()->info("  Total luminosity: " + QString::number(Ltot/Units::Lsun()) + " Lsun");

    // if requested, write a data file with the luminosities per wavelength
    if (_writeLuminosities)
    {
        Units* units = find<Units>();
        WavelengthGrid* lambdagrid = find<WavelengthGrid>();

        // Create a text file
        TextOutFile file(this, _sedFamily->sourceName() + "_luminosities", "SPH source luminosities");

        // Write the header
        file.addColumn("lambda (" + units->uwavelength() + ")");
        file.addColumn("luminosity (" + units->ubolluminosity() + ")");

        // Write the body
        for (int ell=0; ell<Nlambda; ell++)
        {
            file.writeRow(QList<double>() << units->owavelength(lambdagrid->lambda(ell))
                                          << units->obolluminosity(_Ltotv[ell]));
        }
    }
}

//////////////////////////////////////////////////////////////////////

void SPHStellarComp::setFilename(QString value)
{
    _filename = value;
}

//////////////////////////////////////////////////////////////////////

QString SPHStellarComp::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////

void SPHStellarComp::setSedFamily(SEDFamily* value)
{
    if (_sedFamily) delete _sedFamily;
    _sedFamily = value;
    if (_sedFamily) _sedFamily->setParent(this);
}

//////////////////////////////////////////////////////////////////////

SEDFamily*SPHStellarComp::sedFamily() const
{
    return _sedFamily;
}

//////////////////////////////////////////////////////////////////////

void SPHStellarComp::setWriteLuminosities(bool value)
{
    _writeLuminosities = value;
}

//////////////////////////////////////////////////////////////////////

bool SPHStellarComp::writeLuminosities() const
{
    return _writeLuminosities;
}

//////////////////////////////////////////////////////////////////////

int SPHStellarComp::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

double SPHStellarComp::luminosity(int ell) const
{
    return _Ltotv[ell];
}

//////////////////////////////////////////////////////////////////////

void SPHStellarComp::launch(PhotonPackage* pp, int ell, double L) const
{
    // select random particle
    int i = NR::locate_clip(_Xvv[ell], _random->uniform());

    // determine random position in Gaussian particle
    double x = _random->gauss();
    double y = _random->gauss();
    double z = _random->gauss();
    Position bfr( _rv[i] + Vec(x,y,z) * (_hv[i] / 2.42 / M_SQRT2) );

    // if we have velocity data, launch using the particle's anisotropic luminosity distribution
    if (_velocity)
    {
        pp->launch(L, ell, bfr, _av[i]->generateDirection(bfr));
        pp->setAngularDistribution(_av[i]);
    }
    // otherwise launch using the default isotropic distribution
    else
    {
        pp->launch(L, ell, bfr, _random->direction());
    }
}

//////////////////////////////////////////////////////////////////////
