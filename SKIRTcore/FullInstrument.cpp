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
#include "Units.hpp"
#include "WavelengthGrid.hpp"

#include <iostream>

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
    _ftrav.resize(Nlambda*_Nframep);
    _Ftrav.resize(Nlambda);
    if (_dustsystem)
    {
        _fstrdirv.resize(Nlambda*_Nframep);
        _Fstrdirv.resize(Nlambda);
        _fstrscav.resize(Nlambda*_Nframep);
        _Fstrscav.resize(Nlambda);
        if (_dustemission)
        {
            _fdusdirv.resize(Nlambda*_Nframep);
            _Fdusdirv.resize(Nlambda);
            _fdusscav.resize(Nlambda*_Nframep);
            _Fdusscav.resize(Nlambda);
        }
        if (_Nscatt > 0)
        {
            _fstrscavv.resize(_Nscatt, Nlambda*_Nframep);
            _Fstrscavv.resize(_Nscatt, Nlambda);
        }
        if (_polarization)
        {
            _ftotQv.resize(Nlambda*_Nframep);
            _FtotQv.resize(Nlambda);
            _ftotUv.resize(Nlambda*_Nframep);
            _FtotUv.resize(Nlambda);
            _ftotVv.resize(Nlambda*_Nframep);
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
        size_t m = l + ell*_Nframep;
        if (pp->isStellar())
        {
            if (nscatt==0)
            {
                LockFree::add(_ftrav[m], L);
                if (_dustsystem) LockFree::add(_fstrdirv[m], Lextf);
            }
            else
            {
                LockFree::add(_fstrscav[m], Lextf);
                if (nscatt<=_Nscatt) LockFree::add(_fstrscavv[nscatt-1][m], Lextf);
            }
        }
        else
        {
            if (nscatt==0) LockFree::add(_fdusdirv[m], Lextf);
            else LockFree::add(_fdusscav[m], Lextf);
        }
        if (_polarization)
        {
            LockFree::add(_ftotQv[m], Lextf*pp->stokesQ());
            LockFree::add(_ftotUv[m], Lextf*pp->stokesU());
            LockFree::add(_ftotVv[m], Lextf*pp->stokesV());
        }
    }
}

////////////////////////////////////////////////////////////////////

void FullInstrument::write()
{
    // compute the total flux and the total dust flux in temporary arrays
    Array ftotv;
    Array Ftotv;
    Array ftotdusv;
    Array Ftotdusv;
    if (_dustemission)
    {
        ftotv = _fstrdirv + _fstrscav + _fdusdirv + _fdusscav;
        Ftotv = _Fstrdirv + _Fstrscav + _Fdusdirv + _Fdusscav;
        ftotdusv = _fdusdirv + _fdusscav;
        Ftotdusv = _Fdusdirv + _Fdusscav;
    }
    else if (_dustsystem)
    {
        ftotv = _fstrdirv + _fstrscav;
        Ftotv = _Fstrdirv + _Fstrscav;
    }
    else
    {
        // don't output transparent frame separately because it is identical to the total frame
        ftotv = _ftrav;
        _ftrav.resize(0);
        // do output integrated fluxes to avoid confusing zeros
        Ftotv = _Ftrav;
        _Fstrdirv = _Ftrav;
    }

    // lists of f-array and F-array pointers, and the corresponding file and column names
    QList<Array*> farrays, Farrays;
    QStringList fnames, Fnames;
    farrays << &ftotv << &_fstrdirv << &_fstrscav << &ftotdusv << &_fdusscav << &_ftrav;
    Farrays << &Ftotv << &_Fstrdirv << &_Fstrscav << &Ftotdusv << &_Fdusscav << &_Ftrav;
    fnames << "total" << "direct" << "scattered" << "dust" << "dustscattered" << "transparent";
    Fnames << "total flux" << "direct stellar flux" << "scattered stellar flux"
           << "total dust emission flux" << "dust emission scattered flux" << "transparent flux";
    if (_polarization)
    {
        farrays << &_ftotQv << &_ftotUv << &_ftotVv;
        Farrays << &_FtotQv << &_FtotUv << &_FtotVv;
        fnames << "stokesQ" << "stokesU" << "stokesV";
        Fnames << "total Stokes Q" << "total Stokes U" << "total Stokes V";
    }
    for (int nscatt=0; nscatt<_Nscatt; nscatt++)
    {
        farrays << &(_fstrscavv[nscatt]);
        Farrays << &(_Fstrscavv[nscatt]);
        fnames << ("scatteringlevel" + QString::number(nscatt+1));
        Fnames << (QString::number(nscatt+1) + "-times scattered flux");
    }

    // Sum the flux arrays element-wise across the different processes
    sumResults(farrays);
    sumResults(Farrays);

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
