/*=========================================================================

 Program: MAF2
 Module: mafDeviceSet
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// To be included first because of wxWindows

// base
#include "mafDeviceSet.h"

// events
#include "mafEventBase.h"

// serialization
#include "ftk/IO/StorageElement.h"

#include <mutex>
#include "mmuIdFactory.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDeviceSet::DEVICE_ADDED);
MAF_ID_IMP(mafDeviceSet::DEVICE_REMOVING);
//MAF_ID_IMP(mafDeviceSet::MCH_DEVICE_SETUP);
//MAF_ID_IMP(mafDeviceSet::MCH_DEVICE_SETDOWN);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDeviceSet)

//------------------------------------------------------------------------------
mafDeviceSet::mafDeviceSet()
//------------------------------------------------------------------------------
{
  m_DevicesMutex = new std::mutex;
}

//------------------------------------------------------------------------------
mafDeviceSet::~mafDeviceSet()
//------------------------------------------------------------------------------
{
  RemoveAllDevices(true);
  cppDEL(m_DevicesMutex);
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalInitialize()
//------------------------------------------------------------------------------
{
  Superclass::InternalInitialize();
  m_DevicesMutex->lock();
  for (auto& device : m_Devices)
  {
    assert(device);
    if (device->StartUp())
    {
      mafErrorMacro("Cannot Initilized Device: "<<device->GetName().GetCStr());
		  return MAF_ERROR;
    }
  }
  m_DevicesMutex->unlock();

  return MAF_OK;
}

//------------------------------------------------------------------------------
void mafDeviceSet::InternalShutdown()
//------------------------------------------------------------------------------
{
  m_DevicesMutex->lock();
  for (auto& device : m_Devices)
  {
    assert(device);
    device->Stop();
  }
  m_DevicesMutex->unlock();
  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
void mafDeviceSet::InternalStore(mafStorageElementBuilder& node)
//------------------------------------------------------------------------------
{
  Superclass::InternalStore(node);
  std::lock_guard lock(*m_DevicesMutex);
  for (auto& device : m_Devices)
  {
    if (device->IsPersistent()) // do not store persistent devices
      continue;
    node[_R("Device")].SetValue(device.get());
  }
}

//------------------------------------------------------------------------------
void mafDeviceSet::InternalRestore(const mafStorageElement& node)
//------------------------------------------------------------------------------
{
  int fail=MAF_OK;
  int old_state=IsInitialized();
  Stop();
  RemoveAllDevices();

  Superclass::InternalRestore(node);

  auto devices = node[_R("Device")];
  for (size_t i = 0; i < devices.size(); i++)
  {
    if (auto device = devices[i].As<mafDevice>())
    {
      AddDevice(device);
    }
    else
    {
      mafErrorMacro("Unknown Device type, I/O parse error.");
      fail = MAF_ERROR;
    }
  }

  if (old_state)
    Start();
}

//------------------------------------------------------------------------------
int mafDeviceSet::GetNumberOfDevices()
//------------------------------------------------------------------------------
{
  int num=m_Devices.size();
  return num;
}

//------------------------------------------------------------------------------
void mafDeviceSet::AddDevice(std::shared_ptr<mafDevice> device)
//------------------------------------------------------------------------------
{
  assert (device);
  assert (!device->GetName().empty()); // all devices must have a name
  std::unique_lock guard(*m_DevicesMutex);
  m_Devices.push_back(device);
  device->SetListener(this);
  device->PlugEventSource(this,MCH_DOWN);
  
  InvokeEvent(DEVICE_ADDED,MCH_UP,device.get());
}

//------------------------------------------------------------------------------
std::shared_ptr<mafDevice> mafDeviceSet::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  std::unique_lock guard(*m_DevicesMutex);
  for (auto& device : m_Devices)
  {
    if (device->GetName()==_R(name))
    {
      return device;
    }
  }

  return nullptr;
}
//------------------------------------------------------------------------------
std::shared_ptr<mafDevice> mafDeviceSet::GetDevice(mafID id)
//------------------------------------------------------------------------------
{
  std::unique_lock guard(*m_DevicesMutex);
  for (auto& device : m_Devices)
  {
    if (device && device->GetID()==id)
    {
      return device;
    }
  }

  for (auto& device : m_Devices)
  {
    if (auto device_set=mafDeviceSet::SafeDownCast(device))
    {
      if (auto sub_device=device_set->GetDevice(id))
      {
        return sub_device;
      }
    }
  }
  return nullptr;
}

//------------------------------------------------------------------------------
std::shared_ptr<mafDevice> mafDeviceSet::GetDeviceByIndex(int idx)
//------------------------------------------------------------------------------
{
  std::unique_lock guard(*m_DevicesMutex);
  auto  it = m_Devices.begin();
  
  std::advance(it, idx);
	return *it;
  
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDeviceByIndex(int idx, bool force)
//------------------------------------------------------------------------------
{
  if (auto device = GetDeviceByIndex(idx))
  {
    return RemoveDevice(device.get(), force);
  }

  mafErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(mafID id, bool force)
//------------------------------------------------------------------------------
{
  if (auto device = GetDevice(id))
  {
    return RemoveDevice(device.get(), force);
  }

  mafErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(mafDevice *device, bool force)
//------------------------------------------------------------------------------
{
  assert(device);
  auto it = m_Devices.begin();
  for (it=m_Devices.begin();it!=m_Devices.end();++it)
  {
    if (device==it->get())
      break;
  }
  
  if (it != m_Devices.end())
  {
    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force) 
      return false;
  
    InvokeEvent(DEVICE_REMOVING,MCH_UP,device);
    std::unique_lock guard(*m_DevicesMutex);
    device->Stop();
    device->RemoveObserver(this);
    m_Devices.erase(it);
    m_DevicesMutex->unlock();
    return MAF_OK;
  }
  
  return MAF_ERROR;
}


//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(const char *name, bool force)
//------------------------------------------------------------------------------
{
  auto device = GetDevice(name);

  return RemoveDevice(device.get(),force);
}

//------------------------------------------------------------------------------ 
void mafDeviceSet::RemoveAllDevices(bool force)
//------------------------------------------------------------------------------
{
  std::unique_lock guard(*m_DevicesMutex);

  // Remove All (non-persistent) devices
  auto it = m_Devices.begin();
  for (auto& device : m_Devices)
  {
    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force)
      continue;

    device->Stop();
    InvokeEvent(DEVICE_REMOVING,MCH_UP,device.get());
    
    device.reset();
  }

  m_Devices.remove_if([](const std::shared_ptr<mafDevice>& p) {return !p; });
}

//------------------------------------------------------------------------------
void mafDeviceSet::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  Superclass::OnEvent(event);
}
