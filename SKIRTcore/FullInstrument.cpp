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
    WavelengthGrid* wavelengthGrid = find<WavelengthGrid>();
    const ProcessAssigner* wavelengthAssigner = wavelengthGrid->assigner();
    int Nlambda = wavelengthGrid->Nlambda();

    _ftrav.resize(Nlambda*_Nframep);
    _distftrav.initialize(wavelengthAssigner, _Nframep);
    _Ftrav.resize(Nlambda);
    if (_dustsystem)
    {
        _fstrdirv.resize(Nlambda*_Nframep);
        _distfstrdirv.initialize(wavelengthAssigner, _Nframep);
        _Fstrdirv.resize(Nlambda);
        _fstrscav.resize(Nlambda*_Nframep);
        _distfstrscav.initialize(wavelengthAssigner, _Nframep);
        _Fstrscav.resize(Nlambda);
        if (_dustemission)
        {
            _fdusdirv.resize(Nlambda*_Nframep);
            _distfdusdirv.initialize(wavelengthAssigner, _Nframep);
            _Fdusdirv.resize(Nlambda);
            _fdusscav.resize(Nlambda*_Nframep);
            _distfdusscav.initialize(wavelengthAssigner, _Nframep);
            _Fdusscav.resize(Nlambda);
        }
        if (_Nscatt > 0)
        {
            _fstrscavv.resize(_Nscatt, Nlambda*_Nframep);
            _distfstrscavv.resize(_Nscatt);
            for (auto& cube : _distfstrscavv) cube.initialize(wavelengthAssigner, _Nframep);
            _Fstrscavv.resize(_Nscatt, Nlambda);
        }
        if (_polarization)
        {
            _ftotQv.resize(Nlambda*_Nframep);
            _distftotQv.initialize(wavelengthAssigner, _Nframep);
            _FtotQv.resize(Nlambda);
            _ftotUv.resize(Nlambda*_Nframep);
            _distftotUv.initialize(wavelengthAssigner, _Nframep);
            _FtotUv.resize(Nlambda);
            _ftotVv.resize(Nlambda*_Nframep);
            _distftotVv.initialize(wavelengthAssigner, _Nframep);
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
                LockFree::add(_distftrav(ell,l), L);
                if (_dustsystem)
                {
                    LockFree::add(_fstrdirv[m], Lextf);
                    LockFree::add(_distfstrdirv(ell,l), Lextf);
                }
            }
            else
            {
                LockFree::add(_fstrscav[m], Lextf);
                LockFree::add(_distfstrscav(ell,l), Lextf);
                if (nscatt<=_Nscatt)
                {
                    LockFree::add(_fstrscavv[nscatt-1][m], Lextf);
                    LockFree::add(_distfstrscavv[nscatt-1](ell,l), Lextf);
                }
            }
        }
        else
        {
            if (nscatt==0)
            {
                LockFree::add(_fdusdirv[m], Lextf);
                LockFree::add(_distfdusdirv(ell,l), Lextf);
            }
            else
            {
                LockFree::add(_fdusscav[m], Lextf);
                LockFree::add(_distfdusscav(ell,l), Lextf);
            }
        }
        if (_polarization)
        {
            LockFree::add(_ftotQv[m], Lextf*pp->stokesQ());
            LockFree::add(_distftotQv(ell,l), Lextf*pp->stokesQ());
            LockFree::add(_ftotUv[m], Lextf*pp->stokesU());
            LockFree::add(_distftotUv(ell,l), Lextf*pp->stokesU());
            LockFree::add(_ftotVv[m], Lextf*pp->stokesV());
            LockFree::add(_distftotVv(ell,l), Lextf*pp->stokesV());
        }
    }
}

////////////////////////////////////////////////////////////////////

void FullInstrument::write()
{
    // SOMMATIEOPERATOR VOOR PARALLELDATACUBES NODIG?

    // collect all the (or only the necessary) cubes
    std::shared_ptr<Array> ftravComp = _distftrav.constructCompleteCube();
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
        fstrdirvComp = _distfstrdirv.constructCompleteCube();
        fstrscavComp = _distfstrscav.constructCompleteCube();
        if (_dustemission)
        {
            fdusdirvComp = _distfdusdirv.constructCompleteCube();
            fdusscavComp = _distfdusscav.constructCompleteCube();
        }
        if (_Nscatt > 0)
        {
            fstrscavvComp.reserve(_Nscatt);
            for (auto& cube : _distfstrscavv) fstrscavvComp.push_back(cube.constructCompleteCube());
        }
        if (_polarization)
        {
            ftotQvComp = _distftotQv.constructCompleteCube();
            ftotUvComp = _distftotUv.constructCompleteCube();
            ftotVvComp = _distftotVv.constructCompleteCube();
        }
    }

    // compute the total flux and the total dust flux in temporary arrays
    Array ftotv;
    Array newftotv;
    Array Ftotv;
    Array ftotdusv;
    Array newftotdusv;
    Array Ftotdusv;
    if (_dustemission)
    {
        ftotv = _fstrdirv + _fstrscav + _fdusdirv + _fdusscav;
        newftotv = *fstrdirvComp + *fstrscavComp + *fdusdirvComp + *fdusscavComp;
        Ftotv = _Fstrdirv + _Fstrscav + _Fdusdirv + _Fdusscav;
        ftotdusv = _fdusdirv + _fdusscav;
        newftotdusv = *fdusdirvComp + *fdusscavComp;
        Ftotdusv = _Fdusdirv + _Fdusscav;
    }
    else if (_dustsystem)
    {
        ftotv = _fstrdirv + _fstrscav;
        newftotv = *fstrdirvComp + *fstrscavComp;
        Ftotv = _Fstrdirv + _Fstrscav;
    }
    else
    {
        // don't output transparent frame separately because it is identical to the total frame
        ftotv = _ftrav;
        newftotv = *ftravComp;
        _ftrav.resize(0);
        ftravComp->resize(0); // kan in dit geval geen kwaad, maar is natuurlijk wel niet zo netjes
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

    farrays << &newftotv << fstrdirvComp.get() << fstrscavComp.get() << &newftotdusv << fdusscavComp.get() << ftravComp.get();
    fnames << "newtotal" << "newdirect" << "newscattered" << "newdust" << "newdustscattered" << "newtransparent";
    if (_polarization)
    {
        farrays << ftotQvComp.get() << ftotUvComp.get() << ftotVvComp.get();
        fnames << "newstokesQ" << "newstokesU" << "newstokesV";
    }
    for (int nscatt=0; nscatt<_Nscatt; nscatt++)
    {
        farrays << fstrscavvComp[nscatt].get();
        fnames << ("newscatteringlevel" + QString::number(nscatt+1));
    }

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
