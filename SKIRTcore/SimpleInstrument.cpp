/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "LockFree.hpp"
#include "PhotonPackage.hpp"
#include "SimpleInstrument.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SimpleInstrument::SimpleInstrument()
{
}

////////////////////////////////////////////////////////////////////

void SimpleInstrument::setupSelfBefore()
{
    SingleFrameInstrument::setupSelfBefore();

    WavelengthGrid* wavelengthGrid = find<WavelengthGrid>();
    int Nlambda = wavelengthGrid->Nlambda();

    _ftotv.resize(Nlambda*_Nframep);
    _Ftotv.resize(Nlambda);
    _distftotv.initialize(wavelengthGrid->assigner(), _Nframep);
}

////////////////////////////////////////////////////////////////////

void
SimpleInstrument::detect(PhotonPackage* pp)
{
    int l = pixelondetector(pp);
    int ell = pp->ell();
    double L = pp->luminosity();
    double taupath = opticalDepth(pp);
    double extf = exp(-taupath);
    double Lextf = L*extf;

    LockFree::add(_Ftotv[ell], Lextf);
    if (l>=0)
    {
        size_t m = l + ell*_Nframep;
        LockFree::add(_ftotv[m], Lextf);
        LockFree::add(_distftotv(ell,l), Lextf);
    }
}

////////////////////////////////////////////////////////////////////

void
SimpleInstrument::write()
{
    // lists of f-array and F-array pointers, and the corresponding file and column names
    QList< Array* > farrays, Farrays;
    QStringList fnames, Fnames;
    farrays << &_ftotv;
    Farrays << &_Ftotv;
    fnames << "total";
    Fnames << "total flux";

    // Sum the flux arrays element-wise across the different processes
    sumResults(farrays);
    sumResults(Farrays);

    std::shared_ptr<Array> completeCube = _distftotv.constructCompleteCube();
    farrays << completeCube.get();
    fnames << "new cube";

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
