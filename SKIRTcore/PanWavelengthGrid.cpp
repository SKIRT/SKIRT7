/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include <iomanip>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "PanWavelengthGrid.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "Units.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

PanWavelengthGrid::PanWavelengthGrid()
    : _writeWavelengths(false)
{
}

////////////////////////////////////////////////////////////////////

void PanWavelengthGrid::setupSelfAfter()
{
    WavelengthGrid::setupSelfAfter();

    // a wavelength range should be sampled with at least 3 bins for integration algorithms not to crash
    if (_Nlambda < 3) throw FATALERROR("There must be at least three bins in a panchromatic wavelength grid");

    // determine wavelength bin widths
    _dlambdav.resize(_Nlambda);
    for (int ell=0; ell<_Nlambda; ell++)
    {
        _dlambdav[ell] = lambdamax(ell)-lambdamin(ell);
    }

    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();

    // if requested, write a data file with the wavelengths and bin widths
    if (_writeWavelengths && comm->isRoot())
    {
        Units* units = find<Units>();

        QString filename = find<FilePaths>()->output("wavelengths.dat");
        find<Log>()->info("Writing wavelengths to " + filename + "...");
        ofstream file(filename.toLocal8Bit().constData());
        file << "# column 1: lambda;  column 2: delta lambda  (" << units->uwavelength().toStdString() << ")\n";
        file << scientific << setprecision(8);
        for (int ell=0; ell<_Nlambda; ell++)
        {
            file << units->owavelength(_lambdav[ell]) << '\t'
                 << units->owavelength(_dlambdav[ell]) << '\n';
        }
    }
}

//////////////////////////////////////////////////////////////////////

void PanWavelengthGrid::setWriteWavelengths(bool value)
{
    _writeWavelengths = value;
}

//////////////////////////////////////////////////////////////////////

bool PanWavelengthGrid::writeWavelengths() const
{
    return _writeWavelengths;
}

//////////////////////////////////////////////////////////////////////

bool PanWavelengthGrid::issampledrange() const
{
    return true;
}

//////////////////////////////////////////////////////////////////////
