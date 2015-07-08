/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

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

SPHStellarComp::SPHStellarComp()
    : _sedFamily(0), _writeLuminosities(false)
{
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
    const vector<Array>& stars = TextInFile(this, _filename, description).readAllRows(4+Nparams);

    find<Log>()->info("Processing the particle properties... ");

    // provide room for the relevant particle properties and statistics
    //  - the permanent vectors with the particle positions and sizes
    //  - a temporary matrix Lvv with the luminosity of each particle at each wavelength
    //  - the permanent vector _Ltotv with the total luminosity for every wavelength bin
    //  - total luminosity and total mass
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    int Nstars = stars.size();
    _rv.resize(Nstars);
    _hv.resize(Nstars);
    ArrayTable<2> Lvv(Nlambda,Nstars);
    _Ltotv.resize(Nlambda);
    double Ltot = 0;
    double Mtot = 0;

    // actually gather the information
    for (int i=0; i!=Nstars; ++i)
    {
        const Array& star = stars[i];

        _rv[i] = Vec(star[0],star[1],star[2])*pc;
        _hv[i] = star[3]*pc;

        const Array& Lv = _sedFamily->luminosities_generic(star, 4);
        for (int ell=0; ell<Nlambda; ell++)
        {
            Lvv[ell][i] = Lv[ell];
            _Ltotv[ell] += Lv[ell];
            Ltot += Lv[ell];
        }

        Mtot += star[4];  // total mass in Msun
    }
    find<Log>()->info("  Number of particles: " + QString::number(Nstars));
    find<Log>()->info("  Total mass: " + QString::number(Mtot) + " Msun");
    find<Log>()->info("  Total luminosity: " + QString::number(Ltot/Units::Lsun()) + " Lsun");

    // construct the permanent vectors _Xvv with the normalized cumulative luminosities (per wavelength bin)
    _Xvv.resize(Nlambda,0);
    for (int ell=0; ell<Nlambda; ell++)
    {
        NR::cdf(_Xvv[ell], Lvv[ell]);
    }

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
}

//////////////////////////////////////////////////////////////////////
