/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "FullInstrument.hpp"
#include "LockFree.hpp"
#include "PhotonPackage.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

FullInstrument::FullInstrument()
    : _Nscatt(0)
{
}

////////////////////////////////////////////////////////////////////

void FullInstrument::setupSelfBefore()
{
    SingleFrameInstrument::setupSelfBefore();

    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _fdirv.resize(Nlambda*_Nxp*_Nyp);
    _fscav.resize(Nlambda*_Nxp*_Nyp);
    _ftrav.resize(Nlambda*_Nxp*_Nyp);
    _fdusv.resize(Nlambda*_Nxp*_Nyp);
    _Fdirv.resize(Nlambda);
    _Fscav.resize(Nlambda);
    _Ftrav.resize(Nlambda);
    _Fdusv.resize(Nlambda);

    if (_Nscatt > 0)
    {
        _fscavv.resize(_Nscatt+1, Nlambda*_Nxp*_Nyp);
        _Fscavv.resize(_Nscatt+1, Nlambda);
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

void
FullInstrument::detect(PhotonPackage* pp)
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
            LockFree::add(_Ftrav[ell], L);
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
                LockFree::add(_ftrav[m], L);
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

void
FullInstrument::write()
{
    // temporary arrays with the total flux
    Array ftotv(_fdirv + _fscav + _fdusv);
    Array Ftotv(_Fdirv + _Fscav + _Fdusv);

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

    sumResults(farrays);
    sumResults(Farrays);

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
