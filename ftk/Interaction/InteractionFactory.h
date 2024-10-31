#pragma once

#include "ftkConfigure.h"

#include "mafInteractionConfigure.h"

#include "mafAgent.h"

#include <map>
#include <set>
#include <string>

#define mafPlugDeviceMacro(node_type,descr) \
  InteractionFactory::RegisterNewDevice(node_type::GetStaticTypeName(), descr, node_type::NewObject);

#define mafPlugAvatarMacro(node_type,descr) \
  InteractionFactory::RegisterNewAvatar(node_type::GetStaticTypeName(), descr, node_type::NewObject);

using InteractionCreateType = mafObject*;
using CreateInteractionFunction = InteractionCreateType(*)();

BEGIN_FTK_NAMESPACE

class FTK_INTERACTION_EXPORT InteractionFactory
{
public:
  static int Initialize();

  static const std::set<std::string> *GetAvatarNames();

  static int GetNumberOfAvatars();

  static const char *GetAvatarName(int idx);
  
  static void RegisterNewAvatar(const char* ClassName, const char* description, CreateInteractionFunction createFunction);

  static const std::set<std::string> *GetDeviceNames();
  
  static const char *GetDeviceName(int idx);

  static int GetNumberOfDevices();

  static void RegisterNewDevice(const char* ClassName, const char* description, CreateInteractionFunction createFunction);

  static InteractionCreateType CreateInteraction(const char* ClassName);

  static const char* GetDescription(const char* ClassName);

private:
  InteractionFactory() = default;

  static InteractionFactory& GetFactory();

  static void RegisterNewInteraction(const char* ClassName, const char* description, CreateInteractionFunction createFunction);

	struct CreateInformation
  {
    std::string               m_Description;
    CreateInteractionFunction m_CreateObject;
  };
  std::map<std::string, CreateInformation, std::less<> > m_creatorsMap;
  std::set<std::string> m_DeviceNames;
  std::set<std::string> m_AvatarNames;
};


template <class T>
class mafPlugAvatar
{
public:
  mafPlugAvatar(const char* description);

};

template <class T>
mafPlugAvatar<T>::mafPlugAvatar(const char* description)
{
  InteractionFactory::RegisterNewAvatar(T::GetStaticTypeName(), description, T::NewObject);
}

template <class T>
class mafPlugDevice
{
public:
  mafPlugDevice(const char* description);

};

template <class T>
mafPlugDevice<T>::mafPlugDevice(const char* description)
{
  InteractionFactory::RegisterNewDevice(T::GetStaticTypeName(), description, T::NewObject);
}

END_FTK_NAMESPACE
