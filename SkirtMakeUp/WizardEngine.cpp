/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "WizardEngine.hpp"

#include "SimulationItem.hpp"
#include "BasicChoiceWizardPane.hpp"
#include <QLabel>

////////////////////////////////////////////////////////////////////

WizardEngine::WizardEngine(QObject* parent)
    : QObject(parent), _state(BasicChoice), _choice(Unknown), _root(0), _current(0)
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
    switch (_state)
    {
    case BasicChoice:
        return _choice != Unknown;
    case CreateRoot:
        return true;
    case ConstructHierarchy:
        return true;
    case SaveHierarchy:
        return false;
    }
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
    if (canAdvance())
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

void WizardEngine::setBasicChoice(int newChoice)
{
    _choice = static_cast<Choice>(newChoice);
    emit canAdvanceChangedTo(canAdvance());
}

////////////////////////////////////////////////////////////////////

QWidget* WizardEngine::createPane()
{
    switch (_state)
    {
    case BasicChoice:
        return new BasicChoiceWizardPane(_choice, this);
    case CreateRoot:
        return new QLabel("Dummy wizard pane for state CreateRoot");
    case ConstructHierarchy:
        return new QLabel("Dummy wizard pane for state ConstructHierarchy");
    case SaveHierarchy:
        return new QLabel("Dummy wizard pane for state SaveHierarchy");
    }
}

////////////////////////////////////////////////////////////////////
