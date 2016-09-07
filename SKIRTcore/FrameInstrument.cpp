/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <memory>

#include "FatalError.hpp"
#include "FrameInstrument.hpp"
#include "LockFree.hpp"
#include "PhotonPackage.hpp"
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

    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();
    if (comm->dataParallel())
        _distftotv.initialize(_Nframep, comm);
    else
        _distftotv.initialize(_Nframep, comm);
}

////////////////////////////////////////////////////////////////////

void
FrameInstrument::detect(PhotonPackage* pp)
{
    int l = pixelondetector(pp);
    if (l >= 0)
    {
        int ell = pp->ell();
        double L = pp->luminosity();
        double taupath = opticalDepth(pp);
        double extf = exp(-taupath);
        double Lextf = L*extf;

        LockFree::add(_distftotv(ell,l), Lextf);
    }
}

////////////////////////////////////////////////////////////////////

void
FrameInstrument::write()
{
    // lists of f-array pointers, and the corresponding file and column names
    QList< Array* > farrays;
    QStringList fnames;

    // Sum the flux arrays element-wise across the different processes
    std::shared_ptr<Array> completeCube = _distftotv.constructCompleteCube();
    farrays << completeCube.get();
    fnames << "total";

    // calibrate and output the arrays
    calibrateAndWriteDataCubes(farrays, fnames);
}

////////////////////////////////////////////////////////////////////
