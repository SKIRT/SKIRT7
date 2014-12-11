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
    : _Nscatt(0), _dustsystem(false), _dustemission(false)
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

    // determine whether the simulation includes dust emission
    if (_dustsystem)
    {
        try
        {
            // attempt to locate a panchromatic dust system without performing setup
            // to avoid catching (and hiding) fatal errors during such setup
            // and, if successful, ask it whether dust emission has been turned on
            _dustemission = find<PanDustSystem>(false)->dustemission();
        }
        catch (FatalError) { }
    }

    // resize the detector arrays only when meaningful
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _fdirv.resize(Nlambda*_Nxp*_Nyp);
    _Fdirv.resize(Nlambda);
    if (_dustsystem)
    {
        _ftrav.resize(Nlambda*_Nxp*_Nyp);
        _Ftrav.resize(Nlambda);
        _fscav.resize(Nlambda*_Nxp*_Nyp);
        _Fscav.resize(Nlambda);
        if (_Nscatt > 0)
        {
            _fscavv.resize(_Nscatt+1, Nlambda*_Nxp*_Nyp);
            _Fscavv.resize(_Nscatt+1, Nlambda);
        }
        if (_dustemission)
        {
            _fdusv.resize(Nlambda*_Nxp*_Nyp);
            _Fdusv.resize(Nlambda);
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
    int l = pixelondetector(pp);
    int ell = pp->ell();
    int m = l + ell*_Nxp*_Nyp;
    double L = pp->luminosity();
    double taupath = opticalDepth(pp);
    double extf = exp(-taupath);
    double Lextf = L*extf;

    if (pp->isStellar())
    {
        int nscatt = pp->nScatt();
        if (nscatt==0)
        {
            if (_dustsystem) LockFree::add(_Ftrav[ell], L);
            LockFree::add(_Fdirv[ell], Lextf);
        }
        else
        {
            LockFree::add(_Fscav[ell], Lextf);
            if (nscatt<=_Nscatt) LockFree::add(_Fscavv[nscatt][ell], Lextf);
        }
    }
    else
    {
        LockFree::add(_Fdusv[ell], Lextf);
    }

    if (l>=0)
    {
        if (pp->isStellar())
        {
            int nscatt = pp->nScatt();
            if (nscatt==0)
            {
                if (_dustsystem) LockFree::add(_ftrav[m], L);
                LockFree::add(_fdirv[m], Lextf);
            }
            else
            {
                LockFree::add(_fscav[m], Lextf);
                if (nscatt<=_Nscatt) LockFree::add(_fscavv[nscatt][m], Lextf);
            }
        }
        else
        {
            LockFree::add(_fdusv[m], Lextf);
        }
    }
}

////////////////////////////////////////////////////////////////////

void FullInstrument::write()
{
    // compute the total flux in temporary arrays
    Array ftotv;
    Array Ftotv;
    if (_dustemission)
    {
        ftotv = _fdirv + _fscav + _fdusv;
        Ftotv = _Fdirv + _Fscav + _Fdusv;
    }
    else if (_dustsystem)
    {
        ftotv = _fdirv + _fscav;
        Ftotv = _Fdirv + _Fscav;
    }
    else
    {
        // don't output direct or transparent frame's separately because they are identical to the total frame
        ftotv = _fdirv;
        _fdirv.resize(0);
        // do output integrated fluxes to avoid confusing zeros
        Ftotv = _Fdirv;
        _Ftrav = _Fdirv;
    }

    // lists of f-array and F-array pointers, and the corresponding file and column names
    QList< Array* > farrays, Farrays;
    QStringList fnames, Fnames;
    farrays << &ftotv << &_fdirv << &_fscav << &_fdusv << &_ftrav;
    Farrays << &Ftotv << &_Fdirv << &_Fscav << &_Fdusv << &_Ftrav;
    fnames << "total" << "direct" << "scattered" << "dust" << "transparent";
    Fnames << "total flux" << "direct stellar flux" << "scattered stellar flux" << "dust flux" << "transparent flux";
    for (int nscatt=1; nscatt<=_Nscatt; nscatt++)
    {
        farrays << &(_fscavv[nscatt]);
        Farrays << &(_Fscavv[nscatt]);
        fnames << ("scatteringlevel" + QString::number(nscatt));
        Fnames << (QString::number(nscatt) + "-times scattered flux");
    }

    // Sum the flux arrays element-wise across the different processes
    sumResults(farrays);
    sumResults(Farrays);

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
