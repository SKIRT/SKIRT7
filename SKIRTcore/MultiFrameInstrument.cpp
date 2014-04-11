/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "InstrumentFrame.hpp"
#include "PeelOffPhotonPackage.hpp"
#include "MultiFrameInstrument.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

MultiFrameInstrument::MultiFrameInstrument()
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

void MultiFrameInstrument::detect(const PeelOffPhotonPackage *pp)
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
