/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include "HIDistribution.hpp"
#include "HISystem.hpp"
#include "DustGridPath.hpp"
#include "DustGrid.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Image.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "SpecialFunctions.hpp"
#include "TextOutFile.hpp"
#include "TimeLogger.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"
#include "LyaWavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

HISystem::HISystem()
    : _hid(0), _grid(0), _Nrandom(100)
{
}

////////////////////////////////////////////////////////////////////

void HISystem::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    if (_Nrandom < 1) throw FATALERROR("Number of random samples must be at least 1");
    if (!_hid) throw FATALERROR("Neutral hydrogen distribution was not set");
    if (!_grid) throw FATALERROR("Grid was not set");

    // set a number of local caches
    _lambdagrid = find<LyaWavelengthGrid>();
    _units = find<Units>();
}

//////////////////////////////////////////////////////////////////////

void HISystem::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    // Copy some basic properties
    _Ncells = _grid->numCells();

    // Resize the tables that hold essential cell properties
    _volumev.resize(_Ncells);
    _rhov.resize(_Ncells);
    _Tv.resize(_Ncells);
    _vbulkv.resize(_Ncells);

    // Set the volume of the cells (parallelization is turned off)
    find<Log>()->info("Calculating the volume of the cells...");
    for (int m=0; m<_Ncells; m++)
        _volumev[m] = (_grid->weight(m) > 0) ? _grid->volume(m) : 0;

    // Calculate all other properties of the cells
    find<Log>()->info("Setting the value of the density, temperature and bulk velocity in the cells...");
    for (int m=0; m<_Ncells; m++)
    {
        if (m%100000==0)
            find<Log>()->info("  Computing density for cell " + QString::number(m));
        double weight = _grid->weight(m);
        if (weight > 0)
        {
            double sumrho = 0.0;
            double sumT = 0.0;
            Vec sumvbulk;
            for (int n=0; n<_Nrandom; n++)
            {
                Position bfr = _grid->randomPositionInCell(m);
                sumrho += _hid->density(bfr);
                sumT += _hid->gastemperature(bfr);
                sumvbulk += _hid->bulkvelocity(bfr);
            }
            _rhov[m] = weight*sumrho/_Nrandom;
            _Tv[m] = sumT/_Nrandom;
            _vbulkv[m] = sumvbulk/_Nrandom;
        }
        else
        {
            _rhov[m] = 0;
            _Tv[m] = 0;
            _vbulkv[m] = Vec();
        }
    }
}

////////////////////////////////////////////////////////////////////

void HISystem::setHIDistribution(HIDistribution* value)
{
    if (_hid) delete _hid;
    _hid = value;
    if (_hid) _hid->setParent(this);
}

////////////////////////////////////////////////////////////////////

HIDistribution* HISystem::hIDistribution() const
{
    return _hid;
}

////////////////////////////////////////////////////////////////////

void HISystem::setGrid(DustGrid* value)
{
    if (_grid) delete _grid;
    _grid = value;
    if (_grid) _grid->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustGrid* HISystem::grid() const
{
    return _grid;
}

////////////////////////////////////////////////////////////////////

void HISystem::setSampleCount(int value)
{
    _Nrandom = value;
}

////////////////////////////////////////////////////////////////////

int HISystem::sampleCount() const
{
    return _Nrandom;
}

////////////////////////////////////////////////////////////////////

// Private class to encapsulate the call-back function for calculating optical depths
namespace
{
    class KappaRho
    {
    private:
        // data members initialized in constructor
        const HISystem* _gs;
        const Units* _units;
        const LyaWavelengthGrid* _lambdagrid;
        double _nu;
        Direction _bfk;

    public:
        // constructor
        // stores the extinction coefficients
        KappaRho(const HISystem* gs, const Units* units, const LyaWavelengthGrid* lambdagrid, int ell, Direction bfk)
            : _gs(gs), _units(units), _lambdagrid(lambdagrid), _nu(0), _bfk(bfk)
        {
            _nu = _units->c()/lambdagrid->lambda(ell);
        }

        // call-back function
        // returns kappa*rho for the specified cell number (and for the wavelength-index and direction bound in the constructor)
        double operator() (int m)
        {
            double c = _units->c();
            double lambda0 = _units->lambdaLya();
            double nu0 = c/lambda0;
            double T = _gs->gastemperature(m);
            Vec vbulk = _gs->bulkvelocity(m);
            double rho = _gs->density(m);
            double DeltanuD = sqrt(2.0*_units->k()*T/_units->massproton())/lambda0;
            double DeltanuL = 9.936e7;
            double a = DeltanuL/(2.0*DeltanuD);
            double vbulkparallel = Vec::dot(vbulk,_bfk);
            double x = 1.0/DeltanuD * ( _nu - (1-vbulkparallel/c)*nu0 );
            double kappa0 = 7.499e12; // in m2/kg
            double kappa = kappa0 * a * SpecialFunctions::voigthjerting(a,x);
            return kappa * rho;
        }
    };
}

//////////////////////////////////////////////////////////////////////

int HISystem::dimension() const
{
    return _hid->dimension();
}

//////////////////////////////////////////////////////////////////////

int HISystem::Ncells() const
{
    return _Ncells;
}

//////////////////////////////////////////////////////////////////////

int HISystem::whichcell(Position bfr) const
{
    return _grid->whichcell(bfr);
}

//////////////////////////////////////////////////////////////////////

Position HISystem::randomPositionInCell(int m) const
{
    return _grid->randomPositionInCell(m);
}

//////////////////////////////////////////////////////////////////////

double HISystem::volume(int m) const
{
    return _volumev[m];
}

//////////////////////////////////////////////////////////////////////

double HISystem::density(int m) const
{
    return (m >= 0) ? _rhov[m] : 0;
}

//////////////////////////////////////////////////////////////////////

double HISystem::gastemperature(int m) const
{
    return (m >= 0) ? _Tv[m] : 0;
}

//////////////////////////////////////////////////////////////////////

Vec HISystem::bulkvelocity(int m) const
{
    return (m >= 0) ? _vbulkv[m] : Vec();
}

//////////////////////////////////////////////////////////////////////

void HISystem::fillOpticalDepth(PhotonPackage* pp)
{
    // determine the path and store the geometric details in the photon package
    _grid->path(pp);

    // calculate and store the optical depth details in the photon package
    pp->fillOpticalDepth(KappaRho(this, _units, _lambdagrid, pp->ell(), pp->direction()));

    // verify that the result makes sense
    double tau = pp->tau();
    if (tau<0.0 || std::isnan(tau) || std::isinf(tau))
        throw FATALERROR("The optical depth along the path is not a positive number: tau = " + QString::number(tau));
}

//////////////////////////////////////////////////////////////////////

double HISystem::opticaldepth(PhotonPackage* pp, double distance)
{
    // determine the path and store the geometric details in the photon package
    _grid->path(pp);

    // calculate and return the optical depth at the specified distance
    return pp->opticalDepth(KappaRho(this, _units, _lambdagrid, pp->ell(), pp->direction()), distance);
}

//////////////////////////////////////////////////////////////////////
