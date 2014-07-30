/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "WizardEngine.hpp"

#include "SimulationItem.hpp"
#include <QLabel>

////////////////////////////////////////////////////////////////////

WizardEngine::WizardEngine(QObject* parent)
    : QObject(parent), _state(BasicChoice), _root(0)
{
}

////////////////////////////////////////////////////////////////////

WizardEngine::~WizardEngine()
{
    delete _root;
}

////////////////////////////////////////////////////////////////////

bool WizardEngine::canAdvance()
{
    return _state != SaveHierarchy;
}

////////////////////////////////////////////////////////////////////

bool WizardEngine::canRetreat()
{
    return _state != BasicChoice;
}

////////////////////////////////////////////////////////////////////

bool WizardEngine::isDirty()
{
    return _state == ConstructHierarchy;
}

////////////////////////////////////////////////////////////////////

void WizardEngine::advance()
{
    switch (_state)
    {
    case BasicChoice:
        _state = CreateRoot;
        emitStateChanged();
        break;
    case CreateRoot:
        _state = ConstructHierarchy;
        emitStateChanged();
        break;
    case ConstructHierarchy:
        _state = SaveHierarchy;
        emitStateChanged();
        break;
    case SaveHierarchy:
        break;
    }
}

////////////////////////////////////////////////////////////////////

void WizardEngine::retreat()
{
    switch (_state)
    {
    case BasicChoice:
        break;
    case CreateRoot:
        _state = BasicChoice;
        emitStateChanged();
        break;
    case ConstructHierarchy:
        _state = CreateRoot;
        emitStateChanged();
        break;
    case SaveHierarchy:
        _state = ConstructHierarchy;
        emitStateChanged();
        break;
    }
}

////////////////////////////////////////////////////////////////////

void WizardEngine::emitStateChanged()
{
    emit stateChanged();
    emit canAdvanceChangedTo(canAdvance());
    emit canRetreatChangedTo(canRetreat());
}

////////////////////////////////////////////////////////////////////

QWidget* WizardEngine::createPane()
{
    return new QLabel(QString::number(_state));
}

////////////////////////////////////////////////////////////////////
