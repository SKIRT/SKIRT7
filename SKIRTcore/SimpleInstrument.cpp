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

    _Ftotv.resize(Nlambda);
    _ftotv.initialize(wavelengthGrid->assigner(), _Nframep);
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
        LockFree::add(_ftotv(ell,l), Lextf);
    }
}

////////////////////////////////////////////////////////////////////

void
SimpleInstrument::write()
{
    // lists of f-array and F-array pointers, and the corresponding file and column names
    QList< Array* > farrays, Farrays;
    QStringList fnames, Fnames;

    Farrays << &_Ftotv;
    Fnames << "total flux";
    // Sum the flux arrays element-wise across the different processes
    sumResults(Farrays);

    // Assemble the datacube
    std::shared_ptr<Array> completeCube = _ftotv.constructCompleteCube();
    farrays << completeCube.get();
    fnames << "total";

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
