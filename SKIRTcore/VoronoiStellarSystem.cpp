/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "VoronoiMesh.hpp"
#include "VoronoiMeshFile.hpp"
#include "VoronoiStellarSystem.hpp"
#include "BruzualCharlotSEDFamily.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

VoronoiStellarSystem::VoronoiStellarSystem()
    : _meshfile(0), _densityIndex(0), _metallicityIndex(1), _ageIndex(2),
      _xmax(0), _ymax(0), _zmax(0),
      _random(0), _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

VoronoiStellarSystem::~VoronoiStellarSystem()
{
    delete _mesh;
}

//////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setupSelfBefore()
{
    StellarSystem::setupSelfBefore();

    // verify property values
    if (_xmax <= 0 || _ymax <= 0 || _zmax <= 0) throw FATALERROR("Domain size should be positive");

    // cache the random generator
    _random = find<Random>();

    // import the Voronoi mesh
    _mesh = new VoronoiMesh(_meshfile, QList<int>() << _densityIndex << _metallicityIndex << _ageIndex,
                             Box(-_xmax,-_ymax,-_zmax, _xmax,_ymax,_zmax));
    find<Log>()->info("Voronoi mesh data was successfully imported: " + QString::number(_mesh->Ncells()) + " cells.");

    // construct the library of SED models
    BruzualCharlotSEDFamily bc(this);

    find<Log>()->info("Filling the vectors with the SEDs of the cells... ");

    // local constants for units
    const double pc = Units::pc();
    const double pc3 = pc*pc*pc;

    // the sizes of our vectors
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    int Ncells = _mesh->Ncells();

    // construct a temporary matrix Lv with the luminosity of each cell at each wavelength
    // and also the permanent vector _Ltotv with the total luminosity for every wavelength bin
    ArrayTable<2> Lvv(Nlambda,Ncells);
    _Ltotv.resize(Nlambda);
    for (int m=0; m<Ncells; m++)
    {
        double rho = _mesh->value(_densityIndex, m);    // density in Msun / pc^3
        double V = _mesh->volume(m);                    // volume in m^3
        double M = rho * ( V/pc3 );                     // mass in Msun
        double Z = _mesh->value(_metallicityIndex, m);  // metallicity as dimensionless fraction
        double t = _mesh->value(_ageIndex, m);          // age in years

        const Array& Lv = bc.luminosities(M,Z,t);
        for (int ell=0; ell<Nlambda; ell++)
        {
            Lvv[ell][m] = Lv[ell];
            _Ltotv[ell] += Lv[ell];
        }
    }

    // construct the permanent vectors _Xvv with the normalized cumulative luminosities (per wavelength bin)
    _Xvv.resize(Nlambda,0);
    for (int ell=0; ell<Nlambda; ell++)
    {
        NR::cdf(_Xvv[ell], Lvv[ell]);
    }
}

//////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setVoronoiMeshFile(VoronoiMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

VoronoiMeshFile* VoronoiStellarSystem::voronoiMeshFile() const
{
    return _meshfile;
}

////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setDensityIndex(int value)
{
    _densityIndex = value;
}

////////////////////////////////////////////////////////////////////

int VoronoiStellarSystem::densityIndex() const
{
    return _densityIndex;
}

////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setMetallicityIndex(int value)
{
    _metallicityIndex = value;
}

////////////////////////////////////////////////////////////////////

int VoronoiStellarSystem::metallicityIndex() const
{
    return _metallicityIndex;
}

////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setAgeIndex(int value)
{
    _ageIndex = value;
}

////////////////////////////////////////////////////////////////////

int VoronoiStellarSystem::ageIndex() const
{
    return _ageIndex;
}

//////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setExtentX(double value)
{
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiStellarSystem::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setExtentY(double value)
{
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiStellarSystem::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::setExtentZ(double value)
{
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiStellarSystem::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

double VoronoiStellarSystem::luminosity(int ell) const
{
    return _Ltotv[ell];
}

//////////////////////////////////////////////////////////////////////

int VoronoiStellarSystem::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

void VoronoiStellarSystem::launch(PhotonPackage* pp, int ell, double L) const
{
    int m = NR::locate_clip(_Xvv[ell], _random->uniform());
    Position bfr = _mesh->randomPosition(_random, m);
    Direction bfk = _random->direction();
    pp->set(true,ell,bfr,bfk,L,0);
}

//////////////////////////////////////////////////////////////////////
