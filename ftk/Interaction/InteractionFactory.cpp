#include "ftk/Interaction/InteractionFactory.h"

#include "mafDevice.h"
#include "mafAvatar.h"

int InteractionFactory::Initialize()
{

  return MAF_OK;
}

const std::set<std::string>* InteractionFactory::GetAvatarNames()
{
  return &GetFactory().m_AvatarNames;
}

int InteractionFactory::GetNumberOfAvatars()
{
  return GetFactory().m_AvatarNames.size();
}

const char* InteractionFactory::GetAvatarName(int idx)
{
  auto it = GetFactory().m_AvatarNames.begin();
  for (int i = 0; i < idx; i++)
    it++;
  return it->c_str();
}

void InteractionFactory::RegisterNewAvatar(const char* node_name, const char* description, CreateInteractionFunction createFunction)
{
  GetFactory().m_AvatarNames.insert(node_name);
  RegisterNewInteraction(node_name, description, createFunction);
}

const std::set<std::string>* InteractionFactory::GetDeviceNames()
{
  return &GetFactory().m_DeviceNames;
}

int InteractionFactory::GetNumberOfDevices()
{
  return GetFactory().m_DeviceNames.size();
}

const char* InteractionFactory::GetDeviceName(int idx)
{
  auto it = GetFactory().m_DeviceNames.begin();
  for (int i = 0; i < idx; i++)
    it++;
  return it->c_str();
}

void InteractionFactory::RegisterNewDevice(const char* node_name, const char* description, CreateInteractionFunction createFunction)
{
  GetFactory().m_DeviceNames.insert(node_name);
  RegisterNewInteraction(node_name, description, createFunction);
}

InteractionCreateType InteractionFactory::CreateInteraction(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_CreateObject();
  }
  return nullptr;
}

const char* InteractionFactory::GetDescription(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_Description.c_str();
  }
  return nullptr;
}

void InteractionFactory::RegisterNewInteraction(const char* ClassName, const char* description, CreateInteractionFunction createFunction)
{
  CreateInformation info;
  info.m_Description = description;
  info.m_CreateObject = createFunction;
  GetFactory().m_creatorsMap[ClassName] = info;
}

InteractionFactory& InteractionFactory::GetFactory()
{
  static InteractionFactory instance;
  return instance;
}
