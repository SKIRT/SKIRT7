/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DustEmissivity.hpp"
#include "FatalError.hpp"
#include "FullInstrument.hpp"
#include "LockFree.hpp"
#include "PanDustSystem.hpp"
#include "PhotonPackage.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

FullInstrument::FullInstrument()
    : _Nscatt(0), _dustsystem(false), _dustemission(false), _polarization(false)
{
}

////////////////////////////////////////////////////////////////////

void FullInstrument::setupSelfBefore()
{
    SingleFrameInstrument::setupSelfBefore();

    // determine whether the simulation contains a dust system
    try
    {
        // attempt to locate the dust system without performing setup
        // to avoid catching (and hiding) fatal errors during such setup
        find<DustSystem>(false);
        _dustsystem = true;
    }
    catch (FatalError) { }

    if (_dustsystem)
    {
        // determine whether the simulation includes dust emission
        try
        {
            // attempt to locate a panchromatic dust system without performing setup
            // to avoid catching (and hiding) fatal errors during such setup
            // and, if successful, ask it whether dust emission has been turned on
            _dustemission = find<PanDustSystem>(false)->dustemission();
        }
        catch (FatalError) { }

        // determine whether the simulation includes polarization;
        // a dust mix knows whether it supports polarization only after it has been setup,
        // so here we need to fully setup the dust system before querying it
        _polarization = find<DustSystem>()->polarization();
    }

    // resize the detector arrays only when meaningful
    int Nlambda = find<WavelengthGrid>()->Nlambda();

    _ftrav.initialize(_Nframep, this);
    _Ftrav.resize(Nlambda);
    if (_dustsystem)
    {
        _fstrdirv.initialize(_Nframep, this);
        _Fstrdirv.resize(Nlambda);
        _fstrscav.initialize(_Nframep, this);
        _Fstrscav.resize(Nlambda);
        if (_dustemission)
        {
            _fdusdirv.initialize(_Nframep, this);
            _Fdusdirv.resize(Nlambda);
            _fdusscav.initialize(_Nframep, this);
            _Fdusscav.resize(Nlambda);
        }
        if (_Nscatt > 0)
        {
            _fstrscavv.resize(_Nscatt);
            for (auto& cube : _fstrscavv) cube.initialize(_Nframep, this);
            _Fstrscavv.resize(_Nscatt, Nlambda);
        }
        if (_polarization)
        {
            _ftotQv.initialize(_Nframep, this);
            _FtotQv.resize(Nlambda);
            _ftotUv.initialize(_Nframep, this);
            _FtotUv.resize(Nlambda);
            _ftotVv.initialize(_Nframep, this);
            _FtotVv.resize(Nlambda);
        }
    }
}

////////////////////////////////////////////////////////////////////

void FullInstrument::setScatteringLevels(int value)
{
    _Nscatt = value;
}

////////////////////////////////////////////////////////////////////

int FullInstrument::scatteringLevels() const
{
    return _Nscatt;
}

////////////////////////////////////////////////////////////////////

void FullInstrument::detect(PhotonPackage* pp)
{
    int nscatt = pp->nScatt();
    int l = pixelondetector(pp);
    int ell = pp->ell();
    double L = pp->luminosity();
    double taupath = opticalDepth(pp);
    double extf = exp(-taupath);
    double Lextf = L*extf;

    // SEDs
    if (pp->isStellar())
    {
        if (nscatt==0)
        {
            LockFree::add(_Ftrav[ell], L);
            if (_dustsystem) LockFree::add(_Fstrdirv[ell], Lextf);
        }
        else
        {
            LockFree::add(_Fstrscav[ell], Lextf);
            if (nscatt<=_Nscatt) LockFree::add(_Fstrscavv[nscatt-1][ell], Lextf);
        }
    }
    else
    {
        if (nscatt==0) LockFree::add(_Fdusdirv[ell], Lextf);
        else LockFree::add(_Fdusscav[ell], Lextf);
    }
    if (_polarization)
    {
        LockFree::add(_FtotQv[ell], Lextf*pp->stokesQ());
        LockFree::add(_FtotUv[ell], Lextf*pp->stokesU());
        LockFree::add(_FtotVv[ell], Lextf*pp->stokesV());
    }

    // frames
    if (l>=0)
    {
        if (pp->isStellar())
        {
            if (nscatt==0)
            {
                LockFree::add(_ftrav(ell,l), L);
                if (_dustsystem)
                    LockFree::add(_fstrdirv(ell,l), Lextf);
            }
            else
            {
                LockFree::add(_fstrscav(ell,l), Lextf);
                if (nscatt<=_Nscatt)
                    LockFree::add(_fstrscavv[nscatt-1](ell,l), Lextf);
            }
        }
        else
        {
            if (nscatt==0)
                LockFree::add(_fdusdirv(ell,l), Lextf);
            else
                LockFree::add(_fdusscav(ell,l), Lextf);
        }
        if (_polarization)
        {
            LockFree::add(_ftotQv(ell,l), Lextf*pp->stokesQ());
            LockFree::add(_ftotUv(ell,l), Lextf*pp->stokesU());
            LockFree::add(_ftotVv(ell,l), Lextf*pp->stokesV());
        }
    }
}

////////////////////////////////////////////////////////////////////

void FullInstrument::write()
{
    // collect all the (or only the necessary) cubes
    std::shared_ptr<Array> ftravComp = _ftrav.constructCompleteCube();
    std::shared_ptr<Array> fstrdirvComp;
    std::shared_ptr<Array> fstrscavComp;
    std::shared_ptr<Array> fdusdirvComp;
    std::shared_ptr<Array> fdusscavComp;
    std::vector<std::shared_ptr<Array>> fstrscavvComp;
    std::shared_ptr<Array> ftotQvComp;
    std::shared_ptr<Array> ftotUvComp;
    std::shared_ptr<Array> ftotVvComp;

    if (_dustsystem)
    {
        fstrdirvComp = _fstrdirv.constructCompleteCube();
        fstrscavComp = _fstrscav.constructCompleteCube();
        if (_dustemission)
        {
            fdusdirvComp = _fdusdirv.constructCompleteCube();
            fdusscavComp = _fdusscav.constructCompleteCube();
        }
        if (_Nscatt > 0)
        {
            fstrscavvComp.reserve(_Nscatt);
            for (auto& cube : _fstrscavv) fstrscavvComp.push_back(cube.constructCompleteCube());
        }
        if (_polarization)
        {
            ftotQvComp = _ftotQv.constructCompleteCube();
            ftotUvComp = _ftotUv.constructCompleteCube();
            ftotVvComp = _ftotVv.constructCompleteCube();
        }
    }

    // compute the total flux and the total dust flux in temporary arrays
    Array ftotv;
    Array Ftotv;
    Array ftotdusv;
    Array Ftotdusv;
    if (_dustemission)
    {
        ftotv = *fstrdirvComp + *fstrscavComp + *fdusdirvComp + *fdusscavComp;
        Ftotv = _Fstrdirv + _Fstrscav + _Fdusdirv + _Fdusscav;
        ftotdusv = *fdusdirvComp + *fdusscavComp;
        Ftotdusv = _Fdusdirv + _Fdusscav;
    }
    else if (_dustsystem)
    {
        ftotv = *fstrdirvComp + *fstrscavComp;
        Ftotv = _Fstrdirv + _Fstrscav;
    }
    else
    {
        // don't output transparent frame separately because it is identical to the total frame
        ftotv = *ftravComp;
        ftravComp->resize(0);
        // do output integrated fluxes to avoid confusing zeros
        Ftotv = _Ftrav;
        _Fstrdirv = _Ftrav;
    }

    // lists of f-array and F-array pointers, and the corresponding file and column names
    QList<Array*> farrays, Farrays;
    QStringList fnames, Fnames;

    // SEDs
    Farrays << &Ftotv << &_Fstrdirv << &_Fstrscav << &Ftotdusv << &_Fdusscav << &_Ftrav;
    Fnames << "total flux" << "direct stellar flux" << "scattered stellar flux"
           << "total dust emission flux" << "dust emission scattered flux" << "transparent flux";
    if (_polarization)
    {
        Farrays << &_FtotQv << &_FtotUv << &_FtotVv;
        Fnames << "total Stokes Q" << "total Stokes U" << "total Stokes V";
    }
    for (int nscatt=0; nscatt<_Nscatt; nscatt++)
    {
        Farrays << &(_Fstrscavv[nscatt]);
        Fnames << (QString::number(nscatt+1) + "-times scattered flux");
    }

    // Sum the SED arrays element-wise across the different processes
    sumResults(Farrays);

    // Frames
    farrays << &ftotv << fstrdirvComp.get() << fstrscavComp.get() << &ftotdusv << fdusscavComp.get() << ftravComp.get();
    fnames << "total" << "direct" << "scattered" << "dust" << "dustscattered" << "transparent";
    if (_polarization)
    {
        farrays << ftotQvComp.get() << ftotUvComp.get() << ftotVvComp.get();
        fnames << "stokesQ" << "stokesU" << "stokesV";
    }
    for (int nscatt=0; nscatt<_Nscatt; nscatt++)
    {
        farrays << fstrscavvComp[nscatt].get();
        fnames << ("scatteringlevel" + QString::number(nscatt+1));
    }

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
