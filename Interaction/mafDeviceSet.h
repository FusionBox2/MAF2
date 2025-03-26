#pragma once

#include "mafDevice.h"
#include <list>
#include <mutex>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
/**
  This class manages a set of usb-devices, and stores a list of all child devices' pointers. It also
  dispatches some commands to child devices, e.g. initializzation commands. Being also an event 
  queue, this class gathers events coming from all devices (e.g. running on different threads) and
  serializes their dispatching.
  @sa mafDeviceManager mafEvent
  */
class MAF_EXPORT mafDeviceSet : public mafDevice
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events 
      Issued when a new device (passed in the Data argument) is added to this set */
  MAF_ID_DEC(DEVICE_ADDED);
  /** @ingroup Events 
      Issued when a device (passed in the Data argument) is being removed from this set */
  MAF_ID_DEC(DEVICE_REMOVING); 
  /** @ingroup channel
      New channel for settings */
  //MAF_ID_DEC(MCH_DEVICE_SETUP);
  /** @ingroup channel 
      New channel for settings */
  //MAF_ID_DEC(MCH_DEVICE_SETDOWN);

  mafTypeMacro(mafDeviceSet,mafDevice);

  /** Add a new device. If the returned value is false the operation has failed! */
  virtual void AddDevice(std::shared_ptr<mafDevice> device);

  /**  Return a device in the set given its ID */
  std::shared_ptr<mafDevice> GetDeviceByIndex(int idx);

  /**  Return a device in the set given its ID */
  std::shared_ptr<mafDevice> GetDevice(mafID id);
  
  /**  Return a device in the set given its name */
  std::shared_ptr<mafDevice> GetDevice(const char *name);

  /** Return the number of devices assigned to this manager */
  int GetNumberOfDevices();
  
  /** Remove a device from the manager's list given its ID. */
  int RemoveDeviceByIndex(int idx, bool force=false);

  /** Remove a device from the manager's list given its ID. */
  int RemoveDevice(mafID id, bool force=false);
  
  /** Remove a device from the manager's list */
  int RemoveDevice(mafDevice *device, bool force=false);
  
  /** Remove a device from the manager's list given its name. */
  int RemoveDevice(const char *name, bool force=false);

  /** Remove all devices (as as RemoveAllSenders()) */
  void RemoveAllDevices(bool force=false);

  /** Reimplemented to manage AddDevice event */
  void OnEvent(mafEventBase *event) override;

  /** Return the list of devices pluged into this device set */
  std::list<std::shared_ptr<mafDevice> > *GetDevices() {return &m_Devices;}

protected:
  mafDeviceSet();
  ~mafDeviceSet() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** Initialization automatically initialize subdevices */
  int InternalInitialize() override;

  /** shutdown also subdevices */
  void InternalShutdown() override;

  std::list<std::shared_ptr<mafDevice> > m_Devices;

  std::mutex *          m_DevicesMutex;

private:
  mafDeviceSet(const mafDeviceSet&);  // Not implemented.
  void operator=(const mafDeviceSet&);  // Not implemented.
};
