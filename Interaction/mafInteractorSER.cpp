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

#include "mafInteractorSER.h"
#include "ftk/Interaction/InteractionFactory.h"
#include "mafDeviceButtonsPadTracker.h"
#include "ftk/IO/StorageElement.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractorSER)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractorSER::mafInteractorSER()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafInteractorSER::~mafInteractorSER()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void mafInteractorSER::InternalStore(mafStorageElementBuilder& node)
//------------------------------------------------------------------------------
{
  for (mmuActionsMap::iterator it=m_Actions.begin();it!=m_Actions.end();it++)
  {
    mafAction *action=it->second.get();
    node[_R("Action")].SetValue(action);
  }
}

//------------------------------------------------------------------------------
void mafInteractorSER::InternalRestore(const mafStorageElement& node)
//------------------------------------------------------------------------------
{
  auto children = node[_R("Action")];
  for (int i=0;i<children.GetNumItems();i++)
  {
    // create the object to be restored mannualy since mafAction is not in the factory
    mafString action_name = children[i](_R("Name")).As<mafString>();
    auto action = GetAction(action_name.GetCStr());

    if (action)
    {
      action->Restore(children[i]);
      //mafErrorMacro("I/O Error restoring action");
    }
  }
}

//------------------------------------------------------------------------------
std::shared_ptr<mafAction> mafInteractorSER::GetAction(const char *name)
//------------------------------------------------------------------------------
{
  auto it=m_Actions.find(_R(name));
  return (it!=m_Actions.end()?it->second :nullptr);
}

//------------------------------------------------------------------------------
void mafInteractorSER::GetActions(std::vector<std::shared_ptr<mafAction> > &actions)
//------------------------------------------------------------------------------
{
  std::vector<std::shared_ptr<mafAction> > res;
  res.reserve(m_Actions.size());
  for (auto& entry : m_Actions)
  {
    res.push_back(entry.second);
  }
  actions = std::move(res);
}
//------------------------------------------------------------------------------
int mafInteractorSER::BindAction(const char *action,mafInteractor *agent)
//------------------------------------------------------------------------------
{
  if (auto a = GetAction(action))
  {
    a->BindInteractor(agent);
    return 0;
  }
  return -1;
}

//------------------------------------------------------------------------------
int mafInteractorSER::UnBindAction(const char *action,mafInteractor *agent)
//------------------------------------------------------------------------------
{
  if (auto a = GetAction(action))
  {
    a->UnBindInteractor(agent);
    return 0;
  }
  return -1;
}

//------------------------------------------------------------------------------
std::shared_ptr<mafAction> mafInteractorSER::AddAction(const char *name, float priority, int type)
//------------------------------------------------------------------------------
{
  if (auto old_action = GetAction(name))
    return old_action;

  auto action = mafAction::NewSPtr();
  action->SetName(_R(name));
  action->SetType(type);
  AddAction(action, priority);
  
  return action;
}

//------------------------------------------------------------------------------
void mafInteractorSER::AddAction(std::shared_ptr<mafAction> action, float priority)
//------------------------------------------------------------------------------
{
  assert(action);
  m_Actions[action->GetName()]=action;
  
  // attach the action both as a listener and an event source 
  action->AddObserver(this);
  //action->PlugEventSource(this,MCH_CAMERA CameraUpdateChannel);
}
//------------------------------------------------------------------------------
void mafInteractorSER::BindDeviceToAction(std::shared_ptr<mafDevice> device,mafAction *action)
//------------------------------------------------------------------------------
{
  assert(device);
  assert(action);
  action->BindDevice(device);
}
//------------------------------------------------------------------------------
void mafInteractorSER::BindDeviceToAction(std::shared_ptr<mafDevice> device,const char *action_name)
//------------------------------------------------------------------------------
{
  BindDeviceToAction(device,GetAction(action_name).get());
}
//------------------------------------------------------------------------------
void mafInteractorSER::UnBindDeviceFromAction(mafDevice *device,mafAction *action)
//------------------------------------------------------------------------------
{
  assert(device);
  assert(action);
  action->UnBindDevice(device);
}
//------------------------------------------------------------------------------
void mafInteractorSER::UnBindDeviceFromAction(mafDevice *device,const char *action_name)
//------------------------------------------------------------------------------
{
  UnBindDeviceFromAction(device,GetAction(action_name).get());
}

//------------------------------------------------------------------------------
void mafInteractorSER::UnBindDeviceFromAllActions(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);
  std::vector<mafAction *> device_actions;
  GetDeviceBindings(device,device_actions);

  for (auto& action : device_actions)
  {
    action->UnBindDevice(device);
  }
}

//------------------------------------------------------------------------------
void mafInteractorSER::GetDeviceBindings(mafDevice *device, std::vector<mafAction *> &actions)
//------------------------------------------------------------------------------
{
  assert(device);

  std::vector<mafAction*> res;

  std::vector<mafBaseEventHandler *> observers;
  device->GetObservers(MCH_INPUT,observers);
  
  for (auto& observer : observers)
  {
    if (auto action = dynamic_cast<mafAction *>(observer))
    {
      actions.push_back(action);
    }  
  }
  actions = std::move(res);
}

std::shared_ptr<mafInteractorSER> mafInteractorSER::Create(const char* InteractorSERType)
{
  if (auto object = InteractionFactory::CreateInteraction(InteractorSERType))
  {
    if (auto interactorSER = mafInteractorSER::SafeDownCast(object))
    {
      return interactorSER;
    }
  }
  return nullptr;
}
