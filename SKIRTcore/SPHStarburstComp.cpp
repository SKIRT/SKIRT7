/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QFile>
#include "MappingsSEDFamily.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "SPHStarburstComp.hpp"
#include "TextFile.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SPHStarburstComp::SPHStarburstComp()
    : _writeLuminosities(false)
{
}

//////////////////////////////////////////////////////////////////////

void SPHStarburstComp::setupSelfBefore()
{
    StellarComp::setupSelfBefore();

    // cache the random generator
    _random = find<Random>();

    // local constant for units
    const double pc = Units::pc();

    // vectors storing the particle attributes (r and h are data members since we need those after setup)
    std::vector<double> SFRv;
    std::vector<double> Zv;
    std::vector<double> logCv;
    std::vector<double> Pv;
    std::vector<double> fPDRv;

    // load the SPH star particles
    QString filepath = find<FilePaths>()->input(_filename);
    QFile infile(filepath);
    if (!infile.open(QIODevice::ReadOnly|QIODevice::Text))
        throw FATALERROR("Could not open the SPH HII region data file " + filepath);
    find<Log>()->info("Reading SPH HII region particles from file " + filepath + "...");
    int Nparts = 0;
    double SFRtot = 0;
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
            SFRv.push_back(columns.value(4).toDouble());  // star formation rate in Msun / yr
            Zv.push_back(columns.value(5).toDouble());    // metallicity as dimensionless fraction
            logCv.push_back(columns.value(6).toDouble()); // log compactness (Groves 2008) as dimensionless value
            Pv.push_back(columns.value(7).toDouble());    // ISM pressure in cgs units
            fPDRv.push_back(columns.value(8).toDouble()); // dimensionless photo-dissociation region covering fraction

            Nparts++;
            SFRtot += columns.value(4).toDouble();
        }
    }
    infile.close();

    double Mtot = SFRtot * 1.e7;  // total stellar mass from total sfr over the last 10 Myr

    find<Log>()->info("  Total number of SPH HII region particles: " + QString::number(Nparts));
    find<Log>()->info("  Total stellar mass: " + QString::number(Mtot) + " Msun");

    find<Log>()->info("Filling the vectors with the SEDs of the particles... ");

    // construct the library of SED models
    MappingsSEDFamily map(this);

    // construct a temporary matrix Lvv with the luminosity of each particle at each wavelength
    // and also the permanent vector _Ltotv with the total luminosity for every wavelength bin
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    ArrayTable<2> Lvv(Nlambda,Nparts);
    _Ltotv.resize(Nlambda);
    double Ltot = 0;
    for (int i=0; i<Nparts; i++)
    {
        const Array& Lv = map.luminosities(SFRv[i], Zv[i], logCv[i], Pv[i], fPDRv[i]);
        for (int ell=0; ell<Nlambda; ell++)
        {
            Lvv[ell][i] = Lv[ell];
            _Ltotv[ell] += Lv[ell];
            Ltot += Lv[ell];
        }
    }
    find<Log>()->info("  HII luminosity: " + QString::number(Ltot/Units::Lsun()) + " Lsun");

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

        QString filename = find<FilePaths>()->output("HII_luminosities.dat");
        find<Log>()->info("Writing luminosities to " + filename + "...");

        // Create a text file
        TextFile file(filename);

        file.writeLine("# column 1: lambda (" + units->uwavelength() + ");"
                     + "  column 2: luminosity (" + units->ubolluminosity() + ")");
        for (int ell=0; ell<Nlambda; ell++)
        {
            file.writeLine(QString::number(units->owavelength(lambdagrid->lambda(ell)), 'e', 8) + '\t'
                        + QString::number(units->obolluminosity(_Ltotv[ell]), 'e', 8));
        }
    }
}

//////////////////////////////////////////////////////////////////////

void SPHStarburstComp::setFilename(QString value)
{
    _filename = value;
}

//////////////////////////////////////////////////////////////////////

QString SPHStarburstComp::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////

void SPHStarburstComp::setWriteLuminosities(bool value)
{
    _writeLuminosities = value;
}

//////////////////////////////////////////////////////////////////////

bool SPHStarburstComp::writeLuminosities() const
{
    return _writeLuminosities;
}

//////////////////////////////////////////////////////////////////////

int SPHStarburstComp::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

double SPHStarburstComp::luminosity(int ell) const
{
    return _Ltotv[ell];
}

//////////////////////////////////////////////////////////////////////

void SPHStarburstComp::launch(PhotonPackage* pp, int ell, double L) const
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
