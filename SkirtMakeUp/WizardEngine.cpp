/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "WizardEngine.hpp"

#include "BasicChoiceWizardPane.hpp"
#include "BoolPropertyHandler.hpp"
#include "CreateRootWizardPane.hpp"
#include "DoubleListPropertyHandler.hpp"
#include "DoublePropertyHandler.hpp"
#include "DoublePropertyWizardPane.hpp"
#include "EnumPropertyHandler.hpp"
#include "IntPropertyHandler.hpp"
#include "ItemListPropertyHandler.hpp"
#include "ItemPropertyHandler.hpp"
#include "ItemPropertyWizardPane.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "StringPropertyHandler.hpp"
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
        {
            QByteArray currentType = _root ? itemType(_root) : "";
            QByteArray abstractType = _choice==NewSki ? "Simulation" : "FitScheme";
            return SimulationItemDiscovery::inherits(currentType, abstractType);
        }
    case ConstructHierarchy:
        return _propertyValid;
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
    switch (_state)
    {
    case BasicChoice:
        {
            _state = CreateRoot;
        }
        break;
    case CreateRoot:
        {
            _state = ConstructHierarchy;
            _current = _root;
            _propertyIndex = 0; // assumes that the root has at least one property
            _propertyValid = false;
        }
        break;
    case ConstructHierarchy:
        {
            if (_propertyIndex+1 < properties(_current).size())
            {
                _propertyIndex++;
                _propertyValid = false;
            }
            else _state = SaveHierarchy;
        }
        break;
    case SaveHierarchy:
        break;
    }
    emitStateChanged();
}

////////////////////////////////////////////////////////////////////

void WizardEngine::retreat()
{
    switch (_state)
    {
    case BasicChoice:
        break;
    case CreateRoot:
        {
            _state = BasicChoice;
        }
        break;
    case ConstructHierarchy:
        {
            if (_propertyIndex > 0)
            {
                _propertyIndex--;
                _propertyValid = false;
            }
            else _state = CreateRoot;
        }
        break;
    case SaveHierarchy:
        {
            _state = ConstructHierarchy;
            _propertyValid = false;
        }
        break;
    }
    emitStateChanged();
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

void WizardEngine::setPropertyValid(bool valid)
{
    _propertyValid = valid;
    emit canAdvanceChangedTo(canAdvance());
}

////////////////////////////////////////////////////////////////////

QWidget* WizardEngine::createPane()
{
    switch (_state)
    {
    case BasicChoice:
        {
            return new BasicChoiceWizardPane(_choice, this);
        }
    case CreateRoot:
        {
            QByteArray currentType = _root ? itemType(_root) : "";
            QByteArray abstractType = _choice==NewSki ? "Simulation" : "FitScheme";
            return new CreateRootWizardPane(abstractType, currentType, this);
        }
    case ConstructHierarchy:
        {
            PropertyHandlerPtr handler = createPropertyHandler(_current, properties(_current)[_propertyIndex]);
            if (handler.dynamicCast<BoolPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
            if (handler.dynamicCast<IntPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
            if (handler.dynamicCast<DoublePropertyHandler>())
                return new DoublePropertyWizardPane(handler, this);
            if (handler.dynamicCast<DoubleListPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
            if (handler.dynamicCast<StringPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
            if (handler.dynamicCast<EnumPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
            if (handler.dynamicCast<ItemPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
            if (handler.dynamicCast<ItemListPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
        }
    case SaveHierarchy:
        return new QLabel("Dummy wizard pane for state SaveHierarchy");
    }
}

////////////////////////////////////////////////////////////////////
