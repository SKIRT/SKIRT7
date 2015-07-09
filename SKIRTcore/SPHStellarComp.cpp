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

using namespace std;

//////////////////////////////////////////////////////////////////////

namespace SPHStellarComp_Private
{
    /** An instance of this class holds all relevant luminosity information to implement the
      AngularDistribution interface for an SPH source particle with velocity information. */
    class VelocityAnisotropy : public AngularDistribution
    {
    public:
        /** The constructor */
        VelocityAnisotropy() { }

        /** This function returns the probability \f$P(\Omega)\f$ for a given direction
            \f$(\theta,\phi)\f$ at the specified position. We assume that the specified position
            matches the position of the particle represented by this instance. */
        double probabilityForDirection(Position /*bfr*/, Direction bfk) const
        {
            (void)bfk;
            return 0;
        }

        /** This function generates a random direction \f$(\theta,\phi)\f$ drawn from the
            probability distribution \f$P(\Omega)\,{\mathrm{d}}\Omega\f$ at the specified position.
            We assume that the specified position matches the position of the particle represented
            by this instance. */
        Direction generateDirection(Position /*bfr*/) const
        {
            return Direction();
        }
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
    int Nparams = _sedFamily->nparams();
    QString description = "SPH " + _sedFamily->sourceDescription() + " particles";
    const vector<Array>& particles = TextInFile(this, _filename, description).readAllRows(4+Nparams);

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
        Mtot += _sedFamily->mass_generic(particle, 4);
    }

    // construct a temporary matrix with the luminosity of each particle at each wavelength
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    ArrayTable<2> Lvv(Np,0);  // [i,ell]
    for (int i=0; i!=Np; ++i)
    {
        Lvv[i] = _sedFamily->luminosities_generic(particles[i], 4);
    }

    // calculate the total luminosity for every wavelength bin, and the grand total luminosity
    _Ltotv.resize(Nlambda);
    double Ltot = 0;
    for (int i=0; i!=Np; ++i)
    {
        for (int ell=0; ell<Nlambda; ell++)
        {
            _Ltotv[ell] += Lvv(i,ell);
            Ltot += Lvv(i,ell);
        }
    }

    // construct the normalized cumulative luminosity distribution over particles, for each wavelength bin
    _Xvv.resize(Nlambda,0);  // [ell,i]
    for (int ell=0; ell<Nlambda; ell++)
    {
        NR::cdf(_Xvv[ell], Np, [&Lvv, ell](int i) { return Lvv(i,ell); });
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
    int i = NR::locate_clip(_Xvv[ell], _random->uniform());
    double x = _random->gauss();
    double y = _random->gauss();
    double z = _random->gauss();
    Position bfr( _rv[i] + Vec(x,y,z) * (_hv[i] / 2.42 / M_SQRT2) );
    Direction bfk( _random->direction() );
    pp->launch(L,ell,bfr,bfk);

    // if we have particle velocity data, setup the resulting anisotropic luminosity distribution
    if (_velocity) pp->setAngularDistribution(_av[i]);
}

//////////////////////////////////////////////////////////////////////
