/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <QFile>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "SPHGeometry.hpp"
#include "SPHGasParticleGrid.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SPHGeometry::SPHGeometry()
    : _Tmax(0), _grid(0)
{
}

////////////////////////////////////////////////////////////////////

SPHGeometry::~SPHGeometry()
{
    delete _grid;
}

//////////////////////////////////////////////////////////////////////

void SPHGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // get conversion factors for units
    const double pc = Units::pc();
    const double Msun = Units::Msun();

    // read in the SPH gas particles
    QString filepath = find<FilePaths>()->input(_filename);
    QFile infile(filepath);
    if (!infile.open(QIODevice::ReadOnly|QIODevice::Text))
        throw FATALERROR("Could not open the SPH gas data file " + filepath);
    find<Log>()->info("Reading SPH gas particles from file " + filepath + "...");
    int Nignored = 0;
    double Mtot = 0;
    double Mmetal = 0;
    while (!infile.atEnd())
    {
        // read a line, split it in columns, and skip empty and comment lines
        QList<QByteArray> columns = infile.readLine().simplified().split(' ');
        if (!columns.isEmpty() && !columns[0].startsWith('#'))
        {
            // get the optional temperature value; missing or illegal values default to zero
            double T = columns.value(6).toDouble();

            // ignore particle if the temperature is higher than the maximum (assuming both T and Tmax are valid)
            if (T > 0 && _Tmax > 0 && T > _Tmax)
            {
                Nignored++;
            }
            else
            {
                // add a particle using the other column values; missing or illegal values default to zero
                _pv.push_back(SPHGasParticle(Vec(columns.value(0).toDouble()*pc,
                                                 columns.value(1).toDouble()*pc,
                                                 columns.value(2).toDouble()*pc),
                                             columns.value(3).toDouble()*pc,
                                             columns.value(4).toDouble()*Msun,
                                             columns.value(5).toDouble()));
                Mtot += columns.value(4).toDouble();
                Mmetal += columns.value(4).toDouble() * columns.value(5).toDouble();
            }
       }
    }
    infile.close();
    find<Log>()->info("  Number of high-temperature particles ignored: " + QString::number(Nignored));
    find<Log>()->info("  Number of SPH gas particles containing dust: " + QString::number(_pv.size()));
    find<Log>()->info("  Total gas mass: " + QString::number(Mtot) + " Msun");
    find<Log>()->info("  Total metal mass: " + QString::number(Mmetal) + " Msun");

    // construct a 3D-grid over the particle space, and create a list of particles that overlap each grid cell
    const int GRIDSIZE = 20;
    QString size = QString::number(GRIDSIZE);
    find<Log>()->info("Constructing intermediate " + size + "x" + size + "x" + size + " grid for particles...");
    _grid = new SPHGasParticleGrid(_pv, GRIDSIZE);
    find<Log>()->info("  Smallest number of particles per cell: " + QString::number(_grid->minParticlesPerCell()));
    find<Log>()->info("  Largest  number of particles per cell: " + QString::number(_grid->maxParticlesPerCell()));
    find<Log>()->info("  Average  number of particles per cell: "
                      + QString::number(_grid->totalParticles() / double(GRIDSIZE*GRIDSIZE*GRIDSIZE),'f',1));

    // construct a vector with the normalized cumulative particle densities
    NR::cdf(_cumrhov, _pv.size(), [this](int i){return _pv[i].metalMass();} );

    // remember the normalization factor
    _norm = 1. / (Mmetal*Msun);
}

//////////////////////////////////////////////////////////////////////

void SPHGeometry::setFilename(QString value)
{
    _filename = value;
}

//////////////////////////////////////////////////////////////////////

QString SPHGeometry::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////

void SPHGeometry::setMaximumTemperature(double value)
{
    _Tmax = value;
}

//////////////////////////////////////////////////////////////////////

double SPHGeometry::maximumTemperature() const
{
    return _Tmax;
}

//////////////////////////////////////////////////////////////////////

double SPHGeometry::density(Position bfr) const
{
    const vector<const SPHGasParticle*>& particles = _grid->particlesFor(bfr);

    double sum = 0.0;
    int n = particles.size();
    for (int i=0; i<n; i++)
        sum += particles[i]->metalDensity(bfr);  // sum contains the density in metals
    sum *= _norm;    // sum now contains the normalized density
    return sum;
}

//////////////////////////////////////////////////////////////////////

Position SPHGeometry::generatePosition() const
{
    Random* random = find<Random>();
    int i = NR::locate_clip(_cumrhov, random->uniform());
    double r = random->gauss()*_pv[i].radius();
    return Position(_pv[i].center() + r*random->direction());
}

//////////////////////////////////////////////////////////////////////

double SPHGeometry::SigmaX() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double xmin = _grid->xmin();
    double xmax = _grid->xmax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(xmin + k*(xmax-xmin)/NSAMPLES, 0, 0));
    }
    return (sum/NSAMPLES)*(xmax-xmin);
}

//////////////////////////////////////////////////////////////////////

double SPHGeometry::SigmaY() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double ymin = _grid->ymin();
    double ymax = _grid->ymax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(0, ymin + k*(ymax-ymin)/NSAMPLES, 0));
    }
    return (sum/NSAMPLES)*(ymax-ymin);
}

//////////////////////////////////////////////////////////////////////

double SPHGeometry::SigmaZ() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double zmin = _grid->zmin();
    double zmax = _grid->zmax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(0, 0, zmin + k*(zmax-zmin)/NSAMPLES));
    }
    return (sum/NSAMPLES)*(zmax-zmin);
}

//////////////////////////////////////////////////////////////////////

int SPHGeometry::numParticles() const
{
    return _pv.size();
}

//////////////////////////////////////////////////////////////////////

Vec SPHGeometry::particleCenter(int index) const
{
    int n = _pv.size();
    if (index<0 || index>=n) throw FATALERROR("Particle index out of range: " + QString::number(index));

    return _pv[index].center();
}

//////////////////////////////////////////////////////////////////////
