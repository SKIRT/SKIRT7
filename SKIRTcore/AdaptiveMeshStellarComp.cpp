/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AdaptiveMesh.hpp"
#include "AdaptiveMeshFile.hpp"
#include "AdaptiveMeshStellarComp.hpp"
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

AdaptiveMeshStellarComp::AdaptiveMeshStellarComp()
    : _meshfile(0), _densityIndex(0), _metallicityIndex(1), _ageIndex(2),
      _xmax(0), _ymax(0), _zmax(0),
      _random(0), _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

AdaptiveMeshStellarComp::~AdaptiveMeshStellarComp()
{
    delete _mesh;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::setupSelfBefore()
{
    StellarComp::setupSelfBefore();

    // verify property values
    if (_xmax <= 0 || _ymax <= 0 || _zmax <= 0) throw FATALERROR("Domain size should be positive");

    // cache the random generator
    _random = find<Random>();

    // import the adaptive mesh
    _mesh = new AdaptiveMesh(_meshfile, QList<int>() << _densityIndex << _metallicityIndex << _ageIndex,
                             Box(-_xmax,-_ymax,-_zmax, _xmax,_ymax,_zmax));
    find<Log>()->info("Adaptive mesh data was successfully imported: " + QString::number(_mesh->Ncells()) + " cells.");

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

void AdaptiveMeshStellarComp::setAdaptiveMeshFile(AdaptiveMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

AdaptiveMeshFile* AdaptiveMeshStellarComp::adaptiveMeshFile() const
{
    return _meshfile;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::setDensityIndex(int value)
{
    _densityIndex = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshStellarComp::densityIndex() const
{
    return _densityIndex;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::setMetallicityIndex(int value)
{
    _metallicityIndex = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshStellarComp::metallicityIndex() const
{
    return _metallicityIndex;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::setAgeIndex(int value)
{
    _ageIndex = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshStellarComp::ageIndex() const
{
    return _ageIndex;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::setExtentX(double value)
{
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshStellarComp::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::setExtentY(double value)
{
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshStellarComp::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::setExtentZ(double value)
{
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshStellarComp::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

int AdaptiveMeshStellarComp::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshStellarComp::luminosity(int ell) const
{
    return _Ltotv[ell];
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshStellarComp::launch(PhotonPackage* pp, int ell, double L) const
{
    int m = NR::locate_clip(_Xvv[ell], _random->uniform());
    Position bfr = _mesh->randomPosition(_random, m);
    Direction bfk = _random->direction();
    pp->launch(L,ell,bfr,bfk);
}

//////////////////////////////////////////////////////////////////////
