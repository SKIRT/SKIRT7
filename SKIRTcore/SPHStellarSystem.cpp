/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <fstream>
#include <iomanip>
#include <QFile>
#include "BruzualCharlotSEDFamily.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "SPHStellarSystem.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SPHStellarSystem::SPHStellarSystem()
    : _writeLuminosities(false)
{
}

//////////////////////////////////////////////////////////////////////

void SPHStellarSystem::setupSelfBefore()
{
    StellarSystem::setupSelfBefore();

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

        QString filename = find<FilePaths>()->output("luminosities.dat");
        find<Log>()->info("Writing luminosities to " + filename + "...");
        ofstream file(filename.toLocal8Bit().constData());
        file << "# column 1: lambda (" << units->uwavelength().toStdString() << ");"
             << "  column 2: luminosity (" << units->ubolluminosity().toStdString() << ")\n";
        file << scientific << setprecision(8);
        for (int ell=0; ell<Nlambda; ell++)
        {
            file << units->owavelength(lambdagrid->lambda(ell)) << '\t'
                 << units->obolluminosity(_Ltotv[ell]) << '\n';
        }
    }
}

//////////////////////////////////////////////////////////////////////

void SPHStellarSystem::setFilename(QString value)
{
    _filename = value;
}

//////////////////////////////////////////////////////////////////////

QString SPHStellarSystem::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////

void SPHStellarSystem::setWriteLuminosities(bool value)
{
    _writeLuminosities = value;
}

//////////////////////////////////////////////////////////////////////

bool SPHStellarSystem::writeLuminosities() const
{
    return _writeLuminosities;
}

//////////////////////////////////////////////////////////////////////

double SPHStellarSystem::luminosity(int ell) const
{
    return _Ltotv[ell];
}

//////////////////////////////////////////////////////////////////////

int SPHStellarSystem::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

void SPHStellarSystem::launch(PhotonPackage* pp, int ell, double L) const
{
    int i = NR::locate_clip(_Xvv[ell], _random->uniform());
    double h = _random->gauss()*_hv[i];
    Position bfr = Position(_rv[i] + h*_random->direction());
    Direction bfk = _random->direction();
    pp->set(true,ell,bfr,bfk,L,0);
}

//////////////////////////////////////////////////////////////////////
