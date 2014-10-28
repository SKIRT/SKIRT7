/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "LockFree.hpp"
#include "PhotonPackage.hpp"
#include "SEDInstrument.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SEDInstrument::SEDInstrument()
{
}

////////////////////////////////////////////////////////////////////

void SEDInstrument::setupSelfBefore()
{
    DistantInstrument::setupSelfBefore();

    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _Ftotv.resize(Nlambda);
}

////////////////////////////////////////////////////////////////////

void
SEDInstrument::detect(PhotonPackage* pp)
{
    int ell = pp->ell();
    double L = pp->luminosity();
    double taupath = opticalDepth(pp);
    double extf = exp(-taupath);
    double Lextf = L*extf;

    LockFree::add(_Ftotv[ell], Lextf);
}

////////////////////////////////////////////////////////////////////

void
SEDInstrument::write()
{
    // lists of F-array pointers, and the corresponding file and column names
    QList< Array* > Farrays;
    QStringList Fnames;
    Farrays << &_Ftotv;
    Fnames << "total flux";

    sumResults(Farrays);

    // calibrate and output the arrays
    calibrateAndWriteSEDs(Farrays, Fnames);
}

////////////////////////////////////////////////////////////////////
