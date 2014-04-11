/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "FrameInstrument.hpp"
#include "PeelOffPhotonPackage.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

FrameInstrument::FrameInstrument()
{
}

////////////////////////////////////////////////////////////////////

void FrameInstrument::setupSelfBefore()
{
    SingleFrameInstrument::setupSelfBefore();

    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _ftotv.resize(Nlambda*_Nxp*_Nyp);
}

////////////////////////////////////////////////////////////////////

void
FrameInstrument::detect(const PeelOffPhotonPackage* pp)
{
    int l = pixelondetector(pp);
    if (l >= 0)
    {
        int ell = pp->ell();
        int m = l + ell*_Nxp*_Nyp;
        double L = pp->luminosity();
        double taupath = pp->opticaldepth();
        double extf = exp(-taupath);
        double Lextf = L*extf;

        record(&_ftotv[m], Lextf);
    }
}

////////////////////////////////////////////////////////////////////

void
FrameInstrument::write()
{
    // lists of f-array pointers, and the corresponding file and column names
    QList< Array* > farrays;
    QStringList fnames;
    farrays << &_ftotv;
    fnames << "total";

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
}

////////////////////////////////////////////////////////////////////
