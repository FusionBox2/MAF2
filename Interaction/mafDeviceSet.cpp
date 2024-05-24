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
#include "mafStorageElement.h"

#include <mutex>
#include "mmuIdFactory.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDeviceSet::DEVICE_ADD);
MAF_ID_IMP(mafDeviceSet::DEVICE_REMOVE);
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
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
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
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    assert(device);
    device->Stop();
  }
  m_DevicesMutex->unlock();
  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalStore(mafStorageElementBuilder& node)
//------------------------------------------------------------------------------
{
  if (Superclass::InternalStore(node))
    return -1;

  m_DevicesMutex->lock();
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    if (device->IsPersistent()) // do not store persistent devices
      continue;

    if (node.StoreObject(_R("Device"),device) != MAF_OK)
    {
      mafErrorMacro("Error Writing "<<device->GetName().GetCStr() <<" device");
      m_DevicesMutex->unlock();
		  return MAF_ERROR;;
    }
  }
  m_DevicesMutex->unlock();
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalRestore(const mafStorageElement& node)
//------------------------------------------------------------------------------
{
  int fail=MAF_OK;
  int old_state=IsInitialized();
  Stop();
  RemoveAllDevices();

  Superclass::InternalRestore(node);

  std::vector<mafStorageElement*> devices = node.GetElementsByName(_R("Device"));
  for (auto& device_node : devices)
  {
    // Must create the object before restoring since
    // the device must be already connected to the
    // device manager
    {
      mafObject* obj = nullptr;
      if (device_node->RestoreObject(obj) == MAF_OK)
      {
        if (mafDevice *device=mafDevice::SafeDownCast(obj)) // check the restored object is really a mafDevice
        {
          AddDevice(device);
        } 
        else
        {
          mafErrorMacro("Wrong object type, expect \"mafDevice\" found \""<<obj->GetTypeName()<<"\".");
          fail=MAF_ERROR;
          obj->Delete(); // release memory
        }
      }
      else
      {
        mafErrorMacro("Unknown Device type, I/O parse error.");
        fail=MAF_ERROR;
      }
      
    }
  }

  if (old_state)
    return Start();

  return fail;
}

//------------------------------------------------------------------------------
int mafDeviceSet::GetNumberOfDevices()
//------------------------------------------------------------------------------
{
  int num=m_Devices.size();
  return num;
}

//------------------------------------------------------------------------------
void mafDeviceSet::AddDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert (device);
  assert (!device->GetName().empty()); // all devices must have a name
  m_DevicesMutex->lock();
  m_Devices.push_back(device);
  device->Register(this);
  device->SetListener(this);
  device->PlugEventSource(this,MCH_DOWN);
  m_DevicesMutex->unlock();
  
  InvokeEvent(DEVICE_ADDED,MCH_UP,device);
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->lock();
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    if (device->GetName()==_R(name))
    {
      m_DevicesMutex->unlock();
      return device;
    }
  }

  m_DevicesMutex->unlock();
  return NULL;
}
//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDevice(mafID id)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->lock();
  std::list<mafDevice*>::iterator it;
  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    if (device && device->GetID()==id)
    {
      m_DevicesMutex->unlock();
      return device;
    }
  }

  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    mafDeviceSet *device_set=mafDeviceSet::SafeDownCast(device);
    if (device_set)
    {
      mafDevice *sub_device=device_set->GetDevice(id);
      if (sub_device)
      {
        m_DevicesMutex->unlock();
        return sub_device;
      }
    }
  }

  m_DevicesMutex->unlock();
  return NULL;
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDeviceByIndex(int idx)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->lock();
  std::list<mafDevice*>::iterator it=m_Devices.begin();
  
  for (int i=0;i<idx;i++) it++;
  
  mafDevice *device=*it;
  m_DevicesMutex->unlock();
  return device;
  
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDeviceByIndex(int idx, bool force)
//------------------------------------------------------------------------------
{
  if (mafDevice *device=GetDeviceByIndex(idx))
  {
    return RemoveDevice(device,force);
  }

  mafErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(mafID id, bool force)
//------------------------------------------------------------------------------
{
  if (mafDevice *device=GetDevice(id))
  {
    return RemoveDevice(device,force);
  }

  mafErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(mafDevice *device, bool force)
//------------------------------------------------------------------------------
{
  assert(device);
  std::list<mafDevice*>::iterator it;
  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    if (device==*it)
      break;
  }
  
  if (it!=m_Devices.end())
  {
    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force) 
      return false;
  
    InvokeEvent(DEVICE_REMOVING,MCH_UP,device);
    m_DevicesMutex->lock();
    device->Stop();
    device->RemoveObserver(this);
    m_Devices.erase(it);
    device->Delete();
    m_DevicesMutex->unlock();
    return MAF_OK;
  }
  
  return MAF_ERROR;
}


//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(const char *name, bool force)
//------------------------------------------------------------------------------
{
  mafDevice *device=GetDevice(name);

  return RemoveDevice(device,force);
}

//------------------------------------------------------------------------------ 
void mafDeviceSet::RemoveAllDevices(bool force)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->lock();

  // Remove All (non-persistent) devices
  std::list<mafDevice*>::iterator it;
  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;

    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force)
      continue;

    device->Stop();
    InvokeEvent(DEVICE_REMOVING,MCH_UP,device);
    
    device->Delete();
    *it=NULL;
  }
  
  // remove cleaned nodes
  std::list<mafDevice*>::iterator next_it;

  for (it=m_Devices.begin();it!=m_Devices.end();it=next_it)
  {
    next_it=it;
    next_it++;

    if (*it==NULL)
      m_Devices.erase(it);
  }
  
  m_DevicesMutex->unlock();
}

//------------------------------------------------------------------------------
void mafDeviceSet::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event&&event->GetSender());

  int id = event->GetId();
  int channel = event->GetChannel();

  if (channel == MCH_UP)
  {
    if (id==DEVICE_ADD)
    {
      // this could create problems since there's no control on data to really be of the right type
      AddDevice((mafDevice *)event->GetData());
      return;
    }
    else if (id==DEVICE_REMOVE)
    {
      this->RemoveDevice((mafDevice *)event->GetData());
      return;
    }
  }
  
  Superclass::OnEvent(event);
}
