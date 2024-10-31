#pragma once

#include "ftkConfigure.h"

#include "lhpTagHandler.h"

#include <map>
#include <string>

#define lhpPlugTagHandlerMacro(tag_handler_type,descr) \
  TagHandlerFactory::RegisterNewTagHandler(tag_handler_type::GetStaticTypeName(), descr, tag_handler_type::NewObject);

using TagHandlerCreateType = mafObject*;
using CreateTagHandlerFunction = TagHandlerCreateType(*)();

BEGIN_FTK_NAMESPACE

class FTK_OPERATION_EXPORT TagHandlerFactory
{
public:
  static int Initialize();

  static TagHandlerCreateType CreateTagHandler(const char* ClassName);

  static const char* GetDescription(const char* ClassName);

  const static std::vector<std::string>& GetTagHandlerNames();

  static void RegisterNewTagHandler(const char* ClassName, const char* description, CreateTagHandlerFunction createFunction);

private:
  TagHandlerFactory() = default;

  static TagHandlerFactory& GetFactory();

  struct CreateInformation
  {
    std::string              m_Description;
    CreateTagHandlerFunction m_CreateObject;
  };
  std::map<std::string, CreateInformation, std::less<> > m_creatorsMap;
  std::vector<std::string> m_TagHandlerNames;
};

template <class T>
class lhpPlugTagHandler
{
public:
  lhpPlugTagHandler(const char* description);
};

template <class T>
lhpPlugTagHandler<T>::lhpPlugTagHandler(const char* description)
{
  TagHandlerFactory::RegisterNewTagHandler(T::GetStaticTypeName(), description, T::NewObject);
}

END_FTK_NAMESPACE
