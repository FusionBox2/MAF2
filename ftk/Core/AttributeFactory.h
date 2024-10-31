#pragma once

#include "ftkConfigure.h"

#include "mafAttribute.h"

#include <map>
#include <string>

#define mafPlugAttributeMacro(node_type,descr) \
  AttributeFactory::RegisterNewAttribute(node_type::GetStaticTypeName(), descr, node_type::NewObject);

using AttributeCreateType = mafObject*;
using CreateAttributeFunction = AttributeCreateType(*)();

BEGIN_FTK_NAMESPACE

class FTK_CORE_EXPORT AttributeFactory
{
public:
  static int Initialize();

  static AttributeCreateType CreateAttribute(const char* ClassName);

  static const char* GetDescription(const char* ClassName);

  static void RegisterNewAttribute(const char* ClassName, const char* description, CreateAttributeFunction createFunction);

private:
  AttributeFactory() = default;

  static AttributeFactory& GetFactory();

  struct CreateInformation
  {
    std::string             m_Description;
    CreateAttributeFunction m_CreateObject;
  };
	std::map<std::string, CreateInformation, std::less<> > m_creatorsMap;
};

END_FTK_NAMESPACE
