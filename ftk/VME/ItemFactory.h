#pragma once

#include "ftkConfigure.h"

#include "mafVMEItem.h"

#include <map>
#include <string>

#define mafPlugItemMacro(node_type,descr) \
  ItemFactory::RegisterNewItem(node_type::GetStaticTypeName(), descr, node_type::NewObject);

using ItemCreateType = mafObject*;
using CreateItemFunction = ItemCreateType(*)();

BEGIN_FTK_NAMESPACE

class FTK_CORE_EXPORT ItemFactory
{
public:
  static int Initialize();

  static ItemCreateType CreateItem(const char* ClassName);

  static const char* GetDescription(const char* ClassName);

  static void RegisterNewItem(const char* ClassName, const char* description, CreateItemFunction createFunction);

private:
  ItemFactory() = default;

  static ItemFactory& GetFactory();

  struct CreateInformation
  {
    std::string        m_Description;
    CreateItemFunction m_CreateObject;
  };
  std::map<std::string, CreateInformation, std::less<> > m_creatorsMap;
};

END_FTK_NAMESPACE
