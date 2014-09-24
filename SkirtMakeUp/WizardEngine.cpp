/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "WizardEngine.hpp"

#include "BasicChoiceWizardPane.hpp"
#include "BoolPropertyHandler.hpp"
#include "BoolPropertyWizardPane.hpp"
#include "CreateRootWizardPane.hpp"
#include "DoubleListPropertyHandler.hpp"
#include "DoubleListPropertyWizardPane.hpp"
#include "DoublePropertyHandler.hpp"
#include "DoublePropertyWizardPane.hpp"
#include "EnumPropertyHandler.hpp"
#include "EnumPropertyWizardPane.hpp"
#include "IntPropertyHandler.hpp"
#include "IntPropertyWizardPane.hpp"
#include "ItemListPropertyHandler.hpp"
#include "ItemListPropertyWizardPane.hpp"
#include "ItemPropertyHandler.hpp"
#include "ItemPropertyWizardPane.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "StringPropertyHandler.hpp"
#include "StringPropertyWizardPane.hpp"
#include <QLabel>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

WizardEngine::WizardEngine(QObject* parent)
    : QObject(parent), _state(BasicChoice), _choice(NewSki), _root(0), _current(0)
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

namespace
{
    // returns the property index for the specified child item in the specified parent item
    int propertyIndexForChild(SimulationItem* parent, SimulationItem* child)
    {
        int index = 0;
        for (auto property : properties(parent))
        {
            PropertyHandlerPtr handler = createPropertyHandler(parent, property);

            // check the value of item properties
            auto itemhandler = handler.dynamicCast<ItemPropertyHandler>();
            if (itemhandler && itemhandler->value() == child) return index;

            // check the values of item list properties
            auto itemlisthandler = handler.dynamicCast<ItemListPropertyHandler>();
            if (itemlisthandler)
            {
                for (auto item : itemlisthandler->value()) if (item == child) return index;
            }

            index++;
        }
        return 0;  // this should never happen
    }
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
            // if the property being handled is an item, and the item has properties, then descend the hierarchy
            auto handler = createPropertyHandler(_current, properties(_current)[_propertyIndex]);
            auto itemhandler = handler.dynamicCast<ItemPropertyHandler>();
            if (itemhandler && itemhandler->value() && properties(itemhandler->value()).size()>0)
            {
                _current = itemhandler->value();
                _propertyIndex = 0;
            }

            // otherwise, if the property being handled is an item list, ...  TO DO
            // ...

            // otherwise, if this was the last property at this level, move up the hierarchy to a level where
            // there are properties to advance to; if we encounter the root item, then move to the SaveHierarchy state
            else
            {
                while (_propertyIndex+1 == properties(_current).size())
                {
                    if (_current == _root)
                    {
                        _state = SaveHierarchy;
                        break;
                    }
                    auto parent = dynamic_cast<SimulationItem*>(_current->parent());
                    _propertyIndex = propertyIndexForChild(parent, _current);
                    _current = parent;
                }

                // advance to the next property (meaningless and harmless if state has changed to SaveHierarchy)
                _propertyIndex++;
            }

            // indicate property invalid (meaningless and harmless if state has changed to SaveHierarchy)
            _propertyValid = false;
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
            // if this was the first property at this level, move up the hierarchy to the previous level
            // unless this is already the root item, in which case we move to the CreateRoot state
            if (_propertyIndex == 0)
            {
                if (_current == _root)
                {
                    _state = CreateRoot;
                }
                else
                {
                    auto parent = dynamic_cast<SimulationItem*>(_current->parent());
                    _propertyIndex = propertyIndexForChild(parent, _current);
                    _current = parent;
                }
            }

            // otherwise, retreat to the previous property, and descend its hierarchy as deep as possible
            else
            {
                _propertyIndex--;
                while (true)
                {
                    // if the current property is an item, and the item has properties, then descend the hierarchy
                    auto handler = createPropertyHandler(_current, properties(_current)[_propertyIndex]);
                    auto itemhandler = handler.dynamicCast<ItemPropertyHandler>();
                    if (itemhandler && itemhandler->value() && properties(itemhandler->value()).size()>0)
                    {
                        _current = itemhandler->value();
                        _propertyIndex = properties(itemhandler->value()).size() - 1;
                    }
                    else break;

                    // otherwise, if the property being handled is an item list, ...  TO DO
                    // ...
                }
             }

            // indicate property invalid (meaningless and harmless if state has changed to CreateRoot)
            _propertyValid = false;
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
            if (!SimulationItemDiscovery::inherits(currentType, abstractType)) currentType.clear();
            return new CreateRootWizardPane(abstractType, currentType, this);
        }
    case ConstructHierarchy:
        {
            PropertyHandlerPtr handler = createPropertyHandler(_current, properties(_current)[_propertyIndex]);
            if (handler.dynamicCast<BoolPropertyHandler>())
                return new BoolPropertyWizardPane(handler, this);
            if (handler.dynamicCast<IntPropertyHandler>())
                return new IntPropertyWizardPane(handler, this);
            if (handler.dynamicCast<DoublePropertyHandler>())
                return new DoublePropertyWizardPane(handler, this);
            if (handler.dynamicCast<DoubleListPropertyHandler>())
                return new DoubleListPropertyWizardPane(handler, this);
            if (handler.dynamicCast<StringPropertyHandler>())
                return new StringPropertyWizardPane(handler, this);
            if (handler.dynamicCast<EnumPropertyHandler>())
                return new EnumPropertyWizardPane(handler, this);
            if (handler.dynamicCast<ItemPropertyHandler>())
                return new ItemPropertyWizardPane(handler, this);
            if (handler.dynamicCast<ItemListPropertyHandler>())
                return new ItemListPropertyWizardPane(handler, this);
        }
    case SaveHierarchy:
        return new QLabel("Dummy wizard pane for state SaveHierarchy");
    }
}

////////////////////////////////////////////////////////////////////
