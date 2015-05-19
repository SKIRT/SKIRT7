/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include "DustDistribution.hpp"
#include "DustGridDensityInterface.hpp"
#include "DustGridPath.hpp"
#include "DustGridStructure.hpp"
#include "DustMix.hpp"
#include "DustSystem.hpp"
#include "DustSystemDensityCalculator.hpp"
#include "DustSystemDepthCalculator.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"
#include <QVarLengthArray>

using namespace std;

//////////////////////////////////////////////////////////////////////

DustSystem::DustSystem()
    : _dd(0), _grid(0), _gdi(0), _Nrandom(100),
      _writeConvergence(true), _writeDensity(true), _writeDepthMap(false),
      _writeQuality(false), _writeCellProperties(false), _writeCellsCrossed(false)
{
}

////////////////////////////////////////////////////////////////////

void DustSystem::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    if (_Nrandom < 1) throw FATALERROR("Number of random samples must be at least 1");
    if (!_dd) throw FATALERROR("Dust distribution was not set");
    if (!_grid) throw FATALERROR("Dust grid structure was not set");
}

//////////////////////////////////////////////////////////////////////

void DustSystem::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    // Copy some basic properties
    _Ncomp = _dd->Ncomp();
    _Ncells = _grid->Ncells();

    // Resize the tables that hold essential dust cell properties
    _volumev.resize(_Ncells);
    _rhovv.resize(_Ncells,_Ncomp);

    // Set the volume of the cells
    find<Log>()->info("Calculating the volume of the cells...");
    find<ParallelFactory>()->parallel()->call(this, &DustSystem::setVolumeBody, _Ncells);

    // Set the density of the cells
    _gdi = _grid->interface<DustGridDensityInterface>();
    if (_gdi)
    {
        // if the dust grid offers a special interface, use it
        find<Log>()->info("Setting the value of the density in the cells using grid interface...");
        find<ParallelFactory>()->parallel()->call(this, &DustSystem::setGridDensityBody, _Ncells);
    }
    else
    {
        // otherwise take an average of the density in 100 random positions in the cell (parallelized)
        find<Log>()->info("Setting the value of the density in the cells...");
        find<ParallelFactory>()->parallel()->call(this, &DustSystem::setSampleDensityBody, _Ncells);
    }

    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();

    // Perform a convergence check on the grid.
    if (_writeConvergence && comm->isRoot()) writeconvergence();

    // Write the density in the xy plane, xz plane and yz plane to a file.
    if (_writeDensity && comm->isRoot()) writedensity();

    // Output optical depth map as seen from the center
    if (_writeDepthMap && comm->isRoot()) writedepthmap();

    // Calculate and output some quality metrics for the dust grid
    if (_writeQuality && comm->isRoot()) writequality();

    // Output properties for all cells in the dust grid
    if (_writeCellProperties && comm->isRoot()) writecellproperties();
}

////////////////////////////////////////////////////////////////////

// parallelized body used above
void DustSystem::setVolumeBody(size_t m)
{
    _volumev[m] = _grid->weight(m) > 0 ? _grid->volume(m) : 0;
}

////////////////////////////////////////////////////////////////////

// parallelized body used above
void DustSystem::setGridDensityBody(size_t m)
{
    for (int h=0; h<_Ncomp; h++)
        _rhovv(m,h) = _gdi->density(h,m);
}

////////////////////////////////////////////////////////////////////

// parallelized body used above
void DustSystem::setSampleDensityBody(size_t m)
{
    if (m%100000==0)
    {
        find<Log>()->info("  Computing density for cell " + QString::number(m)
                          + " (" + QString::number(floor(100.*m/_Ncells)) + "%)");
    }
    if (_grid->weight(m) > 0)
    {
        Array sumv(_Ncomp);
        for (int n=0; n<_Nrandom; n++)
        {
            Position bfr = _grid->randomPositionInCell(m);
            for (int h=0; h<_Ncomp; h++) sumv[h] += _dd->density(h,bfr);
        }
        for (int h=0; h<_Ncomp; h++)
        {
            _rhovv(m,h) = sumv[h]/_Nrandom;
        }
    }
    else
    {
        for (int h=0; h<_Ncomp; h++) _rhovv(m,h) = 0;
    }
}

////////////////////////////////////////////////////////////////////

void DustSystem::writeconvergence() const
{
    // Perform a convergence check on the grid. First calculate the total
    // mass and the principle axes surface densities by integrating
    // over the grid.

    find<Log>()->info("Performing a convergence check on the grid...");

    // calculation of the mass

    double M = 0.0;
    for (int m=0; m<_Ncells; m++)
    {
        M += density(m)*volume(m);
    }

    // calculation of the X-axis surface density

    double SigmaX = 0;
    DustGridPath dgp(Position(0.,0.,0.), Direction(1.,0.,0.));
    _grid->path(&dgp);
    SigmaX += dgp.opticalDepth([this](int m){ return density(m); });
    dgp.setDirection(Direction(-1.,0.,0.));
    _grid->path(&dgp);
    SigmaX += dgp.opticalDepth([this](int m){ return density(m); });

    // calculation of the Y-axis surface density

    double SigmaY = 0.0;
    dgp.setDirection(Direction(0.,1.,0.));
    _grid->path(&dgp);
    SigmaY += dgp.opticalDepth([this](int m){ return density(m); });
    dgp.setDirection(Direction(0.,-1.,0.));
    _grid->path(&dgp);
    SigmaY += dgp.opticalDepth([this](int m){ return density(m); });

    // calculation of the Z-axis surface density

    double SigmaZ = 0.0;
    dgp.setDirection(Direction(0.,0.,1.));
    _grid->path(&dgp);
    SigmaZ += dgp.opticalDepth([this](int m){ return density(m); });
    dgp.setDirection(Direction(0.,0.,-1.));
    _grid->path(&dgp);
    SigmaZ += dgp.opticalDepth([this](int m){ return density(m); });

    // Compare these values to the expected values and write the result to file

    QString filename = find<FilePaths>()->output("ds_convergence.dat");
    find<Log>()->info("Writing convergence check on the dust system to " + filename + "...");
    Units* units = find<Units>();
    ofstream file(filename.toLocal8Bit().constData());
    file << "Convergence check on the grid: \n";
    switch (_dd->dimension())
    {
        case 1:
        {
            double Sigmar = 0.5*SigmaX;
            double Sigmarref = 0.5*_dd->SigmaX();
            file << "   - radial (r-axis) surface density\n"
                 << "         expected value = " << units->omasssurfacedensity(Sigmarref)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "         actual value =   " << units->omasssurfacedensity(Sigmar)
                 << " " << units->umasssurfacedensity().toStdString() << "\n";
        }
        break;
        case 2:
        {
            double SigmaR = 0.5*SigmaX;
            double SigmaRref = 0.5*_dd->SigmaX();
            double SigmaZref = _dd->SigmaZ();
            file << "   - edge-on (R-axis) surface density\n"
                 << "         expected value = " << units->omasssurfacedensity(SigmaRref)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "         actual value =   " << units->omasssurfacedensity(SigmaR)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "   - face-on (Z-axis) surface density\n"
                 << "         expected value = " << units->omasssurfacedensity(SigmaZref)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "         actual value =   " << units->omasssurfacedensity(SigmaZ)
                 << " " << units->umasssurfacedensity().toStdString() << "\n";
        }
        break;
        case 3:
        {
            double SigmaXref = _dd->SigmaX();
            double SigmaYref = _dd->SigmaY();
            double SigmaZref = _dd->SigmaZ();
            file << "   - X-axis surface density\n"
                 << "         expected value = " << units->omasssurfacedensity(SigmaXref)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "         actual value =   " << units->omasssurfacedensity(SigmaX)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "   - Y-axis surface density\n"
                 << "         expected value = " << units->omasssurfacedensity(SigmaYref)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "         actual value =   " << units->omasssurfacedensity(SigmaY)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "   - Z-axis surface density\n"
                 << "         expected value = " << units->omasssurfacedensity(SigmaZref)
                 << " " << units->umasssurfacedensity().toStdString() << "\n"
                 << "         actual value =   " << units->omasssurfacedensity(SigmaZ)
                 << " " << units->umasssurfacedensity().toStdString() << "\n";
        }
        break;
        default:
            throw FATALERROR("Wrong dimension in dust distribution");
    }
    double Mref = _dd->mass();
    file << "   - total dust mass\n"
         << "         expected value = " << units->omass(Mref)
         << " " << units->umass().toStdString() << "\n"
         << "         actual value =   " << units->omass(M)
         << " " << units->umass().toStdString() << "\n";
    file.close();
    find<Log>()->info("File " + filename + " created.");
}

////////////////////////////////////////////////////////////////////

// Private class to output FITS files with the theoretical dust density and the constructed grid density
// in one of the coordinate planes (xy, xz, or yz).
namespace
{
    // The image size in each direction, in pixels
    const int Np = 1024;

    class WriteDensity : public ParallelTarget
    {
    private:
        // results -- sized to fit in constructor
        Array trhov;
        Array grhov;

        // data members initialized in constructor
        const DustSystem* _ds;
        DustDistribution* _dd;
        DustGridStructure* _grid;
        Units* _units;
        FilePaths* _paths;
        Log* _log;
        double xbase, ybase, zbase, xres, yres, zres;

        // data members initialized in setup()
        bool xd, yd, zd; // direction of coordinate plane (110, 101, 011)
        QString plane;    // name of the coordinate plane (xy, xz, yz)

    public:
        // constructor
        WriteDensity(const DustSystem* ds)
            : trhov(Np*Np), grhov(Np*Np)
        {
            _ds = ds;
            _dd = ds->dustDistribution();
            _grid = ds->dustGridStructure();
            _units = ds->find<Units>();
            _log = ds->find<Log>();
            _paths = ds->find<FilePaths>();

            double xmax = _grid->xmax();
            double ymax = _grid->ymax();
            double zmax = _grid->zmax();
            xres = 2.0*xmax/Np;
            yres = 2.0*ymax/Np;
            zres = 2.0*zmax/Np;
            xbase = -xmax + 0.5*xres;
            ybase = -ymax + 0.5*yres;
            zbase = -zmax + 0.5*zres;
        }

        // setup for calculating a specific coordinate plane
        void setup(bool xdir, bool ydir, bool zdir)
        {
            xd = xdir;
            yd = ydir;
            zd = zdir;
            plane = "";
            if (xd) plane += "x";
            if (yd) plane += "y";
            if (zd) plane += "z";
            _log->info("Calculating density in the " + plane + " plane...");
        }

        // the parallized loop body; calculates the results for a single line in the images
        void body(size_t j)
        {
            double z = zd ? (zbase + j*zres) : 0.;
            for (int i=0; i<Np; i++)
            {
                int l = i + Np*j;
                double x = xd ? (xbase + i*xres) : 0.;
                double y = yd ? (ybase + (zd ? i : j)*yres) : 0.;
                Position bfr(x,y,z);
                trhov[l] = _units->omassvolumedensity(_dd->density(bfr));
                int m = _grid->whichcell(bfr);
                if (m==-1)
                    grhov[l] = 0.0;
                else
                    grhov[l] = _units->omassvolumedensity(_ds->density(m));
            }
        }

        // write the results to two FITS files with appropriate names
        void write()
        {
            write(trhov, "theoretical", "ds_trho");
            write(grhov, "grid", "ds_grho");
        }

    private:
        void write(const Array& rhov, QString label, QString prefix)
        {
            QString filename = _paths->output(prefix + plane + ".fits");
            FITSInOut::write(filename, rhov, Np, Np, 1,
                           _units->olength(xd?xres:yres), _units->olength(zd?zres:yres),
                           _units->umassvolumedensity(), _units->ulength());
            _log->info("Written " + label + " density to file " + filename);
        }
    };
}

////////////////////////////////////////////////////////////////////

void DustSystem::writedensity() const
{
    // construct a private class instance to do the work (parallelized)
    WriteDensity wd(this);
    Parallel* parallel = find<ParallelFactory>()->parallel();

    // get the dimension of the dust system
    int dimDust = dimension();

    // For the xy plane (always)
    {
        wd.setup(1,1,0);
        parallel->call(&wd, Np);
        wd.write();
    }

    // For the xz plane (only if dimension is at least 2)
    if (dimDust >= 2)
    {
        wd.setup(1,0,1);
        parallel->call(&wd, Np);
        wd.write();
    }

    // For the yz plane (only if dimension is 3)
    if (dimDust == 3)
    {
        wd.setup(0,1,1);
        parallel->call(&wd, Np);
        wd.write();
    }
}

////////////////////////////////////////////////////////////////////

// Private class to encapsulate the call-back function for calculating optical depths
namespace
{
    class KappaRho
    {
    private:
        // data members initialized in constructor
        const DustSystem* _ds;
        int _Ncomp;
        QVarLengthArray<double,8> _kappaextv;

    public:
        // constructor
        // stores the extinction coefficients at the specified wavelength for all dust mixes
        KappaRho(const DustSystem* ds, int ell) : _ds(ds), _Ncomp(ds->Ncomp()), _kappaextv(_Ncomp)
        {
            for (int h=0; h<_Ncomp; h++)
                _kappaextv[h] = _ds->mix(h)->kappaext(ell);
        }

        // call-back function
        // returns kappa*rho for the specified cell number (and for the wavelength-index bound in the constructor)
        double operator() (int m)
        {
            double result = 0;
            for (int h=0; h<_Ncomp; h++)
                result += _kappaextv[h] * _ds->density(m,h);
            return result;
        }
    };
}

////////////////////////////////////////////////////////////////////

// Private class to output a FITS file with an optical depth map viewed from the center using Mollweide projection
namespace
{
    // The image size in each direction, in pixels
    const int Npx = 1600;
    const int Npy = 800;

    class WriteDepthMap : public ParallelTarget
    {
    private:
        // results -- sized to fit in constructor
        Array tauv;

        // data members initialized in constructor
        const DustSystem* _ds;
        DustGridStructure* _grid;
        FilePaths* _paths;
        Log* _log;
        int _ell;

    public:
        // constructor
        WriteDepthMap(const DustSystem* ds)
            : tauv(Npx*Npy)
        {
            _ds = ds;
            _grid = ds->dustGridStructure();
            _log = ds->find<Log>();
            _paths = ds->find<FilePaths>();
            _log->info("Calculating optical depth map viewed from the center...");
            _ell = max(0, ds->find<WavelengthGrid>()->nearest(Units::lambdaV()));
        }

        // the parallized loop body; calculates the results for a single line in the image
        void body(size_t j)
        {
            double y = (j+0.5) / Npy;
            for (int i=0; i<Npx; i++)
            {
                double x = (i+0.5) / Npx;

                // perform the inverse Mollweide projection
                double alpha = asin(2*y-1);
                double theta = acos((2*alpha+sin(2*alpha))/M_PI);
                double phi = M_PI*(2*x-1)/cos(alpha);

                // if the deprojected direction is within range, compute the optical depth
                if (phi > -M_PI && phi < M_PI)
                    tauv[i+Npx*j] = opticaldepth(_ell, Position(), Direction(theta, phi));
            }
        }

        // write the results to a FITS file with an appropriate name
        void write()
        {
            QString filename = _paths->output("ds_tau.fits");
            FITSInOut::write(filename, tauv, Npx, Npy, 1,  360./Npx, 180./Npy, "deg", "deg");
            WavelengthGrid* lambdagrid = _ds->find<WavelengthGrid>();
            Units* units = _ds->find<Units>();
            _log->info("Written optical depth map at λ = " +
                       QString::number(units->owavelength(lambdagrid->lambda(_ell))) + " " +
                       units->uwavelength() + " to file " + filename);
        }

    private:
        double opticaldepth(int ell, Position bfr, Direction bfk)
        {
            DustGridPath dgp(bfr, bfk);
            _grid->path(&dgp);
            return dgp.opticalDepth(KappaRho(_ds, ell));
        }
    };
}

////////////////////////////////////////////////////////////////////

void DustSystem::writedepthmap() const
{
    // construct a private class instance to do the work (parallelized)
    WriteDepthMap wdm(this);
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(&wdm, Npy);
    wdm.write();
}

////////////////////////////////////////////////////////////////////

void DustSystem::writequality() const
{
    Log* log = find<Log>();
    Units* units = find<Units>();
    Parallel* parallel = find<ParallelFactory>()->parallel();

    // Density metric

    log->info("Calculating quality metric for the grid density...");
    DustSystemDensityCalculator calc1(this, _Nrandom, _Ncells/5);
    parallel->call(&calc1, _Nrandom);

    log->info("  Mean value of density delta: "
              + QString::number(units->omassvolumedensity(calc1.meanDelta()*1e9))
              + " nano" + units->umassvolumedensity());
    log->info("  Standard deviation of density delta: "
              + QString::number(units->omassvolumedensity(calc1.stddevDelta()*1e9))
              + " nano" + units->umassvolumedensity());

    // Optical depth metric

    log->info("Calculating quality metric for the optical depth in the grid...");
    DustSystemDepthCalculator calc2(this, _Nrandom, _Ncells/50, _Nrandom*10);
    parallel->call(&calc2, _Nrandom);

    log->info("  Mean value of optical depth delta: " + QString::number(calc2.meanDelta()));
    log->info("  Standard deviation of optical depth delta: " + QString::number(calc2.stddevDelta()));

    // Output to file

    QString filename = find<FilePaths>()->output("ds_quality.dat");
    log->info("Writing quality metrics for the grid to " + filename + "...");
    ofstream file(filename.toLocal8Bit().constData());
    file << "Mean value of density delta: "
         << units->omassvolumedensity(calc1.meanDelta()) << ' '
         << units->umassvolumedensity().toStdString() << '\n'
         << "Standard deviation of density delta: "
         << units->omassvolumedensity(calc1.stddevDelta()) << ' '
         << units->umassvolumedensity().toStdString() << '\n';
    file << "Mean value of optical depth delta: "
         << calc2.meanDelta() << '\n'
         << "Standard deviation of optical depth delta: "
         << calc2.stddevDelta() << '\n';
    file.close();
    log->info("File " + filename + " created.");
}

////////////////////////////////////////////////////////////////////

void DustSystem::writecellproperties() const
{
    Log* log = find<Log>();
    Units* units = find<Units>();

    // open the file and write a header
    QString filename = find<FilePaths>()->output("ds_cellprops.dat");
    log->info("Writing dust cell properties to " + filename + "...");
    ofstream file(filename.toLocal8Bit().constData());
    file << "# column 1: volume (" << units->uvolume().toStdString() << ")\n";
    file << "# column 2: density (" << units->umassvolumedensity().toStdString() << ")\n";
    file << "# column 3: mass fraction\n";
    file << "# column 4: optical depth\n";

    // write a line for each cell; remember the tau values so we can compute some statistics
    Array tauV(_Ncells);
    double totalmass = _dd->mass();
    for (int m=0; m<_Ncells; m++)
    {
        double rho = density(m);
        double V = volume(m);
        double delta = (rho*V)/totalmass;
        double tau = Units::kappaV()*rho*pow(V,1./3.);
        file << units->ovolume(V) << '\t' << units->omassvolumedensity(rho) << '\t' << delta << '\t' << tau << '\n';
        tauV[m] = tau;
    }

    // calculate some statistics on optical depth
    double tauavg = tauV.sum()/_Ncells;
    double taumin = tauV.min();
    double taumax = tauV.max();
    const int Nbins = 500;
    vector<int> countV(Nbins+1);
    for (int m=0; m<_Ncells; m++)
    {
        int index = qMax(0,qMin(Nbins, static_cast<int>((tauV[m]-taumin)/(taumax-taumin)*Nbins)));
        countV[index]++;
    }
    int count = 0;
    int index = 0;
    for (; index<Nbins; index++)
    {
        count += countV[index];
        if (count > 0.9*_Ncells) break;
    }
    double tau90 = taumin + index*(taumax-taumin)/Nbins;

    // write the statistics on optical depth to the file
    file << "# smallest optical depth: " << taumin << '\n';
    file << "# largest optical depth:  " << taumax << '\n';
    file << "# average optical depth:  " << tauavg << '\n';
    file << "# 90 % of the cells have optical depth smaller than: " << tau90 << '\n';

    // close the file
    file.close();
    log->info("File " + filename + " created.");

    // report the statistics on optical depth to the console
    log->info("  Smallest optical depth: " + QString::number(taumin));
    log->info("  Largest optical depth:  " + QString::number(taumax));
    log->info("  Average optical depth:  " + QString::number(tauavg));
    log->info("  90 % of the cells have optical depth smaller than: " + QString::number(tau90));
}

////////////////////////////////////////////////////////////////////

void DustSystem::setDustDistribution(DustDistribution* value)
{
    if (_dd) delete _dd;
    _dd = value;
    if (_dd) _dd->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustDistribution* DustSystem::dustDistribution() const
{
    return _dd;
}

////////////////////////////////////////////////////////////////////

void DustSystem::setDustGridStructure(DustGridStructure* value)
{
    if (_grid) delete _grid;
    _grid = value;
    if (_grid) _grid->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustGridStructure* DustSystem::dustGridStructure() const
{
    return _grid;
}

////////////////////////////////////////////////////////////////////

void DustSystem::setSampleCount(int value)
{
    _Nrandom = value;
}

////////////////////////////////////////////////////////////////////

int DustSystem::sampleCount() const
{
    return _Nrandom;
}

////////////////////////////////////////////////////////////////////

void DustSystem::setWriteConvergence(bool value)
{
    _writeConvergence = value;
}

////////////////////////////////////////////////////////////////////

bool DustSystem::writeConvergence() const
{
    return _writeConvergence;
}

////////////////////////////////////////////////////////////////////

void DustSystem::setWriteDensity(bool value)
{
    _writeDensity = value;
}

////////////////////////////////////////////////////////////////////

bool DustSystem::writeDensity() const
{
    return _writeDensity;
}

//////////////////////////////////////////////////////////////////////

void DustSystem::setWriteDepthMap(bool value)
{
    _writeDepthMap = value;
}

//////////////////////////////////////////////////////////////////////

bool DustSystem::writeDepthMap() const
{
    return _writeDepthMap;
}

//////////////////////////////////////////////////////////////////////

void DustSystem::setWriteQuality(bool value)
{
    _writeQuality = value;
}

//////////////////////////////////////////////////////////////////////

bool DustSystem::writeQuality() const
{
    return _writeQuality;
}

//////////////////////////////////////////////////////////////////////

void DustSystem::setWriteCellProperties(bool value)
{
    _writeCellProperties = value;
}

//////////////////////////////////////////////////////////////////////

bool DustSystem::writeCellProperties() const
{
    return _writeCellProperties;
}

//////////////////////////////////////////////////////////////////////

void DustSystem::setWriteCellsCrossed(bool value)
{
    _writeCellsCrossed = value;
}

//////////////////////////////////////////////////////////////////////

bool DustSystem::writeCellsCrossed() const
{
    return _writeCellsCrossed;
}

//////////////////////////////////////////////////////////////////////

int DustSystem::dimension() const
{
    return _dd->dimension();
}

//////////////////////////////////////////////////////////////////////

int DustSystem::Ncells() const
{
    return _Ncells;
}

//////////////////////////////////////////////////////////////////////

int DustSystem::Ncomp() const
{
    return _Ncomp;
}

//////////////////////////////////////////////////////////////////////

DustMix* DustSystem::mix(int h) const
{
    return _dd->mix(h);
}

//////////////////////////////////////////////////////////////////////

DustMix* DustSystem::randomMixForPosition(Position bfr, int ell) const
{
    int hmix = 0;
    if (_Ncomp>1)
    {
        int m = whichcell(bfr);
        if (m>=0)
        {
            Array Xv;
            NR::cdf(Xv, _Ncomp, [this,ell,m](int h){return mix(h)->kappasca(ell)*density(m,h);} );
            hmix = NR::locate_clip(Xv, find<Random>()->uniform());
        }
    }
    return mix(hmix);
}

//////////////////////////////////////////////////////////////////////

int DustSystem::whichcell(Position bfr) const
{
    return _grid->whichcell(bfr);
}

//////////////////////////////////////////////////////////////////////

Position DustSystem::randomPositionInCell(int m) const
{
    return _grid->randomPositionInCell(m);
}

//////////////////////////////////////////////////////////////////////

double DustSystem::volume(int m) const
{
    return _volumev[m];
}

//////////////////////////////////////////////////////////////////////

double DustSystem::density(int m, int h) const
{
    return m >= 0 ? _rhovv(m,h) : 0;
}

//////////////////////////////////////////////////////////////////////

double DustSystem::density(int m) const
{
    double rho = 0;
    if (m >= 0)
        for (int h=0; h<_Ncomp; h++) rho += _rhovv(m,h);
    return rho;
}

//////////////////////////////////////////////////////////////////////

void DustSystem::fillOpticalDepth(PhotonPackage* pp)
{
    // determine the path and store the geometric details in the photon package
    _grid->path(pp);

    // if such statistics are requested, keep track of the number of cells crossed
    if (_writeCellsCrossed)
    {
        QMutexLocker lock(&_crossedMutex);
        unsigned int index = pp->size();
        if (index >= _crossed.size()) _crossed.resize(index+1);
        _crossed[index] += 1;
    }

    // calculate and store the optical depth details in the photon package
    pp->fillOpticalDepth(KappaRho(this, pp->ell()));

    // verify that the result makes sense
    double tau = pp->tau();
    if (tau<0.0 || std::isnan(tau) || std::isinf(tau))
        throw FATALERROR("The optical depth along the path is not a positive number: tau = " + QString::number(tau));
}

//////////////////////////////////////////////////////////////////////

double DustSystem::opticaldepth(PhotonPackage* pp, double distance)
{
    // determine the path and store the geometric details in the photon package
    _grid->path(pp);

    // if such statistics are requested, keep track of the number of cells crossed
    if (_writeCellsCrossed)
    {
        QMutexLocker lock(&_crossedMutex);
        unsigned int index = pp->size();
        if (index >= _crossed.size()) _crossed.resize(index+1);
        _crossed[index] += 1;
    }

    // calculate and return the optical depth at the specified distance
    return pp->opticalDepth(KappaRho(this, pp->ell()), distance);
}

////////////////////////////////////////////////////////////////////

void DustSystem::write() const
{
    // If requested, output statistics on the number of cells crossed
    if (_writeCellsCrossed)
    {
        QString filename = find<FilePaths>()->output("ds_crossed.dat");
        find<Log>()->info("Writing number of cells crossed to " + filename + "...");
        ofstream file(filename.toLocal8Bit().constData());

        file << "# total number of cells in grid: " << _Ncells << '\n';
        file << "# column 1: number of cells crossed\n";
        file << "# column 2: number of paths that crossed this number of cells\n";

        int Nlines = _crossed.size();
        for (int index=0; index<Nlines; index++)
        {
            file << index << '\t' << _crossed[index] << '\n';
        }

        file.close();
        find<Log>()->info("File " + filename + " created.");
    }
}

////////////////////////////////////////////////////////////////////

bool DustSystem::polarization() const
{
    for (int h=0; h<_Ncomp; h++)
    {
        if (_dd->mix(h)->polarization()) return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////
