/*=========================================================================

 Program: MAF2
 Module: mafAction
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafAction_h
#define __mafAction_h

#include "mafAgent.h"
#include "ftk/Base/RegisteringPointer.h"
#include "mafTo.h"
#include <list>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
template <class T> class vtkTemplatedList;
class mafDevice;
class mafInteractor;
class vtkRenderer;
class mafStorageElement;
class mafStorageElementBuilder;

/** Class used to route events from devices to interactors */
class MAF_EXPORT mafAction : public mafAgent
{
public:
  /** @ingroup Events */
  /** @{ */
  MAF_ID_DEC(DEVICE_BIND);            ///< force the action to bind to the device
  MAF_ID_DEC(QUERY_CONNECTED_DEVICES);///< Issued by interactors to query for connected devices. @sa ConnectedDeviceEvent
  MAF_ID_DEC(DEVICE_PLUGGED);         ///< Issued by devices when connected to actions or when queried.
  MAF_ID_DEC(DEVICE_UNPLUGGED);       ///< Issued by devices when connected to actions or when queried.

  enum ACTION_TYPE
  {
    SHARED_ACTION = 0, 
    EXCLUSIVE_ACTION 
  };

  typedef std::list<mafAutoPointer<mafDevice> > mmuDeviceList;
  
  mafTypeMacro(mafAction,mafAgent);

  static mafAction* Create(const char* ActionType);
  /** Set the type of action*/
  int GetType() {return m_Type;}
  void SetType(int t) {if (t==SHARED_ACTION||t==EXCLUSIVE_ACTION) m_Type=t;}
  void SetTypeToShared() {SetType(SHARED_ACTION);}
  void SetTypeToExclusive() {SetType(EXCLUSIVE_ACTION);}
  
  /** Bind/Unbind a device to this action */
  void BindDevice(mafDevice *device);
  void UnBindDevice(mafDevice *device);

  /** Bind/Unbind an interactor to this action */
  void BindInteractor(mafInteractor *inter);
  void UnBindInteractor(mafInteractor *inter);

  /** Get list of devices assigned to this action */
  const mmuDeviceList *GetDevices() {return &m_Devices;}


  /** Redefined to answer incoming queries about connected devices. */
  void OnEvent(mafEventBase *event) override;

  void Store(mafStorageElementBuilder& element) { InternalStore(element); }
  void Restore(const mafStorageElement& element) { InternalRestore(element); }

protected:
  mafAction();
  ~mafAction() override;

  virtual void InternalStore(mafStorageElementBuilder& node);
  virtual void InternalRestore(const mafStorageElement& node);

  int   m_Type;
  std::list<mafAutoPointer<mafDevice> > m_Devices;

private:
  mafAction(const mafAction&);  // Not implemented.
  void operator=(const mafAction&);  // Not implemented.
};

namespace parser
{
  template<class Value>
  mafAction* Parse(const Value& value, parser::To<mafAction>)
  {
    mafString type_name = value(_R("Type")).template As<mafString>();
    if (auto action = mafAction::Create(type_name.GetCStr()))
    {
      action->Restore(value);
      return action;
    }
    return nullptr;
  }
}

namespace serializer
{
  template<class Value>
  void Serialize(Value& value, mafAction* const& action)
  {
    assert(action);
    mafString type_name = _R(action->GetTypeName());
    value(_R("Type")).SetValue(type_name);
    action->Store(value);
  }
}

#endif 
