/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Instrument.hpp"
#include "DustSystem.hpp"
#include "FatalError.hpp"
#include "PhotonPackage.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

Instrument::Instrument()
    : _ds(0)
{
}

////////////////////////////////////////////////////////////////////

void Instrument::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    try
    {
        // get a pointer to the dust system without performing setup
        // to avoid catching (and hiding) fatal errors during such setup
        _ds = find<DustSystem>(false);
    }
    catch (FatalError)
    {
        _ds = 0;
    }
}

////////////////////////////////////////////////////////////////////

void Instrument::setInstrumentName(QString value)
{
    _instrumentname = value;
}

////////////////////////////////////////////////////////////////////

QString Instrument::instrumentName() const
{
    return _instrumentname;
}

////////////////////////////////////////////////////////////////////

double Instrument::opticalDepth(PhotonPackage* pp, double distance) const
{
    return _ds ? _ds->opticaldepth(pp,distance) : 0;
}

////////////////////////////////////////////////////////////////////
