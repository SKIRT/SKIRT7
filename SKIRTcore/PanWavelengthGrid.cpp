/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "PanWavelengthGrid.hpp"
#include "TextFile.hpp"
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

    // if requested, write a data file with the wavelengths and bin widths
    if (_writeWavelengths)
    {
        Units* units = find<Units>();

        QString filename = find<FilePaths>()->output("wavelengths.dat");
        find<Log>()->info("Writing wavelengths to " + filename + "...");

        // Create a text file
        TextFile file(filename);

        file.writeLine("# column 1: lambda;  column 2: delta lambda  (" + units->uwavelength() + ")");
        for (int ell=0; ell<_Nlambda; ell++)
        {
            file.writeLine(QString::number(units->owavelength(_lambdav[ell]), 'e', 8) + '\t'
                         + QString::number(units->owavelength(_dlambdav[ell]), 'e', 8));
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
