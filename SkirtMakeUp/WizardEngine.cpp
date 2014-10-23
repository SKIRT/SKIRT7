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
#include "SubItemPropertyWizardPane.hpp"
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
            // indicate not editing sub-item (meaningless and harmless if current item is not an item list)
            _subItemIndex = -1;
        }
        break;
    case ConstructHierarchy:
        {
            auto handler = createPropertyHandler(_current, properties(_current)[_propertyIndex]);
            auto itemhdlr = handler.dynamicCast<ItemPropertyHandler>();
            auto itemlisthdlr = handler.dynamicCast<ItemListPropertyHandler>();

            // if the property being handled is an item, and the item has properties, then descend the hierarchy
            if (itemhdlr && itemhdlr->value() && properties(itemhdlr->value()).size()>0)
            {
                _current = itemhdlr->value();
                _propertyIndex = 0;
            }

            // otherwise, if the property being handled is an item list, and we're editing one of its subitems,
            // and the subitem has properties, then descend the hierarchy into that subitem
            else if (itemlisthdlr && _subItemIndex>=0 && properties(itemlisthdlr->value()[_subItemIndex]).size()>0)
            {
                _current = itemlisthdlr->value()[_subItemIndex];
                _propertyIndex = 0;
            }

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

                    // if we're advancing out of a subitem, go to the item list property rather than the next property
                    if (createPropertyHandler(_current, properties(_current)[_propertyIndex])
                            .dynamicCast<ItemListPropertyHandler>()) _propertyIndex--;
                }

                // advance to the next property (meaningless and harmless if state has changed to SaveHierarchy)
                _propertyIndex++;
            }

            // indicate property invalid (meaningless and harmless if state has changed to SaveHierarchy)
            _propertyValid = false;

            // indicate not editing sub-item (meaningless and harmless if current item is not an item list)
            _subItemIndex = -1;
        }
        break;
    case SaveHierarchy:
        break;
    }
    emitStateChanged();
}

////////////////////////////////////////////////////////////////////

namespace
{
    // private helper function used in retreat():
    // starting from the current item & property index, descend the hierarchy as deep as possible,
    // and update the current item & property index in place
    void descendToDeepest(SimulationItem*& _current, int& _propertyIndex)
    {
        while (true)
        {
            // if the current property is an item, and the item has properties, then descend the hierarchy
            auto handler = createPropertyHandler(_current, properties(_current)[_propertyIndex]);
            auto itemhandler = handler.dynamicCast<ItemPropertyHandler>();
            if (itemhandler && itemhandler->value() && properties(itemhandler->value()).size()>0)
            {
                _current = itemhandler->value();
                _propertyIndex = properties(_current).size() - 1;
            }
            else break;
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
        {
            _state = BasicChoice;
        }
        break;
    case ConstructHierarchy:
        {
            // if this is an item list property, and we're editing a subitem,
            // go to the item list property rather than the previous property
            if (createPropertyHandler(_current, properties(_current)[_propertyIndex])
                    .dynamicCast<ItemListPropertyHandler>() && _subItemIndex>=0) _subItemIndex = -1;

            // otherwise, if this was the first property at this level, move up the hierarchy to the previous level
            // unless this is already the root item, in which case we move to the CreateRoot state
            else if (_propertyIndex == 0)
            {
                if (_current == _root)
                {
                    _state = CreateRoot;
                }
                else
                {
                    auto child = _current;
                    _current = dynamic_cast<SimulationItem*>(child->parent());
                    _propertyIndex = propertyIndexForChild(_current, child);

                    // if we're retreating out of a subitem, go to the subitem choice pane first
                    auto handler = createPropertyHandler(_current, properties(_current)[_propertyIndex]);
                    auto itemlisthdlr = handler.dynamicCast<ItemListPropertyHandler>();
                    if (itemlisthdlr) _subItemIndex = itemlisthdlr->value().indexOf(child);
                    else _subItemIndex = -1;
                }
            }

            // otherwise, retreat to the previous property, and descend its hierarchy as deep as possible
            else
            {
                _propertyIndex--;
                descendToDeepest(_current, _propertyIndex);

                // indicate not editing sub-item (meaningless and harmless if current item is not an item list)
                _subItemIndex = -1;
             }

            // indicate property invalid (meaningless and harmless if state has changed to CreateRoot)
            _propertyValid = false;
        }
        break;
    case SaveHierarchy:
        {
            // go back to hierarchy construction
            _state = ConstructHierarchy;
            _propertyValid = false;

            // descend the existing hierarchy as deep as possible
            _current = _root;
            _propertyIndex = properties(_root).size() - 1; // assumes that the root has at least one property
            descendToDeepest(_current, _propertyIndex);

            // indicate not editing sub-item (meaningless and harmless if current item is not an item list)
            _subItemIndex = -1;
        }
        break;
    }
    emitStateChanged();
}

////////////////////////////////////////////////////////////////////

void WizardEngine::advanceToEditSubItem(int subItemIndex)
{
    // indicate that we're editing the current sub-item
    _subItemIndex = subItemIndex;
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
            {
                if (_subItemIndex<0) return new ItemListPropertyWizardPane(handler, this);
                else                 return new SubItemPropertyWizardPane(handler, this);
            }
        }
    case SaveHierarchy:
        return new QLabel("Dummy wizard pane for state SaveHierarchy");
    }
}

////////////////////////////////////////////////////////////////////
