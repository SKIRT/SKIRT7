/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QFile>
#include "BruzualCharlotSEDFamily.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "SPHStellarComp.hpp"
#include "TextOutFile.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SPHStellarComp::SPHStellarComp()
    : _writeLuminosities(false)
{
}

//////////////////////////////////////////////////////////////////////

void SPHStellarComp::setupSelfBefore()
{
    StellarComp::setupSelfBefore();

    // cache the random generator
    _random = find<Random>();

    // local constant for units
    const double pc = Units::pc();

    // load the SPH star particles
    QString filepath = find<FilePaths>()->input(_filename);
    QFile infile(filepath);
    if (!infile.open(QIODevice::ReadOnly|QIODevice::Text))
        throw FATALERROR("Could not open the SPH star data file " + filepath);
    find<Log>()->info("Reading SPH star particles from file " + filepath + "...");
    int Nstars = 0;
    double Mtot = 0;
    while (!infile.atEnd())
    {
        // read a line, split it in columns, and skip empty and comment lines
        QList<QByteArray> columns = infile.readLine().simplified().split(' ');
        if (!columns.isEmpty() && !columns[0].startsWith('#'))
        {
            // get the column values; missing or illegal values default to zero
            _rv.push_back(Vec(columns.value(0).toDouble()*pc,
                              columns.value(1).toDouble()*pc,
                              columns.value(2).toDouble()*pc));
            _hv.push_back(columns.value(3).toDouble()*pc);
            _Mv.push_back(columns.value(4).toDouble());  // mass in Msun
            _Zv.push_back(columns.value(5).toDouble());  // metallicity as dimensionless fraction
            _tv.push_back(columns.value(6).toDouble());  // age in years
            Nstars++;
            Mtot += columns.value(4).toDouble();
        }
    }
    infile.close();
    find<Log>()->info("  Total number of SPH star particles: " + QString::number(Nstars));
    find<Log>()->info("  Total stellar mass: " + QString::number(Mtot) + " Msun");

    find<Log>()->info("Filling the vectors with the SEDs of the particles... ");

    // construct the library of SED models
    BruzualCharlotSEDFamily bc(this);

    // construct a temporary matrix Lvv with the luminosity of each particle at each wavelength
    // and also the permanent vector _Ltotv with the total luminosity for every wavelength bin
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    ArrayTable<2> Lvv(Nlambda,Nstars);
    _Ltotv.resize(Nlambda);
    double Ltot = 0;
    for (int i=0; i<Nstars; i++)
    {
        const Array& Lv = bc.luminosities(_Mv[i], _Zv[i], _tv[i]);
        for (int ell=0; ell<Nlambda; ell++)
        {
            Lvv[ell][i] = Lv[ell];
            _Ltotv[ell] += Lv[ell];
            Ltot += Lv[ell];
        }
    }
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
        TextOutFile file(this, "luminosities", "luminosities");

        // Write the header
        file.addColumn("lambda (" + units->uwavelength() + ")", 'e', 8);
        file.addColumn("luminosity (" + units->ubolluminosity() + ")", 'e', 8);

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
