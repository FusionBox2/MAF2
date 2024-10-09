#pragma once

#include "ftkConfigure.h"

#include "mafPipe.h"

#include <map>
#include <string>

#define mafPlugPipeMacro(node_type,descr) \
  PipeFactory::RegisterNewPipe(node_type::GetStaticTypeName(), descr, node_type::NewObjectS);

using PipeCreateType = std::shared_ptr<mafPipe>;
using CreatePipeFunction = PipeCreateType(*)();

BEGIN_FTK_NAMESPACE

class FTK_CORE_EXPORT PipeFactory
{
public:
  static int Initialize();

  static PipeCreateType CreatePipe(const char* ClassName);

  static const char* GetDescription(const char* ClassName);

  static void RegisterNewPipe(const char* ClassName, const char* description, CreatePipeFunction createFunction);

private:
  PipeFactory() = default;

  static PipeFactory& GetFactory();

  struct CreateInformation
  {
    std::string        m_Description;
    CreatePipeFunction m_CreateObject;
  };
  std::map<std::string, CreateInformation, std::less<> > m_creatorsMap;
};

template <class T>
class mafPlugPipe
{
public:
  mafPlugPipe(const char* description);

};

template <class T>
mafPlugPipe<T>::mafPlugPipe(const char *description)
{ 
  PipeFactory::RegisterNewPipe(T::GetStaticTypeName(), description, T::NewObject);
}

END_FTK_NAMESPACE
