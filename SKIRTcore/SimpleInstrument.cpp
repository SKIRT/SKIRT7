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

    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _ftotv.resize(Nlambda*_Nxp*_Nyp);
    _Ftotv.resize(Nlambda);
}

////////////////////////////////////////////////////////////////////

void
SimpleInstrument::detect(PhotonPackage* pp)
{
    int l = pixelondetector(pp);
    int ell = pp->ell();
    int m = l + ell*_Nxp*_Nyp;
    double L = pp->luminosity();
    double taupath = opticalDepth(pp);
    double extf = exp(-taupath);
    double Lextf = L*extf;

    LockFree::add(_Ftotv[ell], Lextf);
    if (l>=0) LockFree::add(_ftotv[m], Lextf);
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

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
