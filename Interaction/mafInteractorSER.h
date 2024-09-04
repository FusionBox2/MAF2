/*=========================================================================

 Program: MAF2
 Module: mafInteractorSER
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafInteractorSER_h
#define __mafInteractorSER_h

#include "mafInteractor.h"
#include "mafObjectFactory.h"
#include "mafAction.h"
#include "mafTo.h"
#include <map>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafDevice;
class mafString;
class mafStorageElement;
class mafStorageElementBuilder;

/** Class implementing static routing from devices to interactors passing through actions.
  This class routes events from input devices to interactors according to a static binding
  defined by user (or application logic). Devices are connected to "actions", special 
  bridge agents that simply deliver events on the "DeviceInputChannelsEvents" coming from
  connected devices to any connected interactor, and events coming on the "DeviceOutputChannel",
  coming from any interactor to all connected devices. This allow the user to define how to use
  devices connected to the system, and the interactor to ignore which devices are providing 
  inputs. Its clear that an interactor will ignore any event coming from incompatible devices.
  Currently there is not device type control implementation.
  @todo
  - to implement a type safe static binding mechanims
   */
class MAF_EXPORT mafInteractorSER : public mafInteractor
{
public: 
  mafTypeMacro(mafInteractorSER,mafInteractor);

   /** Un/Bind a device to an action */
  void BindDeviceToAction(mafDevice *device,mafAction *action);
  /** Un/Bind a device to an action */
  void BindDeviceToAction(mafDevice *device,const char *action_name);
  /** Un/Bind a device to an action */
  void UnBindDeviceFromAction(mafDevice *device,mafAction *action);
  /** Un/Bind a device to an action */
  void UnBindDeviceFromAction(mafDevice *device,const char *action_name);
  /** Unbind a device from all actions: used when a device is removed */
  void UnBindDeviceFromAllActions(mafDevice *device);

  /** return list of device bindings to actions */
  void GetDeviceBindings(mafDevice *device,std::vector<mafAction *> &actions);
  /** 
    Bind an agent to the specified action. 
    If the action is not present return -1 */
  int BindAction(const char *action,mafInteractor *agent);
  /** 
    Unbind an agent from the specified action. 
    If the action is not present return -1 */
  int UnBindAction(const char *action,mafInteractor *agent);

  /** Define a new action router.*/
  mafAction *AddAction(const char *name, float priority = 0.0, int type = mafAction::SHARED_ACTION);
  void AddAction(mafAction *action, float priority = 0.0);
  
  /** Get an action router.*/
  mafAction *GetAction(const char *name);
  
  typedef std::map<mafString,mafAutoPointer<mafAction> > mmuActionsMap;

  /** Return the actions container */
  void GetActions(std::vector<mafAction *> &actions);
  const mmuActionsMap *GetActions() {return &m_Actions;}

  void Store(mafStorageElementBuilder& element) { InternalStore(element); }
  void Restore(const mafStorageElement& element) { InternalRestore(element); }

protected:
  virtual void InternalStore(mafStorageElementBuilder& node);
  virtual void InternalRestore(const mafStorageElement& node);

  mafInteractorSER();
  virtual ~mafInteractorSER();

  mmuActionsMap  m_Actions; ///< takes a list of the active actions

private:
  mafInteractorSER(const mafInteractorSER&);  // Not implemented.
  void operator=(const mafInteractorSER&);  // Not implemented.
};

namespace parser
{
  template<class Value>
  mafInteractorSER* Parse(const Value& value, parser::To<mafInteractorSER>)
  {
    mafString type_name = value(_R("Type")).As<mafString>();
    auto object = mafObjectFactory::CreateInstance(type_name.GetCStr());
    if (auto interactor = mafInteractorSER::SafeDownCast(object))
    {
      interactor->Restore(value);
      return interactor;
    }
    return nullptr;
  }
}

namespace serializer
{
  template<class Value>
  void Serialize(Value& value, mafInteractorSER* const& interactor)
  {
    assert(interactor);
    mafString type_name = _R(interactor->GetTypeName());
    value(_R("Type")).SetValue(type_name);
    interactor->Store(value);
  }
}

#endif 
