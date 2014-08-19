/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "WizardEngine.hpp"

#include "BasicChoiceWizardPane.hpp"
#include "CreateRootWizardPane.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include <QLabel>

using namespace SimulationItemDiscovery;

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
        return _root != 0;
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

void WizardEngine::setRootType(QByteArray newRootType)
{
    if (_root && itemType(_root)==newRootType) return;
    delete _root;
    _root = createSimulationItem(newRootType);
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
        {
            QByteArray currentType = _root ? itemType(_root) : "";
            QByteArray abstractType = _choice==NewSki ? "Simulation" : "FitScheme";
            return new CreateRootWizardPane(abstractType, currentType, this);
        }
    case ConstructHierarchy:
        return new QLabel("Dummy wizard pane for state ConstructHierarchy");
    case SaveHierarchy:
        return new QLabel("Dummy wizard pane for state SaveHierarchy");
    }
}

////////////////////////////////////////////////////////////////////
