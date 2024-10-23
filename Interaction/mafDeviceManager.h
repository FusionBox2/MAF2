/*=========================================================================

 Program: MAF2
 Module: mafDeviceManager
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/ 
#ifndef __mafDeviceManager_h
#define __mafDeviceManager_h

#include "mafAgentEventHandler.h"
#include "ftk/IO/To.h"

#include <list>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDevice;
class mafDeviceSet;
class mafStorageElement;
class mafStorageElementBuilder;

/**
  Class managing the devices inside a MAF application and the synchronization with wxWindows message pump.
  This class is used both as a container for all active devices and manages synchronization
  of events coming from input devices with wxWindows message pump.
  The synchronization mechanisms is accomplished by executing the "dispatching" on a separate thread, and this is 
  currently imlemented by sending an event on the GUI message pump with a request for dispatching the events. This
  make dispatching to occurs on the right thread @sa mafAgentEventThreaded.

  Device instances can be of two different types: persistent and non-persistent. The first class of devices are
  typically created by the application code, while the seconds are created by user using the Interaction Settings
  configuration dialog. Persistent devices are not stored to disk and neither they are stopped or destroyed when
  a new device configuration is loaded from disk. Typically a non persistent device object pointer should not be
  stored by other componenents, since when loading new configuration these devices are destroyed a new one are 
  created.  An example of persistent device is the mouse, which is created at application start.
  
  To store devices a mafDeviceSet object is internally used as the root of the devices tree. Devices that could have
  subdevices can inherit from mafDeviceSet to correctly manage the start/stop mechanisms of all subdevices.
  
  @sa mafDevice mafDeviceSet
  @todo 
  - implement AddDevice()
 */
class MAF_EXPORT mafDeviceManager : public mafAgentEventHandler
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      advise dispatching has started */
  MAF_ID_DEC(DISPATCH_START)
  /** @ingroup Events
      advise dispatching has finished  */
  MAF_ID_DEC(DISPATCH_END) 

  mafDeviceManager();
  ~mafDeviceManager() override;

  mafTypeMacro(mafDeviceManager,mafAgentEventHandler);

  static mafDeviceManager* Create(const char* DeviceManagerType);
  /**
    Add a new device. If the returned value is 0 the operation has failed! */
  virtual mafID AddDevice(mafDevice *device);

  /**
    Add a new device specifying only its class name.
    If the returned pointer is NULL the operation has failed! "persistent" flag must be
    used to add persistent devices */
  virtual mafDevice *AddDevice(const char *type, bool persistent=false);

  /** Remove a device from the manager's list. force flag must be used to remove
      persistent devices */
  int RemoveDevice(mafDevice *device, bool force=false);
  /** Remove a device with the given name from the manager's list. */
  int RemoveDevice(const char *name, bool force=false);

  /**  Return a device handler given its name. */
  mafDevice *GetDevice(const char *name);

  /**  Return a device handler given its ID. Persistent devices have ID < MIN_DEVICE_ID */
  mafDevice *GetDevice(mafID id);

  /** Return the number of devices assigned to this manager */
  int GetNumberOfDevices();

   /** Remove all devices (as as RemoveAllSenders()), the "force" flag must be used
       to remove also persistent devices */
  void RemoveAllDevices(bool force=false);

  /** return the list of active devices */
  std::list<mafDevice *> *GetDevices();

  /** return the device set storing first level devices */
  mafDeviceSet *GetDeviceSet() {return m_DeviceSet;}

  void OnEvent(mafEventBase *event) override;

  /**
    Redefined to manage the synchronization with display rendering: during the event
    dispatching all rendering requests must be queued. For this reason at dispatching
    start a StartDispatchingEvent is (synchronously) sent on the default channel, and
    StopDispatchingEvent is sent when dispatching is finished */
  bool DispatchEvents() override;

  void Store(mafStorageElementBuilder& element) { InternalStore(element); }
  void Restore(const mafStorageElement& element) { InternalRestore(element); }

protected:
  virtual void InternalStore(mafStorageElementBuilder& node);
  virtual void InternalRestore(const mafStorageElement& node);

  int InternalInitialize() override;
  void InternalShutdown() override;

  mafDeviceSet*   m_DeviceSet;        ///< container for first level devices
  mafID           m_DeviceIdCounter;  ///< number of plugged devices
  mafID           m_PersistentDeviceIdCounter; ///< number of persistent plugged devices
  bool            m_RestoringFlag;
  
private:
  mafDeviceManager(const mafDeviceManager&);  // Not implemented.
  void operator=(const mafDeviceManager&);  // Not implemented.
};

namespace parser
{
  template<class Value>
  mafDeviceManager* Parse(const Value& value, parser::To<mafDeviceManager>)
  {
    mafString type_name = value(_R("Type")).template As<mafString>();
    if (auto deviceManager = mafDeviceManager::Create(type_name.GetCStr()))
    {
      deviceManager->Restore(value);
      return deviceManager;
    }
    return nullptr;
  }
}

namespace serializer
{
  template<class Value>
  void Serialize(Value& value, mafDeviceManager* const& deviceManager)
  {
    mafString type_name = _R(deviceManager->GetTypeName());
    assert(deviceManager);
    value(_R("Type")).SetValue(type_name);
    deviceManager->Store(value);
  }
}

#endif 
