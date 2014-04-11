/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <fstream>
#include "ArrayTable.hpp"
#include "DustEmissivity.hpp"
#include "DustGridStructure.hpp"
#include "DustLib.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "FITSInOut.hpp"
#include "FilePaths.hpp"
#include "ISRF.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PanDustCell.hpp"
#include "PanDustSystem.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

PanDustSystem::PanDustSystem()
    : _dustemissivity(0), _dustlib(0), _selfabsorption(true), _writeEmissivity(false),
      _writeTemp(true), _writeISRF(true), _Nlambda(0)
{
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setupSelfBefore()
{
    DustSystem::setupSelfBefore();

    // if there is no dust emission, turn off the irrelevant flags just to be sure
    if (!_dustemissivity)
    {
        setDustLib(0);
        setSelfAbsorption(false);
        setWriteTemperature(false);
        setWriteISRF(false);
        setWriteEmissivity(false);
    }
    // if there is dust emission, make sure that there is a dust library as well
    else
    {
        if (!_dustlib) throw FATALERROR("There should be a dust library when dust emission is turned on");
    }

    // verify that the wavelength range includes the V-band center 0.55 micron (needed for normalization of dust)
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    if (lambdagrid->nearest(0.55e-6) < 0)
        throw FATALERROR("Wavelength range should include 0.55 micron for a panchromatic simulation with dust");

    // provide size of wavelength grid to createDustCell() function
    _Nlambda = lambdagrid->Nlambda();
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setupSelfAfter()
{
    DustSystem::setupSelfAfter();

    if (writeEmissivity())
    {
        WavelengthGrid* lambdagrid = find<WavelengthGrid>();
        Units* units = find<Units>();
        Log* log = find<Log>();

        // get the local interstellar radiation field, to be used as "input"
        Array Jv = ISRF::mathis(this);

        // get emissivity for each dust mix and for several input field strengths
        // (emissivity is rescaled for ease of plotting)
        const double factors[] = { 1e-4, 1., 1e4, 1e8 };
        int Nfields = sizeof(factors)/sizeof(factors[0]);
        ArrayTable<3> evv(_Ncomp,Nfields,0);
        for (int h=0; h<_Ncomp; h++)
        {
            log->info("Calculating emissivities for dust component " + QString::number(h) + "...");
            double input = ( mix(h)->kappaabsv() * Jv * lambdagrid->dlambdav() ).sum();
            for (int f=0; f<Nfields; f++)
            {
                double factor = factors[f];
                evv(h,f) = _dustemissivity->emissivity(mix(h), Jv*factor) / factor;
                double output = ( evv(h,f) * lambdagrid->dlambdav() ).sum();
                log->info("  with factor " + QString::number(factor) +
                          "  ; energy difference: " + QString::number(100.*(output-input)/input,'f',3) + " %");
            }
        }

        QString filename = find<FilePaths>()->output("ds_emissivity.dat");
        log->info("Writing emissivities to " + filename + "...");

        // write the input and the emissivity for each dust component to file
        ofstream file(filename.toLocal8Bit().constData());
        file << "# column 1: lambda (" << units->uwavelength().toLocal8Bit().constData() << ")\n";
        file << "# column 2: ISRF mean intensity J_lambda (W/m3/sr)\n";
        file << "# subsequent columns: lambda*emissivity[ISRF*factor]/factor (W/hydrogen atom/sr)\n";
        file << "#                     for each dust component and for several input field strengths\n";
        int column = 2;
        for (int h=0; h<_Ncomp; h++)
            for (int f=0; f<Nfields; f++)
                file << "# column " << ++column << ": dust component " << h << ", factor " << factors[f] << "\n";
        for (int ell=0; ell<_Nlambda; ell++)
        {
            double lambda = lambdagrid->lambda(ell);
            file << units->owavelength(lambda) << ' ' << Jv[ell];
            for (int h=0; h<_Ncomp; h++)
                for (int f=0; f<Nfields; f++)
                    file << ' ' << mix(h)->mu()*lambda*evv(h,f,ell);
            file << '\n';
        }
        file.close();
        log->info("File " + filename + " created.");
    }
}

////////////////////////////////////////////////////////////////////

DustCell* PanDustSystem::createDustCell()
{
    return new PanDustCell(_Ncomp, _Nlambda);
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setDustEmissivity(DustEmissivity* value)
{

    if (_dustemissivity) delete _dustemissivity;
    _dustemissivity = value;
    if (_dustemissivity) _dustemissivity->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustEmissivity* PanDustSystem::dustEmissivity() const
{
    return _dustemissivity;
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setDustLib(DustLib* value)
{

    if (_dustlib) delete _dustlib;
    _dustlib = value;
    if (_dustlib) _dustlib->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustLib* PanDustSystem::dustLib() const
{
    return _dustlib;
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setSelfAbsorption(bool value)
{
    _selfabsorption = value;
}

////////////////////////////////////////////////////////////////////

bool PanDustSystem::selfAbsorption() const
{
    return _dustemissivity && _selfabsorption;
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setWriteEmissivity(bool value)
{
    _writeEmissivity = value;
}

////////////////////////////////////////////////////////////////////

bool PanDustSystem::writeEmissivity() const
{
    return _dustemissivity && _writeEmissivity;
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setWriteTemperature(bool value)
{
    _writeTemp = value;
}

////////////////////////////////////////////////////////////////////

bool PanDustSystem::writeTemperature() const
{
    return _dustemissivity && _writeTemp;
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::setWriteISRF(bool value)
{
    _writeISRF = value;
}

////////////////////////////////////////////////////////////////////

bool PanDustSystem::writeISRF() const
{
    return _dustemissivity && _writeISRF;
}

////////////////////////////////////////////////////////////////////

bool PanDustSystem::dustemission() const
{
    return _dustemissivity!=0;
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::calculatedustemission()
{
    if (_dustemissivity) _dustlib->calculate();
}

////////////////////////////////////////////////////////////////////

double PanDustSystem::dustluminosity(int m, int ell) const
{
    return _dustemissivity ? _dustlib->luminosity(m,ell) : 0.;
}

////////////////////////////////////////////////////////////////////

// Private class to output a FITS file with the mean dust temperatures
// in one of the coordinate planes (xy, xz, or yz).
namespace
{
    // The image size in each direction, in pixels
    const int Np = 1024;

    class WriteTemp : public ParallelTarget
    {
    private:
        // cached values initialized in constructor
        const DustSystem* _ds;
        DustGridStructure* _grid;
        Units* _units;
        FilePaths* _paths;
        Log* _log;
        double xbase, ybase, zbase, xres, yres, zres;
        int Nmaps;

        // data members initialized in setup()
        bool xd, yd, zd;  // direction of coordinate plane (110, 101, 011)
        QString plane;    // name of the coordinate plane (xy, xz, yz)

        // results vector, properly sized in constructor and initialized to zero in setup()
        Array tempv;

    public:
        // constructor
        WriteTemp(const DustSystem* ds)
        {
            _ds = ds;
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
            Nmaps = 0;
            for (int h=0; h<_ds->Ncomp(); h++) Nmaps += _ds->mix(h)->Npop();

            tempv.resize(Np*Np*Nmaps);
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
            _log->info("Calculating dust temperatures in the " + plane + " plane...");

            tempv = 0.0;  // initialize all values to zero to facilitate the code in body()
        }

        // the parallized loop body; calculates the results for a single line in the images
        void body(int j)
        {
            double z = zd ? (zbase + j*zres) : 0.;
            for (int i=0; i<Np; i++)
            {
                double x = xd ? (xbase + i*xres) : 0.;
                double y = yd ? (ybase + (zd ? i : j)*yres) : 0.;
                Position bfr(x,y,z);
                int m = _grid->whichcell(bfr);
                if (m!=-1 && _ds->Labstot(m)>0.0)
                {
                    const Array& Jv = _ds->meanintensityv(m);
                    int p = 0;
                    for (int h=0; h<_ds->Ncomp(); h++)
                    {
                        double rho = _ds->density(m,h);
                        int Npop = _ds->mix(h)->Npop();
                        for (int c=0; c<Npop; c++)
                        {
                            if (rho>0.0)
                            {
                                double T = _ds->mix(h)->equilibrium(Jv,c);
                                int l = i + Np*j + Np*Np*p;
                                tempv[l] = _units->otemperature(T);
                            }
                            p++;
                        }
                    }
                }
            }
        }

        // write the results to a FITS file with an appropriate name
        void write()
        {
            QString filename = _paths->output("ds_temp" + plane + ".fits");
            FITSInOut::write(filename, tempv, Np, Np, Nmaps,
                           _units->olength(xd?xres:yres), _units->olength(zd?zres:yres),
                           _units->utemperature(), _units->ulength());
            _log->info("Written dust temperatures to file " + filename);
        }
    };
}

////////////////////////////////////////////////////////////////////

void PanDustSystem::write() const
{
    DustSystem::write();

    // If requested, output the interstellar radiation field in every dust cell to a data file
    if (_writeISRF)
    {
        WavelengthGrid* lambdagrid = find<WavelengthGrid>();
        Units* units = find<Units>();
        Log* log = find<Log>();

        QString filename = find<FilePaths>()->output("ds_isrf.dat");
        log->info("Writing ISRF to " + filename + "...");
        ofstream file(filename.toLocal8Bit().constData());

        for (int ell=0; ell<_Nlambda; ell++)
            file << units->owavelength(lambdagrid->lambda(ell)) << '\t';
        file << '\n' << '\n';
        for (int m=0; m<_Ncells; m++)
        {
            double Ltotm = Labstot(m);
            if (Ltotm>0.0)
            {
                Position bfr = _grid->centralPositionInCell(m);
                double x, y, z;
                bfr.cartesian(x,y,z);
                file << m << '\t'
                          << units->olength(x) << '\t'
                          << units->olength(y) << '\t'
                          << units->olength(z) << '\t';
                const Array& Jv = meanintensityv(m);
                for (int ell=0; ell<_Nlambda; ell++)
                    file << Jv[ell] << '\t';
                file << '\n';
            }
        }
        file.close();
        log->info("File " + filename + " created.");
    }

    // If requested, output temperate map(s) along coordiate axes cuts
    if (_writeTemp)
    {
        // construct a private class instance to do the work (parallelized)
        WriteTemp wt(this);
        Parallel* parallel = find<ParallelFactory>()->parallel();

        // get the dimension of the dust grid
        int dimDust = _grid->dimension();

        // For the xy plane (always)
        {
            wt.setup(1,1,0);
            parallel->call(&wt, Np);
            wt.write();
        }

        // For the xz plane (only if dimension is at least 2)
        if (dimDust >= 2)
        {
            wt.setup(1,0,1);
            parallel->call(&wt, Np);
            wt.write();
        }

        // For the yz plane (only if dimension is 3)
        if (dimDust == 3)
        {
            wt.setup(0,1,1);
            parallel->call(&wt, Np);
            wt.write();
        }
    }
}

//////////////////////////////////////////////////////////////////////
