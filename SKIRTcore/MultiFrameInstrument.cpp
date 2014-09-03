/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "InstrumentFrame.hpp"
#include "PhotonPackage.hpp"
#include "MultiFrameInstrument.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

MultiFrameInstrument::MultiFrameInstrument()
    : _writeTotal(true), _writeStellarComps(false)
{
}

////////////////////////////////////////////////////////////////////

void MultiFrameInstrument::setupSelfBefore()
{
    DistantInstrument::setupSelfBefore();

    // verify attribute values
    if (_frames.size() != find<WavelengthGrid>()->Nlambda())
        throw FATALERROR("Number of instrument frames must equal number of wavelengths");
}

////////////////////////////////////////////////////////////////////

void MultiFrameInstrument::setWriteTotal(bool value)
{
    _writeTotal = value;
}

////////////////////////////////////////////////////////////////////

bool MultiFrameInstrument::writeTotal() const
{
    return _writeTotal;
}

////////////////////////////////////////////////////////////////////

void MultiFrameInstrument::setWriteStellarComps(bool value)
{
    _writeStellarComps = value;
}

////////////////////////////////////////////////////////////////////

bool MultiFrameInstrument::writeStellarComps() const
{
    return _writeStellarComps;
}

////////////////////////////////////////////////////////////////////

void MultiFrameInstrument::addFrame(InstrumentFrame* value)
{
    if (!value) throw FATALERROR("Instrument frame pointer shouldn't be null");
    value->setParent(this);
    _frames << value;
}

////////////////////////////////////////////////////////////////////

QList<InstrumentFrame*> MultiFrameInstrument::frames() const
{
    return _frames;
}

////////////////////////////////////////////////////////////////////

void MultiFrameInstrument::detect(PhotonPackage* pp)
{
    _frames[pp->ell()]->detect(pp);
}

////////////////////////////////////////////////////////////////////

void MultiFrameInstrument::write()
{
    int Nlambda = _frames.size();
    for (int ell=0; ell<Nlambda; ell++)
    {
        _frames[ell]->calibrateAndWriteData(ell);
    }
}

////////////////////////////////////////////////////////////////////
