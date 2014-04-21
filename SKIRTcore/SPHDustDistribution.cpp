/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <QFile>
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "SPHDustDistribution.hpp"
#include "SPHGasParticleGrid.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////


SPHDustDistribution::SPHDustDistribution()
    : _fdust(0), _Tmax(0), _mix(0), _grid(0)
{
}

////////////////////////////////////////////////////////////////////

SPHDustDistribution::~SPHDustDistribution()
{
    delete _grid;
}

//////////////////////////////////////////////////////////////////////

void SPHDustDistribution::setupSelfBefore()
{
    // verify appropriate setup
    DustDistribution::setupSelfBefore();
    if (!_mix) throw FATALERROR("Dust mix was not set");

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
            }
       }
    }
    infile.close();
    find<Log>()->info("  Number of high-temperature particles ignored: " + QString::number(Nignored));
    find<Log>()->info("  Number of SPH gas particles containing dust: " + QString::number(_pv.size()));
    find<Log>()->info("  Total gas mass: " + QString::number(Mtot) + " Msun");

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
}

//////////////////////////////////////////////////////////////////////

void SPHDustDistribution::setFilename(QString value)
{
    _filename = value;
}

//////////////////////////////////////////////////////////////////////

QString SPHDustDistribution::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////

void SPHDustDistribution::setDustFraction(double value)
{
    if (value <= 0.0) throw FATALERROR("The dust fraction should be positive");
    _fdust = value;
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::dustFraction() const
{
    return _fdust;
}

//////////////////////////////////////////////////////////////////////

void SPHDustDistribution::setMaximumTemperature(double value)
{
    _Tmax = value;
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::maximumTemperature() const
{
    return _Tmax;
}

//////////////////////////////////////////////////////////////////////

void SPHDustDistribution::setDustMix(DustMix *value)
{
    if (_mix) delete _mix;
    _mix = value;
    if (_mix) _mix->setParent(this);
}

//////////////////////////////////////////////////////////////////////

DustMix *SPHDustDistribution::dustMix() const
{
    return _mix;
}

//////////////////////////////////////////////////////////////////////

int SPHDustDistribution::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

int SPHDustDistribution::Ncomp() const
{
    return 1;
}

//////////////////////////////////////////////////////////////////////

DustMix* SPHDustDistribution::mix(int h) const
{
    if (h!=0) throw FATALERROR("Wrong value for h (" + QString::number(h) + ")");
    return _mix;
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::density(int h, Position bfr) const
{
    if (h!=0) throw FATALERROR("Wrong value for h (" + QString::number(h) + ")");
    return density(bfr);
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::density(Position bfr) const
{
    const vector<const SPHGasParticle*>& particles = _grid->particlesFor(bfr);

    double sum = 0.0;
    int n = particles.size();
    for (int i=0; i<n; i++)
        sum += particles[i]->metalDensity(bfr);  // sum contains the total density in metals
    sum *= _fdust;    // sum now contains the total density in metals locked up in dust grains
    return sum;
}

//////////////////////////////////////////////////////////////////////

Position SPHDustDistribution::generatePosition() const
{
    Random* random = find<Random>();
    int i = NR::locate_clip(_cumrhov, random->uniform());
    double r = random->gauss()*_pv[i].radius();
    return Position(_pv[i].center() + r*random->direction());
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::massInBox(int h, const Box& box) const
{
    if (h!=0) throw FATALERROR("Wrong value for h (" + QString::number(h) + ")");
    return massInBox(box);
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::massInBox(const Box& box) const
{
    const vector<const SPHGasParticle*>& particles = _grid->particlesFor(box);

    double sum = 0.0;
    int n = particles.size();
    for (int i=0; i<n; i++)
        sum += particles[i]->metalMassInBox(box);  // total mass in metals
    sum *= _fdust;    // total mass in metals locked up in dust grains
    return sum;
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::mass() const
{
    double sum = 0.0;
    int n = _pv.size();
    for (int i=0; i<n; i++)
        sum += _pv[i].metalMass();  // sum contains the total mass in metals
    sum *= _fdust;    // sum now contains the total mass in metals locked up in dust grains
    return sum;
}

//////////////////////////////////////////////////////////////////////

double SPHDustDistribution::SigmaX() const
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

double SPHDustDistribution::SigmaY() const
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

double SPHDustDistribution::SigmaZ() const
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

int SPHDustDistribution::numParticles() const
{
    return _pv.size();
}

//////////////////////////////////////////////////////////////////////

Vec SPHDustDistribution::particleCenter(int index) const
{
    int n = _pv.size();
    if (index<0 || index>=n) throw FATALERROR("Particle index out of range: " + QString::number(index));

    return _pv[index].center();
}

//////////////////////////////////////////////////////////////////////
